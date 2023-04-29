// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Character.h"

#include "ACGameInstance.h"
#include "AIController.h"
#include "A_PoolManager.h"
#include "S_Inventory.h"
#include "A_Projectile.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

// Sets default values
AA_Character::AA_Character()
{
	PrimaryActorTick.bCanEverTick = true;	
	
	// Make Skeletal Meshes for Modular Character
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	SKM_UpperBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodySkeletalMesh_UpperBody"));
	SKM_UpperBody->SetupAttachment(GetMesh());
	SKM_BackAccessory = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodySkeletalMesh_BackAcc"));
	SKM_BackAccessory->SetupAttachment(SKM_UpperBody, FName(TEXT("attach_rocketpack")));
	SKM_BackAccessory->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
	SKM_LowerBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodySkeletalMesh_LowerBody"));
	SKM_LowerBody->SetupAttachment(SKM_UpperBody);
	SKM_LeftArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodySkeletalMesh_LeftHand"));
	SKM_LeftArm->SetupAttachment(SKM_UpperBody);
	SKM_RightArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodySkeletalMesh_RightHand"));
	SKM_RightArm->SetupAttachment(SKM_UpperBody);
	SKM_Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodySkeletalMesh_Head"));
	SKM_Head->SetupAttachment(SKM_UpperBody);
	SKM_FaceAccessory = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodySkeletalMesh_FaceAcc"));
	SKM_FaceAccessory->SetupAttachment(SKM_Head);

	AiPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSourceComponent"));
	
	bIsMovable = true;
	
	MaxAccel = 1.5f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	MaxBoostHeat = 2.0f;
	CurrentBoostHeat = 0.0f;
	bIsBoostOverHeat = false;
	MaxBoostOverHeatCoolTime = 3.0f;
	JumpMaxCount = INT32_MAX;
	GetCharacterMovement()->JumpZVelocity = 20.f;
	BoostHeatReduceRate = 0.5f;

	DashSpeedRate = 4.0f;

	// Set Default Status for NPC and Player
	MovementComponent = GetCharacterMovement();

	if(MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = 330.0f;
		MovementComponent->MaxWalkSpeedCrouched = 150.0f;
		MovementComponent->JumpZVelocity = 250.0f;
		MovementComponent->AirControl = 0.3f;
		MovementComponent->AirControlBoostMultiplier = 1.0f;
		MovementComponent->AirControlBoostVelocityThreshold = 200.0f;
		MovementComponent->MaxFlySpeed = 300.0f;

		OriginalMaxWalkSpeed = MovementComponent->MaxWalkSpeed;
	}

	bIsShootable = true;

	CurAttackState = EAttackState::EIdle;

	LeftArmDamage = 10.0f;
	LeftArmShootTerm = 1.0f;

	RightArmDamage = 5.0f;
	RightArmShootTerm = 0.5f;

	SetCharacterState(ECharacterState::EIdle);
	InitDamageMap();
	InitPartsData();
}

// Called when the game starts or when spawned
void AA_Character::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BeginPlay : MXBoostHeat : %lf, JumpMXHld : %lf."), MaxBoostHeat, JumpMaxHoldTime);
}

void AA_Character::DoAfterDead(const float& DeltaSeconds)
{
	return;
}

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

// Called every frame
void AA_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch(CurCharacterState)
	{
	case ECharacterState::EDead:
		DoRagDoll();
		DoAfterDead(DeltaTime);
		break;

	default:
		// Set Dash
		if(bIsDash && CanDashTimeCount <= DashCoolTime)
		{
			CanDashTimeCount += DeltaTime;
		}
		else if(bIsDash && CanDashTimeCount > DashCoolTime)
		{
			bIsDash = false;
			MovementComponent->MaxWalkSpeed = TempMaxWalkSpeed;
			MovementComponent->Velocity = GetVelocity().GetClampedToMaxSize(OriginalMaxWalkSpeed);
		}

		// Increase variable CurRepairProgressTime and RepairLoading Widget's ProgressBar
		if(CurCharacterState == ECharacterState::ERepairing)
		{
			CurRepairProgressTime += DeltaTime;
		}

		GunCooling(DeltaTime);

		// Detect Character is Idle
		if(GetMovementComponent()->Velocity.IsZero())
		{
			if(GetCharacterState() == ECharacterState::EAttack || GetCharacterState() == ECharacterState::ECheckItems || GetCharacterState() == ECharacterState::ERepairing)
			{
				return;
			}
		
			if(GetCharacterState() != ECharacterState::EIdle)
			{
				SetCharacterState(ECharacterState::EIdle);
			}
		}
		break;
	}
}

