// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "A_AIController.generated.h"

UCLASS()
class ESCAPEFROMAC_API AA_AIController : public AAIController
{
	GENERATED_BODY()

public:
	AA_AIController();

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnPawnDetected(const TArray<AActor*> &DetectedPawns);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	float AISightRadius = 800.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	float AILoseSightRadius = AISightRadius * 1.4f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	float AISightAge = 5.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	float AIFielOfView = 45.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	bool bIsDetectPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	class AC_Player* TargetPlayer;

	
	/**
	 * Set MovingLocation to unknown attacker when AI did not detect attacker
	 */
	UFUNCTION()
	void SetUnKnownPlayerTarget(const FVector& UnknownAttackerLocation);

private:
	UPROPERTY()
	class UBehaviorTree* BehaviorTree;

	UPROPERTY()
	class UBlackboardData* BlackboardData;

	/**
	 * Change  Boolean variables to false relate with Fight and Detect Player
	 */
	UFUNCTION()
	void TargetIsDisappear();
	
};
