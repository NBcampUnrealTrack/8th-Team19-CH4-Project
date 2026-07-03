// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TMPlayerBase.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class THEMERCENARIUS_API ATMPlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATMPlayerBase();

	virtual void Tick(float DeltaTime) override;

	void SetMoveToTarget(const FVector& NewTarget);
private:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerComponent")
	TObjectPtr<USpringArmComponent>springArmComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerComponent")
	TObjectPtr<UCameraComponent>cameraComponent;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float arriveToIerance;

	FVector targetLocation;

	bool bIsMovingToTarget;
};
