// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TMCharacter_Hunter.h"
#include "Game/TMStatComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Weapon/TMGranade.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Controller/TMPlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/TMPlayerBase.h"
#include "Weapon/TMWeaponBase.h"

ATMCharacter_Hunter::ATMCharacter_Hunter() : useASkill(false), ACoolTime(30.f), QCoolTime(10.f), WCoolTime(5.f), ECoolTime(3.f), RCoolTime(20.f), bAIsOnCooldown(false), bQIsOnCooldown(false),
bWIsOnCooldown(false), bEIsOnCooldown(false), bRIsOnCooldown(false), ADuration(5.f), damage(60.f)
{
	StateComponent = CreateDefaultSubobject<UTMStatComponent>(TEXT("StateComponent"));

	if (StateComponent)
	{
		StateComponent->MaxHealth = 100.f + StateComponent->Shield;
		StateComponent->Defense = 10.f;
		StateComponent->BaseAttackPower = damage + StateComponent->BuffeAttackPower;
		StateComponent->CriticalHitChance = 0.7f;
		StateComponent->MovementSpeed = 800.f + StateComponent->BuffeMoveSpeed;


	}

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(RootComponent);
	AttackSphere->SetSphereRadius(AttackRange);
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATMCharacter_Hunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (QSkillAction)
			EnhancedInputComponent->BindAction(QSkillAction, ETriggerEvent::Started, this, &ATMCharacter_Hunter::InputSkillQ);

		if (WSkillAction)
			EnhancedInputComponent->BindAction(WSkillAction, ETriggerEvent::Started, this, &ATMCharacter_Hunter::InputSkillW);

		if (ESkillAction)
			EnhancedInputComponent->BindAction(ESkillAction, ETriggerEvent::Started, this, &ATMCharacter_Hunter::InputSkillE);

		if (ESkillAction)
			EnhancedInputComponent->BindAction(ESkillAction, ETriggerEvent::Completed, this, &ATMCharacter_Hunter::EndDesperado);

		if (RSkillAction)
			EnhancedInputComponent->BindAction(RSkillAction, ETriggerEvent::Started, this, &ATMCharacter_Hunter::InputSkillR);
		
		if(ASkillAction)
			EnhancedInputComponent->BindAction(ASkillAction, ETriggerEvent::Started, this, &ATMCharacter_Hunter::InputSkillA);

	}
}

void ATMCharacter_Hunter::InputSkillA(const FInputActionValue& Value)
{
	if (bAIsOnCooldown == true || useASkill == true)
	{
		return;
	}
	useASkill = true;
	StateComponent->BaseAttackPower = damage * 3.5f;

	GetWorld()->GetTimerManager().SetTimer(ADurationTimerHandle, this, &ATMCharacter_Hunter::ASkillFinished, ADuration, false);
}

void ATMCharacter_Hunter::InputSkillQ(const FInputActionValue& Value)
{
	ATMPlayerBase* PlayerBase = Cast<ATMPlayerBase>(this);
	if (bQIsOnCooldown == true || CurrentComboCount >= MaxComboCount)
	{
		return;
	}
	if (PlayerBase)
	{
		PlayerBase->bIsMovingToTarget = false;
	}
	ATMPlayerController* PlayerController = GetController<ATMPlayerController>();
	if (PlayerController)
	{
		FHitResult Hit;
		if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			FRotator TargetRotator = FRotationMatrix::MakeFromX(Hit.Location - GetActorLocation()).Rotator();
			TargetRotator.Pitch = 0;
			SetActorRotation(TargetRotator);
		}
	}
	LaunchCharacter(GetActorForwardVector() * 1500.f, true, true);

	//PlayAnimMontage 애니메이션 몽타주 들어갈 부분

	PerformSwiftShot();

	CurrentComboCount++;

	GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &ATMCharacter_Hunter::StartQCooldown, 1.f, false);
}

void ATMCharacter_Hunter::InputSkillW(const FInputActionValue& Value)
{
	ATMPlayerBase* PlayerBase = Cast<ATMPlayerBase>(this);
	if (bWIsOnCooldown == true)
	{
		return;
	}
	bWIsOnCooldown = true;
	if (PlayerBase)
	{
		PlayerBase->bIsMovingToTarget = false;
	}
	ATMPlayerController* PlayerController = Cast<ATMPlayerController>(GetController());
	FHitResult hitResult;
	if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, hitResult))
	{
		GranadeAttackLocation = hitResult.Location;

		float CurrentRange = FVector::Dist(GranadeAttackLocation, GetActorLocation());
		if (CurrentRange > 500.f)
		{
			bIsMovingToThrow = true;
		}
		else
		{
			PerformThrow(GranadeAttackLocation);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(WCooldownTimerHandle, this, &ATMCharacter_Hunter::WCoolDownFinished, WCoolTime, false);
}

void ATMCharacter_Hunter::InputSkillE(const FInputActionValue& Value)
{
	if (bEIsOnCooldown == true)
	{
		return;
	}
	ATMPlayerBase* MoveControll = Cast<ATMPlayerBase>(this);
	if (MoveControll)
	{
		MoveControll->bIsMovingToTarget = false;
	}
	ATMPlayerController* PlayerController = GetController<ATMPlayerController>();
	
	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(true);
	}
	GetWorld()->GetTimerManager().SetTimer(DesperadoTimerHandle, this, &ATMCharacter_Hunter::PerformDesperado, DesperadoDamageInterval, true);
	GetWorld()->GetTimerManager().SetTimer(DesperadoDurationTimerHandle, this, &ATMCharacter_Hunter::EndDesperado, DesperadoDuration, false);

}

