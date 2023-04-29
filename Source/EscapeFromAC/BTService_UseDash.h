// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UseDash.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPEFROMAC_API UBTService_UseDash : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UseDash();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
