# EscapeFromAC

![EscapeFromAC_Default](https://user-images.githubusercontent.com/46564046/235349657-70bffb32-990e-4209-841a-a9af6a830dee.gif)

[전체영상링크](https://youtu.be/VfgMDnHjERs)

---

## 프로젝트 소개 | 모듈식 파츠 교체를 활용해 아머드코어식 슈팅 게임 구현
- 게임 장르 : 3인칭 슈팅
- 목표 : 모듈식 아이템 장착을 통한 다양한 외형 및 능력치를 활용하는 최적화 된 게임 제작
- 플랫폼 : Mac
- 사용 엔진과 언어 : Unreal, C++


## 목표를 위한 구현사항

---

### 오브젝트 풀링


### 부위파위 및 HP에 따른 능력치 조정

- 원하는 클래스에 대한 오브젝트 풀을 자유롭게 생성 및 사용

[PoolManager 전체 코드 바로가기]
(https://github.com/YosephKim0207/EscapeFromAC/blob/main/Source/EscapeFromAC/A_PoolManager.h)
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

### 모듈식 캐릭터 생성

- 포즈복사, 메시병합 대비 구성 비용 및 게임 스레드 비용이 낮은 마스터 포즈 컴포넌트 이용
