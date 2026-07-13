// Fill out your copyright notice in the Description page of Project Settings.

#include "BossCharacterZombie.h"
#include "BossZombieProjectile.h"

#include "AIController.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABossCharacterZombie::ABossCharacterZombie()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	GetCharacterMovement()->MaxWalkSpeed = 220.0f;

	BasicAttackWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BasicAttackWarningMesh"));
	BasicAttackWarningMesh->SetupAttachment(RootComponent);
	BasicAttackWarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BasicAttackWarningMesh->SetHiddenInGame(true);
	BasicAttackWarningMesh->SetVisibility(false);
	BasicAttackWarningMesh->SetRelativeLocation(FVector(350.0f, 0.0f, -80.0f));
	BasicAttackWarningMesh->SetRelativeScale3D(FVector(5.0f, 5.0f, 0.05f));

	RangedWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RangedWarningMesh"));
	RangedWarningMesh->SetupAttachment(RootComponent);
	RangedWarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RangedWarningMesh->SetHiddenInGame(true);
	RangedWarningMesh->SetVisibility(false);
	RangedWarningMesh->SetRelativeLocation(FVector(180.0f, 0.0f, 80.0f));
	RangedWarningMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));

	DashWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DashWarningMesh"));
	DashWarningMesh->SetupAttachment(RootComponent);
	DashWarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DashWarningMesh->SetHiddenInGame(true);
	DashWarningMesh->SetVisibility(false);
	DashWarningMesh->SetRelativeLocation(FVector(800.0f, 0.0f, -80.0f));
	DashWarningMesh->SetRelativeScale3D(FVector(16.0f, 2.5f, 0.05f));

	Phase2AuraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Phase2AuraMesh"));
	Phase2AuraMesh->SetupAttachment(RootComponent);
	Phase2AuraMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Phase2AuraMesh->SetHiddenInGame(true);
	Phase2AuraMesh->SetVisibility(false);
	Phase2AuraMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -80.0f));
	Phase2AuraMesh->SetRelativeScale3D(FVector(8.0f, 8.0f, 0.05f));

	InstantKillWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InstantKillWarningMesh"));
	InstantKillWarningMesh->SetupAttachment(RootComponent);
	InstantKillWarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InstantKillWarningMesh->SetHiddenInGame(true);
	InstantKillWarningMesh->SetVisibility(false);
	InstantKillWarningMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -75.0f));
	InstantKillWarningMesh->SetRelativeScale3D(FVector(12.0f, 12.0f, 0.05f));

	LaserWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserWarningMesh"));
	LaserWarningMesh->SetupAttachment(RootComponent);
	LaserWarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserWarningMesh->SetHiddenInGame(true);
	LaserWarningMesh->SetVisibility(false);
	LaserWarningMesh->SetRelativeLocation(FVector(900.0f, 0.0f, -70.0f));
	LaserWarningMesh->SetRelativeScale3D(FVector(18.0f, 6.0f, 0.05f));

	LaserPivotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LaserPivotComponent"));
	LaserPivotComponent->SetupAttachment(RootComponent);
	LaserPivotComponent->SetRelativeLocation(FVector::ZeroVector);
	LaserPivotComponent->SetRelativeRotation(FRotator::ZeroRotator);

	LaserBeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserBeamMesh"));
	LaserBeamMesh->SetupAttachment(LaserPivotComponent);
	LaserBeamMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserBeamMesh->SetHiddenInGame(true);
	LaserBeamMesh->SetVisibility(false);
	LaserBeamMesh->SetRelativeLocation(FVector(LaserRange * 0.5f, 0.0f, -60.0f));
	LaserBeamMesh->SetRelativeRotation(FRotator::ZeroRotator);
	LaserBeamMesh->SetRelativeScale3D(FVector(LaserRange / 100.0f, 0.45f, 0.05f));
}

void ABossCharacterZombie::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	CurrentHP = MaxHP;

	if (bStartWithPhase2)
	{
		CurrentHP = MaxHP * Phase2Threshold - 1.0f;
		CheckPhase2();
	}

	bCanDashAttack = false;

	GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &ABossCharacterZombie::ResetDashCooldown, InitialDashCooldown, false);
	GetWorldTimerManager().SetTimer(TargetUpdateTimerHandle, this, &ABossCharacterZombie::UpdateTarget, TargetUpdateInterval, true);
	GetWorldTimerManager().SetTimer(AttackDecisionTimerHandle, this, &ABossCharacterZombie::UpdateBossAI, AttackDecisionInterval, true);
}

void ABossCharacterZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && bIsDashing)
	{
		UpdateDash(DeltaTime);
	}

	if (InstantKillWarningMesh && !InstantKillWarningMesh->bHiddenInGame && InstantKillWarningMID)
	{
		InstantKillWarningElapsed += DeltaTime;

		const float Alpha = FMath::Clamp(InstantKillWarningElapsed / InstantKillWarningTime, 0.0f, 1.0f);
		const float WarningAlpha = FMath::Lerp(0.15f, 0.85f, Alpha);

		InstantKillWarningMID->SetScalarParameterValue(TEXT("WarningAlpha"), WarningAlpha);
	}

	if (HasAuthority() && bIsLaserAttacking)
	{
		UpdateLaserAttack(DeltaTime);
	}
}

void ABossCharacterZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABossCharacterZombie::UpdateTarget()
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

void ABossCharacterZombie::UpdateBossAI()
{
	if (!HasAuthority())
	{
		return;
	}

	if (bIsPhase2PatternRunning || bIsAttacking || bIsDashing)
	{
		return;
	}

	if (!TargetPlayer)
	{
		UpdateTarget();
		return;
	}

	const float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

	if (bCanBasicAttack && Distance <= BasicAttackRadius)
	{
		StartBasicAttack();
		return;
	}

	if (bCanDashAttack && Distance >= DashMinRange && Distance <= DashMaxRange)
	{
		StartDashAttack();
		return;
	}

	if (bCanRangedAttack && Distance >= RangedAttackMinRange && Distance <= RangedAttackMaxRange)
	{
		StartRangedAttack();
		return;
	}

	MoveToTarget();
}

void ABossCharacterZombie::MoveToTarget()
{
	if (!HasAuthority() || bIsDashing || !TargetPlayer)
	{
		return;
	}

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	AIController->MoveToActor(TargetPlayer, ChaseAcceptanceRadius);
}

void ABossCharacterZombie::StartBasicAttack()
{
	if (!HasAuthority() || !bCanBasicAttack)
	{
		return;
	}

	bIsAttacking = true;
	bCanBasicAttack = false;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	if (TargetPlayer)
	{
		FVector Direction = TargetPlayer->GetActorLocation() - GetActorLocation();
		Direction.Z = 0.0f;

		if (!Direction.IsNearlyZero())
		{
			SetActorRotation(Direction.Rotation());
		}
	}

	Multicast_SetBasicAttackWarningVisible(true);

	GetWorldTimerManager().SetTimer(BasicAttackDamageTimerHandle, this, &ABossCharacterZombie::ApplyBasicAttackDamage, BasicAttackWarningTime, false);
}

void ABossCharacterZombie::ApplyBasicAttackDamage()
{
	if (!HasAuthority())
	{
		return;
	}

	const FVector BossLocation = GetActorLocation();

	FVector BossForward = GetActorForwardVector();
	BossForward.Z = 0.0f;
	BossForward = BossForward.GetSafeNormal();

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

		FVector ToPlayer = PlayerPawn->GetActorLocation() - BossLocation;
		ToPlayer.Z = 0.0f;

		const float Distance = ToPlayer.Size();
		if (Distance > BasicAttackRadius || Distance <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		const FVector DirectionToPlayer = ToPlayer.GetSafeNormal();
		const float Dot = FMath::Clamp(FVector::DotProduct(BossForward, DirectionToPlayer), -1.0f, 1.0f);
		const float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

		if (Angle <= BasicAttackHalfAngle)
		{
			UGameplayStatics::ApplyDamage(PlayerPawn, BasicAttackDamage, GetController(), this, nullptr);
		}
	}

	Multicast_SetBasicAttackWarningVisible(false);

	GetWorldTimerManager().SetTimer(BasicAttackAfterDelayTimerHandle, this, &ABossCharacterZombie::FinishBasicAttack, BasicAttackAfterDelay, false);
}

void ABossCharacterZombie::FinishBasicAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsAttacking = false;

	GetWorldTimerManager().SetTimer(BasicAttackCooldownTimerHandle, this, &ABossCharacterZombie::ResetBasicAttackCooldown, BasicAttackCooldown, false);
}

