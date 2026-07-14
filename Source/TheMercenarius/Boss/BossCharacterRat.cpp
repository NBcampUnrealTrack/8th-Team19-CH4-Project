#include "BossCharacterRat.h"

#include "AIController.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	DashWarningPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DashWarningPivot"));
	DashWarningPivot->SetupAttachment(RootComponent);
	DashWarningPivot->SetRelativeLocation(FVector(0.0f, 0.0f, -130.0f));
	DashWarningPivot->SetRelativeRotation(FRotator::ZeroRotator);

	DashWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DashWarningMesh"));
	DashWarningMesh->SetupAttachment(DashWarningPivot);
	DashWarningMesh->SetRelativeLocation(FVector(DashDistance * 0.5f, 0.0f, 0.0f));
	DashWarningMesh->SetRelativeRotation(FRotator::ZeroRotator);
	DashWarningMesh->SetRelativeScale3D(FVector(DashDistance / 100.0f, DashHitWidth / 100.0f, 0.05f));
	DashWarningMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DashWarningMesh->SetHiddenInGame(true);

	DamageAuraSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageAuraSphere"));
	DamageAuraSphere->SetupAttachment(RootComponent);
	DamageAuraSphere->SetSphereRadius(AuraRadius);
	DamageAuraSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DamageAuraWarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DamageAuraWarningMesh"));
	DamageAuraWarningMesh->SetupAttachment(RootComponent);
	DamageAuraWarningMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
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
	bCanDash = false;

	GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &ABossCharacterRat::ResetDashCooldown, InitialDashCooldown, false);

	if (bStartWithPhase2)
	{
		CurrentHP = MaxHP * Phase2Threshold - 1.0f;
		CheckPhase2();
	}

	GetWorldTimerManager().SetTimer(TargetUpdateTimerHandle, this, &ABossCharacterRat::UpdateTarget, TargetUpdateInterval, true);
	GetWorldTimerManager().SetTimer(MoveTimerHandle, this, &ABossCharacterRat::MoveToTarget, MoveUpdateInterval, true);
	GetWorldTimerManager().SetTimer(AttackDecisionTimerHandle, this, &ABossCharacterRat::UpdateBossAI, AttackDecisionInterval, true);
}

void ABossCharacterRat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && bIsDashing)
	{
		UpdateDash(DeltaTime);
	}
}

void ABossCharacterRat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
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
	CheckPhase2();

	return ActualDamage;
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
	if (!HasAuthority() || bIsAttacking || bIsDashing)
	{
		return;
	}

	if (!TargetPlayer)
	{
		UpdateTarget();
		return;
	}

	const float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

	if (bCanScratch && Distance <= ScratchRange)
	{
		if (IsTargetInScratchAngle())
		{
			StartScratchAttack();
			return;
		}

		RotateToTarget();
		return;
	}

	if (bCanDash)
	{
		StartDashAttack();
		return;
	}

	MoveToTarget();
}

void ABossCharacterRat::RotateToTarget()
{
	if (!TargetPlayer)
	{
		return;
	}

	FVector Direction = TargetPlayer->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		return;
	}

	SetActorRotation(Direction.Rotation());
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

	RotateToTarget();

	Multicast_SetScratchWarningVisible(true);

	GetWorldTimerManager().SetTimer(ScratchDamageTimerHandle, this, &ABossCharacterRat::ApplyScratchDamage, ScratchWarningTime, false);
}

void ABossCharacterRat::ApplyScratchDamage()
{
	if (!HasAuthority())
	{
		return;
	}

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

	GetWorldTimerManager().SetTimer(AttackEndTimerHandle, this, &ABossCharacterRat::FinishScratchAttack, ScratchAfterDelay, false);
}

void ABossCharacterRat::FinishScratchAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsAttacking = false;

	GetWorldTimerManager().SetTimer(ScratchCooldownTimerHandle, this, &ABossCharacterRat::ResetScratchCooldown, ScratchCooldown, false);
}

void ABossCharacterRat::ResetScratchCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	bCanScratch = true;
}

