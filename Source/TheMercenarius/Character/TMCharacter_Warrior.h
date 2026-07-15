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

	// 멀티플레이 네트워크 변수 동기화를 위한 필수 오버라이드 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 애님 노티파이(블루프린트)가 도끼를 찍을 때 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteSkillQImpact();

	// 애님 노티파이(블루프린트)가 R스킬 도끼를 휘두를 때 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ExecuteSkillRImpact();

	/* ---------------------------------------------------------
	 * 멀티플레이용 RPC 네트워크 함수 설정 
	 * --------------------------------------------------------- */

	 // 클라이언트가 서버에게 스킬 쓸게요 요청하는 함수
	UFUNCTION(Server, Reliable)
	void Server_InputSkillQ();
	
	UFUNCTION(Server, Reliable)
	void Server_InputSkillW();

	UFUNCTION(Server, Reliable)
	void Server_InputSkillE();

	UFUNCTION(Server, Reliable)
	void Server_InputSkillR();
	//여기까지 요청하는 함수


	// 서버가 모든 클라이언트 화면에 애니메이션 재생을 지시하는 방송 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillQMontage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillWMontage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillEMontage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillRMontage();

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

	/* ---------------------------------------------------------
	 * 애니메이션 몽타주
	 * --------------------------------------------------------- */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SkillQMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SkillWMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SkillEMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SkillRMontage;

	/* ---------------------------------------------------------
	 * Q 스킬 스펙 데이터
	 * --------------------------------------------------------- */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQSlowModifier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Q", meta = (AllowPrivateAccess = "true"))
	float SkillQSlowDuration;

	/* ---------------------------------------------------------
	 * R 스킬 스펙 데이터 (추가)
	 * --------------------------------------------------------- */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R", meta = (AllowPrivateAccess = "true"))
	float SkillRRadius; // 360도 범위

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|R", meta = (AllowPrivateAccess = "true"))
	float SkillRDamage; // 타격당 데미지 (연타를 위해 낮게 설정)

	/* ---------------------------------------------------------
	 * 입력 처리 및 내부 로직 함수
	 * --------------------------------------------------------- */
	void InputSkillQ(const FInputActionValue& Value);
	void InputSkillW(const FInputActionValue& Value);
	void InputSkillE(const FInputActionValue& Value);
	void InputSkillR(const FInputActionValue& Value);

	// 실제 충돌 판정을 처리하는 내부 로직 함수 (오직 서버에서만 실행되도록 보호할 예정)
	void ProcessSphereOverlap();

	// R 스킬 실제 충돌 판정
	void ProcessSkillROverlap();

	// 3초 뒤에 몬스터의 속도를 원래대로 돌려놓을 타이머용 함수
	void ResetTargetSpeed(class ACharacter* TargetCharacter, float OriginalSpeed);
};