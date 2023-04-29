// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "A_PoolManager.h"
#include "ACGameInstance.generated.h"

UCLASS()
class ESCAPEFROMAC_API UACGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UACGameInstance();

	UFUNCTION()
	TSubclassOf<class AC_Player> GetPlayer();
	
	UFUNCTION()
	class UDataTable* GetPickupDataTable();

	/**
	 * Get Row Names from Pickup Data Table
	 * @param ReturnItemCounts : Set Return Row Name Counts
	 * @param RepairKitRate : Pickup Item type's rate. ReturnItemCounts will divide by RepairKitRate, Round down
	 */
	UFUNCTION()
	TArray<FName> GetPickupDataTableRows(const int32& ReturnItemCounts, const float& RepairKitRate); 

	/** Return BP_Bag Instance */
	UFUNCTION()
	TSubclassOf<AActor> GetBag();

	UFUNCTION()
	AA_PoolManager* GetPoolManager();

	UPROPERTY()
	AA_PoolManager* PoolManager;
	
private:
	UPROPERTY()
	TSubclassOf<AC_Player> Player;
	
	UPROPERTY()
	UDataTable* PickupDataTable;

	UPROPERTY()
	TArray<FName> PickupItemRowNames;

	UPROPERTY()
	TSubclassOf<AActor> Bag;
	
	UPROPERTY()
	TSubclassOf<AA_PoolManager> PoolManagerClass;
};
