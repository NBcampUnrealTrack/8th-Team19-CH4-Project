#include "BossCharacterRat.h"

#include "AIController.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


ABossCharacterRat::ABossCharacterRat()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	GetCharacterMovement()->MaxWalkSpeed = 250.0f;

	ScratchHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ScratchHitBox"));
	ScratchHitBox->SetupAttachment(RootComponent);
	ScratchHitBox->SetRelativeLocation(FVector(180.0f, 0.0f, 0.0f));
	ScratchHitBox->SetBoxExtent(FVector(160.0f, 120.0f, 80.0f));
	ScratchHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ScratchWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScratchWarningMesh"));
	ScratchWarningMesh->SetupAttachment(ScratchHitBox);
	ScratchWarningMesh->SetRelativeLocation(FVector::ZeroVector);
	ScratchWarningMesh->SetRelativeScale3D(FVector(3.2f, 2.4f, 0.05f));
	ScratchWarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScratchWarningMesh->SetHiddenInGame(true);

	DashWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DashWarningMesh"));
	DashWarningMesh->SetupAttachment(RootComponent);
	DashWarningMesh->SetRelativeLocation(FVector(DashDistance * 0.5f, 0.0f, 5.0f));
	DashWarningMesh->SetRelativeScale3D(FVector(DashDistance / 100.0f, DashHitWidth / 100.0f, 0.05f));
	DashWarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DashWarningMesh->SetHiddenInGame(true);

	DamageAuraSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageAuraSphere"));
	DamageAuraSphere->SetupAttachment(RootComponent);
	DamageAuraSphere->SetSphereRadius(AuraRadius);
	DamageAuraSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DamageAuraWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DamageAuraWarningMesh"));
	DamageAuraWarningMesh->SetupAttachment(RootComponent);
	DamageAuraWarningMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 5.0f));
	DamageAuraWarningMesh->SetRelativeScale3D(FVector(AuraRadius / 50.0f, AuraRadius / 50.0f, 1.0f));
	DamageAuraWarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageAuraWarningMesh->SetHiddenInGame(true);
}

void ABossCharacterRat::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	CurrentHP = MaxHP;

	UE_LOG(LogTemp, Warning, TEXT("StartWithPhase2: %s"),
		bStartWithPhase2 ? TEXT("true") : TEXT("false")
	);

	bCanDash = false;

	GetWorldTimerManager().SetTimer(
		DashCooldownTimerHandle,
		this,
		&ABossCharacterRat::ResetDashCooldown,
		InitialDashCooldown,
		false
	);

	if (bStartWithPhase2)
	{
		CurrentHP = MaxHP * 0.49f;

		UE_LOG(LogTemp, Warning, TEXT("Force Phase2 HP: %f / %f"), CurrentHP, MaxHP);

		CheckPhase2();
	}

	GetWorldTimerManager().SetTimer(TargetUpdateTimerHandle, this, &ABossCharacterRat::UpdateTarget, TargetUpdateInterval, true);
	GetWorldTimerManager().SetTimer(MoveTimerHandle, this, &ABossCharacterRat::MoveToTarget, MoveUpdateInterval, true);
	GetWorldTimerManager().SetTimer(AttackDecisionTimerHandle, this, &ABossCharacterRat::UpdateBossAI, AttackDecisionInterval, true);
}

void ABossCharacterRat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossCharacterRat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABossCharacterRat::UpdateTarget()
{
	if (!HasAuthority())
	{
		return;
	}

	AActor* ClosestPlayer = nullptr;
	float ClosestDistanceSq = TNumericLimits<float>::Max();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (!PlayerController)
		{
			continue;
		}

		APawn* PlayerPawn = PlayerController->GetPawn();
		if (!PlayerPawn)
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(GetActorLocation(), PlayerPawn->GetActorLocation());
		if (DistanceSq < ClosestDistanceSq)
		{
			ClosestDistanceSq = DistanceSq;
			ClosestPlayer = PlayerPawn;
		}
	}

	TargetPlayer = ClosestPlayer;
}

void ABossCharacterRat::MoveToTarget()
{
	if (!HasAuthority() || bIsAttacking)
	{
		return;
	}

	if (!TargetPlayer)
	{
		UpdateTarget();
		return;
	}

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToActor(TargetPlayer, ChaseAcceptanceRadius);
	}
}

void ABossCharacterRat::UpdateBossAI()
{
	if (!HasAuthority() || bIsAttacking)
	{
		return;
	}

	if (bCanDash)
	{
		StartDashAttack();
		return;
	}

	if (!TargetPlayer)
	{
		UpdateTarget();
		return;
	}

	const float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

	if (Distance <= ScratchRange && bCanScratch)
	{
		StartScratchAttack();
		return;
	}

	MoveToTarget();
}

