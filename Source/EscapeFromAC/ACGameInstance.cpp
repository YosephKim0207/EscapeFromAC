// Fill out your copyright notice in the Description page of Project Settings.


#include "ACGameInstance.h"

#include "A_PoolManager.h"
#include "C_Player.h"
#include "ToolBuilderUtil.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

UACGameInstance::UACGameInstance()
{
	FString PlayerBPPath = TEXT("Blueprint'/Game/BluePrint/BP_Player.BP_Player'");
	static ConstructorHelpers::FObjectFinder<UBlueprint> BP_Player(*PlayerBPPath);
	if(BP_Player.Succeeded())
	{
		Player = BP_Player.Object->GeneratedClass;
	}
	
	FString PickupDataTablePath = TEXT("DataTable'/Game/BluePrint/DataTable/DT_PickupItemData.DT_PickupItemData'");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Pickup(*PickupDataTablePath);
	if(DT_Pickup.Succeeded())
	{
		PickupDataTable = DT_Pickup.Object;
		PickupItemRowNames = PickupDataTable->GetRowNames();
	}

	FString BagPath = TEXT("Blueprint'/Game/BluePrint/PickupItem/BP_Bag.BP_Bag'");
	static ConstructorHelpers::FObjectFinder<UBlueprint> BP_Bag(*BagPath);
	if(BP_Bag.Succeeded())
	{
		Bag = BP_Bag.Object->GeneratedClass;
	}

	
	FString PoolManagerPath = TEXT("Class'/Script/EscapeFromAC.A_PoolManager'");
	static ConstructorHelpers::FClassFinder<AA_PoolManager> CLS_PoolManager(*PoolManagerPath);
	if(CLS_PoolManager.Succeeded())
	{
		PoolManagerClass = CLS_PoolManager.Class;
	}
}


TSubclassOf<AC_Player> UACGameInstance::GetPlayer()
{
	return Player;
}

UDataTable* UACGameInstance::GetPickupDataTable()
{
	return PickupDataTable;
}

TArray<FName> UACGameInstance::GetPickupDataTableRows(const int32& ReturnItemCounts, const float& RepairKitRate)
{
	
	TArray<FName> ReturnItemNames;
	int32 RepairKitCounts = ReturnItemCounts * RepairKitRate;
	
	
	//Check Data Table and RepairKit type's number
	int32 RepairKitTypeCounts = 1;
	
	for(int32 index = 0; index < RepairKitCounts; ++index)
	{
		int32 RandomRepairKit = FMath::RandRange(0, RepairKitTypeCounts - 1);
		FName RowName = PickupItemRowNames[RandomRepairKit];
		ReturnItemNames.Add(RowName);
	}

	int32 ModularItemCounts = ReturnItemCounts - RepairKitCounts;
	int32 TotalPickupItemCounts = PickupItemRowNames.Num();
	int32 ModularItemStartIndex = RepairKitTypeCounts;
	
	for(int32 index = 0; index < ModularItemCounts; ++index)
	{
		int32 RandomRepairKit = FMath::RandRange(ModularItemStartIndex, TotalPickupItemCounts - 1);
		FName RowName = PickupItemRowNames[RandomRepairKit];
		ReturnItemNames.Add(RowName);
	}
	
	return ReturnItemNames;
}

AA_PoolManager* UACGameInstance::GetPoolManager()
{
	
	UWorld* World = GetWorld();
	if(World)
	{
		if(PoolManager == nullptr)
		{
			AA_PoolManager* TempPoolManager = Cast<AA_PoolManager>(World->SpawnActor(PoolManagerClass));

			UE_LOG(LogTemp, Warning, TEXT("ACGameInstance : Make PoolManager Success!"));

			PoolManager = TempPoolManager;
			
			return PoolManager;
		}
		
		UE_LOG(LogTemp, Log, TEXT("ACGameInstance : Return PoolManager Success!"));
		
		return PoolManager;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ACGameInstance : GetPool doesn't have World!"));

	return nullptr;
}

TSubclassOf<AActor> UACGameInstance::GetBag()
{
	return Bag;
}