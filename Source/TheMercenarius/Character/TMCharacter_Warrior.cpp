#include "TMCharacter_Warrior.h"
#include "Game/TMStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h" 
#include "Animation/AnimInstance.h"
#include "Engine/OverlapResult.h" 
#include "Engine/World.h"
#include "Net/UnrealNetwork.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h" 
#include "TimerManager.h"            

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

		// Q 스킬 기본값
		SkillQRadius = 300.0f;
		SkillQDamage = 50.0f;
		SkillQSlowModifier = 0.4f;
		SkillQSlowDuration = 3.0f;

		// W 스킬 기본값 (공/방 버프)
		SkillWDuration = 5.0f;
		SkillWDefenseBuff = 30.0f;
		SkillWAttackBuff = 20.0f;

		// E 스킬 기본값 (차지 스트라이크)
		SkillERadius = 300.0f;
		SkillEDamage = 40.0f;
		SkillEDashSpeed = 1500.0f; // 앞으로 돌진하는 힘

		// R 스킬 기본값
		SkillRRadius = 400.0f;
		SkillRDamage = 20.0f;
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
		if (SkillQAction) EnhancedInputComponent->BindAction(SkillQAction, ETriggerEvent::Started, this, &ATMCharacter_Warrior::InputSkillQ);
		if (SkillWAction) EnhancedInputComponent->BindAction(SkillWAction, ETriggerEvent::Started, this, &ATMCharacter_Warrior::InputSkillW);
		if (SkillEAction) EnhancedInputComponent->BindAction(SkillEAction, ETriggerEvent::Started, this, &ATMCharacter_Warrior::InputSkillE);
		if (SkillRAction) EnhancedInputComponent->BindAction(SkillRAction, ETriggerEvent::Started, this, &ATMCharacter_Warrior::InputSkillR);
	}
}

/* ---------------------------------------------------------
 * Q 스킬 (대지가르기)
 * --------------------------------------------------------- */
void ATMCharacter_Warrior::InputSkillQ(const FInputActionValue& Value) { Server_InputSkillQ(); }
void ATMCharacter_Warrior::Server_InputSkillQ_Implementation() { Multicast_PlaySkillQMontage(); }
void ATMCharacter_Warrior::Multicast_PlaySkillQMontage_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();

	if (SkillQMontage && GetMesh()->GetAnimInstance()) GetMesh()->GetAnimInstance()->Montage_Play(SkillQMontage);
}

/* ---------------------------------------------------------
 * W 스킬 (분노의 함성 - 자가 버프)
 * --------------------------------------------------------- */
void ATMCharacter_Warrior::InputSkillW(const FInputActionValue& Value) { Server_InputSkillW(); }
void ATMCharacter_Warrior::Server_InputSkillW_Implementation() { Multicast_PlaySkillWMontage(); }
void ATMCharacter_Warrior::Multicast_PlaySkillWMontage_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();

	if (SkillWMontage && GetMesh()->GetAnimInstance()) GetMesh()->GetAnimInstance()->Montage_Play(SkillWMontage);
}

/* ---------------------------------------------------------
 * E 스킬 (차지 스트라이크 - 마우스 방향 돌진)
 * --------------------------------------------------------- */
void ATMCharacter_Warrior::InputSkillE(const FInputActionValue& Value) { Server_InputSkillE(); }
void ATMCharacter_Warrior::Server_InputSkillE_Implementation() { Multicast_PlaySkillEMontage(); }
void ATMCharacter_Warrior::Multicast_PlaySkillEMontage_Implementation()
{

	if (SkillEMontage && GetMesh()->GetAnimInstance()) GetMesh()->GetAnimInstance()->Montage_Play(SkillEMontage);

	// 💡 [핵심] 캐릭터가 바라보는 정면(마우스/카메라 방향)으로 훅 날려보냅니다!
	FVector DashDirection = GetActorForwardVector();
	LaunchCharacter(DashDirection * SkillEDashSpeed, true, true);
}

/* ---------------------------------------------------------
 * R 스킬 (풀스윙 - 360도 연타)
 * --------------------------------------------------------- */
void ATMCharacter_Warrior::InputSkillR(const FInputActionValue& Value) { Server_InputSkillR(); }
void ATMCharacter_Warrior::Server_InputSkillR_Implementation() { Multicast_PlaySkillRMontage(); }
void ATMCharacter_Warrior::Multicast_PlaySkillRMontage_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();

	if (SkillRMontage && GetMesh()->GetAnimInstance()) GetMesh()->GetAnimInstance()->Montage_Play(SkillRMontage);
}

