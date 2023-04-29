// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_Pickup.h"
#include "A_Bag.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPEFROMAC_API AA_Bag : public AA_Pickup
{
	GENERATED_BODY()

public:
	AA_Bag();

	virtual void BeginPlay() override;

protected:
	virtual void SetItemOutline_Implementation() override;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BagInfo", meta = (AllowPrivateAccess = true))
	int32 BagVolume;
	
	UPROPERTY(BlueprintReadWrite, Category = "BagInfo", meta = (AllowPrivateAccess = true))
	TArray<FDataTableRowHandle> BagInventory;

	UFUNCTION()
	void MakeRandomItemToInventory();

	/**
	 * 1. Define Bag Level
	 * 0~59 : Level_1 (60%)
	 * 60~89 : Level_2 (35%)
	 * 90~99 : Levle_3 (5%)
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
