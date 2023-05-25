# EscapeFromAC

![EscapeFromAC_Final](https://github.com/YosephKim0207/Resume/assets/46564046/5185392a-2b47-4039-9736-62f4440b5dd6)

[전체영상링크](https://youtu.be/z8HBJEYXrPg)

---

## 프로젝트 소개 | 모듈식 파츠 교체를 활용한 아머드코어식 슈팅 게임 구현
- 게임 장르 : 3인칭 슈팅
- 목표 : 모듈식 아이템 장착을 통한 다양한 외형 및 능력치를 활용하는 최적화 된 게임 제작
- 플랫폼 : Mac
- 사용 엔진과 언어 : Unreal, C++


## 목표를 위한 구현사항

---

### 오브젝트 풀링

![오브젝트풀링 성능테스트](https://user-images.githubusercontent.com/46564046/235351089-926c57bd-8237-45f6-97b5-52238a8c8360.gif)

문제
- 사용시 직관적이지 못한 일반적으로 사용되는 언리얼 오브젝트 풀링 코드

원인
- 오브젝트 풀 이용을 원하는 액터에 대해 별도의 컴포넌트 추가 없이 사용을 희망

해결
- Class를 Key, 오브젝트 풀을 Value로 하는 TMap 형태의 PoolManager를 GameInstance에서 관리
- 템플릿을 통해 Class에 대한 오브젝트 풀을 조회 및 액터르 반환하는 함수 구현
- 원하는 클래스에 대한 오브젝트 풀을 자유롭게 생성 및 사용
- 프로파일러 기준 평균 게임 스레드 비용 13% 향상

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

### 모듈식 캐릭터 생성

- 포즈복사, 메시병합 대비 구성 비용 및 게임 스레드 비용이 낮은 마스터 포즈 컴포넌트 이용

### 부위 파괴 및 능력치에 따른 기능 저하 구현

[A_Chracter 전체 코드 바로가기](https://github.com/YosephKim0207/EscapeFromAC/blob/main/Source/EscapeFromAC/A_Character.cpp)
<details>
<summary>CheckEachPartPerformance 구현부 펼치기</summary>


```cpp
void AA_Character::CheckEachPartPerformance()
{
	for(FModularItemStatData* Part : PartsData)
	{
		float CurrentHP = Part->HP;
		float MaxHP = Part->MaxHP;

		// if CurrentHP is under 50% of MaxHP
		if(CurrentHP * 2.0f > MaxHP)
		{
			EModular CheckingModularPart = Part->Parts;
			switch (CheckingModularPart)
			{
			case EModular::EUpperBody:
				UpperBodyDefense = UpperBodyData.Defense * 0.5f;
				break;
				
			case EModular::ELowerBody:
				LowerBodyDefense = LowerBodyData.Defense * 0.5f;
				MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed * 0.8f;
				break;

			case EModular::EHead:
				HeadDefense = 0.0f;
				HeadGunHeatCoolingRate = HeadData.GunHeatCoolingRate * 0.8f;
				break;
				
			case EModular::ELeftArm:
				LeftArmShootTerm = LeftArmData.ShootTerm * 0.8f;
				break;
				
			case EModular::ERightArm:
				RightArmShootTerm = RightArmData.ShootTerm * 0.8f;
				break;
			}
		}
		else
		{
			EModular CheckingModularPart = Part->Parts;
			switch (CheckingModularPart)
			{
				// Decrease MaxWalkSpeed
			case EModular::EUpperBody:
				UpperBodyDefense = UpperBodyData.Defense;
				break;
				
			case EModular::ELowerBody:
				LowerBodyDefense = LowerBodyData.Defense;
				MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed;
				break;

				// Decrease ShootTerm
			case EModular::EHead:
				HeadDefense = HeadData.Defense;
				HeadGunHeatCoolingRate = HeadData.GunHeatCoolingRate;
				break;
				
			case EModular::ELeftArm:
				LeftArmShootTerm = LeftArmData.ShootTerm;
				break;
				
			case EModular::ERightArm:
				RightArmShootTerm = RightArmData.ShootTerm;
				break;
			}
		}
	}
}
```

</details>

<details>
<summary>RefreshModularParts_Implementation 구현부 펼치기</summary>


```cpp
void AA_Character::RefreshModularParts_Implementation()
{
	for(FDataTableRowHandle PartPickupItem : ModularInventory)
	{
		FName PickupItemRowName = PartPickupItem.RowName;
		FPickupItemData* PickupItemData = PartPickupItem.DataTable->FindRow<FPickupItemData>(PickupItemRowName, "");

		if(PickupItemData != nullptr)
		{
			if(PickupItemData->ItemType != EItemType::EModular)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s : This Item is Not Modular Item"), *PickupItemRowName.ToString());
				return;
			}
			
			UE_LOG(LogTemp, Log, TEXT("%s : PartPickupItem find in Row"), *PickupItemRowName.ToString());

			FName ModularItemRowName = PickupItemData->SelectItemStatDataTable.RowName;
			FModularItemStatData* ModularItemStatData = PickupItemData->SelectItemStatDataTable.DataTable->FindRow<FModularItemStatData>(ModularItemRowName, ""); 
			
			if(ModularItemStatData != nullptr)
			{
				UE_LOG(LogTemp, Log, TEXT("%s : ModularItem find in Row"), *ModularItemRowName.ToString());
	
				SetEachModularPartStat(*ModularItemStatData);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s : ModularItem Can't find in Row"), *ModularItemRowName.ToString());
				return;
			}

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s : PartPickupItem Can't find Row"), *PickupItemRowName.ToString());
			return;
		}
	}

	// Set Total Weight
	TotalWeight = HeadData.Weight + UpperBodyData.Weight + LeftArmData.Weight + RightArmData.Weight + LowerBodyData.Weight;

	// Fix MaxWalkSpeed by MaxWeight divided by TotalWeight
	float MaxDividedTotalWeightRate = MaxWeight / TotalWeight;
	float MaxWalkSpeedRatePerWeight = 1.0f;
	if(MaxDividedTotalWeightRate < MaxWalkSpeedRatePerWeight)
	{
		MaxWalkSpeedRatePerWeight = MaxDividedTotalWeightRate;
		MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed * MaxWalkSpeedRatePerWeight;
	}
	else
	{
		MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed;
	}

	UE_LOG(LogTemp, Log, TEXT("%s : MaxWeight is %f"), *GetName(), MaxWeight);
	UE_LOG(LogTemp, Log, TEXT("%s : TotalWeight is %f"), *GetName(), TotalWeight);
	UE_LOG(LogTemp, Log, TEXT("%s : MaxDividedTotalWeightRate is %f"), *GetName(), MaxDividedTotalWeightRate);
	UE_LOG(LogTemp, Log, TEXT("%s : MaxWalkSpeed is %f"), *GetName(), MovementComponent->MaxWalkSpeed);
	
	// Set Total Defense
	TotalDefense = HeadData.Defense + UpperBodyData.Defense + LowerBodyData.Defense;
}
```

</details>


### 언리얼 자체 컴포넌트 내 함수를 활용한 기능 구현

[A_Chracter 전체 코드 바로가기](https://github.com/YosephKim0207/EscapeFromAC/blob/main/Source/EscapeFromAC/A_Character.cpp)
<details>
<summary>InternalTakePointDamage 구현부 펼치기</summary>


```cpp
float AA_Character::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Damage;
	FPointDamageEvent DamageEvent = PointDamageEvent;
	
	FString BoneName = PointDamageEvent.HitInfo.BoneName.ToString();

	if(BoneName.Compare(TEXT("None")) == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : Projectile Hit to %s"), *GetName(), *BoneName);
		return ActualDamage;
	}
	
	switch(DamageMap[BoneName])
	{
	case EModular::EHead:
		UE_LOG(LogTemp, Log, TEXT("InternalTakePointDamage : Head Hit!"));

		ActualDamage -= FMath::RoundToFloat(HeadDefense * 0.1f);
		
		// MaxHP = 40
		// Dead
		if(HeadData.HP <= 0.0f)
		{
			UE_LOG(LogTemp, Log, TEXT("%s Kill : %s"), *EventInstigator->GetName(), *GetName());
			SetCharacterState(ECharacterState::EDead);
			ImpulseToRagdoll(DamageEvent);
		}
		else
		{
			if(ActualDamage > HeadData.HP)
			{
				ActualDamage = HeadData.HP;
			}
			HeadData.HP -= (ActualDamage);
		}
		break;
		
	case EModular::EUpperBody:
		UE_LOG(LogTemp, Log, TEXT("Upper Body Hit!"));

		ActualDamage -= FMath::RoundToFloat(UpperBodyDefense * 0.1f);
		// MaxHP = 70
		// Broken
		if(UpperBodyData.HP <= 0.0f)
		{
			ActualDamage = DamageSpreadToOtherParts(EModular::EUpperBody, ActualDamage, DamageEvent);
		}
		else
		{
			if(ActualDamage > UpperBodyData.HP)
			{
				ActualDamage = UpperBodyData.HP;
			}
			UpperBodyData.HP -= ActualDamage;
		}
		UE_LOG(LogTemp, Log, TEXT("HP : %f"), UpperBodyData.HP);
		break;
		
	case EModular::ELeftArm:
		UE_LOG(LogTemp, Log, TEXT("Left Arm Hit!"));

		// MaxHP = 60
		// Broken
		if(LeftArmData.HP <= 0.0f)
		{
			ActualDamage = DamageSpreadToOtherParts(EModular::ELeftArm, ActualDamage, DamageEvent);
		}
		else
		{
			if(ActualDamage > LeftArmData.HP)
			{
				ActualDamage = LeftArmData.HP;
			}
			LeftArmData.HP -= ActualDamage;
		}
		break;
		
	case EModular::ERightArm:
		UE_LOG(LogTemp, Log, TEXT("Right Arm Hit!"));

		// MaxHP = 60
		// Broken
		if(RightArmData.HP <= 0.0f)
		{
			ActualDamage = DamageSpreadToOtherParts(EModular::ERightArm, ActualDamage, DamageEvent);
		}
		else
		{
			if(ActualDamage > RightArmData.HP)
			{
				ActualDamage = RightArmData.HP;
			}
			RightArmData.HP -= ActualDamage;
		}
		break;
		
	case EModular::ELowerBody:
		UE_LOG(LogTemp, Log, TEXT("Lower Body Hit!"));

		ActualDamage -= FMath::RoundToFloat(LowerBodyDefense * 0.1f);
		
		// MaxHP = 60
		// Broken
		if(LowerBodyData.HP <= 0.0f)
		{
			ActualDamage = DamageSpreadToOtherParts(EModular::ELowerBody, ActualDamage, DamageEvent);
		}
		else
		{
			if(ActualDamage > LowerBodyData.HP)
			{
				ActualDamage = LowerBodyData.HP;
			}
			LowerBodyData.HP -= ActualDamage;
		}
		break;
	}

	// TODO
	// Dead State 만들기
	if(HeadData.HP <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("%s Kill : %s"), *DamageCauser->GetName(), *GetName());
		SetCharacterState(ECharacterState::EDead);
	}

	UE_LOG(LogTemp, Log, TEXT("%s : Hit %f Damage by %s!"), *GetName(), ActualDamage, *EventInstigator->GetName());
	
	return ActualDamage;
}
```

</details>

<details>
<summary>CheckJumpInput 구현부 펼치기</summary>



```cpp
void AA_Character::CheckJumpInput(float DeltaTime)
{
	if(bIsBoostOverHeat)
	{
		if(GetBoostHeat() <= 0.0f)
		{
			bIsBoostOverHeat = false;
		}
		
		SetBoostHeat(-(MaxBoostHeat/GetMaxBoostOverHeatCoolTime()) * DeltaTime);
		
		return;
	}
	
	Super::CheckJumpInput(DeltaTime);
	
	if(bPressedJump)
	{
		BoostEffect();
		SetBoostHeat(DeltaTime);
		if(GetBoostHeat() >= MaxBoostHeat)
		{
			UE_LOG(LogTemp, Log, TEXT("bIsBoostOverHeat"));
			bIsBoostOverHeat = true;
			bPressedJump = false;
		}
	}
	else
	{
		if(!bPressedJump && !bIsBoostOverHeat && GetBoostHeat() >= 0.0f)
		{
			SetBoostHeat(-DeltaTime * BoostHeatReduceRate);
		}
	}
}
```

</details>

<details>
<summary>ResetJumpState 구현부 펼치기</summary>



```cpp
void AA_Character::ResetJumpState()
{
	
	// OnMovementModChanged Function will Try ResetJumpState
	// But Do not Reset Jump State because multiple Jump
	
	if(bIsBoostOverHeat || GetCharacterMovement()->IsFalling())
	{
		UE_LOG(LogTemp, Log, TEXT("ResetJumpState_return 1"));
		return;
	}

	if(GetCharacterMovement()->IsFalling())
	{
		UE_LOG(LogTemp, Log, TEXT("ResetJumpState_IsFalling"));
		bWasJumping = false;
	}
	
	if(GetBoostHeat() <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("ResetJumpState_Super"));
		Super::ResetJumpState();
	}
}
```

</details>

	
## 기타

---

### NPC AI 비헤이비어 트리
	
![image](https://github.com/YosephKim0207/EscapeFromAC/assets/46564046/29967ed6-d01d-4d5e-8ba3-7c1d32f85c7f)