/* ---------------------------------------------------------
 * 스킬 충돌 및 버프 실행 (애님 노티파이 연동)
 * --------------------------------------------------------- */
void ATMCharacter_Warrior::ExecuteSkillQImpact() { if (HasAuthority()) ProcessSphereOverlap(); }
void ATMCharacter_Warrior::ExecuteSkillWImpact() { if (HasAuthority()) ProcessSkillWOverlap(); }
void ATMCharacter_Warrior::ExecuteSkillEImpact() { if (HasAuthority()) ProcessSkillEOverlap(); }
void ATMCharacter_Warrior::ExecuteSkillRImpact() { if (HasAuthority()) ProcessSkillROverlap(); }

// Q 스킬 판정 (슬로우 디버프 포함)
void ATMCharacter_Warrior::ProcessSphereOverlap()
{
	FVector CenterLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	FCollisionQueryParams Params(NAME_None, false, this);
	TArray<FOverlapResult> OverlapResults;

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(OverlapResults, CenterLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SkillQRadius), Params);

	if (bHasOverlap)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (IsValid(OverlappedActor) && OverlappedActor != this)
			{
				UGameplayStatics::ApplyDamage(OverlappedActor, SkillQDamage, GetController(), this, UDamageType::StaticClass());

				if (ACharacter* HitCharacter = Cast<ACharacter>(OverlappedActor))
				{
					if (HitCharacter->GetCharacterMovement())
					{
						float OriginalSpeed = HitCharacter->GetCharacterMovement()->MaxWalkSpeed;
						HitCharacter->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed * (1.0f - SkillQSlowModifier);

						FTimerHandle SlowTimerHandle;
						FTimerDelegate SlowDelegate = FTimerDelegate::CreateUObject(this, &ATMCharacter_Warrior::ResetTargetSpeed, HitCharacter, OriginalSpeed);
						GetWorld()->GetTimerManager().SetTimer(SlowTimerHandle, SlowDelegate, SkillQSlowDuration, false);
					}
				}
			}
		}
	}
}

void ATMCharacter_Warrior::ResetTargetSpeed(ACharacter* TargetCharacter, float OriginalSpeed)
{
	if (IsValid(TargetCharacter) && TargetCharacter->GetCharacterMovement())
	{
		TargetCharacter->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
	}
}

// W 스킬 판정 (순수 공/방 버프)
void ATMCharacter_Warrior::ProcessSkillWOverlap()
{
	if (StatComp)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(WBuffTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(WBuffTimerHandle, this, &ATMCharacter_Warrior::ResetWBuff, SkillWDuration, false);
		}
		else
		{
			StatComp->Defense += SkillWDefenseBuff;
			StatComp->BaseAttackPower += SkillWAttackBuff;

			GetWorld()->GetTimerManager().SetTimer(WBuffTimerHandle, this, &ATMCharacter_Warrior::ResetWBuff, SkillWDuration, false);
		}
	}
}

void ATMCharacter_Warrior::ResetWBuff()
{
	if (StatComp)
	{
		StatComp->Defense -= SkillWDefenseBuff;
		StatComp->BaseAttackPower -= SkillWAttackBuff;
	}
}

// E 스킬 판정 (돌진 후 도끼 타격)
void ATMCharacter_Warrior::ProcessSkillEOverlap()
{
	FVector CenterLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	FCollisionQueryParams Params(NAME_None, false, this);
	TArray<FOverlapResult> OverlapResults;

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(OverlapResults, CenterLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SkillERadius), Params);

	if (bHasOverlap)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (IsValid(OverlappedActor) && OverlappedActor != this)
			{
				UGameplayStatics::ApplyDamage(OverlappedActor, SkillEDamage, GetController(), this, UDamageType::StaticClass());
			}
		}
	}
}

// R 스킬 판정 (풀스윙 360도 연타)
void ATMCharacter_Warrior::ProcessSkillROverlap()
{
	FVector CenterLocation = GetActorLocation();
	FCollisionQueryParams Params(NAME_None, false, this);
	TArray<FOverlapResult> OverlapResults;

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(OverlapResults, CenterLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SkillRRadius), Params);

	if (bHasOverlap)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (IsValid(OverlappedActor) && OverlappedActor != this)
			{
				UGameplayStatics::ApplyDamage(OverlappedActor, SkillRDamage, GetController(), this, UDamageType::StaticClass());
			}
		}
	}
}