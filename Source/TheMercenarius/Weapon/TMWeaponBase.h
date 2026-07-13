// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TMWeaponBase.generated.h"

UCLASS()
class THEMERCENARIUS_API ATMWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATMWeaponBase();

	void SetWeaponCollision(bool bEnable);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;

	bool bIsAttacking = false;

	UPROPERTY()
	TArray<AActor*> HitActors;

	void ExecuteBoxTrace();

	//FName StartSocketName = TEXT("StartSocket");
	//FName EndSocketName = TEXT("EndSocket");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Collision")
	FVector BoxHalfSize = FVector(5.f, 15.f, 5.f);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Damage")
	float damage = 300.f;
};