void ATMCharacter_Hunter::InputSkillR(const FInputActionValue& Value)
{
	if (bRIsOnCooldown || bIsShotgunMasterActive)
	{
		return;
	}
	ATMPlayerBase* PlayerBase = Cast<ATMPlayerBase>(this);
	if (PlayerBase)
	{
		PlayerBase->bIsMovingToTarget = false;
	}
	ATMPlayerController* PlayerController = GetController<ATMPlayerController>();
	if (PlayerController)
	{
		FHitResult Hit;

		if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			FRotator TargetRotator = FRotationMatrix::MakeFromX(Hit.Location - GetActorLocation()).Rotator();

			TargetRotator.Pitch = 0.f;
			TargetRotator.Roll = 0.f;

			SetActorRotation(TargetRotator);
		}
	}

	bIsShotgunMasterActive = true;
	CurrentShotCount = 0;

	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(true);
	}

	//애니메이션 몽타주 재생 PlayAnimMontage();

	PerformShotgunShot();

	GetWorld()->GetTimerManager().SetTimer(
	ShotgunShotTimerHandle,
		this,
		&ATMCharacter_Hunter::PerformShotgunShot,
		ShotTimeInterval,
		true);
}


void ATMCharacter_Hunter::ASkillFinished()
{
	bAIsOnCooldown = true;
	useASkill = false;
	StateComponent->BaseAttackPower = damage;
	GetWorld()->GetTimerManager().SetTimer(ACooldownTimerHandle, this, &ATMCharacter_Hunter::ACoolDownFinished, ACoolTime, false);
}

void ATMCharacter_Hunter::ACoolDownFinished()
{
	bAIsOnCooldown = false;
}

void ATMCharacter_Hunter::StartQCooldown()
{
	CurrentComboCount = 0;
	bQIsOnCooldown = true;

	GetWorld()->GetTimerManager().SetTimer(QCooldownTimerHandle, this, &ATMCharacter_Hunter::QCoolDownFinished, QCoolTime, false);
}

void ATMCharacter_Hunter::QCoolDownFinished()
{
	bQIsOnCooldown = false;
}

void ATMCharacter_Hunter::WCoolDownFinished()
{
	bWIsOnCooldown = false;
}

void ATMCharacter_Hunter::ECoolDownFinished()
{
	bEIsOnCooldown = false;
}

void ATMCharacter_Hunter::RCoolDownFinished()
{
	bRIsOnCooldown = false;
}

void ATMCharacter_Hunter::PerformSwiftShot()
{
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackSphere->UpdateOverlaps();

	TArray<AActor*> OverlappingActors;
	AttackSphere->GetOverlappingActors(OverlappingActors);

	FVector Forward = GetActorForwardVector();
	FVector ActorLocation = GetActorLocation();

	for (AActor* Target : OverlappingActors)
	{
		if (!Target || Target == this || Target->ActorHasTag("Team") || Target->IsA(ATMWeaponBase::StaticClass()))
		{
			continue;
		}
		FVector DirToTarget = (Target->GetActorLocation() - ActorLocation).GetSafeNormal();

		if(FVector::DotProduct(Forward, DirToTarget) > 0.0f)
		{
			UGameplayStatics::ApplyDamage(Target, StateComponent->BaseAttackPower * 2.5f, GetController(), this, UDamageType::StaticClass());


		}
	}
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATMCharacter_Hunter::PerformThrow(FVector TargetLocation)
{
	FVector LookDir = (TargetLocation - GetActorLocation()).GetSafeNormal2D();
	FRotator LookRot = LookDir.Rotation();
	SetActorRotation(LookRot);

	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	ATMGranade* Granade = GetWorld()->SpawnActor<ATMGranade>(GranadeClass, SpawnLocation, GetActorRotation());
	if (Granade && Granade->ProjectileMovement)
	{
		Granade->ExplosionDamage = StateComponent->BaseAttackPower;
		FVector LaunchVelocity;
		float GravityScale = 5.f;
		float GravityOverride = -980.f * GravityScale;


		bool bSuccess = UGameplayStatics::SuggestProjectileVelocity(
			this,
			LaunchVelocity,
			SpawnLocation,
			GranadeAttackLocation,
			1500.f,
			true,
			0.f,
			GravityOverride,
			ESuggestProjVelocityTraceOption::DoNotTrace
		);
			Granade->CollisionComponent->IgnoreActorWhenMoving(this, true);
			Granade->ProjectileMovement->Velocity = LaunchVelocity;

			Granade->ProjectileMovement->ProjectileGravityScale = GravityScale;
	}
}

void ATMCharacter_Hunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMovingToThrow)
	{
		float Distance = FVector::Dist2D(GetActorLocation(), GranadeAttackLocation);
		if (Distance <= 500.f)
		{
			bIsMovingToThrow = false;
			PerformThrow(GranadeAttackLocation);
		}
		else
		{
			FVector Direction = (GranadeAttackLocation - GetActorLocation()).GetSafeNormal2D();
			AddMovementInput(Direction, 1.f);
		}
	}
}

