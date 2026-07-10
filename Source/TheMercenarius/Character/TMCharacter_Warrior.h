#pragma once

#include "CoreMinimal.h"
#include "Character/TMPlayerBase.h"
#include "InputActionValue.h"
#include "TMCharacter_Warrior.generated.h"

class UTMStatComponent;
class UInputAction;
class UAnimMontage;

UCLASS()
class THEMERCENARIUS_API ATMCharacter_Warrior : public ATMPlayerBase
{
	GENERATED_BODY()

public:
	ATMCharacter_Warrior();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTMStatComponent> StatComp;

	/* ---------------------------------------------------------
	 * 스킬 입력 액션 (에디터에서 할당)
	 * --------------------------------------------------------- */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SkillQAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SkillWAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SkillEAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SkillRAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SkillQMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SkillWMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SkillEMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SkillRMontage;


	/* ---------------------------------------------------------
	 * 스킬 실행 함수
	 * --------------------------------------------------------- */
	void InputSkillQ(const FInputActionValue& Value);
	void InputSkillW(const FInputActionValue& Value);
	void InputSkillE(const FInputActionValue& Value);
	void InputSkillR(const FInputActionValue& Value);
}; 