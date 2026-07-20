// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TMCharacter_Supporter.h"
#include "Character/TMPlayerBase.h"
#include "Weapon/TMWeaponBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Controller/TMPlayerController.h"
#include "Game/TMStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"


ATMCharacter_Supporter::ATMCharacter_Supporter() : bAIsOnCooldown(false), bQIsOnCooldown(false), bWIsOnCooldown(false)
, bEIsOnCooldown(false), bRIsOnCooldown(false), ACoolTime(30.f), QCoolTime(30.f), WCoolTime(10.f), ECoolTime(20.f), RCoolTime(20.f)
{
	StateComponent = CreateDefaultSubobject<UTMStatComponent>(TEXT("StateComponent"));

	if (StateComponent)
	{
		StateComponent->MaxHealth = 80.f + StateComponent->Shield;
		StateComponent->Defense = 3.f;
		StateComponent->BaseAttackPower = damage + StateComponent->BuffeAttackPower;
		StateComponent->CriticalHitChance = 0.02f;
		StateComponent->MovementSpeed = 650.f + StateComponent->BuffeMoveSpeed;


	}

}

void ATMCharacter_Supporter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (QSkillAction)
			EnhancedInputComponent->BindAction(QSkillAction, ETriggerEvent::Started, this, &ATMCharacter_Supporter::InputSkillQ);

		if (WSkillAction)
			EnhancedInputComponent->BindAction(WSkillAction, ETriggerEvent::Started, this, &ATMCharacter_Supporter::InputSkillW);

		if (ESkillAction)
			EnhancedInputComponent->BindAction(ESkillAction, ETriggerEvent::Started, this, &ATMCharacter_Supporter::InputSkillE);

		if (RSkillAction)
			EnhancedInputComponent->BindAction(RSkillAction, ETriggerEvent::Started, this, &ATMCharacter_Supporter::InputSkillR);

		if (ASkillAction)
			EnhancedInputComponent->BindAction(ASkillAction, ETriggerEvent::Started, this, &ATMCharacter_Supporter::InputSkillA);
	}
}

void ATMCharacter_Supporter::InputSkillQ(const FInputActionValue& Value)
{
	StateComponent = GetOwner()->FindComponentByClass<UTMStatComponent>();
	if (!StateComponent || bQIsOnCooldown == true)
	{
		return;
	}

	bQIsOnCooldown = true;

	float BounsValue = StateComponent->BaseAttackPower * 0.1f;
	float BuffSpeed = StateComponent->MovementSpeed * 0.4f;


	for (TActorIterator<ATMPlayerBase> It(GetWorld()); It; ++It)
	{
		ATMPlayerBase* Target = *It;

		if (UTMStatComponent* TargetState = Target->FindComponentByClass<UTMStatComponent>())
		{
			TargetState->SetBuffeAttackPower(BounsValue);
			TargetState->SetBuffeSpeed(BuffSpeed);

			BufferedTargets.Add(TargetState);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(BuffTimerHandle, this, &ATMCharacter_Supporter::RemoveBuff, 10.f, false);
	GetWorld()->GetTimerManager().SetTimer(QCooldownTimerHandle, this, &ATMCharacter_Supporter::QCoolDownFinished, QCoolTime, false);

}

void ATMCharacter_Supporter::RemoveBuff()
{
	for (auto& TargetStatePtr : BufferedTargets)
	{
		if (TargetStatePtr.IsValid())
		{
			TargetStatePtr->SetBuffeAttackPower(0.f);
			TargetStatePtr->SetBuffeSpeed(0.f);
		}
	}
	BufferedTargets.Empty();
}

void ATMCharacter_Supporter::InputSkillW(const FInputActionValue& Value)
{

}
void ATMCharacter_Supporter::InputSkillE(const FInputActionValue& Value)
{
	StateComponent = GetOwner()->FindComponentByClass<UTMStatComponent>();
	if (!StateComponent || bEIsOnCooldown == true)
	{
		return;
	}

	bEIsOnCooldown = true;

	float ShieldValue = StateComponent->MaxHealth * 0.5f;

	for (TActorIterator<ATMPlayerBase> It(GetWorld()); It; ++It)
	{
		ATMPlayerBase* Target = *It;

		if (UTMStatComponent* TargetState = Target->FindComponentByClass<UTMStatComponent>())
		{
			TargetState->SetShield(ShieldValue);
			ShieldedTargets.Add(TargetState);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(ShieldTimerHandle, this, &ATMCharacter_Supporter::RemoveShield, 7.f, false);
	GetWorld()->GetTimerManager().SetTimer(ECooldownTimerHandle, this, &ATMCharacter_Supporter::ECoolDownFinished, ECoolTime, false);
}

void ATMCharacter_Supporter::RemoveShield()
{
	for (auto& TargetStatePtr : ShieldedTargets)
	{
		if (TargetStatePtr.IsValid())
		{
			TargetStatePtr->SetShield(0.f);
		}
	}
	ShieldedTargets.Empty();
}

void ATMCharacter_Supporter::InputSkillR(const FInputActionValue& Value)
{
	StateComponent = GetOwner()->FindComponentByClass<UTMStatComponent>();
	if (!StateComponent || bRIsOnCooldown == true)
	{
		return;
	}

	bRIsOnCooldown = true;

	float HealAmount = 50.f;

	for (TActorIterator<ATMPlayerBase> It(GetWorld()); It; ++It)
	{
		ATMPlayerBase* Target = *It;

		if (UTMStatComponent* TargetState = Target->FindComponentByClass<UTMStatComponent>())
		{
			TargetState->Heal(HealAmount);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(RCooldownTimerHandle, this, &ATMCharacter_Supporter::RCoolDownFinished, RCoolTime, false);

}
void ATMCharacter_Supporter::InputSkillA(const FInputActionValue& Value)
{

}

void ATMCharacter_Supporter::ACoolDownFinished()
{

}
void ATMCharacter_Supporter::QCoolDownFinished()
{
	bQIsOnCooldown = false;
}

void ATMCharacter_Supporter::WCoolDownFinished()
{

}
void ATMCharacter_Supporter::ECoolDownFinished()
{
	bEIsOnCooldown = false;
}

void ATMCharacter_Supporter::RCoolDownFinished()
{
	bRIsOnCooldown = false;
}