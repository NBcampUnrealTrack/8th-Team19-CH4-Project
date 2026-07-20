#include "TMCharacter_Assassin.h"
#include "Game/TMStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h" 
#include "Animation/AnimInstance.h"
#include "Engine/OverlapResult.h" 
#include "Engine/World.h"
#include "DrawDebugHelpers.h" 
#include "Net/UnrealNetwork.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h" 
#include "TimerManager.h"

ATMCharacter_Assassin::ATMCharacter_Assassin()
{
	StatComp = CreateDefaultSubobject<UTMStatComponent>(TEXT("StatComp"));
	if (StatComp)
	{
		// 암살자 특화 스탯
		StatComp->MaxHealth = 90.0f;
		StatComp->Defense = 10.0f;
		StatComp->BaseAttackPower = 40.0f;
		StatComp->CriticalHitChance = 0.3f;
		StatComp->MovementSpeed = 700.0f;

		// Q 스킬 (맹독 찌르기 - 방깎)
		SkillQRadius = 250.0f;
		SkillQDamage = 35.0f;
		SkillQDefenseDebuff = 20.0f;
		SkillQDuration = 5.0f;

		// W 스킬 (그림자 은신 - 생존 및 이속)
		SkillWDuration = 3.0f;
		SkillWSpeedBuff = 300.0f;

		// E 스킬 (암살자의 스텝 - 초고속 돌진)
		SkillERadius = 200.0f;
		SkillEDamage = 30.0f;
		SkillEDashPower = 3500.0f;

		// R 스킬 (칼날 폭풍 - 다단 히트)
		SkillRRadius = 300.0f;
		SkillRDamage = 10.0f;
	}

	GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	bReplicates = true;
}

void ATMCharacter_Assassin::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATMCharacter_Assassin, bIsStealth);
}

void ATMCharacter_Assassin::BeginPlay()
{
	Super::BeginPlay();
}

void ATMCharacter_Assassin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (SkillQAction) EnhancedInputComponent->BindAction(SkillQAction, ETriggerEvent::Started, this, &ATMCharacter_Assassin::InputSkillQ);
		if (SkillWAction) EnhancedInputComponent->BindAction(SkillWAction, ETriggerEvent::Started, this, &ATMCharacter_Assassin::InputSkillW);
		if (SkillEAction) EnhancedInputComponent->BindAction(SkillEAction, ETriggerEvent::Started, this, &ATMCharacter_Assassin::InputSkillE);
		if (SkillRAction) EnhancedInputComponent->BindAction(SkillRAction, ETriggerEvent::Started, this, &ATMCharacter_Assassin::InputSkillR);
	}
}

/* ---------------------------------------------------------
 * 스킬 입력 및 네트워크 방송 (RPC)
 * --------------------------------------------------------- */
void ATMCharacter_Assassin::InputSkillQ(const FInputActionValue& Value) { Server_InputSkillQ(); }
void ATMCharacter_Assassin::Server_InputSkillQ_Implementation() { Multicast_PlaySkillQMontage(); }
void ATMCharacter_Assassin::Multicast_PlaySkillQMontage_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("[Assassin] Q 맹독 찌르기!"));
	if (SkillQMontage && GetMesh()->GetAnimInstance()) GetMesh()->GetAnimInstance()->Montage_Play(SkillQMontage);
}

void ATMCharacter_Assassin::InputSkillW(const FInputActionValue& Value) { Server_InputSkillW(); }
void ATMCharacter_Assassin::Server_InputSkillW_Implementation() { Multicast_PlaySkillWMontage(); }
void ATMCharacter_Assassin::Multicast_PlaySkillWMontage_Implementation()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Black, TEXT("[Assassin] W 그림자 은신!"));
	if (SkillWMontage && GetMesh()->GetAnimInstance()) GetMesh()->GetAnimInstance()->Montage_Play(SkillWMontage);
}
void ATMCharacter_Assassin::InputSkillE(const FInputActionValue& Value)
{
	// 💡 LaunchCharacter(강제 이동) 코드는 루트 모션을 위해 삭제했습니다.
	// 대신 서버에게 E 스킬을 쓴다고 알려줍니다.
	Server_InputSkillE();
}

void ATMCharacter_Assassin::Server_InputSkillE_Implementation()
{
	// 💡 여기서도 강제 이동 코드를 지웠습니다.
	// 모든 사람의 화면에 스킬 애니메이션(루트 모션 포함)을 재생하라고 명령합니다.
	Multicast_PlaySkillEMontage();
}


void ATMCharacter_Assassin::Multicast_PlaySkillEMontage_Implementation()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("[Assassin] E 암살자의 스텝!"));
	if (SkillEMontage && GetMesh()->GetAnimInstance()) GetMesh()->GetAnimInstance()->Montage_Play(SkillEMontage);
}

void ATMCharacter_Assassin::InputSkillR(const FInputActionValue& Value) { Server_InputSkillR(); }
void ATMCharacter_Assassin::Server_InputSkillR_Implementation() { Multicast_PlaySkillRMontage(); }
void ATMCharacter_Assassin::Multicast_PlaySkillRMontage_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[Assassin] R 칼날 폭풍!"));
	if (SkillRMontage && GetMesh()->GetAnimInstance()) GetMesh()->GetAnimInstance()->Montage_Play(SkillRMontage);
}

