// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "TMPlayerController.generated.h"

class UTMInputConfig;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class THEMERCENARIUS_API ATMPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATMPlayerController();

protected:

	virtual void SetupInputComponent() override;

private:
	void InputMove(const FInputActionValue& inValue);

	void InputSpace(const FInputActionValue& inValue);
protected:
	void MoveToDestination(FVector destination);

	void MoveToDash(FVector destination);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTMInputConfig>playerInputConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext>playerInputMappingContext;

};
