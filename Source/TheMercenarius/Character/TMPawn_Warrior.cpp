#include "TMPawn_Warrior.h"
#include "Game/TMStatComponent.h" // 스탯 컴포넌트 헤더 포함

ATMPawn_Warrior::ATMPawn_Warrior()
{
	// 전사 전용 스탯 컴포넌트 생성 (부모 코드를 건드리지 않고 자식에게만 부착!)
	statComponent = CreateDefaultSubobject<UTMStatComponent>(TEXT("WarriorStatComponent"));

	// 필요하다면 전사 캐릭터의 기본 스탯을 세팅할 수 있습니다.
	// (예: 전사니까 방어력과 체력을 다른 캐릭터보다 높게 초기화)
	if (statComponent)
	{
		statComponent->MaxHealth = 150.0f;
		statComponent->Defense = 20.0f;
		statComponent->BaseAttackPower = 35.0f;
		statComponent->CriticalHitChance = 0.05f; // 전사는 치명타보다 묵직한 한 방
		statComponent->MovementSpeed = 550.0f;   // 중갑을 입어서 조금 느림
	}
}

void ATMPawn_Warrior::BeginPlay()
{
	Super::BeginPlay(); // 부모 클래스의 BeginPlay를 반드시 호출해 주어야 안전합니다.
}