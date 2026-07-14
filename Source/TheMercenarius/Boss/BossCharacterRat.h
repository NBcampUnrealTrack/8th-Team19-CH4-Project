// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossCharacterRat.generated.h"

class UBoxComponent;
class USphereComponent;
class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class THEMERCENARIUS_API ABossCharacterRat : public ACharacter
{
	GENERATED_BODY()

public:
	ABossCharacterRat();

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
	float MaxHP = 800.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Stats")
	float CurrentHP;

	// AI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float ChaseAcceptanceRadius = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float TargetUpdateInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float MoveUpdateInterval = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float AttackDecisionInterval = 0.5f;

	UPROPERTY()
	AActor* TargetPlayer = nullptr;

	FTimerHandle TargetUpdateTimerHandle;
	FTimerHandle MoveTimerHandle;
	FTimerHandle AttackDecisionTimerHandle;
	FTimerHandle AttackEndTimerHandle;

	void UpdateTarget();
	void MoveToTarget();
	void UpdateBossAI();

	// State
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|State")
	bool bIsAttacking = false;

	// Scratch Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Scratch")
	float ScratchRange = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Scratch")
	float ScratchWarningTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Scratch")
	float ScratchAfterDelay = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Scratch")
	float ScratchCooldown = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Scratch")
	float ScratchDamage = 20.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Scratch")
	bool bCanScratch = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Scratch")
	UBoxComponent* ScratchHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Scratch")
	UStaticMeshComponent* ScratchWarningMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Scratch")
	float ScratchStartHalfAngle = 70.0f;

	FTimerHandle ScratchDamageTimerHandle;
	FTimerHandle ScratchCooldownTimerHandle;

	void StartScratchAttack();
	void ApplyScratchDamage();
	void FinishScratchAttack();
	void ResetScratchCooldown();
	void RotateToTarget();
	bool IsTargetInScratchAngle() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetScratchWarningVisible(bool bVisible);

	// Dash Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashWarningTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashAfterDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashCooldown = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float InitialDashCooldown = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashDistance = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashDuration = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashDamage = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashHitWidth = 180.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Dash")
	bool bCanDash = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Dash")
	bool bIsDashing = false;

	UPROPERTY()
	AActor* DashTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Dash")
	UStaticMeshComponent* DashWarningMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Dash")
	USceneComponent* DashWarningPivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashWarningForwardOffset = 350.0f;

	FVector DashDirection;
	FVector DashStartLocation;
	FVector DashEndLocation;
	float DashElapsedTime = 0.0f;

	FTimerHandle DashExecuteTimerHandle;
	FTimerHandle DashCooldownTimerHandle;

	void UpdateDash(float DeltaTime);
	void UpdateDashTarget();
	void StartDashAttack();
	void ExecuteDashAttack();
	void ApplyDashDamage();
	void FinishDashAttack();
	void ResetDashCooldown();
	void StartDashCooldown();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetDashWarningVisible(bool bVisible);

	// Phase 2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase")
	float Phase2Threshold = 0.5f;

	UPROPERTY(ReplicatedUsing = OnRep_IsPhase2, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bIsPhase2 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Debug")
	bool bStartWithPhase2 = false;

	UFUNCTION()
	void OnRep_IsPhase2();

	void CheckPhase2();
	void EnterPhase2();

	// Phase 2 Aura
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Aura")
	float AuraRadius = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Aura")
	float AuraDamage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Aura")
	float AuraDamageInterval = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Aura")
	USphereComponent* DamageAuraSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Aura")
	UStaticMeshComponent* DamageAuraWarningMesh;

	FTimerHandle AuraDamageTimerHandle;

	void ApplyAuraDamage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetAuraVisible(bool bVisible);
};