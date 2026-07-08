#include "TMCharacter_Warrior.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/TMStatComponent.h" // 실제 경로에 맞게 수정 필요!

ATMCharacter_Warrior::ATMCharacter_Warrior()
{
	PrimaryActorTick.bCanEverTick = false;

	/* 1. 기본 내장 컴포넌트 세팅 (캡슐 및 메시) */
	GetCapsuleComponent()->InitCapsuleSize(40.f, 90.f);

	// 언리얼 마네킹 기준 정석 위치 및 회전 세팅
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));

	/* 2. 쿼터뷰 카메라 세팅 */
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 2500.f;
	SpringArmComp->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	// 팁: 쿼터뷰에서는 지형지물에 의해 카메라가 갑자기 줌인되는 것을 막는 것이 좋습니다.
	SpringArmComp->bDoCollisionTest = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	/* 3. 스탯 컴포넌트 부착 및 전사 초기값 설정 */
	StatComp = CreateDefaultSubobject<UTMStatComponent>(TEXT("StatComp"));
	if (StatComp)
	{
		StatComp->MaxHealth = 150.0f;
		StatComp->Defense = 20.0f;
		StatComp->BaseAttackPower = 35.0f;
		StatComp->CriticalHitChance = 0.05f; // 묵직한 한 방 위주이므로 치명타는 낮게
		StatComp->MovementSpeed = 550.0f;   // 기본 600보다 살짝 느린 속도
	}

	// 언리얼 내장 무브먼트에도 스탯의 이동 속도를 동기화해 줍니다.
	GetCharacterMovement()->MaxWalkSpeed = 550.0f;
}

void ATMCharacter_Warrior::BeginPlay()
{
	Super::BeginPlay();
}
