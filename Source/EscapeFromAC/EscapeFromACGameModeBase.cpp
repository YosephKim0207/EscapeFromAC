// Copyright Epic Games, Inc. All Rights Reserved.


#include "EscapeFromACGameModeBase.h"

#include "ACGameInstance.h"
#include "Kismet/GameplayStatics.h"

AEscapeFromACGameModeBase::AEscapeFromACGameModeBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEscapeFromACGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	
}

TArray<FPickupItemData> AEscapeFromACGameModeBase::FillBag(const EBagType& BagType)
{
	TArray<FPickupItemData> Temp;
	EBagType ThisBagType = BagType;

	
	// TODO
	// Make variables for Set this bag

	

	// Set typical variable for set make items
	switch(ThisBagType)
	{
	case EBagType::ECarrier:
		break;

	case EBagType::ERuckSack:
		break;

	case EBagType::EBackPack:
		break;

	case EBagType::ESack:
		break;
	}

	return Temp;
}