void AA_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	JumpMaxHoldTime = MaxBoostHeat;
}

// Called to bind functionality to input
void AA_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAxis("MoveForward", this, &AA_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AA_Character::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	// BInd Action Key
	// Bind Jump Action
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AA_Character::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AA_Character::StopJumping);

	// Bind Fire Action
	PlayerInputComponent->BindAction("RightHandFire", IE_Pressed, this, &AA_Character::RightArmFireHandler);
	PlayerInputComponent->BindAction("RightHandFire", IE_Released, this, &AA_Character::StopFire);

	PlayerInputComponent->BindAction("LeftHandFire", IE_Pressed, this, &AA_Character::LeftArmFireHandler);
	PlayerInputComponent->BindAction("LeftHandFire", IE_Released, this, &AA_Character::StopFire);
	
	PlayerInputComponent->BindAction("Dash", IE_Pressed,this, &AA_Character::Dash);
	
	PlayerInputComponent->BindAction("RespawnTest", IE_Pressed, this, &AA_Character::DeadTest);

}

void AA_Character::MoveForward(float value)
{
	if(bIsMovable && FMath::Abs(value) >= 0.01f)
	{
		SetCharacterState(ECharacterState::EWalking);
		AddMovementInput(GetActorForwardVector(), value);
	}
}

void AA_Character::MoveRight(float value)
{
	if(bIsMovable && FMath::Abs(value) >= 0.01f)
	{
		SetCharacterState(ECharacterState::EWalking);
		AddMovementInput(GetActorRightVector(), value);
	}
}

void AA_Character::AddControllerPitchInput(float Val)
{
	if(bIsMovable)
	{
		Super::AddControllerPitchInput(Val);
	}
}

void AA_Character::AddControllerYawInput(float Val)
{
	if(bIsMovable)
	{
		Super::AddControllerYawInput(Val);
	}
}

float AA_Character::GetHP(const EModular& Part) const
{
	float CurHP = 0.0f;
	switch (Part)
	{
	case EModular::EHead:
		CurHP = HeadData.HP;
		break;
		
	case EModular::EUpperBody:
		CurHP = UpperBodyData.HP;
		break;

	case EModular::ELeftArm:
		CurHP = LeftArmData.HP;
		break;
		
	case EModular::ERightArm:
		CurHP = RightArmData.HP;
		break;
		
	case EModular::ELowerBody:
		CurHP = LowerBodyData.HP;
		break;
	}

	return CurHP;
}

float AA_Character::GetMaxHP(const EModular& Part) const
{
	float MaxHP = 0.0f;
	
	switch (Part)
	{
	case EModular::EHead:
		MaxHP = HeadData.MaxHP;
		break;
		
	case EModular::EUpperBody:
		MaxHP = UpperBodyData.MaxHP;
		break;

	case EModular::ELeftArm:
		MaxHP = LeftArmData.MaxHP;
		break;
		
	case EModular::ERightArm:
		MaxHP = RightArmData.MaxHP;
		break;
		
	case EModular::ELowerBody:
		MaxHP = LowerBodyData.MaxHP;
		break;
	}

	return MaxHP;
}

float AA_Character::GetHPRate(const EModular& Part) const
{
	float CurHP = GetHP(Part);
	float MaxHP = GetMaxHP(Part);
	
	return FMath::Clamp((CurHP / MaxHP), 0.0f, 1.0f);
}

float AA_Character::GetTotalHPRate() const
{
	float TotalHP = 0.0f;
	float TotalMaxHP = 0.0f;
	for(auto Part : PartsData)
	{
		TotalHP += Part->HP;
		TotalMaxHP += Part->MaxHP;
	}

	return FMath::Clamp((TotalHP / TotalMaxHP), 0.0f, 1.0f);
}

