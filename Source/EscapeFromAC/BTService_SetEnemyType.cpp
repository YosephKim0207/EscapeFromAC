// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetEnemyType.h"

#include "AIController.h"
#include "A_EnemyDefault.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SetEnemyType::UBTService_SetEnemyType()
{
	NodeName = TEXT("SetEnemyType");
	Interval = 2.0f;
}

void UBTService_SetEnemyType::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AA_EnemyDefault* NPC = Cast<AA_EnemyDefault>(OwnerComp.GetAIOwner()->GetPawn());
	if(NPC)
	{
		EEnemyType EnemyType = NPC->EnemyType;
		float TotalHPRate = NPC->GetTotalHPRate();
		float HeadHPRate = NPC->GetHPRate(EModular::EHead);
		int ChangeEnemyTypeKey = 0;

		UE_LOG(LogTemp, Log, TEXT("BTService_SetEnemyType : HeadHPRate : %f, TotalHPRate %f"), HeadHPRate, TotalHPRate);

		
		// Change NPC's EnemyType consider Current EnemyType and NPC's HP condition
		switch (EnemyType)
		{
			// EFighter : If HeadHPRate <= 20% or TotalHPRate <= 20%, NPC will Change EnemyType in 33% chance  
		case EEnemyType::EFighter:
			ChangeEnemyTypeKey = FMath::RandRange(0, 2);
			
			UE_LOG(LogTemp, Log, TEXT("BTService_SetEnemyType : CahgeEnemyTypeKeyValue is %d"), ChangeEnemyTypeKey);
			
			if(ChangeEnemyTypeKey < 2)
			{
				return;
			}
			
			if(HeadHPRate <= 0.2f || TotalHPRate <= 0.2f)
			{
				EEnemyType NewEnemyType = EEnemyType::EChicken;
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("EnemyType"), (int8)NewEnemyType);
				
				UE_LOG(LogTemp, Warning, TEXT("BTService_SetEnemyType : Change Type to EChicken!"));
			}
			break;

			// EChicken : If HeadHPRate <= 40% or TotalHPRate <= 60%, NPC will Change EnemyType in 50% chance  
		case EEnemyType::EChicken:
			ChangeEnemyTypeKey = FMath::RandRange(0, 1);

			UE_LOG(LogTemp, Log, TEXT("BTService_SetEnemyType : CahgeEnemyTypeKeyValue is %d"), ChangeEnemyTypeKey);
			
			if(ChangeEnemyTypeKey < 1)
			{
				return;
			}
			
			if(HeadHPRate <= 0.4f || TotalHPRate <= 0.6f)
			{
				EEnemyType NewEnemyType = EEnemyType::EFighter;
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("EnemyType"), (int8)NewEnemyType);

				UE_LOG(LogTemp, Warning, TEXT("BTService_SetEnemyType : Change Type to EFighter!"));
			}
			break;
			
			// ENeutral : If HeadHPRate < 100% or TotalHPRate <= 95%, NPC must Change EnemyType
		case EEnemyType::ENeutral:
			if(HeadHPRate < 1.0f || TotalHPRate < 0.95f)
			{
				EEnemyType NewEnemyType = EEnemyType::EFighter;
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("EnemyType"), (int8)NewEnemyType);

				UE_LOG(LogTemp, Warning, TEXT("BTService_SetEnemyType : Change Type to EFighter!"));
			}
			break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BTService_SetEnemyType : TickNode Can't Cast to AA_EnemyDefault!"));
	}
}
