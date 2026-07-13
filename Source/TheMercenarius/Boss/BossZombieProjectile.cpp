// Fill out your copyright notice in the Description page of Project Settings.

#include "BossZombieProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABossZombieProjectile::ABossZombieProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->InitSphereRadius(35.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABossZombieProjectile::OnProjectileOverlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 1200.0f;
	ProjectileMovementComponent->MaxSpeed = 1200.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
}

void ABossZombieProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeTime);
}

void ABossZombieProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossZombieProjectile::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}

void ABossZombieProjectile::OnProjectileOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	APawn* HitPawn = Cast<APawn>(OtherActor);
	if (!HitPawn)
	{
		return;
	}

	UGameplayStatics::ApplyDamage(
		HitPawn,
		Damage,
		GetInstigatorController(),
		this,
		nullptr
	);

	UE_LOG(LogTemp, Warning, TEXT("Zombie Projectile Hit: %s"), *HitPawn->GetName());

	Destroy();
}