// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TMCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
// Sets default values
ATMCharacterBase::ATMCharacterBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	float capsuleHalfHeight = 90.f;
	float capsuleRadius = 40.f;
	float armLength = 2500.f;
	FRotator armRotator(-60.f, 0.f, 0.f);
	FVector armLocation(0.f, 0.f, 60.f);

	capsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	capsuleComponent->SetCapsuleHalfHeight(capsuleHalfHeight);
	capsuleComponent->SetCapsuleRadius(capsuleRadius);
	SetRootComponent(capsuleComponent);

	skeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	skeletalMeshComponent->SetupAttachment(RootComponent);
	
	springArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	springArmComponent->SetupAttachment(RootComponent);
	springArmComponent->AddRelativeRotation(armRotator);
	springArmComponent->AddRelativeLocation(armLocation);
	springArmComponent->TargetArmLength = armLength;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	cameraComponent->SetupAttachment(springArmComponent);
}