bool ABossCharacterRat::IsTargetInScratchAngle() const
{
	if (!TargetPlayer)
	{
		return false;
	}

	FVector ToTarget = TargetPlayer->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.0f;

	if (ToTarget.IsNearlyZero())
	{
		return false;
	}

	const FVector DirectionToTarget = ToTarget.GetSafeNormal();

	FVector Forward = GetActorForwardVector();
	Forward.Z = 0.0f;
	Forward = Forward.GetSafeNormal();

	const float Dot = FMath::Clamp(FVector::DotProduct(Forward, DirectionToTarget), -1.0f, 1.0f);
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

	return Angle <= ScratchStartHalfAngle;
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

	if (DashWarningPivot)
	{
		DashWarningPivot->SetRelativeLocation(FVector(0.0f, 0.0f, -220.0f));
		DashWarningPivot->SetRelativeRotation(FRotator::ZeroRotator);
	}

	if (DashWarningMesh)
	{
		const FVector BossLocation = GetActorLocation();
		const FVector WarningLocation =
			BossLocation + DashDirection * (DashWarningForwardOffset + DashDistance * 0.5f);

		DashWarningMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		DashWarningMesh->SetWorldLocation(FVector(
			WarningLocation.X,
			WarningLocation.Y,
			5.0f
		));

		DashWarningMesh->SetWorldRotation(DashDirection.Rotation());
		DashWarningMesh->SetWorldScale3D(FVector(DashDistance / 100.0f, DashHitWidth / 100.0f, 0.05f));
	}

	Multicast_SetDashWarningVisible(true);

	GetWorldTimerManager().SetTimer(DashExecuteTimerHandle, this, &ABossCharacterRat::ExecuteDashAttack, DashWarningTime, false);
}

void ABossCharacterRat::ExecuteDashAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	Multicast_SetDashWarningVisible(false);

	ApplyDashDamage();

	DashStartLocation = GetActorLocation();
	DashEndLocation = DashStartLocation + DashDirection * DashDistance;
	DashElapsedTime = 0.0f;
	bIsDashing = true;
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

	bIsAttacking = false;

	GetWorldTimerManager().SetTimer(AttackEndTimerHandle, this, &ABossCharacterRat::StartDashCooldown, DashAfterDelay, false);
}

void ABossCharacterRat::StartDashCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &ABossCharacterRat::ResetDashCooldown, DashCooldown, false);
}

void ABossCharacterRat::ResetDashCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	bCanDash = true;
}

void ABossCharacterRat::UpdateDash(float DeltaTime)
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
		bIsDashing = false;

		GetWorldTimerManager().SetTimer(
			AttackEndTimerHandle,
			this,
			&ABossCharacterRat::FinishDashAttack,
			DashAfterDelay,
			false
		);
	}
}

void ABossCharacterRat::CheckPhase2()
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

void ABossCharacterRat::EnterPhase2()
{
	if (!HasAuthority())
	{
		return;
	}

	bIsPhase2 = true;

	if (DamageAuraSphere)
	{
		DamageAuraSphere->SetSphereRadius(AuraRadius);
	}

	OnRep_IsPhase2();
	ForceNetUpdate();

	GetWorldTimerManager().SetTimer(AuraDamageTimerHandle, this, &ABossCharacterRat::ApplyAuraDamage, AuraDamageInterval, true);

	UE_LOG(LogTemp, Warning, TEXT("Boss Rat: Phase 2 Aura Started"));
}

void ABossCharacterRat::ApplyAuraDamage()
{
	if (!HasAuthority() || !DamageAuraSphere)
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

		UGameplayStatics::ApplyDamage(Actor, AuraDamage, GetController(), this, nullptr);
	}

	DamageAuraSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossCharacterRat::OnRep_IsPhase2()
{
	Multicast_SetAuraVisible(bIsPhase2);
}

void UpdateDash(float DeltaTime);

void ABossCharacterRat::Multicast_SetScratchWarningVisible_Implementation(bool bVisible)
{
	if (ScratchWarningMesh)
	{
		ScratchWarningMesh->SetHiddenInGame(!bVisible);
		ScratchWarningMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterRat::Multicast_SetDashWarningVisible_Implementation(bool bVisible)
{
	if (DashWarningMesh)
	{
		DashWarningMesh->SetHiddenInGame(!bVisible);
		DashWarningMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterRat::Multicast_SetAuraVisible_Implementation(bool bVisible)
{
	if (DamageAuraWarningMesh)
	{
		DamageAuraWarningMesh->SetHiddenInGame(!bVisible);
		DamageAuraWarningMesh->SetVisibility(bVisible, true);
	}
}

void ABossCharacterRat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABossCharacterRat, CurrentHP);
	DOREPLIFETIME(ABossCharacterRat, bIsAttacking);
	DOREPLIFETIME(ABossCharacterRat, bIsDashing);
	DOREPLIFETIME(ABossCharacterRat, bCanScratch);
	DOREPLIFETIME(ABossCharacterRat, bCanDash);
	DOREPLIFETIME(ABossCharacterRat, bIsPhase2);
}