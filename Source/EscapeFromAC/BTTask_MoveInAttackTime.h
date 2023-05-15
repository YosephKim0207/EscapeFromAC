// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveInAttackTime.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPEFROMAC_API UBTTask_MoveInAttackTime : public UBTTaskNode
{
	GENERATED_BODY()

	
public:
	UBTTask_MoveInAttackTime();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool HasReachedToDestination(FVector& Origin, FVector& Destination);

	bool bIsReached = false;

	bool bIsJump = false;

	float AccumulateMoveTime = 0.0f;

	float StopTime = 0.0f;
};