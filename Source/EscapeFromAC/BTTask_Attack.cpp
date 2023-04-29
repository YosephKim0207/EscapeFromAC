// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "AIController.h"
#include "A_AIController.h"
#include "A_EnemyDefault.h"
#include "C_Player.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");

	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	AA_EnemyDefault* NPC = Cast<AA_EnemyDefault>(OwnerComp.GetAIOwner()->GetPawn());
	if(NPC)
	{
		AA_AIController* AIController = Cast<AA_AIController>(OwnerComp.GetAIOwner());
		if(AIController)
		{
			if(!AIController->bIsDetectPlayer)
			{
				return EBTNodeResult::Failed;
			}
			
			if(!NPC->GetbIsRightArmOnFire() && !NPC->GetbIsLeftArmOnFire() && !NPC->GetbIsGunOverHeat(EModular::ERightArm))
			{
				NPC->StopFire();
				NPC->RightArmFireHandler();
				bIsAttacking = true;
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bOnFight"), true);

				UE_LOG(LogTemp, Log, TEXT("BTTask_Attack : Call RightArmHandler"));
			
				NPC->OnStopFire.AddLambda([this]()
				{
					bIsAttacking = false;
				} );

				return EBTNodeResult::Succeeded;
			}

			if(!NPC->GetbIsRightArmOnFire() && !NPC->GetbIsLeftArmOnFire() && !NPC->GetbIsGunOverHeat(EModular::ELeftArm))
			{
				NPC->StopFire();
				NPC->LeftArmFireHandler();
				bIsAttacking = true;
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bOnFight"), true);

				UE_LOG(LogTemp, Log, TEXT("BTTask_Attack : Call LeftArmHandler"));
			
				NPC->OnStopFire.AddLambda([this]()
				{
					bIsAttacking = false;
				} );

				return EBTNodeResult::Succeeded;
			}
			
				return EBTNodeResult::Succeeded;
		}
	}
	
	
	return EBTNodeResult::Failed;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	if(bIsAttacking == false)
	{
		UE_LOG(LogTemp, Log, TEXT("BTTask_Attack : Attack Finish!"));
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bOnFight"), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
