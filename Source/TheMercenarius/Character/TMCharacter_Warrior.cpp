#include "TMCharacter_Warrior.h"
#include "Game/TMStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h" 
#include "Animation/AnimInstance.h"
#include "Engine/OverlapResult.h" // 오버랩 결과를 담는 헤더 필수!
#include "Engine/World.h"
#include "DrawDebugHelpers.h" // 디버그 구체 시각화용 헤더
#include "Kismet/GameplayStatics.h"//데미지 시스템을 쓰기 위한 필수 헤더

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
		SkillQSlowModifier = 0.4f; // 40% 느려짐
		SkillQSlowDuration = 3.0f; // 3초 지속
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
// ---------------------------------------------------------
// Q 스킬 범위 판정 로직
// ---------------------------------------------------------
void ATMCharacter_Warrior::ExecuteSkillQImpact()
{
	ProcessSphereOverlap();
}

void ATMCharacter_Warrior::ProcessSphereOverlap()
{
	// 내 캐릭터 앞쪽으로 100만큼 떨어진 곳을 타격 중심점으로 잡습니다.
	FVector CenterLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	FCollisionQueryParams Params(NAME_None, false, this); // 나 자신은 타격에서 제외

	TArray<FOverlapResult> OverlapResults;

	// 반경(SkillQRadius) 내에 있는 폰(몬스터)들을 싹 다 감지합니다.
	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		CenterLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SkillQRadius),
		Params
	);

	// 눈으로 확인하기 위한 빨간 구체 생성 (서버에서만 보임)
	DrawDebugSphere(GetWorld(), CenterLocation, SkillQRadius, 16, FColor::Red, false, 3.0f);

	// 👇 [핵심] 여기서부터 데미지를 주는 후반부 로직입니다!
	if (bHasOverlap == true)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (IsValid(OverlappedActor) == true && OverlappedActor != this)
			{
				// 1. 화면에 로그를 띄우기
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
					FString::Printf(TEXT("[Hit - Server] %s to %f hit damage!"), *OverlappedActor->GetName(), SkillQDamage));

				// 2. ⚔️ 진짜 데미지를 보스에게 전달!
				UGameplayStatics::ApplyDamage(
					OverlappedActor,          // 맞는 애 (보스 쥐)
					SkillQDamage,             // 깎을 체력 (50.0f)
					GetController(),          // 때린 애의 조종자
					this,                     // 때린 애의 본체 (전사)
					UDamageType::StaticClass()// 데미지 종류
				);
			}
		}
	}
} // 👈 여기서 함수가 딱 한 번만 깔끔하게 닫힙니다! 이 밑으로는 아무것도 없어야 합니다.