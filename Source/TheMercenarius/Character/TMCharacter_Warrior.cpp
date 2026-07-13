#include "TMCharacter_Warrior.h"
#include "Game/TMStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h" 
#include "Animation/AnimInstance.h"
#include "Engine/OverlapResult.h" 
#include "Engine/World.h"
#include "DrawDebugHelpers.h" 
#include "Net/UnrealNetwork.h" //네트워크 동기화를 위한 필수 헤더

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

	//이 캐릭터가 멀티플레이(네트워크)에서 복제되도록 허락합니다.
	bReplicates = true;
}

//네트워크 변수 동기화를 위해 반드시 필요한 기본 세팅 함수입니다.
void ATMCharacter_Warrior::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 향후 체력(Health) 같은 변수를 동기화할 때 여기에 등록합니다.
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
	// 💡 [변경] 내 컴퓨터에서 바로 실행하지 않고, 서버에게 Q 스킬을 쓰겠다고 요청합니다!
	Server_InputSkillQ();
}

// 💡 [추가] 서버가 클라이언트의 요청을 받는 곳
void ATMCharacter_Warrior::Server_InputSkillQ_Implementation()
{
	// 서버가 확인 후, 접속한 모든 사람에게 "Q 애니메이션 재생해!" 라고 방송(Multicast)합니다.
	Multicast_PlaySkillQMontage();
}

// 💡 [추가] 서버의 방송을 듣고 접속한 모든 플레이어의 화면에서 실행되는 곳
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
	// 💡 [핵심] 데미지 판정은 핵 방지를 위해 오직 서버(Authority)에서만 실행합니다!
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

	// 이 빨간 구체는 서버 컴퓨터 화면(또는 서버 로그)에서만 그려집니다.
	DrawDebugSphere(GetWorld(), CenterLocation, SkillQRadius, 16, FColor::Red, false, 3.0f);

	if (bHasOverlap == true)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (IsValid(OverlappedActor) == true && OverlappedActor != this)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
					FString::Printf(TEXT("[Hit - Server] %s"), *OverlappedActor->GetName()));
			}
		}
	}
}