void ABossCharacterRat::StartScratchAttack()
{
	if (!HasAuthority() || !bCanScratch)
	{
		return;
	}

	bIsAttacking = true;
	bCanScratch = false;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	UE_LOG(LogTemp, Warning, TEXT("Boss Rat: Scratch Warning"));

	Multicast_SetScratchWarningVisible(true);

	GetWorldTimerManager().SetTimer(
		ScratchDamageTimerHandle,
		this,
		&ABossCharacterRat::ApplyScratchDamage,
		ScratchWarningTime,
		false
	);
}

void ABossCharacterRat::ApplyScratchDamage()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Boss Rat: Scratch Damage"));

	ScratchHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ScratchHitBox->UpdateOverlaps();

	TArray<AActor*> OverlappingActors;
	ScratchHitBox->GetOverlappingActors(OverlappingActors, APawn::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor || Actor == this)
		{
			continue;
		}

		UGameplayStatics::ApplyDamage(Actor, ScratchDamage, GetController(), this, nullptr);
	}

	ScratchHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Multicast_SetScratchWarningVisible(false);

	GetWorldTimerManager().SetTimer(
		AttackEndTimerHandle,
		this,
		&ABossCharacterRat::FinishScratchAttack,
		ScratchAfterDelay,
		false
	);
}

void ABossCharacterRat::FinishScratchAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsAttacking = false;

	GetWorldTimerManager().SetTimer(
		ScratchCooldownTimerHandle,
		this,
		&ABossCharacterRat::ResetScratchCooldown,
		ScratchCooldown,
		false
	);
}

void ABossCharacterRat::ResetScratchCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	bCanScratch = true;
}

void ABossCharacterRat::UpdateDashTarget()
{
	if (!HasAuthority())
	{
		return;
	}

	AActor* FarthestPlayer = nullptr;
	float FarthestDistanceSq = 0.0f;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (!PlayerController)
		{
			continue;
		}

		APawn* PlayerPawn = PlayerController->GetPawn();
		if (!PlayerPawn)
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(GetActorLocation(), PlayerPawn->GetActorLocation());
		if (DistanceSq > FarthestDistanceSq)
		{
			FarthestDistanceSq = DistanceSq;
			FarthestPlayer = PlayerPawn;
		}
	}

	DashTarget = FarthestPlayer;
}

void ABossCharacterRat::StartDashAttack()
{
	if (!HasAuthority() || !bCanDash)
	{
		return;
	}

	UpdateDashTarget();

	if (!DashTarget)
	{
		return;
	}

	bIsAttacking = true;
	bCanDash = false;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	const FVector ToTarget = DashTarget->GetActorLocation() - GetActorLocation();
	DashDirection = FVector(ToTarget.X, ToTarget.Y, 0.0f).GetSafeNormal();

	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}

	SetActorRotation(DashDirection.Rotation());

	if (DashWarningMesh)
	{
		DashWarningMesh->SetRelativeLocation(FVector(DashDistance * 0.5f, 0.0f, 5.0f));
		DashWarningMesh->SetRelativeScale3D(FVector(DashDistance / 100.0f, DashHitWidth / 100.0f, 0.05f));
	}

	UE_LOG(LogTemp, Warning, TEXT("Boss Rat: Dash Warning"));

	Multicast_SetDashWarningVisible(true);

	GetWorldTimerManager().SetTimer(
		DashExecuteTimerHandle,
		this,
		&ABossCharacterRat::ExecuteDashAttack,
		DashWarningTime,
		false
	);
}

void ABossCharacterRat::ExecuteDashAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Boss Rat: Dash Execute"));

	Multicast_SetDashWarningVisible(false);

	ApplyDashDamage();

	const float DashSpeed = DashDistance / DashDuration;
	LaunchCharacter(DashDirection * DashSpeed, true, false);

	GetWorldTimerManager().SetTimer(
		AttackEndTimerHandle,
		this,
		&ABossCharacterRat::FinishDashAttack,
		DashDuration,
		false
	);
}

