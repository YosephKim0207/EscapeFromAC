// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_MoveInAttackTime.h"

#include "A_AIController.h"
#include "A_EnemyDefault.h"
#include "C_Player.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTaskNode_MoveInAttackTime::UBTTaskNode_MoveInAttackTime()
{
	NodeName = TEXT("Move In Attack Time");

	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_MoveInAttackTime::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	StopTime = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("NextSetMovingLocationTerm"));
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_MoveInAttackTime::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AccumulateMoveTime += DeltaSeconds;
	

	if(StopTime < AccumulateMoveTime)
	{
		AccumulateMoveTime = 0.0f;
		UE_LOG(LogTemp, Log, TEXT("BTTask_MoveInAttackTime : Move Stop"));
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	AA_EnemyDefault* NPC = Cast<AA_EnemyDefault>(OwnerComp.GetAIOwner()->GetPawn());

	if(NPC)
	{
		
		
		// TODO
		// MovingLocation과 NPC 사이의 방향 벡터 구하고
		// 방향 벡터를 토대로 MoveForward / MoveRight 명령
		AA_AIController* AIController = Cast<AA_AIController>(OwnerComp.GetAIOwner());
		if(AIController)
		{
			// TODO
			// Jump나 Boost 때 Enemy MovementMode가 어떻게 변하는지 디버깅화면에서 보고 NPCLocation 결정하기
			// if(NPC->GetCharacterMovement()->MovementMode != (int8)EMovementMode::MOVE_Flying)
			// {
			// 	
			// }
			// else
			// {
			// 	
			// }

			AC_Player* TargetActor = AIController->TargetPlayer;
			FVector TargetActorLocation = TargetActor->GetActorLocation();
			FVector NPCLocation = NPC->GetActorLocation();

			// NPC Look Player
			FVector PlayerDirection = UKismetMathLibrary::GetDirectionUnitVector(NPCLocation, TargetActorLocation);
			PlayerDirection.Normalize();
			NPC->SetActorRotation(PlayerDirection.Rotation());

			float DistanceToPlayer = NPC->GetDistanceTo(TargetActor);
			float MaxDistanceToPlayer = AIController->AILoseSightRadius;
			
			if(DistanceToPlayer > MaxDistanceToPlayer)
			{
				UE_LOG(LogTemp, Log, TEXT("BTTask_MoveInAttackTime : Player Disappear"));
				
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return;
			}
			
			FVector MovingLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("MovingLocation"));
			bIsReached = false;
			
			if(!HasReachedToDestination(NPCLocation, MovingLocation))
			{
				FVector MoveDirection = UKismetMathLibrary::GetDirectionUnitVector(NPCLocation, MovingLocation);
			
				float MoveDirection_X = MoveDirection.X;
				float MoveDirection_Y = MoveDirection.Y;

				if(MoveDirection_X > 0.0f)
				{
					NPC->MoveForward(0.8f);
				}
				else if(MoveDirection_X < 0.0f)
				{
					NPC->MoveForward(-0.8f);
				}

				if(MoveDirection_Y > 0.0f)
				{
					NPC->MoveRight(0.8f);
				}
				else if(MoveDirection_Y < 0.0f)
				{
					NPC->MoveRight(-0.8f);
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("BTTask_MoveInAttackTIme : HasReachedToDestinatioin!"));
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}

			// // NPC Look Player
			// FVector PlayerLocation = AIController->TargetPlayer->GetActorLocation();
			// FVector PlayerDirection = UKismetMathLibrary::GetDirectionUnitVector(NPCLocation, PlayerLocation);
			// PlayerDirection.Normalize();
			// NPC->SetActorRotation(PlayerDirection.Rotation());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BTTask_MoveInAttackTime : Cast To AA_AIController Fail!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_MoveInAttackTime : Cast To AA_EnemyDefault Fail!"));
	}
	
}

bool UBTTaskNode_MoveInAttackTime::HasReachedToDestination(FVector& Origin, FVector& Destination)
{	
	if(FVector::Distance(Origin, Destination) < 30.0f)
	{
		bIsReached = true;

		UE_LOG(LogTemp, Log, TEXT("BTTask_MoveInAttackTime : NPC has reached!"));
	}
	
	return bIsReached;
}
