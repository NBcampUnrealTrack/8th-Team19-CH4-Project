#include "TMCharacter_Warrior.h"
#include "Game/TMStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h" 
#include "Animation/AnimInstance.h"

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
		// Q, W, E, R 바인딩
		if (SkillQAction)
			EnhancedInputComponent->BindAction(SkillQAction, ETriggerEvent::Started, this, &ATMCharacter_Warrior::InputSkillQ);

		if (SkillWAction)
			EnhancedInputComponent->BindAction(SkillWAction, ETriggerEvent::Started, this, &ATMCharacter_Warrior::InputSkillW);

		if (SkillEAction)
			EnhancedInputComponent->BindAction(SkillEAction, ETriggerEvent::Started, this, &ATMCharacter_Warrior::InputSkillE);

		if (SkillRAction)
			EnhancedInputComponent->BindAction(SkillRAction, ETriggerEvent::Started, this, &ATMCharacter_Warrior::InputSkillR);
	}
}

/* ---------------------------------------------------------
 * 스킬 실제 구현부
 * --------------------------------------------------------- */
void ATMCharacter_Warrior::InputSkillQ(const FInputActionValue& Value)
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("[Warrior] Press Q!"));
	UE_LOG(LogTemp, Warning, TEXT("PressQ"));
	
	if (SkillQMontage)
	{
		// 2. 캐릭터 메시에서 애니메이션을 관리하는 인스턴스를 가져옴
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 3. 몽타주 재생!
			AnimInstance->Montage_Play(SkillQMontage);
		}
	}
}

void ATMCharacter_Warrior::InputSkillW(const FInputActionValue& Value)
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("[Warrior] Press W"));
	UE_LOG(LogTemp, Warning, TEXT("Press W"));

	if (SkillWMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) AnimInstance->Montage_Play(SkillWMontage);
	}
}

void ATMCharacter_Warrior::InputSkillE(const FInputActionValue& Value)
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("[Warrior] Press E!"));
	UE_LOG(LogTemp, Warning, TEXT("Press E"));

	if (SkillEMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) AnimInstance->Montage_Play(SkillEMontage);
	}
}

void ATMCharacter_Warrior::InputSkillR(const FInputActionValue& Value)
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[Warrior] Press R!"));
	UE_LOG(LogTemp, Warning, TEXT("Press R"));

	if (SkillRMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) AnimInstance->Montage_Play(SkillRMontage);
	}
}