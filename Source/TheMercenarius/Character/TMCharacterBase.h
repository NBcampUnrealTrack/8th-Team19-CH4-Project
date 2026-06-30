// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TMCharacterBase.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class THEMERCENARIUS_API ATMCharacterBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATMCharacterBase();

private:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerComponent")
	TObjectPtr<UCapsuleComponent>capsuleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerComponent")
	TObjectPtr<USkeletalMeshComponent>skeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerComponent")
	TObjectPtr<USpringArmComponent>springArmComponent;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerComponent")
	TObjectPtr<UCameraComponent>cameraComponent;

};
