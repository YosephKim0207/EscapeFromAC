// Fill out your copyright notice in the Description page of Project Settings.


#include "A_EnemyDefault.h"

#include "ACGameInstance.h"
#include "A_AIController.h"
#include "BrainComponent.h"
#include "C_Player.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"

AA_EnemyDefault::AA_EnemyDefault()
{
	PrimaryActorTick.bCanEverTick = true;	
	// TODO
	// AI Test
	AIControllerClass = AA_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	/**
	 * Set EnemyType
	 * 0~6 : EFighter_70%
	 * 7~8 : EChicken_20%
	 * 9 : ENeutral_10%
	 */
	int8 EnemyTypeRandSetValue = FMath::RandRange(0, 9);
	
	if(EnemyTypeRandSetValue <= 6)
	{
		EnemyType = EEnemyType::EFighter;
		UE_LOG(LogTemp, Log, TEXT("EnemyType : Fighter"));
	}
	else if(EnemyTypeRandSetValue <= 8)
	{
		EnemyType = EEnemyType::EChicken;
		UE_LOG(LogTemp, Log, TEXT("EnemyType : Chicken"));
	}
	else if(EnemyTypeRandSetValue <= 9)
	{
		EnemyType = EEnemyType::ENeutral;
		UE_LOG(LogTemp, Log, TEXT("EnemyType : Neutral"));
	}
}

void AA_EnemyDefault::BeginPlay()
{
	Super::BeginPlay();

	AController* Controller = GetController();
	if(Controller)
	{
		MyAIController = Cast<AA_AIController>(Controller);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Get AController Fail!"), *GetName());
	}
	
	if(MyAIController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Cast to AA_AIController Fail!"), *GetName());
	}
}

void AA_EnemyDefault::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(MyAIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : MyAIController is nullPtr!"), *GetName());
		return;
	}
	
	if(MyAIController->bIsDetectPlayer)
	{
		if(MyAIController->GetBlackboardComponent()->GetValueAsBool(TEXT("bOnFight")) && (MyAIController->TargetPlayer != nullptr))
		{
			// NPC Look Player
			FVector NPCLocation = GetActorLocation();
			TargetPlayerLocation = MyAIController->TargetPlayer->GetActorLocation();
			FVector PlayerDirection = UKismetMathLibrary::GetDirectionUnitVector(NPCLocation, TargetPlayerLocation);
			PlayerDirection.Normalize();
			SetActorRotation(PlayerDirection.Rotation());
		}
	}
	
	if((bIsLeftArmOnFire || bIsRightArmOnFire) && !MyAIController->GetBlackboardComponent()->GetValueAsBool(TEXT("bOnFight")))
	{
		StopFire();
	}
}

void AA_EnemyDefault::StopFire()
{
	Super::StopFire();

	OnStopFire.Broadcast();
}

void AA_EnemyDefault::DoAfterDead(const float& DeltaSecond)
{
	if(bAIOnPossess)
	{
		bAIOnPossess = false;
		MyAIController->GetBrainComponent()->StopLogic(TEXT("DEAD"));
		MyAIController->OnUnPossess();
		UE_LOG(LogTemp, Log, TEXT("%s : UnPossess!"), *GetName());
	}
	
	if(!bAIOnPossess && DestroyActorCountDown > 0.0f)
	{
		DestroyActorCountDown -= DeltaSecond;
	}
	else
	{
		DropBag(BagDropLocation);
		
		UE_LOG(LogTemp, Log, TEXT("%s : Destroy!"), *GetName());
		Destroy();
	}
}

void AA_EnemyDefault::ImpulseToRagdoll(const FPointDamageEvent& PointDamageEvent)
{
	Super::ImpulseToRagdoll(PointDamageEvent);

	BagDropLocation = GetActorLocation() + FVector(0, 0, 5.0f);
	
	if(NPCIsDead.IsBound())
	{
		NPCIsDead.Execute();
	}
}

void AA_EnemyDefault::RightArmFire()
{
	if(SKM_UpperBody->GetSocketByName(TEXT("RightHandProjectileRespawnLocation")))
	{
		ProjectileRespawnLocation = SKM_UpperBody->GetSocketLocation(TEXT("RightHandProjectileRespawnLocation"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player : No Projectile Respawn Socket"));
	}

	TargetPlayer = Cast<AC_Player>(MyAIController->TargetPlayer);

	if(TargetPlayer)
	{
		float TargetHeight= TargetPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f;
		TargetHeight = FMath::RandRange(0.0f, TargetHeight);
		TargetPlayerLocation.Z = TargetHeight;
	}
	
	ProjectileShootRotator = (TargetPlayerLocation - ProjectileRespawnLocation).Rotation();
	
	Super::RightArmFire();
}

void AA_EnemyDefault::LeftArmFire()
{
	if(SKM_UpperBody->GetSocketByName(TEXT("LeftHandProjectileRespawnLocation")))
	{
		ProjectileRespawnLocation = SKM_UpperBody->GetSocketLocation(TEXT("LeftHandProjectileRespawnLocation"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player : No Projectile Respawn Socket"));
	}
	
	TargetPlayer = Cast<AC_Player>(MyAIController->TargetPlayer);

	if(TargetPlayer)
	{
		float TargetHeight= TargetPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.0f;
		TargetHeight = FMath::RandRange(0.0f, TargetHeight);
		TargetPlayerLocation.Z = TargetHeight;
	}
	ProjectileShootRotator = (TargetPlayerLocation - ProjectileRespawnLocation).Rotation();
	
	Super::LeftArmFire();
}

float AA_EnemyDefault::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(OnDamage.IsBound())
	{
		OnDamage.Execute();
	}

	if(!MyAIController->GetBlackboardComponent()->GetValueAsBool(TEXT("bIsDetectPlayer")))
	{
		if(MoveTo.IsBound())
		{	FVector UnKnownPlayerLocation = EventInstigator->GetPawn()->GetActorLocation();
			
			MoveTo.Execute(UnKnownPlayerLocation);
			UE_LOG(LogTemp, Log, TEXT("Executr Delegete : MoveTo"));
			
			
		}
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AA_EnemyDefault::DropBag(const FVector& ActorLocation)
{
	UACGameInstance* ACGameInstance = Cast<UACGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	TSubclassOf<AActor> Bag = ACGameInstance->GetBag();
	if(Bag)
	{
		UWorld* World = GetWorld();
		FRotator Rotator;
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		FVector BagRespwanLoccation = GetActorLocation();
		BagRespwanLoccation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		World->SpawnActor<AActor>(Bag, BagRespwanLoccation, Rotator, SpawnParameters);
	}
}

int8 AA_EnemyDefault::GetBagLevel()
{
	{
		int32 Level1Cut = 60;
		int32 Level2Cut = 95;
		int32 Level3Cut = 99;
		
		int32 RandomNumber = FMath::RandRange(0, Level3Cut);
		if(RandomNumber < Level1Cut)
		{
			return 1;
		}

		if(RandomNumber < Level2Cut)
		{
			return 2;
		}

		return 3;
	}
}

int8 AA_EnemyDefault::GetItemCounts(const int8& BagLevel)
{
	switch (BagLevel)
	{
	case 1:
		return 1;

	case 2:
		return 3;
		
	default:
		return 5;
	}
}
