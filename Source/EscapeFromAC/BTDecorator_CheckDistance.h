// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckDistance.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPEFROMAC_API UBTDecorator_CheckDistance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckDistance();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
