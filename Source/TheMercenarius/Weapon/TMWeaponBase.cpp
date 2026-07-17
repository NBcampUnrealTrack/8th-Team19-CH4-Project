// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TMWeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATMWeaponBase::ATMWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ATMWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATMWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAttacking)
	{
		ExecuteBoxTrace();
	}
}

void ATMWeaponBase::SetWeaponCollision(bool bEnable)
{
	bIsAttacking = bEnable;
	if (bIsAttacking == false)
	{
		HitActors.Empty();
	}
}

void ATMWeaponBase::ExecuteBoxTrace()
{
	//FVector StartPos = WeaponMesh->GetSocketLocation(StartSocketName);
	//FVector EndPos = WeaponMesh->GetSocketLocation(EndSocketName);

	//FRotator TraceRotation = WeaponMesh->GetComponentRotation();

	//TArray<FHitResult> HitResults;
	//TArray<AActor*> ActorsToIgnore;
	//ActorsToIgnore.Add(this);
	//ActorsToIgnore.Add(GetOwner());

	/*bool bHit = UKismetSystemLibrary::BoxTraceMulti(GetWorld(),
		StartPos,
		EndPos,
		BoxHalfSize,
		TraceRotation,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResults,
		true);

	if (bHit)
	{
		for(const FHitResult& Hit : HitResults)
		{
			HitActors.Add(HitActor);

			UGameplayStatics::ApplyDamage(
			HitActor,
			damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
			);
		}
	}
	현재 주석은 무기 소켓의 시작과 끝 부분을 지정하면 제거 할 예정
	*/
}