void ATMCharacter_Hunter::PerformDesperado()
{
	FVector CenterLocation = GetActorLocation();

	TArray<FHitResult> HitResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(DesperadoRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);


	bool bHasHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		CenterLocation,
		CenterLocation,
		FQuat::Identity,
		ECC_Pawn,
		SphereShape,
		QueryParams
	);

	if (bHasHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* Target = Hit.GetActor();
			if (!Target || Target->IsA(ATMPlayerBase::StaticClass()) || Target->IsA(ATMWeaponBase::StaticClass()))
			{
				continue;
			}
			UGameplayStatics::ApplyDamage(
				Target,
				StateComponent->BaseAttackPower * 0.4f,
				GetController(),
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

void ATMCharacter_Hunter::EndDesperado()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(DesperadoTimerHandle))
	{
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(DesperadoTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(DesperadoDurationTimerHandle);
	bEIsOnCooldown = true;
	ATMPlayerController* PlayerController = GetController<ATMPlayerController>();
	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(false);
	}
	GetWorld()->GetTimerManager().SetTimer(ECooldownTimerHandle, this, &ATMCharacter_Hunter::ECoolDownFinished, ECoolTime, false);
}

void ATMCharacter_Hunter::PerformShotgunShot()
{
	CurrentShotCount++;

	FVector CameraOrActorLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();

	TArray<FHitResult> HitResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(ShotgunRange);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);


	bool bHasHit = GetWorld()->SweepMultiByChannel(
		HitResults, CameraOrActorLocation, CameraOrActorLocation, FQuat::Identity, ECC_Pawn, SphereShape, QueryParams
	);

	if (bHasHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* Target = Hit.GetActor();
			if (!Target || Target->IsA(ATMPlayerBase::StaticClass()) || Target->IsA(ATMWeaponBase::StaticClass()))
			{
				continue;
			}

			FVector DirToTarget = (Target->GetActorLocation() - CameraOrActorLocation).GetSafeNormal();
			float DotProduct = FVector::DotProduct(ForwardVector, DirToTarget);

			if (DotProduct >= ShotgunAngleCos)
			{
				UGameplayStatics::ApplyDamage(
					Target,
					StateComponent->BaseAttackPower * 3.5f,
					GetController(),
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
	if (CurrentShotCount >= MaxNormalShots)
	{
		GetWorld()->GetTimerManager().ClearTimer(ShotgunShotTimerHandle);

		GetWorld()->GetTimerManager().SetTimer(
			ShotgunFinisherTimerHandle,
			this,
			&ATMCharacter_Hunter::PerformShotgunFinisher,
			0.5f,
			false
		);
	}
}

void ATMCharacter_Hunter::PerformShotgunFinisher()
{
	FVector CameraOrActorLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();

	float FinisherRange = 650.f;
	TArray<FHitResult> HitResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(FinisherRange);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHasHit = GetWorld()->SweepMultiByChannel(
		HitResults, CameraOrActorLocation, CameraOrActorLocation, FQuat::Identity, ECC_Pawn, SphereShape, QueryParams
	);

	if (bHasHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* Target = Hit.GetActor();
			if (!Target || Target->IsA(ATMPlayerBase::StaticClass()) || Target->IsA(ATMWeaponBase::StaticClass()))
			{
				continue;
			}

			FVector DirToTarget = (Target->GetActorLocation() - CameraOrActorLocation).GetSafeNormal();
			float DotProduct = FVector::DotProduct(ForwardVector, DirToTarget);

			if (DotProduct >= ShotgunAngleCos)
			{
				UGameplayStatics::ApplyDamage(
					Target,
					StateComponent->BaseAttackPower * 10.f,
					GetController(),
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
	EndShotgunMaster();
}

void ATMCharacter_Hunter::EndShotgunMaster()
{
	if (!bIsShotgunMasterActive)
	{
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(ShotgunShotTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ShotgunFinisherTimerHandle);

	bIsShotgunMasterActive = false;
	bRIsOnCooldown = true;

	ATMPlayerController* PlayerController = GetController<ATMPlayerController>();
	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(false);
	}
	GetWorld()->GetTimerManager().SetTimer(
		RCooldownTimerHandle,
		this,
		&ATMCharacter_Hunter::RCoolDownFinished,
		RCoolTime,
		false
	);
}