// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetEnemyType.generated.h"

/**
 * 
 */
UCLASS()
class ESCAPEFROMAC_API UBTService_SetEnemyType : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SetEnemyType();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
