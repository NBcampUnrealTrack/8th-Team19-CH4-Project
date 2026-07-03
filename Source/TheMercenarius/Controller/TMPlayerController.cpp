// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TMPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/TMInputConfig.h"
#include "Character/TMPlayerBase.h"
#include "Kismet/KismetSystemLibrary.h"

ATMPlayerController::ATMPlayerController()
{
	bShowMouseCursor = true;
}

void ATMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* enhancedInputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(enhancedInputSystem) == true)
	{
		enhancedInputSystem->AddMappingContext(playerInputMappingContext, 0);
	}
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (IsValid(enhancedInputComponent) == true)
	{
		enhancedInputComponent->BindAction(playerInputConfig->move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
	}
}

void ATMPlayerController::InputMove(const FInputActionValue& inValue)
{
	FHitResult hitResult;
	if (GetHitResultUnderCursor(ECC_Visibility, true, hitResult))
	{
		MoveToDestination(hitResult.ImpactPoint);
	}
}

void ATMPlayerController::MoveToDestination(FVector destination)
{
	ATMPlayerBase* playerCharacter = Cast<ATMPlayerBase>(GetPawn());
	if (IsValid(playerCharacter) == true)
	{
		playerCharacter->SetMoveToTarget(destination);
	}
}