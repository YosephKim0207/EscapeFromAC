// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/DataTable.h"
#include "S_Inventory.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EModular,
	ERocket,
	ERepairKit,
};

UENUM(BlueprintType)
enum class EModular : uint8
{
	EHead,
	ELeftArm,
	ERightArm,
	EUpperBody,
	ELowerBody,
};

UENUM(BlueprintType)
enum class EBagType : uint8
{
	ECarrier,
	ERuckSack,
	EBackPack,
	ESack,
};

USTRUCT(Atomic, BlueprintType)
struct FPickupItemData:  public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Inventory Data")
	UTexture2D* ItemImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Inventory Data")
	FText PickupText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Inventory Data")
	FText ActionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Data")
	EItemType ItemType;

	// DataTableRowHandle for choose Pickup Item's Type and Stat
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Data")
	FDataTableRowHandle SelectItemStatDataTable;

	// Relate with Item Outline Design
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Item Outline")
	UStaticMesh* StaticMesh;
	
	// Relate with Item Outline Design
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Outline")
	class UMaterial* Material;
	
};

USTRUCT(Atomic, BlueprintType)
struct FModularItemStatData:  public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Outline")
	class USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data")
	EModular Parts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data")
	float MaxHP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	float HP = MaxHP;

	/* Modular Part's Weight. Affect to Rocket's Boost Heat and MaxSpeed
	 * If Character's Total Weight is over Rocket's MaxWeight, 
	 * Boost Heat will increase faster and MaxSpeed value will decrease.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float Weight;

	/* LowerBody's stat
	 * If Modular Character's Total Weight is more than MaxWeight, Move Speed will Decrease
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float MaxWeight;

	/* Only Right Arm and Left Hand use this value.
	 * Projectile's Damage
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float Damage;

	// TODO
	// 부위파괴 방어력으로 사용할지, UpperBody만 사용하는 토탈 방어력으로 사용할지 고민하기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float Defense;

	/* Term between continuous shoot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float ShootTerm;

	/* Value about Gun's Heat increases per projectile shoot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float GunHeatIncreaseValue;

	// If Gun's Heat is over this value, Can't shoot until Gun's Heat is zero
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float MaxGunOverHeatValue;

	// Gun Heat Cooling per Second Rate. Default Value is 1.0f
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float GunHeatCoolingRate = 1.0f;

	// Total time that Gun's Heat value become max to zero
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float GunOverHeatCoolTime;
};

USTRUCT(Atomic, BlueprintType)
struct FRepairItemStatData:  public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float RepairValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float RepairTime;
};

USTRUCT(Atomic, BlueprintType)
struct FRocketItemStatData:  public FTableRowBase
{
	GENERATED_BODY()

	// Relate with Rocket Booster's Max Speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float MaxSpeed;

	/* Relate with Character's Total Modular Weight and Booster's Max Speed
	 * If Character's Total Modular Weight is more than MaxWeight, Booster's MaxSpeed will decrease
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float MaxWeight;

	/* Rate about Rocket's Heat increases per Dash
	 * MaxBoostOverHeatValue * BoostHeatRate is total Increase value
	 * Recommend If Set BoostHeatRate is low, Set MaxBoostOverHeatValue is high 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float BoostHeatRate;

	/* Effect to Boost's cool time
	 * Cooling Function relate with MaxBoostOverHeatValue / OverHeatCoolTime
	 * Recommend If Set MaxBoostOverHeatValue is high, Set BoostHeatRate is low
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float MaxBoostOverHeatValue;

	/* Effect to Boost's cool time
	* Cooling Function relate with MaxBoostOverHeatValue / OverHeatCoolTime
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float MaxBoostOverHeatCoolingRate;
};

USTRUCT(Atomic, BlueprintType)
struct FBagData:  public FTableRowBase
{
	GENERATED_BODY()

	// Relate with Item Outline Design
	UPROPERTY(EditAnywhere, BlueprintReadOnly , Category = "Item Outline")
	UStaticMesh* StaticMesh;
	// Relate with Item Outline Design
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Outline")
	class UMaterial* Material;
};

USTRUCT(Atomic, BlueprintType)
struct FBodyPartsStruct
{
	GENERATED_BODY()

	// TODO
	// Make Modular Parts and will change each body parts
	// Each Item have unique Attack and Defense value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float AttackValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float DefenseValue;

	// Not Holding in this Struct, maybe out
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	float MaxGunHeatValue;
};

USTRUCT(Atomic, BlueprintType)
struct FRepairKitStruct
{
	GENERATED_BODY()

	// TODO
	// As Possible, Character will Damaged each modular and Repair each modular
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	EModular RepairTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Item Data")
	int32 RepairValue;
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPEFROMAC_API US_Inventory : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	US_Inventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
