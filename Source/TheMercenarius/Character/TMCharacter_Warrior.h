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
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteSkillQImpact();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQRadius; // 감지할 구체 반지름 (기본값 추천: 300.f)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQDamage; // 대지가르기 피해량

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQSlowModifier; // 감속할 속도 비율 (예: 0.4f면 속도 40% 감소)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQSlowDuration; // 슬로우 지속 시간

 
	/* ---------------------------------------------------------
	 * 스킬 실행 함수
	 * --------------------------------------------------------- */
	void InputSkillQ(const FInputActionValue& Value);
	void InputSkillW(const FInputActionValue& Value);
	void InputSkillE(const FInputActionValue& Value);
	void InputSkillR(const FInputActionValue& Value);
	// 실제 충돌 판정을 처리하는 내부 로직 함수
	void ProcessSphereOverlap();
}; 