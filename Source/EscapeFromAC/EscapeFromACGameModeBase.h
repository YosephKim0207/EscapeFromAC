// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "S_Inventory.h"
#include "GameFramework/GameModeBase.h"
#include "EscapeFromACGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPEFROMAC_API AEscapeFromACGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEscapeFromACGameModeBase();

	virtual void BeginPlay() override;
	
	// virtual void Tick(float DeltaSeconds) override;

	// UFUNCTION(BlueprintCallable)
	// class UUserWidget* GetHUD();

	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	// TArray<FPickupItemData> PlayerInventory;

	UFUNCTION(BlueprintCallable)
	TArray<FPickupItemData> FillBag(const EBagType& BagType);
	
protected:
	// /** Widget Which on HUD Screen */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
	// TSubclassOf<class UUserWidget> HUDWidgetClass;
	//
	// UPROPERTY()
	// class UUserWidget* CurWidget;
};
