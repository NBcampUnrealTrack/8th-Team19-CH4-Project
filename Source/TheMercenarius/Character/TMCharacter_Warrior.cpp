#include "TMCharacter_Warrior.h"
#include "Game/TMStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "EnhancedInputComponent.h" 
#include "EnhancedInputSubsystems.h"

ATMCharacter_Warrior::ATMCharacter_Warrior()
{
	
	StatComp = CreateDefaultSubobject<UTMStatComponent>(TEXT("StatComp"));
	if (StatComp)
	{
		StatComp->MaxHealth = 150.0f;
		StatComp->Defense = 20.0f;
		StatComp->BaseAttackPower = 35.0f;
		StatComp->CriticalHitChance = 0.05f;
		StatComp->MovementSpeed = 550.0f;
	}

	
	GetCharacterMovement()->MaxWalkSpeed = 550.0f;
}

void ATMCharacter_Warrior::BeginPlay()
{
	Super::BeginPlay();
}

void ATMCharacter_Warrior::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		
		if (SkillQAction)
		{
			EnhancedInputComponent->BindAction(SkillQAction, ETriggerEvent::Started, this, &ATMCharacter_Warrior::InputSkillQ);
		}
	}
}

void ATMCharacter_Warrior::InputSkillQ(const FInputActionValue& Value)
{
	// 1. 시전을 위해 즉시 이동 멈춤
	GetCharacterMovement()->StopMovementImmediately();

	// 2. 작동 확인용 로그
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("[Warrior] pressQ !!"));
	}

	//확인 후 애니메이션 몽타주 재생 및 충돌 판정 구현
}