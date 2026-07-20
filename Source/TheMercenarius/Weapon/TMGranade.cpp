// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TMGranade.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/TMPlayerBase.h"
#include "Weapon/TMWeaponBase.h"

// Sets default values
ATMGranade::ATMGranade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(50.f);

	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));


	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetEnableGravity(true);
	CollisionComponent->BodyInstance.bUseCCD = true;

	SetRootComponent(CollisionComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 2.f;
	ProjectileMovement->bShouldBounce = false;


	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATMGranade::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATMGranade::OnOverlapBegin);

	CollisionComponent->OnComponentHit.AddDynamic(this, &ATMGranade::OnHit);
}

void ATMGranade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalInpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor : %s"), *OtherActor->GetName());
	}
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		Explode();
	}
}

void ATMGranade::Explode()
{
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	ATMPlayerBase* player = Cast<ATMPlayerBase>(GetWorld());
	ATMWeaponBase* Weapon = Cast<ATMWeaponBase>(GetWorld());
	TArray<TEnumAsByte<EObjectTypeQuery>>ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*>IgnoredActors;
	IgnoredActors.Add(this);
	IgnoredActors.Add(player);
	IgnoredActors.Add(Weapon);
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		ExplosionDamage * 3.5f,

		GetActorLocation(),
		500.f,
		nullptr,
		IgnoredActors,
		this,
		nullptr,
		true
	);
	Destroy();
}

void ATMGranade::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		Explode();
	}
}