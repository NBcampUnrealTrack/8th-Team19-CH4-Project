// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossCharacterRat.generated.h"

UCLASS()
class BOSS_API ABossCharacterRat : public ACharacter
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Stats")
	float MaxHP = 800.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Stats")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float ChaseAcceptanceRadius = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float TargetUpdateInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float MoveUpdateInterval = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|AI")
	float AttackDecisionInterval = 0.5f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	bool bIsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Scratch")
	float ScratchRange = 250.0f;

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
	class UBoxComponent* ScratchHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Scratch")
	class UStaticMeshComponent* ScratchWarningMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashWarningTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashAfterDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float DashCooldown = 30.0f;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Dash")
	float InitialDashCooldown = 30.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Dash")
	class UStaticMeshComponent* DashWarningMesh;

	UPROPERTY()
	AActor* TargetPlayer;

	UPROPERTY()
	AActor* DashTarget;

	FVector DashDirection;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bIsPhase2 = false;

	UFUNCTION()
	void OnRep_IsPhase2();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Aura")
	class USphereComponent* DamageAuraSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Aura")
	class UStaticMeshComponent* DamageAuraWarningMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Aura")
	float Phase2Threshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Aura")
	float AuraRadius = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Aura")
	float AuraDamage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Aura")
	float AuraDamageInterval = 1.0f;

	FTimerHandle AuraDamageTimerHandle;	
	FTimerHandle TargetUpdateTimerHandle;
	FTimerHandle MoveTimerHandle;
	FTimerHandle AttackDecisionTimerHandle;
	FTimerHandle AttackEndTimerHandle;

	FTimerHandle ScratchDamageTimerHandle;
	FTimerHandle ScratchCooldownTimerHandle;

	FTimerHandle DashExecuteTimerHandle;
	FTimerHandle DashCooldownTimerHandle;

	void UpdateTarget();
	void MoveToTarget();
	void UpdateBossAI();

	void StartScratchAttack();
	void ApplyScratchDamage();
	void FinishScratchAttack();
	void ResetScratchCooldown();

	void UpdateDashTarget();
	void StartDashAttack();
	void ExecuteDashAttack();
	void ApplyDashDamage();
	void FinishDashAttack();
	void ResetDashCooldown();
	void StartDashCooldown();

	void CheckPhase2();
	void EnterPhase2();
	void ApplyAuraDamage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetScratchWarningVisible(bool bVisible);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetDashWarningVisible(bool bVisible);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetAuraVisible(bool bVisible);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Debug")
	bool bStartWithPhase2 = false;
};