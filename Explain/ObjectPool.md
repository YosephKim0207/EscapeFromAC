# 오브젝트 풀

![오브젝트풀링 성능테스트](https://user-images.githubusercontent.com/46564046/235351089-926c57bd-8237-45f6-97b5-52238a8c8360.gif)

<img width="800" alt="풀링미적용" src="https://github.com/YosephKim0207/EscapeFromAC/assets/46564046/86f76033-a176-4300-b5d9-457d3a53167b">

<오브젝트 풀링 적용 전>


<img width="800" alt="풀링적용" src="https://github.com/YosephKim0207/EscapeFromAC/assets/46564046/1a941e2b-d42e-4504-904d-e28aa4b3d217">

<오브젝트 풀링 적용 후>

---


### 오브젝트 풀링

문제
- 전투시 성능 저하

원인
- 슈팅 게임으로서 빈번한 Projectile 액터 생성 및 제거로 인한 메모리 할당 / 해제 문제

해결
- GameInstance에 오브젝트 풀 템플릿을 만들어 오브젝트 풀 이용을 원하는 클래스는 자유롭게 풀 생성 및 사용

결과
- 프로파일러 기준 평균 게임 스레드 성능 13% 향상



[PoolManager 전체 코드 바로가기](https://github.com/YosephKim0207/EscapeFromAC/blob/main/Source/EscapeFromAC/A_PoolManager.h)
<details>
<summary>PoolManager 구현 템플릿 코드 펼치기</summary>



```cpp
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
```

</details>
	
	
	
[A_Character 전체 코드 바로가기](https://github.com/YosephKim0207/EscapeFromAC/blob/main/Source/EscapeFromAC/A_Character.cpp)
<details>
<summary>A_Character에서의 PoolManager 사용 코드 펼치기</summary>



```cpp
if(GetbIsShootable() && GetbIsRightArmOnFire())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = GetInstigator();

		UWorld* World = GetWorld();
		UACGameInstance* ACGameInstance = Cast<UACGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		AA_PoolManager* PoolManager = ACGameInstance->GetPoolManager();
		if(PoolManager)
		{
			AA_Projectile* Projectile = PoolManager->GetThisObject<AA_Projectile>(TempProjectile, ProjectileRespawnLocation + (ProjectileShootRotator.Vector() * 10.0f), ProjectileShootRotator, SpawnParameters);

			if(Projectile != nullptr)
			{
				SetProjectileData(Projectile, EModular::ERightArm, true);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s : %s Pool is empty! Do SpawnActor"), *GetName(), *TempProjectile->GetName());
				Projectile = World->SpawnActor<AA_Projectile>(TempProjectile, ProjectileRespawnLocation + (ProjectileShootRotator.Vector() * 10.0f), ProjectileShootRotator, SpawnParameters);
				if(Projectile != nullptr)
				{
					SetProjectileData(Projectile, EModular::ERightArm, false);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("%s : Get Projectile Fail!"), *GetName());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s : PoolManager is nullPtr!"), *GetName());
		}
	}
```

</details>
