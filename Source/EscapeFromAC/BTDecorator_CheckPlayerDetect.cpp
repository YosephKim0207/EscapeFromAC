// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckPlayerDetect.h"
#include "A_AIController.h"
#include "A_EnemyDefault.h"
#include "C_Player.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckPlayerDetect::UBTDecorator_CheckPlayerDetect()
{
	NodeName = TEXT("CheckPlayerDetect");
}

bool UBTDecorator_CheckPlayerDetect::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                uint8* NodeMemory) const
{
	AA_AIController* AIController = Cast<AA_AIController>(OwnerComp.GetAIOwner());
	if(AIController)
	{
		if(AIController->bIsDetectPlayer)
		{
			AC_Player* TargetPlayer = AIController->TargetPlayer;
			int8 EnemyTypeIndex = OwnerComp.GetBlackboardComponent()->GetValueAsEnum(TEXT("EnemyType"));
			
			if(TargetPlayer)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPlayer"), TargetPlayer);

				return true;
			}

			return false;
		}
	}

	
	
	return false;
}
