#pragma once

#include "CoreMinimal.h"
#include "Character/TMPlayerBase.h" 
#include "InputActionValue.h"
#include "TMCharacter_Warrior.generated.h"

class UTMStatComponent;
class UInputAction;

UCLASS()
class THEMERCENARIUS_API ATMCharacter_Warrior : public ATMPlayerBase
{
	GENERATED_BODY()

public:
	ATMCharacter_Warrior();
	

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTMStatComponent> StatComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SkillQAction;

	void InputSkillQ(const FInputActionValue& Value);
};