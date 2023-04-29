// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckDistance.h"
#include "A_AIController.h"
#include "A_EnemyDefault.h"
#include "C_Player.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckDistance::UBTDecorator_CheckDistance()
{
	NodeName = TEXT("Check Distance To Player");
}

bool UBTDecorator_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AA_AIController* AIController = Cast<AA_AIController>(OwnerComp.GetAIOwner());
	if(AIController)
	{
		if(AIController->bIsDetectPlayer)
		{
			
			AA_EnemyDefault* NPC = Cast<AA_EnemyDefault>(AIController->GetPawn());
			float DistanceToPlayer = NPC->GetDistanceTo(AIController->TargetPlayer);
			if(NPC)
			{
				if(DistanceToPlayer > AIController->AILoseSightRadius)
				{
					UE_LOG(LogTemp, Log, TEXT("BTDecorator_CheckDistance : Player is too far to follow!"));
					return false;
				}
			}
			else
			{
				return true;
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	
	return false;
}