float AA_Character::GetBoostHeatRate() const
{
	return FMath::Clamp((CurrentBoostHeat / MaxBoostHeat), 0.0f, 1.0f);
}

float AA_Character::GetBoostHeat() const
{
	return CurrentBoostHeat;
}

float AA_Character::GetGunHeatRate(const EModular& Part) const
{
	float CurGunHeat = 0.0f;
	float MaxGunHeat = 0.0f;
	
	if(Part == EModular::ELeftArm)
	{
		CurGunHeat = CurrentLeftGunHeat;
		MaxGunHeat = LeftArmData.MaxGunOverHeatValue;
	}
	else if(Part == EModular::ERightArm)
	{
		CurGunHeat = CurrentRightGunHeat;
		MaxGunHeat = RightArmData.MaxGunOverHeatValue;
	}
	
	return FMath::Clamp((CurGunHeat / MaxGunHeat), 0.0f, 1.0f);
}

bool AA_Character::GetbIsGunOverHeat(const EModular& Part) const
{
	if(Part == EModular::ELeftArm)
	{
		return bLeftGunIsOverHeat;
	}

	if(Part == EModular::ERightArm)
	{
		return bRightGunIsOverHeat;
	}

	return true;
}

FLinearColor AA_Character::GetGunHeatColor(const EModular& Part) const
{
	FLinearColor GunHeatColor;
	float GunHeatRate = GetGunHeatRate(Part);

	if(GunHeatRate < 0.5f)
	{
		return GunHeatColor.Green;
	}

	if(GunHeatRate < 1.0f)
	{
		GunHeatColor.R = 1.0f;
		GunHeatColor.G = 0.25f;
		GunHeatColor.B = 0.0f;
		GunHeatColor.A = 1.0f;
		
		return GunHeatColor;
	}

	return GunHeatColor.Red;
}

void AA_Character::SetBoostHeat(const float& DeltaHeat)
{
	CurrentBoostHeat = FMath::Clamp(CurrentBoostHeat + DeltaHeat, 0.0f, MaxBoostHeat);

	if(CurrentBoostHeat >= MaxBoostHeat)
	{
		bIsBoostOverHeat = true;
		SetBoostHeatBarColor();
	}
}

void AA_Character::SetGunHeat(const EModular& Part, const float& DeltaHeat)
{
	if(Part == EModular::ELeftArm)
	{
		CurrentLeftGunHeat += DeltaHeat;
	}
	else if(Part == EModular::ERightArm)
	{
		CurrentRightGunHeat += DeltaHeat;
	}
}

void AA_Character::BoostEffect_Implementation()
{
	if(bIsBoostOverHeat)
	{
		return;
	}
	
	if(bPressedJump)
	{
		return;
	}
}

bool AA_Character::GetShootable()
{
	return bIsShootable;
}

void AA_Character::SetbIsShootable(bool NewState)
{
	bIsShootable = NewState;
}

void AA_Character::SetbIsRightArmOnFire(bool NewState)
{
	bIsRightArmOnFire = NewState;
}

bool AA_Character::GetbIsRightArmOnFire()
{
	return bIsRightArmOnFire;
}

void AA_Character::SetbIsLeftArmOnFire(bool NewState)
{
	bIsLeftArmOnFire = NewState;
}

bool AA_Character::GetbIsLeftArmOnFire()
{
	return bIsLeftArmOnFire;
}

#pragma region  RelateJump


void AA_Character::Jump()
{
	
	if(!bIsMovable || bIsBoostOverHeat)
	{
		return;
	}

	SetCharacterState(ECharacterState::EJump);
	// MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
	
	bPressedJump = true;
	JumpKeyHoldTime = GetBoostHeat();
	
}

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

void AA_Character::InitDamageMap()
{
	DamageMap.Add(TEXT("Bone_012"), EModular::EHead);
	DamageMap.Add(TEXT("Bone_001"), EModular::EUpperBody);
	DamageMap.Add(TEXT("UpperArm_L"), EModular::ELeftArm);
	DamageMap.Add(TEXT("UpperArm_R"), EModular::ERightArm);
	DamageMap.Add(TEXT("sine_L"), EModular::ELowerBody);
	DamageMap.Add(TEXT("sine_R"), EModular::ELowerBody);
}

