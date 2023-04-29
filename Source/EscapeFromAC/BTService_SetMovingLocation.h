// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetMovingLocation.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPEFROMAC_API UBTService_SetMovingLocation : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SetMovingLocation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
