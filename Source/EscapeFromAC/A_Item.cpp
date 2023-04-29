// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Item.h"

#include "C_Player.h"
#include "Components/SphereComponent.h"

// Sets default values
AA_Item::AA_Item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetSphereRadius(200.0f);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	SphereComponent->SetCollisionProfileName(TEXT("FieldItem"));

	bIsColorChanged = false;

	MaterialParameterName = TEXT("DeltaLerpValue");

	// Set Pickup AActor's figure
	// if(PickupItemData.StaticMesh && PickupItemData.Material)
	// {
	// 	StaticMeshComponent->SetStaticMesh(PickupItemData.StaticMesh);
	// 	StaticMeshComponent->SetMaterial(0, PickupItemData.Material);
	//
	// 	// ItemDataTable = SelectItemData.DataTable;
	// 	// FName ItemName = SelectItemData.RowName;
	// 	// FPickupItemData* ItemData = SelectItemData.GetRow<FPickupItemData>(ItemName, FString(""));
	// }
}

// Called when the game starts or when spawned
void AA_Item::BeginPlay()
{
	Super::BeginPlay();
	
}

// void AA_Item::PostInitializeComponents()
// {
// 	Super::PostInitializeComponents();
//
// 	// Set Pickup AActor's figure
// 	if(PickupItemData.StaticMesh && PickupItemData.Material)
// 	{
// 		StaticMeshComponent->SetStaticMesh(PickupItemData.StaticMesh);
// 		StaticMeshComponent->SetMaterial(0, PickupItemData.Material);
// 	}
// }

void AA_Item::ChangeMaterialColor(const float& A, const float& B, const float& Alpha, const FName& TargetName)
{
	StaticMeshComponent->SetScalarParameterValueOnMaterials(TargetName, FMath::Lerp(A, B, Alpha));
}

void AA_Item::SetItemOutline_Implementation()
{
	ItemDataTable = SelectItemDataTable.DataTable;
	ItemName = SelectItemDataTable.RowName;
}

// Called every frame˙
void AA_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsInRange && !bIsColorChanged)
	{
		ChangeMaterialColor(0.0f, 0.5f, 1.0f, MaterialParameterName);
		bIsColorChanged = true;
		UE_LOG(LogTemp, Warning, TEXT("Color Change"));
	}
	else if(!bIsInRange && bIsColorChanged)
	{
		ChangeMaterialColor(0.5f, 0.0f, 1.0f, MaterialParameterName);
		bIsColorChanged = false;
		UE_LOG(LogTemp, Warning, TEXT("Color Back"));

	}
}

bool AA_Item::GetIsBag()
{
	return bIsBag;
}

void AA_Item::SetIsInRange(const bool& NewState)
{
	bIsInRange = NewState;
}

bool AA_Item::GetIsInRange()
{
	return bIsInRange;
}


void AA_Item::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if(Player)
	{
		Player->SetLineTrace(true);
		UE_LOG(LogTemp, Log, TEXT("bIsLineTrace is True"));
	}
}



void AA_Item::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	AC_Player* Player = Cast<AC_Player>(OtherActor);
	if(Player)
	{
		Player->SetLineTrace(false);
		
		SetIsInRange(false);
	}
}

