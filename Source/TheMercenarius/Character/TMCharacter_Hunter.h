// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TMPlayerBase.h"
#include "InputActionValue.h"
#include "TMCharacter_Hunter.generated.h"

class UTMStatComponent;
class USphereComponent;
class UInputAction;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class THEMERCENARIUS_API ATMCharacter_Hunter : public ATMPlayerBase
{
	GENERATED_BODY()
	
public:

	ATMCharacter_Hunter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;
private:
	int32 CurrentComboCount = 0;
	const int32 MaxComboCount = 3;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTMStatComponent> StateComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> QSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> WSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ESkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> RSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ASkillAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> QSkillMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> WSkillMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ESkillMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> RSkillMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ASkillMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AttackSphere;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class ATMGranade>GranadeClass;

	void InputSkillQ(const FInputActionValue& Value);
	void InputSkillW(const FInputActionValue& Value);
	void InputSkillE(const FInputActionValue& Value);
	void InputSkillR(const FInputActionValue& Value);
	void InputSkillA(const FInputActionValue& Value);

	UFUNCTION()
	void ACoolDownFinished();
	UFUNCTION()
	void QCoolDownFinished();
	UFUNCTION()
	void WCoolDownFinished();
	UFUNCTION()
	void ECoolDownFinished();
	UFUNCTION()
	void RCoolDownFinished();

	void ASkillFinished();

	UFUNCTION()
	void PerformSwiftShot();
	UFUNCTION()
	void PerformThrow(FVector TargetLocation);
	void PerformDesperado();
	void EndDesperado();
	UFUNCTION()
	void StartQCooldown();

	void PerformShotgunShot();
	void PerformShotgunFinisher();
	void EndShotgunMaster();

	float AttackRange = 500.f;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bAIsOnCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bQIsOnCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bWIsOnCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bEIsOnCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bRIsOnCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float ACoolTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float QCoolTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float WCoolTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float ECoolTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float RCoolTime;

	FTimerHandle ADurationTimerHandle;
	FTimerHandle ACooldownTimerHandle;
	FTimerHandle ComboResetTimerHandle;
	FTimerHandle QCooldownTimerHandle;
	FTimerHandle WCooldownTimerHandle;
	FTimerHandle ECooldownTimerHandle;
	FTimerHandle DesperadoTimerHandle;
	FTimerHandle DesperadoDurationTimerHandle;
	FTimerHandle RCooldownTimerHandle;
	FTimerHandle ShotgunShotTimerHandle;
	FTimerHandle ShotgunFinisherTimerHandle;


	bool useASkill;

	bool bIsMovingToThrow = false;
	FVector TargetThrowLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | CoolDown")
	float ADuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float damage;

	float DesperadoRadius = 600.f;
	float DesperadoDamageInterval = 0.15f;
	float DesperadoDuration = 3.f;

	int32 CurrentShotCount = 0;
	const int32 MaxNormalShots = 3;

	float ShotgunRange = 500.f;
	float ShotgunAngleCos = 0.7f;
	float ShotTimeInterval = 0.4f;

	bool bIsShotgunMasterActive = false;

	FVector GranadeAttackLocation;
};
