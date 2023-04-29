// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckArrive.h"
#include "A_AIController.h"
#include "A_EnemyDefault.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"

UBTService_CheckArrive::UBTService_CheckArrive()
{
	NodeName = TEXT("CheckArrive");
	Interval = 5.0f;
}

void UBTService_CheckArrive::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AA_EnemyDefault* NPC = Cast<AA_EnemyDefault>(OwnerComp.GetAIOwner()->GetPawn());
	if(NPC == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTService_CheckArrive : Cast To AA_EnemyDefault is Failed!"));
		return;
	}

	ATargetPoint* TargetPoint = Cast<ATargetPoint>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PatrolDestination")));
	if(TargetPoint == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTService_CheckArrive : Cast To TargetPoint is Failed!"));
		return;
	}

    if(TargetPoint->GetDistanceTo(NPC) <= 100.0f)
    {
    	int8 TargetArrayIndex = NPC->TargetPointArrayIndex;
    	TargetArrayIndex = ++TargetArrayIndex % NPC->TargetPointArray.Num();
    	NPC->TargetPointArrayIndex = TargetArrayIndex;
    }
}