// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S_Inventory.h"
#include "GameFramework/Actor.h"
#include "A_Item.generated.h"

UCLASS()
class ESCAPEFROMAC_API AA_Item : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AA_Item();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Detect Player area
	UPROPERTY(EditDefaultsOnly, Category = "Collsion")
	class USphereComponent* SphereComponent;

	// Set Item's figure
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Info")
	class UStaticMeshComponent* StaticMeshComponent;

	// Init Pickup Item's figure use by PickupItemData's StaticMesh and Material
	// virtual void PostInitializeComponents() override;
	
	// For detect Player is near / far
	// It will erase because line trace will check color change
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item InterAction")
	bool bIsInRange;

	// this field item is bag or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item InterAction")
	bool bIsBag;

	// this item's material color was change or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item InterAction")
	bool bIsColorChanged;

	// Detect Player and Turn On the Player's bIsLineTrace
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// Detect Player and Turn Off the Player's bIsLineTrace
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	// /** Pickup Item Data for Init Item's Figure and Status */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Info")
	// FPickupItemData PickupItemData;

	// DataTableRowHandle for choose 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info", meta = (ExposeOnSpawn = true))
	FDataTableRowHandle SelectItemDataTable;

	// DataTable for Main Category, Outline. Determined by SelectItemDataTable
	UPROPERTY(BlueprintReadWrite)
	const UDataTable* ItemDataTable;

	
	// UPROPERTY()
	// class UDataTable* ItemDataTable;

	/** Change Material's Scalar Parameter for Change Color for Highlight When Player Aimed
	 * @param	A Start Value
	 * @param	B Target Value
	 * @param	Alpha Target Delta Value Rate between Param A and B
	 * @param	TargetName Target Scalar Parameter's name in Material
	 */
	UFUNCTION()
	virtual void ChangeMaterialColor(const float& A, const float& B, const float& Alpha, const FName& TargetName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Item Info")
	void SetItemOutline();
	virtual void SetItemOutline_Implementation();
	

	UPROPERTY()
	FName ItemName;
	
	// Set Target Scalar Parameter's name in Material
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FName MaterialParameterName;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Return bIsBag
	UFUNCTION()
	bool GetIsBag();

	// Set this Item is aimed by player or not
	UFUNCTION()
	void SetIsInRange(const bool& NewState);

	// Return bIsInRange
	UFUNCTION()
	bool GetIsInRange();
};
