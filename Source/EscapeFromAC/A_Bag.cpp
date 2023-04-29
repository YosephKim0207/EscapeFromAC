// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Bag.h"

#include "ACGameInstance.h"
#include "Kismet/GameplayStatics.h"

AA_Bag::AA_Bag()
{
	bIsBag = true;
}

void AA_Bag::BeginPlay()
{
	Super::BeginPlay();

	MakeRandomItemToInventory();
}

void AA_Bag::SetItemOutline_Implementation()
{
	if(SelectItemDataTable.DataTable != nullptr)
	{
		ItemDataTable = SelectItemDataTable.DataTable;
		ItemName = SelectItemDataTable.RowName;
	}
	

	if(ItemDataTable == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : DataTable is null"), *this->GetName());
		
		return;
	}

	const FBagData* BagItemData = ItemDataTable->FindRow<FBagData>(ItemName, "", true);

	if(BagItemData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDataTable::FindRow : specified no row for DataTable '%s'."), *this->GetName());
	}
	else
	{
		StaticMeshComponent->SetStaticMesh(BagItemData->StaticMesh);
		StaticMeshComponent->SetMaterial(0, BagItemData->Material);
	}
}

void AA_Bag::MakeRandomItemToInventory()
{
	
	int8 BagLevel = GetBagLevel();
	UE_LOG(LogTemp, Log, TEXT("%s : BagLevel_%d"), *GetName(), BagLevel);

	int8 ItemCounts = GetItemCounts(BagLevel);
	UE_LOG(LogTemp, Log, TEXT("%s : ItemCounts_%d"), *GetName(),ItemCounts);

	UACGameInstance* ACGameInstance = Cast<UACGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if(ACGameInstance)
	{
		UDataTable* PickupDataTable = ACGameInstance->GetPickupDataTable();
		TArray<FName> RowNameArray = ACGameInstance->GetPickupDataTableRows(ItemCounts, 0.5f);

		if(PickupDataTable && RowNameArray.Num())
		{
			for(int32 Index = 0; Index < ItemCounts; ++Index)
			{
				BagInventory[Index].DataTable = PickupDataTable;
				BagInventory[Index].RowName = RowNameArray[Index];

				UE_LOG(LogTemp, Log, TEXT("%s : Make Drop Item_%s"), *GetName(), *BagInventory[Index].RowName.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("A_Bag : Cast to UACGameInstance Fail!"));
	}
}

int8 AA_Bag::GetBagLevel()
{
	int32 Level1Cut = 60;
	int32 Level2Cut = 95;
	int32 Level3Cut = 99;
	
	int32 RandomNumber = FMath::RandRange(0, Level3Cut);
	UE_LOG(LogTemp, Log, TEXT("%s : RandomNumber = %d"), *GetName(), RandomNumber);

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

int8 AA_Bag::GetItemCounts(const int8& BagLevel)
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