void AA_Character::InitPartsData()
{
	PartsData.Add(&HeadData);
	PartsData.Add(&UpperBodyData);
	PartsData.Add(&LeftArmData);
	PartsData.Add(&RightArmData);
	PartsData.Add(&LowerBodyData);
}

float AA_Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	SetCharacterState(ECharacterState::EDamage);
	
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CheckEachPartPerformance();
	
	return ActualDamage;
}


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

float AA_Character::DamageSpreadToOtherParts(const EModular& CurPart, const float& Damage, const FPointDamageEvent& PointDamageEvent)
{
	float SpreadDamage = Damage;
	float ActualDamage = 0.0f;
	
	switch(CurPart)
	{
		// If UpperBody was Broken, SpreadDamage will 1.5 * Original Damage
	case EModular::EUpperBody:
		SpreadDamage *= 2.0f;
		break;
		// If LeftArm was Broken, SpreadDamage will 0.7 * Original Damage
	case EModular::ELeftArm:
		SpreadDamage *= 0.8f;
		break;
		// RightArm was Broken, SpreadDamage will 0.7 * Original Damage
	case EModular::ERightArm:
		SpreadDamage *= 0.8f;
		break;
		// If LowerBody was Broken, SpreadDamage is same as Original Damage
	case EModular::ELowerBody:
		break;
		// If Head was Broken, It is Dead State
	case EModular::EHead:
		return ActualDamage;
	}

	int8 DeadCount = 0;
	for(FModularItemStatData* Part : PartsData)
	{
		if(Part->Parts == CurPart)
		{
			++DeadCount;
			continue;
		}

		SpreadDamage = (SpreadDamage > Part->HP) ? Part->HP : SpreadDamage;
		Part->HP -= SpreadDamage;
		ActualDamage += SpreadDamage;

		UE_LOG(LogTemp, Log, TEXT("%s_%d : HP is %f"), *GetName(), Part->Parts, Part->HP);

		if(Part->Parts == EModular::EHead && Part->HP <= 0.0f)
		{
			SetCharacterState(ECharacterState::EDead);
			ImpulseToRagdoll(PointDamageEvent);
		}

		if(Part->HP <= 0.0f)
		{
			++DeadCount;

			if(DeadCount >= 4)
			{
				SetCharacterState(ECharacterState::EDead);
				ImpulseToRagdoll(PointDamageEvent);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("%s : %d Parts is Break!"), *GetName(), DeadCount);
	}

	return ActualDamage;
}

FLinearColor AA_Character::SetHPStatColor(EModular Part)
{
	FLinearColor HPStatColor;
	float HPRate = GetHPRate(Part);

	
	if(HPRate > 0.5f)
	{
		return HPStatColor.Green;
	}
	else if(HPRate > 0.0f)
	{
		HPStatColor.R = 1.0f;
		HPStatColor.G = 0.25f;
		HPStatColor.B = 0.0f;
		HPStatColor.A = 1.0f;

		return HPStatColor;
	}

	return HPStatColor.Red;
}

//
// bool AA_Character::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
// 	AActor* DamageCauser) const
// {
// 	return Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
// }

float AA_Character::GetMaxBoostOverHeatCoolTime()
{
	return MaxBoostOverHeatCoolTime;
}

// TODO
// 임시함수
void AA_Character::DoRepair_Implementation(EModular Part,float RepairValue)
{
	switch (Part)
	{
	case EModular::EHead:
		if(HeadData.HP + RepairValue > HeadData.MaxHP)
		{
			HeadData.HP = HeadData.MaxHP;
		}
		else
		{
			HeadData.HP += RepairValue;
		}
		break;
		
	case EModular::EUpperBody:
		if(UpperBodyData.HP + RepairValue > UpperBodyData.MaxHP)
		{
			UpperBodyData.HP = UpperBodyData.MaxHP;
		}
		else
		{
			UpperBodyData.HP += RepairValue;
		}
		break;

	case EModular::ELeftArm:
		if(LeftArmData.HP + RepairValue > LeftArmData.MaxHP)
		{
			LeftArmData.HP = LeftArmData.MaxHP;
		}
		else
		{
			LeftArmData.HP += RepairValue;
		}
		break;
		
	case EModular::ERightArm:
		if(RightArmData.HP + RepairValue > RightArmData.MaxHP)
		{
			RightArmData.HP = RightArmData.MaxHP;
		}
		else
		{
			RightArmData.HP += RepairValue;
		}
		break;
		
	case EModular::ELowerBody:
		if(LowerBodyData.HP + RepairValue > LowerBodyData.MaxHP)
		{
			LowerBodyData.HP = LowerBodyData.MaxHP;
		}
		else
		{
			LowerBodyData.HP += RepairValue;
		}
		break;
	}

	CheckEachPartPerformance();

	CurRepairProgressTime = 0.0f;
	SelectedRepairItemTime = 0.0f;
	SelectedRepairItemValue = 0.0f;
}

FLinearColor AA_Character::SetBoostHeatBarColor() const
{
	FLinearColor ReturnColor;
	if(bIsBoostOverHeat)
	{
		// UE_LOG(LogTemp, Log, TEXT("SetBoostBar_Red"));
		
		return ReturnColor.Red;
	}

	// UE_LOG(LogTemp, Log, TEXT("SetBoostBar_Orange"));
	ReturnColor.R = 1.0f;
	ReturnColor.G = 0.25f;
	ReturnColor.B = 0.0f;
	ReturnColor.A = 1.0f;

	return ReturnColor;
}

void AA_Character::CoolingGunHeat(const EModular& Part, const float& DeltaTime)
{
	if(Part == EModular::ERightArm)
	{
		SetGunHeat(Part, -(RightArmData.MaxGunOverHeatValue * 0.5f  * HeadGunHeatCoolingRate / RightArmData.GunOverHeatCoolTime) * DeltaTime);

		if(CurrentRightGunHeat <= 0.0f)
		{
			bRightGunIsOverHeat = false;
		}
	}
	else if(Part == EModular::ELeftArm)
	{
		SetGunHeat(Part, -(LeftArmData.MaxGunOverHeatValue * 0.5f  * HeadGunHeatCoolingRate / LeftArmData.GunOverHeatCoolTime) * DeltaTime);

		if(CurrentLeftGunHeat <= 0.0f)
		{
			bLeftGunIsOverHeat = false;
		}
	}
}

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

void AA_Character::SetProjectileData(AA_Projectile*& Projectile, const EModular& ShootingSide, const bool& IsPoolProjectile)
{
	// TODO PoolTEST
	if(IsPoolProjectile)
	{
		Projectile->SetLifeSpan(ProjectileLifeSpanTime, true);
	}
	else
	{
		Projectile->SetLifeSpan(ProjectileLifeSpanTime, false);
	}
			
	FVector FireDirection = ProjectileShootRotator.Vector();
	Projectile->FireDirection(FireDirection);

	if(ShootingSide == EModular::ERightArm)
	{
		Projectile->SetProjectieDamage(RightArmDamage);
		CurrentRightGunHeat += RightArmData.GunHeatIncreaseValue;

		// Check OverHeat 
		if(CurrentRightGunHeat >= RightArmData.MaxGunOverHeatValue)
		{
			bRightGunIsOverHeat = true;
			StopFire();
		}
	}
	else if(ShootingSide == EModular::ELeftArm)
	{
		Projectile->SetProjectieDamage(LeftArmDamage);
		CurrentLeftGunHeat += LeftArmData.GunHeatIncreaseValue;

		// Check OverHeat 
		if(CurrentLeftGunHeat >= LeftArmData.MaxGunOverHeatValue)
		{
			bLeftGunIsOverHeat = true;
			StopFire();
		}
	}
			
	UE_LOG(LogTemp, Log, TEXT("Fire!"));
}

void AA_Character::GunCooling(const float& DeltaTime)
{
	// Right Gun OverHeat Check And Cooling
	if(bRightGunIsOverHeat)
	{
		CoolingGunHeat(EModular::ERightArm, DeltaTime);
	}

	// Left Gun OverHeat Check And Cooling
	if(bLeftGunIsOverHeat)
	{
		CoolingGunHeat(EModular::ELeftArm, DeltaTime);
	}

	// Check and Reduce Right Gun's Heat 
	if(CurAttackState != EAttackState::ERightArmAttack)
	{
		if(CurrentRightGunHeat >= 0.0f)
		{
			CoolingGunHeat(EModular::ERightArm, DeltaTime);
		}
	}

	// Check and Reduce Left Gun's Heat 
	if(CurAttackState != EAttackState::ELeftArmAttack)
	{
		if(CurrentLeftGunHeat >= 0.0f)
		{
			CoolingGunHeat(EModular::ELeftArm, DeltaTime);
		}
	}
}

void AA_Character::DoRagDoll()
{
	if(GetCapsuleComponent()->GetCollisionProfileName().Compare(TEXT("Ragdoll")))
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetCapsuleComponent()->SetSimulatePhysics(true);

		TArray<USceneComponent*> SKMArray;
		GetCapsuleComponent()->GetChildrenComponents(true, SKMArray);
		for (auto Component : (SKMArray))
		{
			USkeletalMeshComponent* SKM = Cast<USkeletalMeshComponent>(Component);

			if(SKM)
			{
				SKM->SetCollisionProfileName(TEXT("Ragdoll"));
				SKM->SetSimulatePhysics(true);
			}
		}

		GetCharacterMovement()->DisableMovement();
		UE_LOG(LogTemp, Log, TEXT("%s : Change to Ragdoll"), *GetName());
	}
}

