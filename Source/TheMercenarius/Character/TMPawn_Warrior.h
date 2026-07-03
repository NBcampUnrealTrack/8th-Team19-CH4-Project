#pragma once

#include "CoreMinimal.h"
#include "Character/TMCharacterBase.h" // 부모 클래스 헤더 포함
#include "TMPawn_Warrior.generated.h"

class UTMStatComponent;

UCLASS()
class THEMERCENARIUS_API ATMPawn_Warrior : public ATMCharacterBase
{
	GENERATED_BODY()

public:
	ATMPawn_Warrior();

protected:
	virtual void BeginPlay() override;

private:
	// 전사 캐릭터만의 고유한 스탯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTMStatComponent> statComponent;
};