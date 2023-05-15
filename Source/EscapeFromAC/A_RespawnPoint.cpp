// Fill out your copyright notice in the Description page of Project Settings.


#include "A_RespawnPoint.h"

#include "A_EnemyDefault.h"

// Sets default values
AA_RespawnPoint::AA_RespawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;	
}

// Called when the game starts or when spawned
void AA_RespawnPoint::BeginPlay()
{
	Super::BeginPlay();

	RespawnNPC();
}

void AA_RespawnPoint::RespawnNPC()
{
	GetWorldTimerManager().ClearTimer(RespawnHandle);
	
	UWorld* World = GetWorld();
	
	if(World)
	{
		if(NPC)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AA_EnemyDefault* RespawnNPC = World->SpawnActor<AA_EnemyDefault>(NPC, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

			if(RespawnNPC)
			{
				RespawnNPC->TargetPointArray = TargetPointArray;

				RespawnNPC->NPCIsDead.BindUFunction(this, TEXT("WaitRespawnNPC"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("RespawnNPC Fail!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RespawnPoint's NPC slot is Empty!"));
		}
	}
}

void AA_RespawnPoint::WaitRespawnNPC()
{
	GetWorldTimerManager().SetTimer(RespawnHandle, this, &AA_RespawnPoint::RespawnNPC, RespawnTime, false);
}

// Called every frame
void AA_RespawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

