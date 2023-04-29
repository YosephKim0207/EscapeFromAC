// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetMovingLocation.h"

#include "A_AIController.h"
#include "A_EnemyDefault.h"
#include "C_Player.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SetMovingLocation::UBTService_SetMovingLocation()
{
	NodeName = TEXT("Set Moving Location");
	Interval = 3.0f;
}

void UBTService_SetMovingLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	Interval = FMath::RandRange(2, 4);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(TEXT("NextSetMovingLocationTerm"), Interval);

	AA_AIController* AIController = Cast<AA_AIController>(OwnerComp.GetAIOwner());
	if(AIController)
	{
		AA_EnemyDefault* NPC = Cast<AA_EnemyDefault>(AIController->GetPawn());
		if(NPC)
		{
			FVector NPCOriginLocation = NPC->GetActorLocation();
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
			if(NavSystem)
			{
				FVector PlayerLocation = AIController->TargetPlayer->GetActorLocation();
				FVector MoveTargetLocation;
				FNavLocation NavLocation;

				// Set
				float MinDistanceToPlayer = AIController->AISightRadius * 0.5f;
				float MaxDistanceToPlayer = AIController->AILoseSightRadius - MinDistanceToPlayer * 0.5f;
				for(int8 count = 0; count < 10;)
				{
					NavSystem->GetRandomPointInNavigableRadius(NPCOriginLocation, 300.0f, NavLocation);
					MoveTargetLocation = NavLocation.Location;

					float DistancePlayerToMoveTarget = FVector::Distance(PlayerLocation, MoveTargetLocation);
					
					if(DistancePlayerToMoveTarget < MaxDistanceToPlayer && DistancePlayerToMoveTarget > MinDistanceToPlayer)
					{
						break;
					}

					++count;
					
					if(count == 10)
					{
						NavSystem->GetRandomPointInNavigableRadius(NPCOriginLocation, 150.0f, NavLocation);
						MoveTargetLocation = NavLocation.Location;
					}
				}
				
				
				
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("MovingLocation"), MoveTargetLocation);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("BTService_SetMovingLocatioin : Get UNavigationSystemV1 Fail!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BTService_SetMovingLocatioin : Cast to AA_EnemyDefault Fail!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BTService_SetMovingLocatioin : Cast to AA_AIController Fail!"));
	}
	
}
