// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TMPlayerBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATMPlayerBase::ATMPlayerBase() : bIsMovingToTarget(false), arriveToIerance(5.f)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//arriveToIerance = 3.f;
	float capsuleHalfHeight = 90.f;
	float capsuleRadius = 40.f;
	float armLength = 800.f;
	FVector meshPosition(0.f, 0.f, -capsuleHalfHeight);
	FRotator meshRotation(0.f, -90.f, 0.f);
	FRotator armRotator(-50.f, 0.f, 0.f);
	FVector armLocation(0.f, 0.f, 60.f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(capsuleRadius, capsuleHalfHeight);
	GetMesh()->SetRelativeLocationAndRotation(meshPosition, meshRotation);

	springArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	springArmComponent->SetupAttachment(RootComponent);
	springArmComponent->AddRelativeRotation(armRotator);
	springArmComponent->AddRelativeLocation(armLocation);
	springArmComponent->TargetArmLength = armLength;
	springArmComponent->bUsePawnControlRotation = false; //카메라 지지대의 회전각도를 플레이어 컨트롤의 입력각도와 일치 시킬 것인지에 관한 변수

	springArmComponent->bInheritPitch = false; //RootComponent가 회전할때 자식 컴포넌트도 회전시킬지에 관한 변수
	springArmComponent->bInheritYaw = false;
	springArmComponent->bInheritRoll = false;
	springArmComponent->bDoCollisionTest = false; //장애물이 존재할 시 카메라를 캐릭터쪽으로 당겨오는 변수


	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	cameraComponent->SetupAttachment(springArmComponent);

	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
}

void ATMPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMovingToTarget == true)
	{

		FVector currentLocation = GetActorLocation();
		FString actorLocation = FString::Printf(TEXT("ActorLocation : X = %.f, Y = %.f, Z = %.f"), currentLocation.X, currentLocation.Y, currentLocation.Z);
		float distanceToTarget = FVector::Dist(currentLocation, targetLocation);
		if (distanceToTarget <= arriveToIerance)
		{	
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->Velocity = FVector::ZeroVector;
			GetCharacterMovement()->ClearAccumulatedForces();
			ConsumeMovementInputVector();
			SetActorLocation(targetLocation, true);
			UKismetSystemLibrary::PrintString(this, actorLocation, true, true, FLinearColor::Blue, 10.f);
			bIsMovingToTarget = false;

			return;
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, actorLocation, true, true, FLinearColor::Green, 10.f);
			FVector moveDirection = (targetLocation - currentLocation).GetSafeNormal2D();
			AddMovementInput(moveDirection, 1.f);
		}
	}
}

void ATMPlayerBase::SetMoveToTarget(const FVector& NewTarget)
{
	targetLocation = FVector(NewTarget.X, NewTarget.Y, GetActorLocation().Z);
	bIsMovingToTarget = true;
	FString targetVector = FString::Printf(TEXT("Tarrget Location : X = %f, Y = %f, Z = %f"), targetLocation.X, targetLocation.Y, targetLocation.Z);
	UKismetSystemLibrary::PrintString(this, targetVector, true, true, FLinearColor::Red, 10.f);
}