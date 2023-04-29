// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_PoolManager.generated.h"

UCLASS()
class ESCAPEFROMAC_API AA_PoolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	/** Sets default values for this actor's properties */
	AA_PoolManager();

	template< class T >
	T* GetThisObject(UClass* Class, const FVector& Location, const FRotator& Rotation, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:	
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

private:
	/** Search for Object Pool TArray using Class as Key */
	TMap<UClass*, TArray<AActor*>> ObjectPool;

	/** Initial size for each Object Pool */
	UPROPERTY()
	int8 PoolSize = 10;
	
};

/**
 * Get template class's Actor from Object Pool
 * If Pool is not exist, Make object pool about class and return
 */
template <class T>
T* AA_PoolManager::GetThisObject(UClass* Class, const FVector& Location, const FRotator& Rotation,
	const FActorSpawnParameters& SpawnParameters)
{
	TArray<AActor*>* Pool = ObjectPool.Find(Class);
	
	// Object Pool is Exist
	if(Pool != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("PoolManager : %s has Pool"), *Class->GetName());
		
		for(AActor* PoolingActor : *Pool)
		{
			bool IsHidden = PoolingActor->IsHidden();
		
			if(IsHidden)
			{
				PoolingActor->SetActorLocation(Location);
				PoolingActor->SetActorRotation(Rotation);
				PoolingActor->SetOwner(SpawnParameters.Owner);
				PoolingActor->SetInstigator(SpawnParameters.Instigator);
				PoolingActor->SetActorHiddenInGame(false);
		
				return Cast<T>(PoolingActor);
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("PoolManager : %s Pool is empty"), *Class->GetName());
		
		return nullptr;
	}

	// Pool doesn't Exist
	
	UE_LOG(LogTemp, Warning, TEXT("PoolManager : %s doesn't have Pool"), *Class->GetName());
	
	TArray<AActor*> NewPool;
	UWorld* World = GetWorld();
	if(World)
	{
		for(int8 SpawnIndex = 0; SpawnIndex < PoolSize; ++SpawnIndex)
		{
			T* RespawnActor;
			RespawnActor = GetWorld()->SpawnActor<T>(Class, FVector::ZeroVector, FRotator::ZeroRotator);
			
			RespawnActor->SetActorHiddenInGame(true);
			NewPool.Add(RespawnActor);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PoolManager : World is nullPtr"));

		return nullptr;
	}
	

	UE_LOG(LogTemp, Log, TEXT("PoolManager : %s make Pool"), *Class->GetName());
	
	ObjectPool.Add(Class, NewPool);
	
	AActor* ReturnActor;
	ReturnActor = (*ObjectPool.Find(Class))[0];
	ReturnActor->SetActorLocation(Location);
	ReturnActor->SetActorRotation(Rotation);
	ReturnActor->SetOwner(SpawnParameters.Owner);
	ReturnActor->SetInstigator(SpawnParameters.Instigator);
	ReturnActor->SetActorHiddenInGame(false);

	UE_LOG(LogTemp, Log, TEXT("PoolManager : return %s from init pool"), *Class->GetName());
	
	return Cast<T>(ReturnActor);
}