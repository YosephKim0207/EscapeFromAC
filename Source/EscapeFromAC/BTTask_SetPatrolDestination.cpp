// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetPatrolDestination.h"
#include "A_AIController.h"
#include "A_EnemyDefault.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"

UBTTask_SetPatrolDestination::UBTTask_SetPatrolDestination()
{
	NodeName = TEXT("SetPatrolDestination");
}

EBTNodeResult::Type UBTTask_SetPatrolDestination::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AA_EnemyDefault* NPC = Cast<AA_EnemyDefault>(OwnerComp.GetAIOwner()->GetPawn());
	if(NPC == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_SetPatrolDestination : Cast To AA_EnemyDefault is Failed!"));
		return EBTNodeResult::Failed;
	}

	int8 TargetPointArrayIndex = NPC->TargetPointArrayIndex;
	AActor* TargetPoint = NPC->TargetPointArray[TargetPointArrayIndex];

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("PatrolDestination"), TargetPoint);
	
	return EBTNodeResult::Succeeded;
}
