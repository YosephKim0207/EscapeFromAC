// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_Character.h"
#include "A_EnemyDefault.generated.h"

DECLARE_DELEGATE(FNPCIsDead);
DECLARE_DELEGATE(FOnDamage);
DECLARE_DELEGATE_OneParam(FMoveTo, const FVector &TargetLoaction);
DECLARE_MULTICAST_DELEGATE(FOnStopFire);

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	EFighter,
	EChicken,
	ENeutral,
};

UCLASS()
class ESCAPEFROMAC_API AA_EnemyDefault : public AA_Character
{
	GENERATED_BODY()

public:
	AA_EnemyDefault();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void StopFire() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Patrol")
	TArray<class ATargetPoint*> TargetPointArray;

	UPROPERTY()
	EEnemyType EnemyType;

	UPROPERTY()
	int8 TargetPointArrayIndex = 0;

	FNPCIsDead NPCIsDead;
	
	FOnStopFire OnStopFire;

	FOnDamage OnDamage;

	FMoveTo MoveTo;

protected:
	virtual void DoAfterDead(const float& DeltaSecond) override;
	
	virtual void ImpulseToRagdoll(const FPointDamageEvent& PointDamageEvent) override;

	virtual void RightArmFire() override;

	virtual void LeftArmFire() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY()
	class AA_AIController* MyAIController;

	UPROPERTY()
	class AC_Player* TargetPlayer;
	
	UPROPERTY()
	FVector TargetPlayerLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Drop Item", meta = (AllowPrivateAccess = true))
	TArray<FDataTableRowHandle> DropInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Drop Item", meta = (AllowPrivateAccess = true))
	int32 DropInventoryVolume = 20;

	UPROPERTY()
	FVector BagDropLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float DestroyActorCountDown = 2.0f;

	UPROPERTY()
	bool bAIOnPossess = true;
	
	UFUNCTION()
	void DropBag(const FVector& ActorLocation);

	/**
	 * 1. Define Bag Level
	 * 0~59 : Level_1 (60%)
	 * 60~94 : Level_2 (35%)
	 * 95~99 : Levle_3 (15%)
	 */
	UFUNCTION()
	int8 GetBagLevel();
	
	/**
	 * Counts about making item by BagLevel
	 * Level_1 : 1개
	 * Level_2 : 3개
	 * Level_3 : 5개
	 */
	UFUNCTION()
	int8 GetItemCounts(const int8& BagLevel);
};
