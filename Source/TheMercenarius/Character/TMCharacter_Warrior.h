#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TMCharacter_Warrior.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UTMStatComponent;

UCLASS()
class THEMERCENARIUS_API ATMCharacter_Warrior : public ACharacter
{
	GENERATED_BODY()

public:
	ATMCharacter_Warrior();

protected:
	virtual void BeginPlay() override;

private:
	// 쿼터뷰를 위한 카메라 지지대
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComp;

	// 플레이어의 시야
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComp;

	// 전사의 스탯을 관리할 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTMStatComponent> StatComp;
};