void ABossCharacterZombie::ResetBasicAttackCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	bCanBasicAttack = true;
}

void ABossCharacterZombie::StartRangedAttack()
{
	if (!HasAuthority() || !bCanRangedAttack || !TargetPlayer)
	{
		return;
	}

	bIsAttacking = true;
	bCanRangedAttack = false;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	FVector Direction = TargetPlayer->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	if (!Direction.IsNearlyZero())
	{
		SetActorRotation(Direction.Rotation());
	}

	Multicast_SetRangedWarningVisible(true);

	GetWorldTimerManager().SetTimer(RangedAttackWarningTimerHandle, this, &ABossCharacterZombie::FireRangedProjectile, RangedAttackWarningTime, false);
}

void ABossCharacterZombie::FireRangedProjectile()
{
	if (!HasAuthority())
	{
		return;
	}

	Multicast_SetRangedWarningVisible(false);

	if (!ProjectileClass || !TargetPlayer)
	{
		FinishRangedAttack();
		return;
	}

	FVector Direction = TargetPlayer->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		FinishRangedAttack();
		return;
	}

	Direction = Direction.GetSafeNormal();

	const FVector SpawnLocation = GetActorLocation() + Direction * ProjectileSpawnDistance + FVector(0.0f, 0.0f, ProjectileSpawnHeight);
	const FRotator SpawnRotation = Direction.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	ABossZombieProjectile* Projectile = GetWorld()->SpawnActor<ABossZombieProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (Projectile)
	{
		Projectile->SetDamage(RangedAttackDamage);
	}

	GetWorldTimerManager().SetTimer(RangedAttackAfterDelayTimerHandle, this, &ABossCharacterZombie::FinishRangedAttack, RangedAttackAfterDelay, false);
}

void ABossCharacterZombie::FinishRangedAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsAttacking = false;

	GetWorldTimerManager().SetTimer(RangedAttackCooldownTimerHandle, this, &ABossCharacterZombie::ResetRangedAttackCooldown, RangedAttackCooldown, false);
}

void ABossCharacterZombie::ResetRangedAttackCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	bCanRangedAttack = true;
}

void ABossCharacterZombie::StartDashAttack()
{
	if (!HasAuthority() || !bCanDashAttack || !TargetPlayer)
	{
		return;
	}

	bIsAttacking = true;
	bCanDashAttack = false;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	FVector Direction = TargetPlayer->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		FinishDashAttack();
		return;
	}

	Direction = Direction.GetSafeNormal();
	SetActorRotation(Direction.Rotation());

	DashStartLocation = GetActorLocation();
	DashEndLocation = DashStartLocation + Direction * DashDistance;
	DashElapsedTime = 0.0f;

	Multicast_SetDashWarningVisible(true);

	GetWorldTimerManager().SetTimer(DashWarningTimerHandle, this, &ABossCharacterZombie::ExecuteDashAttack, DashWarningTime, false);
}

void ABossCharacterZombie::ExecuteDashAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	Multicast_SetDashWarningVisible(false);

	bIsDashing = true;
}

void ABossCharacterZombie::UpdateDash(float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	DashElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(DashElapsedTime / DashDuration, 0.0f, 1.0f);
	const FVector NewLocation = FMath::Lerp(DashStartLocation, DashEndLocation, Alpha);

	SetActorLocation(NewLocation, true);

	if (Alpha >= 1.0f)
	{
		ApplyDashDamage();

		bIsDashing = false;

		GetWorldTimerManager().SetTimer(DashAfterDelayTimerHandle, this, &ABossCharacterZombie::FinishDashAttack, DashAfterDelay, false);
	}
}

void ABossCharacterZombie::ApplyDashDamage()
{
	if (!HasAuthority())
	{
		return;
	}

	const FVector DashLine = DashEndLocation - DashStartLocation;
	const float DashLineLengthSq = DashLine.SizeSquared();

	if (DashLineLengthSq <= KINDA_SMALL_NUMBER)
	{
		return;
	}

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

		const FVector PlayerLocation = PlayerPawn->GetActorLocation();
		const FVector ToPlayer = PlayerLocation - DashStartLocation;

		const float T = FMath::Clamp(FVector::DotProduct(ToPlayer, DashLine) / DashLineLengthSq, 0.0f, 1.0f);
		const FVector ClosestPoint = DashStartLocation + DashLine * T;
		const float DistanceToDashPath = FVector::Dist2D(PlayerLocation, ClosestPoint);

		if (DistanceToDashPath <= DashHitWidth)
		{
			UGameplayStatics::ApplyDamage(PlayerPawn, DashDamage, GetController(), this, nullptr);
		}
	}
}

