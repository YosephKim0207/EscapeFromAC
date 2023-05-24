# 모듈식

![EscapeFromAC_Final](https://github.com/YosephKim0207/Resume/assets/46564046/5185392a-2b47-4039-9736-62f4440b5dd6)

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
