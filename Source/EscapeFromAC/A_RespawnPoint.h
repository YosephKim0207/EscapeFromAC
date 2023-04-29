// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_RespawnPoint.generated.h"

UCLASS()
class ESCAPEFROMAC_API AA_RespawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	/** Sets default values for this actor's properties */
	AA_RespawnPoint();
	
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;
	
protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;	

private:
	/** Store Patrol Point for Enemy what Respawn from here */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol", meta = (AllowPrivateAccess = true))
	TArray<class ATargetPoint*> TargetPointArray;

	/** Set Respawn Actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC", meta = (AllowPrivateAccess = true))
	TSubclassOf<class AA_EnemyDefault> NPC;

	UPROPERTY()
	FTimerHandle RespawnHandle;
	
	UFUNCTION()
	void RespawnNPC();

	UFUNCTION()
	void WaitRespawnNPC();
	
};
