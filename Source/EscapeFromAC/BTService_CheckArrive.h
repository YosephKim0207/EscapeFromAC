// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckArrive.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPEFROMAC_API UBTService_CheckArrive : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckArrive();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
