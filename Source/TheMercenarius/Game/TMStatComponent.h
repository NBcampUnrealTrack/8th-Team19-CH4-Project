#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TMStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedDelegate, float, CurrentHealth, float, MaxHealth);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THEMERCENARIUS_API UTMStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTMStatComponent();

protected:
	virtual void BeginPlay() override;

public:

	/* ---------------------------------------------------------
	 * 기본 스탯 (체력)
	 * 마나/스태미나는 추후 사용을 위해 주석 처리
	 * --------------------------------------------------------- */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Health")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Health")
	float CurrentHealth;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Mana")
	// float MaxStamina;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Mana")
	// float CurrentStamina;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Mana")
	// float StaminaRegenRate;

	/* ---------------------------------------------------------
	 * 전투 및 이동 스탯
	 * --------------------------------------------------------- */

	 // 방어력 (데미지를 감쇄시키는 역할)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Combat")
	float Defense;

	// 기본 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Combat")
	float BaseAttackPower;

	// 치명타 확률 (0.0 ~ 1.0, 계산법 ex) 0.2는 20%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Combat", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CriticalHitChance;

	// 이동 속도 (언리얼 캐릭터 무브먼트의 MaxWalkSpeed와 연동)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Movement")
	float MovementSpeed;

	/* ---------------------------------------------------------
	 * 핵심 기능 함수
	 * --------------------------------------------------------- */

	 // 방어력이 적용된 데미지 처리 함수
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ApplyDamage(float DamageAmount);

	// 스태미나 관련 함수 주석 처리
	// UFUNCTION(BlueprintCallable, Category = "Stats")
	// bool ConsumeStamina(float Amount);

	// 이동 속도가 변경되었을 때 캐릭터 무브먼트에 동기화해주는 함수
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void UpdateCharacterMovementSpeed();

	/* ---------------------------------------------------------
	 * 이벤트 (델리게이트)
	 * --------------------------------------------------------- */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathDelegate OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedDelegate OnHealthChanged;

protected:
	// 스태미나 회복 관련 타이머 및 함수 주석 처리
	// FTimerHandle StaminaRegenTimerHandle;
	// void RegenerateStamina();
};