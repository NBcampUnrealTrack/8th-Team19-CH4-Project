// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TMPlayerBase.h"
#include "InputActionValue.h"
#include "TMCharacter_Supporter.generated.h"

class UTMStatComponent;
class USphereComponent;
class UInputAction;
class UAnimMontage;

/**
 * 
 */
UCLASS()
class THEMERCENARIUS_API ATMCharacter_Supporter : public ATMPlayerBase
{
	GENERATED_BODY()
	
public:

	ATMCharacter_Supporter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

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

	FTimerHandle ACooldownTimerHandle;
	FTimerHandle QCooldownTimerHandle;
	FTimerHandle WCooldownTimerHandle;
	FTimerHandle ECooldownTimerHandle;
	FTimerHandle RCooldownTimerHandle;
};
