// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TMPlayerBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;//에니메이션 추가
class ATMWeaponBase;

UCLASS()
class THEMERCENARIUS_API ATMPlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATMPlayerBase();

	virtual void Tick(float DeltaTime) override;

	void SetMoveToTarget(const FVector& newTarget);

	void DashToTarget(const FVector& newTarget);

	void StartDash();

	void StopDash();

	void Attack();

	ATMWeaponBase* GetCurrentWeapon() const
	{
		return CurrentWeapon;
	}

	bool bIsMovingToTarget;

protected:

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerComponent")
	TObjectPtr<USpringArmComponent>springArmComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerComponent")
	TObjectPtr<UCameraComponent>cameraComponent;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float arriveToIerance;

	FVector targetLocation;

	FVector dashLocation;

	FVector moveVector;

	FVector playerActorLocation;


	bool bIsDashToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ATMWeaponBase> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY()
	ATMWeaponBase* CurrentWeapon;
protected:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float dashRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")//스페이스 사용 에니메이션 몽타주
	TObjectPtr<UAnimMontage> DashMontage;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