void AA_Character::DeadTest()
{
	SetCharacterState(ECharacterState::EDead);
}

void AA_Character::ImpulseToRagdoll(const FPointDamageEvent& PointDamageEvent)
{
	FPointDamageEvent DamageEvent = PointDamageEvent;
	FName HitedBoneName = DamageEvent.HitInfo.BoneName;
	switch (DamageMap[HitedBoneName.ToString()])
	{
	case EModular::EHead:
		SKM_Head->SetPhysicsLinearVelocity(DamageEvent.ShotDirection * 400.0f);
		SKM_Head->AddImpulseToAllBodiesBelow(DamageEvent.ShotDirection * 400.0f);
		break;
	case EModular::EUpperBody:
		SKM_UpperBody->SetPhysicsLinearVelocity(DamageEvent.ShotDirection * 400.0f);
		SKM_UpperBody->AddImpulseToAllBodiesBelow(DamageEvent.ShotDirection * 400.0f);
		break;
	case EModular::ELeftArm:
		SKM_LeftArm->SetPhysicsLinearVelocity(DamageEvent.ShotDirection * 400.0f);
		SKM_LeftArm->AddImpulseToAllBodiesBelow(DamageEvent.ShotDirection * 400.0f);
		break;
	case EModular::ERightArm:
		SKM_RightArm->SetPhysicsLinearVelocity(DamageEvent.ShotDirection * 400.0f);
		SKM_RightArm->AddImpulseToAllBodiesBelow(DamageEvent.ShotDirection * 400.0f);
		break;
	case EModular::ELowerBody:
		SKM_LowerBody->SetPhysicsLinearVelocity(DamageEvent.ShotDirection * 400.0f);
		SKM_LowerBody->AddImpulseToAllBodiesBelow(DamageEvent.ShotDirection * 400.0f);
		break;
	}
}

