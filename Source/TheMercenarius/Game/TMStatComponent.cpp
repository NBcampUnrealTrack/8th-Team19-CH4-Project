#include "TMStatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"

UTMStatComponent::UTMStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 기본 스탯 초기화
	MaxHealth = 100.0f;
	CurrentHealth = 100.0f;

	// 마나/스태미나 관련 변수 주석 처리
	// MaxStamina = 100.0f;
	// CurrentStamina = 100.0f;
	// StaminaRegenRate = 10.0f;

	// 전투 및 이동 스탯 초기화
	Defense = 10.0f;
	BaseAttackPower = 25.0f;
	CriticalHitChance = 0.1f;
	MovementSpeed = 600.0f;
}

void UTMStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	// CurrentStamina = MaxStamina;

	UpdateCharacterMovementSpeed();

	// 타이머 주석 처리
	// GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &UTMStatComponent::RegenerateStamina, 0.1f, true);
}

void UTMStatComponent::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f) return;

	float FinalDamage = DamageAmount - Defense;
	FinalDamage = FMath::Max(FinalDamage, 1.0f);

	CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		OnDeath.Broadcast();
	}
}

void UTMStatComponent::UpdateCharacterMovementSpeed()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
		if (MoveComp)
		{
			MoveComp->MaxWalkSpeed = MovementSpeed;
		}
	}
}

// 스태미나 관련 함수 구현부 주석 처리
/*
bool UTMStatComponent::ConsumeStamina(float Amount)
{
	if (CurrentStamina >= Amount)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, MaxStamina);
		return true;
	}
	return false;
}

void UTMStatComponent::RegenerateStamina()
{
	if (CurrentStamina >= MaxStamina) return;
	CurrentStamina = FMath::Clamp(CurrentStamina + (StaminaRegenRate * 0.1f), 0.0f, MaxStamina);
}
*/