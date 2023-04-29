// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UseDash.h"

#include "AIController.h"
#include "A_EnemyDefault.h"

UBTService_UseDash::UBTService_UseDash()
{
	NodeName = TEXT("Use Dash");
	Interval = 3.0f;
}

void UBTService_UseDash::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AA_EnemyDefault* NPC = Cast<AA_EnemyDefault>(OwnerComp.GetAIOwner()->GetPawn());

	if(NPC)
	{
		NPC->Dash();
		Interval = FMath::RandRange(1, 3);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTService_UseBooster : Cast to AA_EnemyDefault Fail!"));
	}
}