//
// void AA_Character::UseInventory()
// {
// 	UUserWidget* CurHUD = 
// }

#pragma endregion RelateJump


void AA_Character::RightArmFireHandler()
{
	if(!GetShootable() || bRightGunIsOverHeat)
	{
		UE_LOG(LogTemp, Log, TEXT("RightArmFireHandler Can't work"));
		UE_LOG(LogTemp, Log, TEXT("Current Right Gun Heat : %f"), CurrentRightGunHeat);
		UE_LOG(LogTemp, Log, TEXT("Right Gun OverHeat : %d"), bRightGunIsOverHeat ? 1 : 0);
		UE_LOG(LogTemp, Log, TEXT("GetShootable : %s"), GetShootable() ? "True" : "False");
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("%s : Enter RightArmFireHandler"), *GetName());
	
	CurAttackState = EAttackState::ERightArmAttack;
	SetCharacterState(ECharacterState::EAttack);
	
	GetWorldTimerManager().SetTimer(ShootHandle, this, &AA_Character::RightArmFire, RightArmShootTerm, true, 0.1f);
}

void AA_Character::RightArmFire()
{
	// TODO
	/* 발사할 Projectile 정보를 RightArm으로부터 가져오는 함수 만들기 및 해당 함수를 RightArm 교체시 실행하기
	* 현재로서는 위의 내용이 구현되어있지 않으므로 임시로 사용할 Projectile 만들어서 사용하기
	*/

	UE_LOG(LogTemp, Log, TEXT("RightHandFire Enter!"));
	
	if(!GetShootable() || !GetbIsRightArmOnFire())
	{
		UE_LOG(LogTemp, Log, TEXT("%s : Can't Shoot!"), *GetName());
		UE_LOG(LogTemp, Warning, TEXT("bIsShootable : %d / bIsCanRightArmFire : %d"), GetShootable(), GetbIsRightArmOnFire());
		return;
	}
	
	if(GetShootable() && GetbIsRightArmOnFire())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = GetInstigator();

		UWorld* World = GetWorld();
		UACGameInstance* ACGameInstance = Cast<UACGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		AA_PoolManager* PoolManager = ACGameInstance->GetPoolManager();
		if(PoolManager)
		{
			// TODO PoolTEST
			AA_Projectile* Projectile = PoolManager->GetThisObject<AA_Projectile>(TempProjectile, ProjectileRespawnLocation + (ProjectileShootRotator.Vector() * 10.0f), ProjectileShootRotator, SpawnParameters);
			// AA_Projectile* Projectile = World->SpawnActor<AA_Projectile>(TempProjectile, ProjectileRespawnLocation + (ProjectileShootRotator.Vector() * 10.0f), ProjectileShootRotator, SpawnParameters);

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
}

void AA_Character::LeftArmFireHandler()
{
	if(!GetShootable() || bLeftGunIsOverHeat)
	{
		UE_LOG(LogTemp, Log, TEXT("LeftArmFireHandler Can't work"));
		UE_LOG(LogTemp, Log, TEXT("Current Left Gun Heat : %f"), CurrentLeftGunHeat);
		UE_LOG(LogTemp, Log, TEXT("Left Gun OverHeat : %d"), bLeftGunIsOverHeat ? 1 : 0);
		UE_LOG(LogTemp, Log, TEXT("GetShootable : %s"), GetShootable() ? "True" : "False");
		return;
	}
	
	// TODO
	// InRate = 총기 발사 텀 LeftArm으로부터 가져오기
	UE_LOG(LogTemp, Log, TEXT("LeftArmFireHandler"));
	CurAttackState = EAttackState::ELeftArmAttack;
	SetCharacterState(ECharacterState::EAttack);
	
	GetWorldTimerManager().SetTimer(ShootHandle, this, &AA_Character::LeftArmFire, LeftArmShootTerm, true, 0.1f);
}

void AA_Character::LeftArmFire()
{	
	if(!GetShootable() || !GetbIsLeftArmOnFire())
	{	
		return;	
	}

	if(GetShootable() && GetbIsLeftArmOnFire())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = GetInstigator();

		UWorld* World = GetWorld();
		UACGameInstance* ACGameInstance = Cast<UACGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		AA_PoolManager* PoolManager = ACGameInstance->GetPoolManager();
		if(PoolManager)
		{
			// TODO PoolTEST
			AA_Projectile* Projectile = PoolManager->GetThisObject<AA_Projectile>(TempProjectile, ProjectileRespawnLocation + (ProjectileShootRotator.Vector() * 10.0f), ProjectileShootRotator, SpawnParameters);
			// AA_Projectile* Projectile = World->SpawnActor<AA_Projectile>(TempProjectile, ProjectileRespawnLocation + (ProjectileShootRotator.Vector() * 10.0f), ProjectileShootRotator, SpawnParameters);

			if(Projectile != nullptr)
			{
				SetProjectileData(Projectile, EModular::ELeftArm, true);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s : %s Pool is empty! Do SpawnActor"), *GetName(), *TempProjectile->GetName());
				Projectile = World->SpawnActor<AA_Projectile>(TempProjectile, ProjectileRespawnLocation + (ProjectileShootRotator.Vector() * 10.0f), ProjectileShootRotator, SpawnParameters);
				if(Projectile != nullptr)
				{
					SetProjectileData(Projectile, EModular::ELeftArm, false);
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
}

void AA_Character::StopFire()
{

	GetWorldTimerManager().ClearTimer(ShootHandle);

	UE_LOG(LogTemp, Warning, TEXT("%s : Stop Fire!"), *GetName());
	
	CurAttackState = EAttackState::EIdle;
	SetCharacterState(ECharacterState::EIdle);
	
	bIsLeftArmOnFire = false;
	bIsRightArmOnFire = false;
}

void AA_Character::SetEachModularPartStat(const FModularItemStatData& ModularItemStatData)
{
	FModularItemStatData ItemStatData = ModularItemStatData;

	EModular Part = ItemStatData.Parts;

	// TODO
	// 디버깅용이므로 추후 삭제?
	const UEnum* CharModularParts = FindObject<UEnum>(ANY_PACKAGE, TEXT("EModular"), true);
	
	// Set each Modular Part's Stat
	switch (Part)
	{
	case EModular::EUpperBody:
		UpperBodyData = ItemStatData;

		if(ItemStatData.SkeletalMesh)
		{
			SKM_UpperBody->SetSkeletalMesh(ItemStatData.SkeletalMesh);
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
		HeadData = ItemStatData;
		HeadDefense = HeadData.Defense;
		HeadGunHeatCoolingRate = HeadData.GunHeatCoolingRate;
		
		if(ItemStatData.SkeletalMesh)
		{
			SKM_Head->SetSkeletalMesh(ItemStatData.SkeletalMesh);
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
		LeftArmData = ItemStatData;
		LeftArmDamage = LeftArmData.Damage;
		LeftArmShootTerm = LeftArmData.ShootTerm;
		
		if(ItemStatData.SkeletalMesh)
		{
			SKM_LeftArm->SetSkeletalMesh(ItemStatData.SkeletalMesh);
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
		RightArmData = ItemStatData;
		RightArmDamage = RightArmData.Damage;
		RightArmShootTerm = RightArmData.ShootTerm;
		
		if(ItemStatData.SkeletalMesh)
		{
			SKM_RightArm->SetSkeletalMesh(ItemStatData.SkeletalMesh);
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
		LowerBodyData = ItemStatData;
		MaxWeight = LowerBodyData.MaxWeight;
		LowerBodyDefense = LowerBodyData.Defense;

		if(ItemStatData.SkeletalMesh)
		{
			SKM_LowerBody->SetSkeletalMesh(ItemStatData.SkeletalMesh);
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
}

/** Temporarly Increase MaxWalkSpeed for Dash */
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

ECharacterState AA_Character::GetCharacterState()
{
	return  CurCharacterState;
}

bool AA_Character::GetMovable()
{
	return bIsMovable;
}

void AA_Character::SetMovable(bool NewMoveState)
{
	bIsMovable = NewMoveState;
}


bool AA_Character::GetbIsRepairing()
{
	return bIsRepairing;
}

void AA_Character::SetbIsReparing(bool NewState)
{
	bIsRepairing = NewState;
}

float AA_Character::GetRepairProgressRate()
{
	if(CurCharacterState != ECharacterState::ERepairing)
	{
		CurRepairProgressTime = 0.0f;
	}
	
	return FMath::Clamp((CurRepairProgressTime / SelectedRepairItemTime), 0.0f, 1.0f);
}

void AA_Character::SetCharacterState(ECharacterState NewState)
{
	CurCharacterState = NewState;
	// TODO
	// 디버깅용이므로 추후 삭제?
	const UEnum* CharCharacterState = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECharacterState"), true);
	
	if(CharCharacterState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character State : %s"), *CharCharacterState->GetNameByValue((int8)CurCharacterState).ToString());
	}
}

// TODO
// void AA_Character::BeginCrouch()
// {
// 	if(bIsMovable)
// 	{
// 		
// 	}
// 	Crouch();
// }
//
// void AA_Character::EndCrouch()
// {
// 	UnCrouch();
// }
