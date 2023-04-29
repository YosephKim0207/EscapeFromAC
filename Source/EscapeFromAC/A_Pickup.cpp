// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Pickup.h"
#include "S_Inventory.h"

// Sets default values
AA_Pickup::AA_Pickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsBag = false;

	// // Set Pickup AActor's figure
	// StaticMeshComponent->SetStaticMesh(PickupItemData.StaticMesh);
	// StaticMeshComponent->SetMaterial(0, PickupItemData.Material);
}

// Called when the game starts or when spawned
void AA_Pickup::BeginPlay()
{
	Super::BeginPlay();

	// Set the Item for Who They are
	// PickupItemData.Item = this;
}

void AA_Pickup::SetItemOutline_Implementation()
{
	Super::SetItemOutline_Implementation();

	if(ItemDataTable == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : DataTable is null"), *this->GetName());
		
		return;
	}

	const FPickupItemData* PickupItemData = ItemDataTable->FindRow<FPickupItemData>(ItemName, "", true);

	if(PickupItemData == nullptr)
	{
		// UE_LOG(LogTemp, Error, TEXT("UDataTable::FindRow : specified no row for DataTable '%s'."), this->GetName());
	}
	else
	{
		StaticMeshComponent->SetStaticMesh(PickupItemData->StaticMesh);
		StaticMeshComponent->SetMaterial(0, PickupItemData->Material);

		// TODO
		// Item Data 다른 값도 초기화
	}
}

// //
// void AA_Pickup::PostInitializeComponents()
// {
// 	Super::PostInitializeComponents();
//
// 	// Set Pickup AActor's figure
// 	StaticMeshComponent->SetStaticMesh(PickupItemData.StaticMesh);
// 	StaticMeshComponent->SetMaterial(0, PickupItemData.Material);
// }

// Called every frame
void AA_Pickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}