void ABossCharacterRat::ApplyDashDamage()
{
	if (!HasAuthority())
	{
		return;
	}

	const FVector Center = GetActorLocation() + DashDirection * (DashDistance * 0.5f);
	const FQuat Rotation = DashDirection.Rotation().Quaternion();
	const FVector BoxExtent = FVector(DashDistance * 0.5f, DashHitWidth, 100.0f);

	TArray<FOverlapResult> Overlaps;
	const FCollisionShape DashShape = FCollisionShape::MakeBox(BoxExtent);

	const bool bHit = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Center,
		Rotation,
		ECC_Pawn,
		DashShape
	);

	if (!bHit)
	{
		return;
	}

	TSet<AActor*> DamagedActors;

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* HitActor = Result.GetActor();

		if (!HitActor || HitActor == this || DamagedActors.Contains(HitActor))
		{
			continue;
		}

		DamagedActors.Add(HitActor);
		UGameplayStatics::ApplyDamage(HitActor, DashDamage, GetController(), this, nullptr);
	}
}

void ABossCharacterRat::FinishDashAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	LaunchCharacter(FVector::ZeroVector, true, true);

	bIsAttacking = false;

	GetWorldTimerManager().SetTimer(
		AttackEndTimerHandle,
		this,
		&ABossCharacterRat::StartDashCooldown,
		DashAfterDelay,
		false
	);
}

void ABossCharacterRat::StartDashCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorldTimerManager().SetTimer(
		DashCooldownTimerHandle,
		this,
		&ABossCharacterRat::ResetDashCooldown,
		DashCooldown,
		false
	);
}

void ABossCharacterRat::ResetDashCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	bCanDash = true;
}

void ABossCharacterRat::Multicast_SetScratchWarningVisible_Implementation(bool bVisible)
{
	if (ScratchWarningMesh)
	{
		ScratchWarningMesh->SetHiddenInGame(!bVisible);
	}
}

void ABossCharacterRat::Multicast_SetDashWarningVisible_Implementation(bool bVisible)
{
	if (DashWarningMesh)
	{
		DashWarningMesh->SetHiddenInGame(!bVisible);
	}
}

void ABossCharacterRat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABossCharacterRat, CurrentHP);
	DOREPLIFETIME(ABossCharacterRat, bIsAttacking);
	DOREPLIFETIME(ABossCharacterRat, bCanScratch);
	DOREPLIFETIME(ABossCharacterRat, bCanDash);
	DOREPLIFETIME(ABossCharacterRat, bIsPhase2);
}

float ABossCharacterRat::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	if (!HasAuthority())
	{
		return 0.0f;
	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0.0f, MaxHP);

	UE_LOG(LogTemp, Warning, TEXT("Boss HP: %f / %f"), CurrentHP, MaxHP);

	CheckPhase2();

	return ActualDamage;
}

void ABossCharacterRat::CheckPhase2()
{
	if (!HasAuthority() || bIsPhase2)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Force Phase2 HP: %f / %f"), CurrentHP, MaxHP);

	if (CurrentHP <= MaxHP * Phase2Threshold)
	{
		EnterPhase2();
	}
}

void ABossCharacterRat::EnterPhase2()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsPhase2 = true;

	DamageAuraSphere->SetSphereRadius(AuraRadius);

	OnRep_IsPhase2();
	ForceNetUpdate();

	GetWorldTimerManager().SetTimer(
		AuraDamageTimerHandle,
		this,
		&ABossCharacterRat::ApplyAuraDamage,
		AuraDamageInterval,
		true
	);

	UE_LOG(LogTemp, Warning, TEXT("Boss Rat: Phase 2 Aura Started"));
}

void ABossCharacterRat::ApplyAuraDamage()
{
	if (!HasAuthority())
	{
		return;
	}

	DamageAuraSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageAuraSphere->UpdateOverlaps();

	TArray<AActor*> OverlappingActors;
	DamageAuraSphere->GetOverlappingActors(OverlappingActors, APawn::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor || Actor == this)
		{
			continue;
		}

		UGameplayStatics::ApplyDamage(
			Actor,
			AuraDamage,
			GetController(),
			this,
			nullptr
		);
	}

	DamageAuraSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossCharacterRat::OnRep_IsPhase2()
{
	if (DamageAuraWarningMesh)
	{
		DamageAuraWarningMesh->SetHiddenInGame(false);
		DamageAuraWarningMesh->SetVisibility(true, true);

		DamageAuraWarningMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
		DamageAuraWarningMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		DamageAuraWarningMesh->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));

		UE_LOG(LogTemp, Warning, TEXT("Aura Mesh forced visible"));
		UE_LOG(LogTemp, Warning, TEXT("Aura Mesh Asset: %s"),
			DamageAuraWarningMesh->GetStaticMesh() ? *DamageAuraWarningMesh->GetStaticMesh()->GetName() : TEXT("None")
		);
	}
}

void ABossCharacterRat::Multicast_SetAuraVisible_Implementation(bool bVisible)
{
	if (DamageAuraWarningMesh)
	{
		DamageAuraWarningMesh->SetHiddenInGame(!bVisible);
	}
}