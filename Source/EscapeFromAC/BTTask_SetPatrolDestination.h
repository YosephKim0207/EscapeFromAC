// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetPatrolDestination.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPEFROMAC_API UBTTask_SetPatrolDestination : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetPatrolDestination();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
