// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossCharacterZombie.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UMaterialInstanceDynamic;
class ABossZombieProjectile;

UCLASS()
class THEMERCENARIUS_API ABossCharacterZombie : public ACharacter
{
	GENERATED_BODY()

public:
	ABossCharacterZombie();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

protected:
	// Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Stats")
	float MaxHP = 1000.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Stats")
	float CurrentHP;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|State")
	bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|State")
	bool bIsDead = false;

	void HandleDeath();

	// AI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float ChaseAcceptanceRadius = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float TargetUpdateInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float AttackDecisionInterval = 0.3f;

	UPROPERTY()
	AActor* TargetPlayer = nullptr;

	FTimerHandle TargetUpdateTimerHandle;
	FTimerHandle AttackDecisionTimerHandle;

	void UpdateTarget();
	void UpdateBossAI();
	void MoveToTarget();

	// Basic Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|BasicAttack")
	float BasicAttackRadius = 750.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|BasicAttack")
	float BasicAttackHalfAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|BasicAttack")
	float BasicAttackWarningTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|BasicAttack")
	float BasicAttackAfterDelay = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|BasicAttack")
	float BasicAttackCooldown = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|BasicAttack")
	float BasicAttackDamage = 25.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|BasicAttack")
	bool bCanBasicAttack = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|BasicAttack")
	UStaticMeshComponent* BasicAttackWarningMesh;

	FTimerHandle BasicAttackDamageTimerHandle;
	FTimerHandle BasicAttackAfterDelayTimerHandle;
	FTimerHandle BasicAttackCooldownTimerHandle;

	void StartBasicAttack();
	void ApplyBasicAttackDamage();
	void FinishBasicAttack();
	void ResetBasicAttackCooldown();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetBasicAttackWarningVisible(bool bVisible);

	// Ranged Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Ranged")
	float RangedAttackMinRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Ranged")
	float RangedAttackMaxRange = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Ranged")
	float RangedAttackWarningTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Ranged")
	float RangedAttackAfterDelay = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Ranged")
	float RangedAttackCooldown = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Ranged")
	float RangedAttackDamage = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Ranged")
	float ProjectileSpawnDistance = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Ranged")
	float ProjectileSpawnHeight = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Ranged")
	TSubclassOf<ABossZombieProjectile> ProjectileClass;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Ranged")
	bool bCanRangedAttack = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Ranged")
	UStaticMeshComponent* RangedWarningMesh;

	FTimerHandle RangedAttackWarningTimerHandle;
	FTimerHandle RangedAttackAfterDelayTimerHandle;
	FTimerHandle RangedAttackCooldownTimerHandle;

	void StartRangedAttack();
	void FireRangedProjectile();
	void FinishRangedAttack();
	void ResetRangedAttackCooldown();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetRangedWarningVisible(bool bVisible);

	// Dash Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashMinRange = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashMaxRange = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashDistance = 1600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashWarningTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashDuration = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashAfterDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashCooldown = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float InitialDashCooldown = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashDamage = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashHitWidth = 250.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Dash")
	bool bCanDashAttack = true;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Dash")
	bool bIsDashing = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Dash")
	UStaticMeshComponent* DashWarningMesh;

	FVector DashStartLocation;
	FVector DashEndLocation;
	float DashElapsedTime = 0.0f;

	FTimerHandle DashWarningTimerHandle;
	FTimerHandle DashAfterDelayTimerHandle;
	FTimerHandle DashCooldownTimerHandle;

	void StartDashAttack();
	void ExecuteDashAttack();
	void UpdateDash(float DeltaTime);
	void ApplyDashDamage();
	void FinishDashAttack();
	void ResetDashCooldown();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetDashWarningVisible(bool bVisible);

	// Phase 2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase")
	float Phase2Threshold = 0.5f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bIsPhase2 = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bIsPhase2PatternRunning = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Debug")
	bool bStartWithPhase2 = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	UStaticMeshComponent* Phase2AuraMesh;

	void CheckPhase2();
	void EnterPhase2();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetPhase2AuraVisible(bool bVisible);

	// Phase 2 - Instant Kill
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseInstantKill")
	float FirstInstantKillDelay = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseInstantKill")
	float InstantKillDelayAfterLaser = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseInstantKill")
	float InstantKillWarningTime = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseInstantKill")
	float InstantKillRadius = 1500.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|PhaseInstantKill")
	UStaticMeshComponent* InstantKillWarningMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* InstantKillWarningMID;

	float InstantKillWarningElapsed = 0.0f;

	FTimerHandle PhaseInstantKillScheduleTimerHandle;
	FTimerHandle InstantKillTimerHandle;

	void ScheduleInstantKillAttack(float Delay);
	void StartInstantKillAttack();
	void ApplyInstantKillAttack();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetInstantKillWarningVisible(bool bVisible);

	// Phase 2 - Laser
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseLaser")
	float LaserDelayAfterInstantKill = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseLaser")
	float LaserWarningTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseLaser")
	float LaserDuration = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseLaser")
	float LaserAfterDelay = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseLaser")
	float LaserRecoveryIdleTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseLaser")
	float LaserRange = 2200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseLaser")
	float LaserDamagePerSecond = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseLaser")
	float LaserSweepHalfAngle = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|PhaseLaser")
	float LaserHitWidth = 220.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|PhaseLaser")
	USceneComponent* LaserPivotComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|PhaseLaser")
	UStaticMeshComponent* LaserWarningMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|PhaseLaser")
	UStaticMeshComponent* LaserBeamMesh;

	bool bIsLaserAttacking = false;
	float LaserElapsedTime = 0.0f;
	float LaserCurrentAngle = 0.0f;

	FTimerHandle PhaseLaserScheduleTimerHandle;
	FTimerHandle LaserWarningTimerHandle;
	FTimerHandle LaserEndTimerHandle;
	FTimerHandle LaserAfterDelayTimerHandle;
	FTimerHandle LaserRecoveryIdleTimerHandle;

	void ScheduleLaserAttack(float Delay);
	void StartLaserWarning();
	void StartLaserAttack();
	void UpdateLaserAttack(float DeltaTime);
	void FinishLaserAttack();
	void FinishLaserAfterDelay();
	void FinishLaserRecoveryIdle();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLaserWarningVisible(bool bVisible);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLaserBeamVisible(bool bVisible);
};