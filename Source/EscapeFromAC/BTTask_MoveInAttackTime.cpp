// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveInAttackTime.h"

#include "A_AIController.h"
#include "A_EnemyDefault.h"
#include "C_Player.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_MoveInAttackTime::UBTTask_MoveInAttackTime()
{
	NodeName = TEXT("Move In Attack Time");

	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveInAttackTime::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	StopTime = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("NextSetMovingLocationTerm"));
	bIsJump = FMath::RandBool();
	
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveInAttackTime::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		AA_AIController* AIController = Cast<AA_AIController>(OwnerComp.GetAIOwner());
		if(AIController)
		{
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

			// Move NPC
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

				// Jump while Move
				if(bIsJump)
				{
					NPC->Jump();
				}
			}
			else
			{
				NPC->StopJumping();
				
				UE_LOG(LogTemp, Log, TEXT("BTTask_MoveInAttackTIme : HasReachedToDestinatioin!"));
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
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

bool UBTTask_MoveInAttackTime::HasReachedToDestination(FVector& Origin, FVector& Destination)
{	
	if(FMath::Abs(Origin.X - Destination.X) < 20.0f && FMath::Abs(Origin.Y - Destination.Y) < 20.0f)
	{
		bIsReached = true;
		bIsJump = false;
		UE_LOG(LogTemp, Log, TEXT("BTTask_MoveInAttackTime : NPC has reached!"));
	}
	
	return bIsReached;
}