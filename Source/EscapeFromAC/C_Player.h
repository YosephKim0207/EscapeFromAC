// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_Character.h"
#include "S_Inventory.h"
#include "C_Player.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FPlayerIsDead)

UCLASS()
class ESCAPEFROMAC_API AC_Player : public AA_Character
{
	GENERATED_BODY()

public:
	AC_Player();

	void virtual BeginPlay() override;

	/** Set bIsLineTrace For Interaction with Item
	 * @param NewState	Update bIsLineTrace, True makes Line Trace which interaction with Field Item  
	 */
	UFUNCTION(BlueprintCallable)
	void SetLineTrace(bool NewState);

	UFUNCTION(BlueprintCallable)
	void CheckFieldItem();

	UPROPERTY(BlueprintReadOnly, Category = "Item InterAction")
	bool bIsBag;

	FPlayerIsDead PlayerIsDead;

protected:
	virtual void RightArmFire() override;

	virtual void LeftArmFire() override;

	virtual void DoAfterDead(const float& DeltaSecond) override;
	
private:
	// // Player's Default Game Widget
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameHUD", meta = (AllowPrivateAccess = true))
	// TSubclassOf<UUserWidget> GameHUDSource;
	//
	// // Player's Default Game Widget
	// UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "GameHUD", meta = (AllowPrivateAccess = true))
	// UUserWidget* GameHUD;
	
	/** CameraBoom that Camera is sticked */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = true))
	class USpringArmComponent* SpringArmComponent;
	
	/** Main Camera */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = true))
	class UCameraComponent* Camera;

	/** Turn On/Off the Line Trace for Interaction with Items */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item InterAction", meta = (AllowPrivateAccess = true))
	bool bIsLineTrace;

	UPROPERTY()
	AController* PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Item InterAction", meta = (AllowPrivateAccess = true))
	class AA_Item* Item;

	UPROPERTY(BlueprintReadWrite, Category = "Item InterAction", meta = (AllowPrivateAccess = true))
	bool bIsItemEmphasis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item InterAction", meta = (AllowPrivateAccess = true))
	float ItemCheckRange = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float ShootRange = 5000.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float RespawnTime = 3.0f;

	UPROPERTY()
	bool bControllerIsAttached = true;
	
	// UPROPERTY(BlueprintReadWrite, Category = "ItemInterAction", meta = (AllowPrivateAccess = true))
	// TArray<FPickupItemData> SelectedBagInventory;

private:
	// virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
};
