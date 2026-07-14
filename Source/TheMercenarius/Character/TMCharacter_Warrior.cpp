#include "TMCharacter_Warrior.h"
#include "Game/TMStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h" 
#include "Animation/AnimInstance.h"
#include "Engine/OverlapResult.h" 
#include "Engine/World.h"
#include "DrawDebugHelpers.h" 
#include "Net/UnrealNetwork.h" 
#include "Kismet/GameplayStatics.h"

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

		SkillQRadius = 300.0f;
		SkillQDamage = 50.0f;
		SkillQSlowModifier = 0.4f;
		SkillQSlowDuration = 3.0f;
	}
	GetCharacterMovement()->MaxWalkSpeed = 550.0f;

	bReplicates = true;
}

void ATMCharacter_Warrior::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
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
 * 스킬 실제 구현부 (멀티플레이 적용)
 * --------------------------------------------------------- */
void ATMCharacter_Warrior::InputSkillQ(const FInputActionValue& Value)
{
	Server_InputSkillQ();
}

void ATMCharacter_Warrior::Server_InputSkillQ_Implementation()
{
	Multicast_PlaySkillQMontage();
}

void ATMCharacter_Warrior::Multicast_PlaySkillQMontage_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("[Warrior] Multi Q Active!"));

	if (SkillQMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(SkillQMontage);
		}
	}
}

// ---------------------------------------------------------
// W 스킬 (분노의 함성 등)
// ---------------------------------------------------------
void ATMCharacter_Warrior::InputSkillW(const FInputActionValue& Value)
{
	Server_InputSkillW();
}

void ATMCharacter_Warrior::Server_InputSkillW_Implementation()
{
	Multicast_PlaySkillWMontage();
}

void ATMCharacter_Warrior::Multicast_PlaySkillWMontage_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("[Warrior] Multi W Active!"));

	if (SkillWMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) AnimInstance->Montage_Play(SkillWMontage);
	}
}

// ---------------------------------------------------------
// E 스킬 (차지 스트라이크 등)
// ---------------------------------------------------------
void ATMCharacter_Warrior::InputSkillE(const FInputActionValue& Value)
{
	Server_InputSkillE();
}

void ATMCharacter_Warrior::Server_InputSkillE_Implementation()
{
	Multicast_PlaySkillEMontage();
}

void ATMCharacter_Warrior::Multicast_PlaySkillEMontage_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("[Warrior] Multi E Active!"));

	if (SkillEMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) AnimInstance->Montage_Play(SkillEMontage);
	}
}

// ---------------------------------------------------------
// R 스킬 (궁극기)
// ---------------------------------------------------------
void ATMCharacter_Warrior::InputSkillR(const FInputActionValue& Value)
{
	Server_InputSkillR();
}

void ATMCharacter_Warrior::Server_InputSkillR_Implementation()
{
	Multicast_PlaySkillRMontage();
}

void ATMCharacter_Warrior::Multicast_PlaySkillRMontage_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[Warrior] Multi R Active!"));

	if (SkillRMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) AnimInstance->Montage_Play(SkillRMontage);
	}
}

// ---------------------------------------------------------
// Q 스킬 범위 판정 로직 (애님 노티파이에서 호출됨)
// ---------------------------------------------------------
void ATMCharacter_Warrior::ExecuteSkillQImpact()
{
	if (HasAuthority())
	{
		ProcessSphereOverlap();
	}
}

void ATMCharacter_Warrior::ProcessSphereOverlap()
{
	FVector CenterLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	FCollisionQueryParams Params(NAME_None, false, this);

	TArray<FOverlapResult> OverlapResults;

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		CenterLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SkillQRadius),
		Params
	);

	DrawDebugSphere(GetWorld(), CenterLocation, SkillQRadius, 16, FColor::Red, false, 3.0f);

	if (bHasOverlap == true)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (IsValid(OverlappedActor) == true && OverlappedActor != this)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
					FString::Printf(TEXT("[Hit - Server] %s to %f hit damage!"), *OverlappedActor->GetName(), SkillQDamage));

				UGameplayStatics::ApplyDamage(
					OverlappedActor,
					SkillQDamage,
					GetController(),
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
}