void ABossCharacterZombie::FinishDashAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsAttacking = false;

	GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &ABossCharacterZombie::ResetDashCooldown, DashCooldown, false);
}

void ABossCharacterZombie::ResetDashCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	bCanDashAttack = true;
}

void ABossCharacterZombie::CheckPhase2()
{
	if (!HasAuthority() || bIsPhase2)
	{
		return;
	}

	if (CurrentHP <= MaxHP * Phase2Threshold)
	{
		EnterPhase2();
	}
}

void ABossCharacterZombie::EnterPhase2()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsPhase2 = true;
	bIsPhase2PatternRunning = false;

	Multicast_SetPhase2AuraVisible(true);

	UE_LOG(LogTemp, Warning, TEXT("Zombie Boss: Phase 2 Started"));

	ScheduleInstantKillAttack(FirstInstantKillDelay);
}

void ABossCharacterZombie::ScheduleInstantKillAttack(float Delay)
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorldTimerManager().SetTimer(PhaseInstantKillScheduleTimerHandle, this, &ABossCharacterZombie::StartInstantKillAttack, Delay, false);
}

void ABossCharacterZombie::StartInstantKillAttack()
{
	if (!HasAuthority() || !bIsPhase2)
	{
		return;
	}

	bIsPhase2PatternRunning = true;
	bIsAttacking = true;
	InstantKillWarningElapsed = 0.0f;

	if (InstantKillWarningMesh)
	{
		InstantKillWarningMID = InstantKillWarningMesh->CreateAndSetMaterialInstanceDynamic(0);

		if (InstantKillWarningMID)
		{
			InstantKillWarningMID->SetScalarParameterValue(TEXT("WarningAlpha"), 0.15f);
		}
	}

	Multicast_SetInstantKillWarningVisible(true);

	GetWorldTimerManager().SetTimer(InstantKillTimerHandle, this, &ABossCharacterZombie::ApplyInstantKillAttack, InstantKillWarningTime, false);
}

void ABossCharacterZombie::ApplyInstantKillAttack()
{
	if (!HasAuthority())
	{
		return;
	}

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

		const float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

		if (Distance <= InstantKillRadius)
		{
			UGameplayStatics::ApplyDamage(PlayerPawn, 99999.0f, GetController(), this, nullptr);
		}
	}

	Multicast_SetInstantKillWarningVisible(false);

	bIsAttacking = false;
	bIsPhase2PatternRunning = false;

	ScheduleLaserAttack(LaserDelayAfterInstantKill);
}

void ABossCharacterZombie::ScheduleLaserAttack(float Delay)
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorldTimerManager().SetTimer(PhaseLaserScheduleTimerHandle, this, &ABossCharacterZombie::StartLaserWarning, Delay, false);
}

void ABossCharacterZombie::StartLaserWarning()
{
	if (!HasAuthority() || !bIsPhase2)
	{
		return;
	}

	bIsPhase2PatternRunning = true;
	bIsAttacking = true;

	Multicast_SetLaserWarningVisible(true);

	GetWorldTimerManager().SetTimer(LaserWarningTimerHandle, this, &ABossCharacterZombie::StartLaserAttack, LaserWarningTime, false);
}

void ABossCharacterZombie::StartLaserAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	Multicast_SetLaserWarningVisible(false);
	Multicast_SetLaserBeamVisible(true);

	bIsLaserAttacking = true;
	LaserElapsedTime = 0.0f;
	LaserCurrentAngle = -LaserSweepHalfAngle;

	if (LaserPivotComponent)
	{
		LaserPivotComponent->SetRelativeRotation(FRotator(0.0f, LaserCurrentAngle, 0.0f));
	}

	GetWorldTimerManager().SetTimer(LaserEndTimerHandle, this, &ABossCharacterZombie::FinishLaserAttack, LaserDuration, false);
}

