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

### 오브젝트 풀

![오브젝트풀링 성능테스트](https://user-images.githubusercontent.com/46564046/235351089-926c57bd-8237-45f6-97b5-52238a8c8360.gif)

<img width="800" alt="풀링미적용" src="https://github.com/YosephKim0207/EscapeFromAC/assets/46564046/86f76033-a176-4300-b5d9-457d3a53167b">

<오브젝트 풀링 적용 전>


<img width="800" alt="풀링적용" src="https://github.com/YosephKim0207/EscapeFromAC/assets/46564046/1a941e2b-d42e-4504-904d-e28aa4b3d217">

<오브젝트 풀링 적용 후>

---


### 오브젝트 풀링

문제
- 전투시 성능 저하

원인
- 슈팅 게임으로서 빈번한 Projectile 액터 생성 및 제거로 인한 메모리 할당 / 해제

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


	
	
### 모듈식 캐릭터 생성

![Modular](https://github.com/YosephKim0207/EscapeFromAC/assets/46564046/b3f9489e-3e05-4bf2-9edf-806c4d8edb2c)

---

### 모듈식 캐릭터 생성

<img width="680" alt="스크린샷 2023-05-24 오후 9 49 17" src="https://github.com/YosephKim0207/EscapeFromAC/assets/46564046/45ed8746-035b-4d93-94ff-d4621acac711">

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
<summary>SetEachModularPartStat_Implementation 구현부 펼치기</summary>


```cpp
void AA_Character::SetEachModularPartStat_Implementation(const FModularItemStatData& ModularItemStatData)
{
	EModular Part = ModularItemStatData.Parts;
	
	const UEnum* CharModularParts = FindObject<UEnum>(ANY_PACKAGE, TEXT("EModular"), true);
	
	// Set each Modular Part's Stat
	switch (Part)
	{
	case EModular::EUpperBody:
		UpperBodyData = ModularItemStatData;

		if(ModularItemStatData.SkeletalMesh)
		{
			SKM_UpperBody->SetSkeletalMesh(ModularItemStatData.SkeletalMesh);
			UpperBodyDefense = UpperBodyData.Defense;
		}
		else
		{
			if(CharModularParts)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s : Modular's Skeletal Mesh is null"), *CharModularParts->GetNameByValue((int8)Part).ToString());
			}
		}
		
		break;
		
	case EModular::EHead:
		HeadData = ModularItemStatData;
		HeadDefense = HeadData.Defense;
		HeadGunHeatCoolingRate = HeadData.GunHeatCoolingRate;
		
		if(ModularItemStatData.SkeletalMesh)
		{
			SKM_Head->SetSkeletalMesh(ModularItemStatData.SkeletalMesh);
		}
		else
		{
			if(CharModularParts)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s : Modular's Skeletal Mesh is null"), *CharModularParts->GetNameByValue((int64)Part).ToString());
			}
		}
		
		break;

	case EModular::ELeftArm:
		LeftArmData = ModularItemStatData;
		LeftArmDamage = LeftArmData.Damage;
		LeftArmShootTerm = LeftArmData.ShootTerm;
		
		if(ModularItemStatData.SkeletalMesh)
		{
			SKM_LeftArm->SetSkeletalMesh(ModularItemStatData.SkeletalMesh);
		}
		else
		{
			if(CharModularParts)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s : Modular's Skeletal Mesh is null"), *CharModularParts->GetNameByValue((int64)Part).ToString());
			}
		}
		
		break;
	
	case EModular::ERightArm:
		RightArmData = ModularItemStatData;
		RightArmDamage = RightArmData.Damage;
		RightArmShootTerm = RightArmData.ShootTerm;
		
		if(ModularItemStatData.SkeletalMesh)
		{
			SKM_RightArm->SetSkeletalMesh(ModularItemStatData.SkeletalMesh);
		}
		else
		{
			if(CharModularParts)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s : Modular's Skeletal Mesh is null"), *CharModularParts->GetNameByValue((int64)Part).ToString());
			}
		}
		
		break;
		
	case EModular::ELowerBody:
		LowerBodyData = ModularItemStatData;
		MaxWeight = LowerBodyData.MaxWeight;
		LowerBodyDefense = LowerBodyData.Defense;

		if(ModularItemStatData.SkeletalMesh)
		{
			SKM_LowerBody->SetSkeletalMesh(ModularItemStatData.SkeletalMesh);
		}
		else
		{
			if(CharModularParts)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s : Modular's Skeletal Mesh is null"), *CharModularParts->GetNameByValue((int64)Part).ToString());
			}
		}
		
		break;
	}

	if(CharModularParts)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : Set Modular SKM & Stat"), *CharModularParts->GetNameByValue((int64)Part).ToString());
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
}
```

</details>



<details>
<summary>Dash 구현부 펼치기 (TotalWeight / MaxWeight에 따른 BoostHeat 변화)</summary>


```cpp
/** Temporaly Increase MaxWalkSpeed for Dash */
void AA_Character::Dash()
{
	
	if(!bIsBoostOverHeat && !bIsDash && bIsMovable && !GetVelocity().IsZero())
	{
		UE_LOG(LogTemp, Log, TEXT("Dash"));
		CanDashTimeCount = 0.0f;
		
		
		
		// Increase Velocity
		if(MovementComponent)
		{
			bIsDash = true;
			SetCharacterState(ECharacterState::EJump);
			TempMaxWalkSpeed = MovementComponent->MaxWalkSpeed;
			UE_LOG(LogTemp, Log, TEXT("Prev Max Walk Speed : %lf"), MovementComponent->GetMaxSpeed());
			MovementComponent->MaxWalkSpeed *= DashSpeedRate;
			UE_LOG(LogTemp, Warning, TEXT("New Max Walk Speed : %lf"), MovementComponent->GetMaxSpeed());
			UE_LOG(LogTemp, Log, TEXT("Prev Walk Speed : %lf"), GetVelocity().Size());
			
			FVector NormaliedVelocity = GetVelocity().GetSafeNormal(1.f);
			MovementComponent->Velocity.X = NormaliedVelocity.X * MovementComponent->GetMaxSpeed();
			MovementComponent->Velocity.Y = NormaliedVelocity.Y * MovementComponent->GetMaxSpeed();
			MovementComponent->Velocity.Z = NormaliedVelocity.Z * MovementComponent->GetMaxSpeed();
			UE_LOG(LogTemp, Warning, TEXT("New Walk Speed : %lf"), GetVelocity().Size());
		}

		float AdditionalHeatRateByWeight = 1.0f;
		if(TotalWeight > MaxWeight)
		{
			AdditionalHeatRateByWeight = TotalWeight / MaxWeight;
		}
		
		SetBoostHeat(MaxBoostHeat * 0.5f * AdditionalHeatRateByWeight);
		UE_LOG(LogTemp, Log, TEXT("%s : AdditionalHeatRateByWeight = %f"), *GetName(), AdditionalHeatRateByWeight);

	}
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
