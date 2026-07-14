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
#include "GameFramework/Character.h" // ACharacter 형변환용
#include "TimerManager.h"            //  3초 타이머용

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

		//R 스킬 기본값 
		SkillRRadius = 400.0f; // Q보다 더 넓은 360도 반경
		SkillRDamage = 20.0f;  // 연타로 때릴 것이므로 1타당 20
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
// W 스킬 (분노의 함성 )
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
// E 스킬 (차지 스트라이크 )
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
				// 👇 [추가] 3. 🐌 슬로우 디버프 적용!
				// 때린 대상이 '캐릭터' 형태인지 확인합니다. (보스 쥐는 캐릭터입니다)
				if (ACharacter* HitCharacter = Cast<ACharacter>(OverlappedActor))
				{
					// 캐릭터의 무브먼트 컴포넌트(이동 담당)가 있는지 확인
					if (HitCharacter->GetCharacterMovement())
					{
						// ① 원래 속도 기억하기
						float OriginalSpeed = HitCharacter->GetCharacterMovement()->MaxWalkSpeed;

						// ② 40% 느려진 속도 계산 (원래 속도 * 0.6)
						float SlowedSpeed = OriginalSpeed * (1.0f - SkillQSlowModifier);

						// ③ 보스 속도 강제로 깎기
						HitCharacter->GetCharacterMovement()->MaxWalkSpeed = SlowedSpeed;

						// ④ 로그 띄우기
						if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("[Debuff] 보스 이동속도 40% 감소!"));

						// ⑤ 3초(SkillQSlowDuration) 뒤에 원래 속도로 복구하는 예약 타이머 걸기!
						FTimerHandle SlowTimerHandle;
						FTimerDelegate SlowDelegate = FTimerDelegate::CreateUObject(this, &ATMCharacter_Warrior::ResetTargetSpeed, HitCharacter, OriginalSpeed);
						GetWorld()->GetTimerManager().SetTimer(SlowTimerHandle, SlowDelegate, SkillQSlowDuration, false);
					}
				}
			}
		}
	}
}
// ---------------------------------------------------------
// 디버프 타이머 복구 함수
// ---------------------------------------------------------
void ATMCharacter_Warrior::ResetTargetSpeed(ACharacter* TargetCharacter, float OriginalSpeed)
{
	// 3초라는 시간 동안 보스가 죽어서 시체가 사라졌을 수도 있으니 안전 검사(IsValid)를 합니다.
	if (IsValid(TargetCharacter) && TargetCharacter->GetCharacterMovement())
	{
		// 보스의 걷기 속도를 아까 기억해둔 원래 속도로 되돌립니다.
		TargetCharacter->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;

		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("[Debuff] 보스 이동속도 원상복구!"));
	}
}
// ---------------------------------------------------------
// R 스킬 범위 판정 로직 (풀스윙 360도)
// ---------------------------------------------------------
void ATMCharacter_Warrior::ExecuteSkillRImpact()
{
	if (HasAuthority())
	{
		ProcessSkillROverlap();
	}
}

void ATMCharacter_Warrior::ProcessSkillROverlap()
{
	// Q와 다르게 앞쪽이 아니라 '내 캐릭터의 중심'에서 구체가 터집니다 (360도 판정)
	FVector CenterLocation = GetActorLocation();
	FCollisionQueryParams Params(NAME_None, false, this);

	TArray<FOverlapResult> OverlapResults;

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		CenterLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SkillRRadius),
		Params
	);

	// 눈으로 확인하기 위한 보라색 구체 생성 (연타 확인을 위해 1초만 표시)
	DrawDebugSphere(GetWorld(), CenterLocation, SkillRRadius, 16, FColor::Magenta, false, 1.0f);

	if (bHasOverlap == true)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (IsValid(OverlappedActor) == true && OverlappedActor != this)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta,
					FString::Printf(TEXT("[Hit - Server] R 풀스윙 적중! %f 데미지"), SkillRDamage));

				UGameplayStatics::ApplyDamage(
					OverlappedActor,
					SkillRDamage,
					GetController(),
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
}