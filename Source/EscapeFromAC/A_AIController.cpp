// Fill out your copyright notice in the Description page of Project Settings.


#include "A_AIController.h"

#include "A_EnemyDefault.h"
#include "C_Player.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AA_AIController::AA_AIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("BehaviorTree'/Game/BluePrint/AI/BT_Enemy.BT_Enemy'"));
	if(BT.Succeeded())
	{
		BehaviorTree = BT.Object;
		UE_LOG(LogTemp, Log, TEXT("BT Success"));
	}

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB(TEXT("BlackboardData'/Game/BluePrint/AI/BB_Enemy.BB_Enemy'"));
	if(BB.Succeeded())
	{
		BlackboardData = BB.Object;
		UE_LOG(LogTemp, Log, TEXT("BB Success"));
	}

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent((*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"))));

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFielOfView;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AA_AIController::OnPawnDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	
}

void AA_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if(UseBlackboard(BlackboardData, Blackboard))
	{
		if(RunBehaviorTree(BehaviorTree))
		{
			// TODO
			UE_LOG(LogTemp, Log, TEXT("AIController : BT Run Success"));
			
			AA_EnemyDefault* NPC = Cast<AA_EnemyDefault>(GetPawn());
			if(NPC)
			{
				EEnemyType EnemyType = NPC->EnemyType;
				
				NPC->OnDamage.BindLambda([this]()
				{
					GetBlackboardComponent()->SetValueAsBool(TEXT("bOnFight"), true);
					UE_LOG(LogTemp, Log, TEXT("Deligate : AIController bOnFight to True"));
				});

				NPC->MoveTo.BindUFunction(this, TEXT("SetUnKnownPlayerTarget"));
				
				GetBlackboardComponent()->SetValueAsEnum(TEXT("EnemyType"), (int8)EnemyType);
				GetBlackboardComponent()->SetValueAsBool(TEXT("bIsDetectPlayer"), bIsDetectPlayer);
				GetBlackboardComponent()->SetValueAsBool(TEXT("bOnFight"), false);

				UE_LOG(LogTemp, Log, TEXT("AIController : Set EnemyType"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("AIController : Can't Cast to AA_EnemyDefault"));
			}
			
		}
	}
	
}

void AA_AIController::OnUnPossess()
{
	Super::OnUnPossess();

	GetPerceptionComponent()->SetSenseEnabled(*SightConfig->GetSenseImplementation(), false);
	SetActorTickEnabled(false);
	// Destroy();
}

void AA_AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(bIsDetectPlayer)
	{
		float DistanceToPlayer = GetPawn()->GetDistanceTo(TargetPlayer);
		if(DistanceToPlayer > AILoseSightRadius)
		{
			TargetIsDisappear();
		}
	}
}

void AA_AIController::OnPawnDetected(const TArray<AActor*>& DetectedPawns)
{
	if(DetectedPawns.Num() > 0)
	{
		for(auto Pawn : DetectedPawns)
		{
			AC_Player* Player = Cast<AC_Player>(Pawn);

			if(Player)
			{
				bIsDetectPlayer = true;
				TargetPlayer = Player;
				GetBlackboardComponent()->SetValueAsBool(TEXT("bIsDetectPlayer"), bIsDetectPlayer);
				GetBlackboardComponent()->SetValueAsBool(TEXT("bOnFight"), false);
				TargetPlayer->PlayerIsDead.AddUFunction(this, TEXT("TargetIsDisappear"));
				
				return;
			}
		}
	}
}

void AA_AIController::SetUnKnownPlayerTarget(const FVector& UnknownAttackerLocation)
{
	GetBlackboardComponent()->SetValueAsVector(TEXT("MovingLocation"), UnknownAttackerLocation);
}

void AA_AIController::TargetIsDisappear()
{
	bIsDetectPlayer = false;
	GetBlackboardComponent()->SetValueAsBool(TEXT("bIsDetectPlayer"), bIsDetectPlayer);
	GetBlackboardComponent()->SetValueAsBool(TEXT("bOnFight"), false);
}
