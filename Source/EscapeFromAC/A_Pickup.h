// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_Item.h"
#include "S_Inventory.h"
#include "A_Pickup.generated.h"

UCLASS()
class ESCAPEFROMAC_API AA_Pickup : public AA_Item
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AA_Pickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetItemOutline_Implementation() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
