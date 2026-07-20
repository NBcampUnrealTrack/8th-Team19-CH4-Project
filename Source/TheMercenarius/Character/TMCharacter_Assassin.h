#pragma once

#include "CoreMinimal.h"
#include "Character/TMPlayerBase.h"
#include "InputActionValue.h"
#include "TMCharacter_Assassin.generated.h"

class UTMStatComponent;
class UInputAction;
class UAnimMontage;

UCLASS()
class THEMERCENARIUS_API ATMCharacter_Assassin : public ATMPlayerBase
{
	GENERATED_BODY()

public:
	ATMCharacter_Assassin();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 보스가 이 변수를 읽고 암살자를 무시하게 만듭니다 (은신 상태)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsStealth = false;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 블루프린트(애님 노티파이) 연동 함수
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteSkillQImpact();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteSkillWImpact();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteSkillEImpact();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteSkillRImpact();

	// RPC 네트워크 함수
	UFUNCTION(Server, Reliable)
	void Server_InputSkillQ();
	UFUNCTION(Server, Reliable)
	void Server_InputSkillW();
	UFUNCTION(Server, Reliable)
	void Server_InputSkillE();
	UFUNCTION(Server, Reliable)
	void Server_InputSkillR();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillQMontage();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillWMontage();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillEMontage();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillRMontage();

private:
	// 👇 여기서부터 모든 변수에 meta = (AllowPrivateAccess = "true") 가 추가되었습니다!
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTMStatComponent> StatComp;

	/* ---------------------------------------------------------
	 * 스킬 입력 & 몽타주
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
	 * Q 스킬 (맹독 찌르기 - 방어력 감소 디버프)
	 * --------------------------------------------------------- */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQDefenseDebuff;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQDuration;

	/* ---------------------------------------------------------
	 * W 스킬 (그림자 은신 - 생존기 및 이속 증가)
	 * --------------------------------------------------------- */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W", meta = (AllowPrivateAccess = "true"))
	float SkillWDuration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|W", meta = (AllowPrivateAccess = "true"))
	float SkillWSpeedBuff;

	FTimerHandle WBuffTimerHandle;

	/* ---------------------------------------------------------
	 * E 스킬 (암살자의 스텝 - 초고속 관통 돌진)
	 * --------------------------------------------------------- */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|E", meta = (AllowPrivateAccess = "true"))
	float SkillERadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|E", meta = (AllowPrivateAccess = "true"))
	float SkillEDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|E", meta = (AllowPrivateAccess = "true"))
	float SkillEDashPower;

	/* ---------------------------------------------------------
	 * R 스킬 (칼날 폭풍 - 다단 히트)
	 * --------------------------------------------------------- */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R", meta = (AllowPrivateAccess = "true"))
	float SkillRRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R", meta = (AllowPrivateAccess = "true"))
	float SkillRDamage;

	/* ---------------------------------------------------------
	 * 내부 로직 함수
	 * --------------------------------------------------------- */
	void InputSkillQ(const FInputActionValue& Value);
	void InputSkillW(const FInputActionValue& Value);
	void InputSkillE(const FInputActionValue& Value);
	void InputSkillR(const FInputActionValue& Value);

	void ProcessSkillQOverlap();
	void ProcessSkillWOverlap();
	void ProcessSkillEOverlap();
	void ProcessSkillROverlap();

	// 타이머 복구용 함수들
	void ResetWBuff();
	void ResetBossDefense(class UTMStatComponent* BossStatComp, float OriginalDefense);
};