void ABossCharacterZombie::UpdateLaserAttack(float DeltaTime)
{
	if (!HasAuthority())
	{
		return;
	}

	LaserElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(LaserElapsedTime / LaserDuration, 0.0f, 1.0f);

	LaserCurrentAngle = FMath::Lerp(-LaserSweepHalfAngle, LaserSweepHalfAngle, Alpha);

	const FRotator LaserRotation = FRotator(0.0f, LaserCurrentAngle, 0.0f);
	const FVector LaserDirection = LaserRotation.RotateVector(GetActorForwardVector());

	if (LaserPivotComponent)
	{
		LaserPivotComponent->SetRelativeRotation(FRotator(0.0f, LaserCurrentAngle, 0.0f));
	}

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

		FVector ToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
		ToPlayer.Z = 0.0f;

		const float DistanceForward = FVector::DotProduct(ToPlayer, LaserDirection);

		if (DistanceForward < 0.0f || DistanceForward > LaserRange)
		{
			continue;
		}

		const FVector ClosestPoint = GetActorLocation() + LaserDirection * DistanceForward;
		const float SideDistance = FVector::Dist2D(PlayerPawn->GetActorLocation(), ClosestPoint);

		if (SideDistance <= LaserHitWidth)
		{
			UGameplayStatics::ApplyDamage(PlayerPawn, LaserDamagePerSecond * DeltaTime, GetController(), this, nullptr);
		}
	}
}

void ABossCharacterZombie::FinishLaserAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsLaserAttacking = false;

	Multicast_SetLaserBeamVisible(false);

	GetWorldTimerManager().SetTimer(LaserAfterDelayTimerHandle, this, &ABossCharacterZombie::FinishLaserAfterDelay, LaserAfterDelay, false);
}

void ABossCharacterZombie::FinishLaserAfterDelay()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsAttacking = true;
	bIsPhase2PatternRunning = true;

	GetWorldTimerManager().SetTimer(LaserRecoveryIdleTimerHandle, this, &ABossCharacterZombie::FinishLaserRecoveryIdle, LaserRecoveryIdleTime, false);
}

void ABossCharacterZombie::FinishLaserRecoveryIdle()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsAttacking = false;
	bIsPhase2PatternRunning = false;

	ScheduleInstantKillAttack(InstantKillDelayAfterLaser);
}

void ABossCharacterZombie::Multicast_SetBasicAttackWarningVisible_Implementation(bool bVisible)
{
	if (BasicAttackWarningMesh)
	{
		BasicAttackWarningMesh->SetHiddenInGame(!bVisible);
		BasicAttackWarningMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterZombie::Multicast_SetRangedWarningVisible_Implementation(bool bVisible)
{
	if (RangedWarningMesh)
	{
		RangedWarningMesh->SetHiddenInGame(!bVisible);
		RangedWarningMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterZombie::Multicast_SetDashWarningVisible_Implementation(bool bVisible)
{
	if (DashWarningMesh)
	{
		DashWarningMesh->SetHiddenInGame(!bVisible);
		DashWarningMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterZombie::Multicast_SetPhase2AuraVisible_Implementation(bool bVisible)
{
	if (Phase2AuraMesh)
	{
		Phase2AuraMesh->SetHiddenInGame(!bVisible);
		Phase2AuraMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterZombie::Multicast_SetInstantKillWarningVisible_Implementation(bool bVisible)
{
	if (InstantKillWarningMesh)
	{
		InstantKillWarningMesh->SetHiddenInGame(!bVisible);
		InstantKillWarningMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterZombie::Multicast_SetLaserWarningVisible_Implementation(bool bVisible)
{
	if (LaserWarningMesh)
	{
		LaserWarningMesh->SetHiddenInGame(!bVisible);
		LaserWarningMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterZombie::Multicast_SetLaserBeamVisible_Implementation(bool bVisible)
{
	if (LaserBeamMesh)
	{
		LaserBeamMesh->SetHiddenInGame(!bVisible);
		LaserBeamMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterZombie::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABossCharacterZombie, CurrentHP);
	DOREPLIFETIME(ABossCharacterZombie, bIsAttacking);
	DOREPLIFETIME(ABossCharacterZombie, bCanBasicAttack);
	DOREPLIFETIME(ABossCharacterZombie, bCanRangedAttack);
	DOREPLIFETIME(ABossCharacterZombie, bCanDashAttack);
	DOREPLIFETIME(ABossCharacterZombie, bIsDashing);
	DOREPLIFETIME(ABossCharacterZombie, bIsPhase2);
}