/* ---------------------------------------------------------
 * 스킬 충돌 및 버프 실행 (애님 노티파이 연동)
 * --------------------------------------------------------- */
void ATMCharacter_Assassin::ExecuteSkillQImpact() { if (HasAuthority()) ProcessSkillQOverlap(); }
void ATMCharacter_Assassin::ExecuteSkillWImpact() { if (HasAuthority()) ProcessSkillWOverlap(); }
void ATMCharacter_Assassin::ExecuteSkillEImpact() { if (HasAuthority()) ProcessSkillEOverlap(); }
void ATMCharacter_Assassin::ExecuteSkillRImpact() { if (HasAuthority()) ProcessSkillROverlap(); }

/* ---------------------------------------------------------
 * Q 스킬 (맹독 찌르기 - 데미지 및 보스 방어력 감소)
 * --------------------------------------------------------- */
void ATMCharacter_Assassin::ProcessSkillQOverlap()
{
	FVector CenterLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	FCollisionQueryParams Params(NAME_None, false, this);
	TArray<FOverlapResult> OverlapResults;

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(OverlapResults, CenterLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SkillQRadius), Params);
	DrawDebugSphere(GetWorld(), CenterLocation, SkillQRadius, 16, FColor::Green, false, 1.0f);

	if (bHasOverlap)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (IsValid(OverlappedActor) && OverlappedActor != this)
			{
				// 1. 일단 기본 데미지를 입힙니다.
				UGameplayStatics::ApplyDamage(OverlappedActor, SkillQDamage, GetController(), this, UDamageType::StaticClass());

				// 2. 💡 [핵심] 맞은 애가 누군진 모르겠지만, '스탯 컴포넌트(UTMStatComponent)'를 가지고 있는지 뒤져봅니다.
				UTMStatComponent* VictimStatComp = OverlappedActor->FindComponentByClass<UTMStatComponent>();

				if (VictimStatComp)
				{
					// 스탯 컴포넌트가 있다면 방어력을 깎습니다!
					float OriginalDefense = VictimStatComp->Defense;
					VictimStatComp->Defense -= SkillQDefenseDebuff;

					// 5초 뒤에 깎인 방어력을 원상복구시키는 타이머 작동
					FTimerHandle DebuffTimerHandle;
					FTimerDelegate DebuffDelegate = FTimerDelegate::CreateUObject(this, &ATMCharacter_Assassin::ResetBossDefense, VictimStatComp, OriginalDefense);
					GetWorld()->GetTimerManager().SetTimer(DebuffTimerHandle, DebuffDelegate, SkillQDuration, false);

					if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("[Debuff] 적 방어력 감소!"));
				}
			}
		}
	}
}

void ATMCharacter_Assassin::ResetBossDefense(UTMStatComponent* BossStatComp, float OriginalDefense)
{
	if (IsValid(BossStatComp))
	{
		BossStatComp->Defense = OriginalDefense;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("[Debuff] 보스 방어력 원상복구!"));
	}
}

/* ---------------------------------------------------------
 * W 스킬 (그림자 은신)
 * --------------------------------------------------------- */
void ATMCharacter_Assassin::ProcessSkillWOverlap()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(WBuffTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(WBuffTimerHandle, this, &ATMCharacter_Assassin::ResetWBuff, SkillWDuration, false);
	}
	else
	{
		bIsStealth = true;
		GetCharacterMovement()->MaxWalkSpeed += SkillWSpeedBuff;

		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Black, TEXT("[Buff] 은신 활성화! 이동속도 증가!"));
		GetWorld()->GetTimerManager().SetTimer(WBuffTimerHandle, this, &ATMCharacter_Assassin::ResetWBuff, SkillWDuration, false);
	}
}

void ATMCharacter_Assassin::ResetWBuff()
{
	bIsStealth = false;
	GetCharacterMovement()->MaxWalkSpeed -= SkillWSpeedBuff;
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("[Buff] 은신 종료!"));
}

/* ---------------------------------------------------------
 * E 스킬 (암살자의 스텝)
 * --------------------------------------------------------- */
void ATMCharacter_Assassin::ProcessSkillEOverlap()
{
	FVector CenterLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	FCollisionQueryParams Params(NAME_None, false, this);
	TArray<FOverlapResult> OverlapResults;

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(OverlapResults, CenterLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SkillERadius), Params);
	DrawDebugSphere(GetWorld(), CenterLocation, SkillERadius, 16, FColor::Cyan, false, 1.0f);

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

/* ---------------------------------------------------------
 * R 스킬 (칼날 폭풍)
 * --------------------------------------------------------- */
void ATMCharacter_Assassin::ProcessSkillROverlap()
{
	FVector CenterLocation = GetActorLocation();
	FCollisionQueryParams Params(NAME_None, false, this);
	TArray<FOverlapResult> OverlapResults;

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(OverlapResults, CenterLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SkillRRadius), Params);
	DrawDebugSphere(GetWorld(), CenterLocation, SkillRRadius, 16, FColor::Red, false, 1.0f);

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
