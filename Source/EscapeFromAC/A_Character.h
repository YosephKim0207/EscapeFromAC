// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "A_Projectile.h"
#include "S_Inventory.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "A_Character.generated.h"


UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	EWalking,
	EJump,
	EAttack,
	EDamage,
	ECheckItems,
	ERepairing,
	EIdle,
	EDead,
};

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	ELeftArmAttack,
	ERightArmAttack,
	EIdle,
};



UCLASS()
class ESCAPEFROMAC_API AA_Character : public ACharacter
{
	GENERATED_BODY()

public:
	/** Sets default values for this character's properties */
	AA_Character();

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Character move Forward and Backward by Keyboard W, S */
	void MoveForward(float value);
	/** Character move Right and Left by Keyboard A, D */
	void MoveRight(float value);

	/** Character Look Rotate Horizontal */
	virtual void AddControllerPitchInput(float Val) override;
	/** Character Look Rotate Vertical */
	virtual void AddControllerYawInput(float Val) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxAccel;

	/** Get Each Body part's HP */
	UFUNCTION(BlueprintCallable, Category = "Character Status")
	float GetHP(const EModular& Part) const;
	
	/** Get Each Body part's Max HP */
	UFUNCTION(BlueprintCallable, Category = "Character Status")
	float GetMaxHP(const EModular& Part) const;
	
	/** Get Each Body part's HP Rate */
	UFUNCTION(BlueprintCallable, Category = "Character Status")
	float GetHPRate(const EModular& Part) const;

	/** Get sum of each Modular Part's Current HP */
	UFUNCTION()
	float GetTotalHPRate() const;
	
	UFUNCTION(BlueprintPure, Category = "Character Status")
	float GetBoostHeatRate() const;
	
	UFUNCTION(BlueprintPure, Category = "Character Status")
	float GetBoostHeat() const;
	
	UFUNCTION(BlueprintPure, Category = "Character Status")
	float GetGunHeatRate(const EModular& Part) const;

	UFUNCTION(BlueprintPure, Category = "Character Status")
	bool GetbIsGunOverHeat(const EModular& Part) const;

	UFUNCTION(BlueprintCallable, Category = "Character Status")
	FLinearColor GetGunHeatColor(const EModular& Part) const;
	
	UFUNCTION(BlueprintCallable, Category = "Character Status")
	void SetBoostHeat(const float& DeltaHeat);

	UFUNCTION(BlueprintCallable, Category = "Character Status")
	void SetGunHeat(const EModular& Part, const float& DeltaHeat);

	UFUNCTION(BlueprintNativeEvent)
	void BoostEffect();
	void BoostEffect_Implementation();

	UFUNCTION()
	bool GetShootable();

	UFUNCTION(BlueprintCallable)
	void SetbIsShootable(bool NewState);

	UFUNCTION(BlueprintCallable)
	void SetbIsRightArmOnFire(bool NewState);

	UFUNCTION(BlueprintCallable)
	bool GetbIsRightArmOnFire();

	UFUNCTION(BlueprintCallable)
	void SetbIsLeftArmOnFire(bool NewState);

	UFUNCTION(BlueprintCallable)
	bool GetbIsLeftArmOnFire();

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/**
	 * Damage to where Projectile conflict to modular part
	 * Work in Supper::TakeDamage Function
	 */
	UFUNCTION()
	virtual float InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	
	UFUNCTION()
	virtual void Dash();

	UFUNCTION(BlueprintPure, Category = "Character State")
	ECharacterState GetCharacterState();

	UPROPERTY(EditDefaultsOnly, Category = "Custom Movement")
	float DashSpeedRate;

	UFUNCTION(BlueprintCallable)
	bool GetMovable();

	UFUNCTION(BlueprintCallable)
	void SetMovable(bool NewMoveState);


	UFUNCTION(BlueprintCallable)
	bool GetbIsRepairing();

	UFUNCTION(BlueprintCallable)
	void SetbIsReparing(bool NewState);

	/** Increase BP_Widget RepairLoading's ProgressBar Percent */
	UFUNCTION(BlueprintCallable)
	float GetRepairProgressRate();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UAIPerceptionStimuliSourceComponent* AiPerceptionStimuliSourceComponent;

	UFUNCTION()
	virtual void RightArmFireHandler();
	
	UFUNCTION()
	virtual void LeftArmFireHandler();

	UFUNCTION()
	virtual void StopFire();
	
protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Called when After Character is Dead */
	UFUNCTION()
	virtual void DoAfterDead(const float& DeltaSeconds);
	
	UFUNCTION()
	virtual void RightArmFire();
	
	UFUNCTION()
	virtual void LeftArmFire();
	
	UFUNCTION()
	virtual void SetEachModularPartStat(const FModularItemStatData& ModularItemStatData);

	/**
	 * Refresh Character's Modular Data
	 * Take Data from Each Part's PickupItem and Assign a value to variables
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterModularData")
	void RefreshModularParts();
	virtual void RefreshModularParts_Implementation();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BodySkeletalMesh")
	class USkeletalMeshComponent* SKM_UpperBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BodySkeletalMesh")
	class USkeletalMeshComponent* SKM_BackAccessory;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BodySkeletalMesh")
	class USkeletalMeshComponent* SKM_LowerBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BodySkeletalMesh")
	class USkeletalMeshComponent* SKM_Head;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BodySkeletalMesh")
	class USkeletalMeshComponent* SKM_LeftArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BodySkeletalMesh")
	class USkeletalMeshComponent* SKM_RightArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BodySkeletalMesh")
	class USkeletalMeshComponent* SKM_FaceAccessory;
	
	/** Accel for Movement, Make Movement looks like Heavy Robot */
	float Accel = 0.1f;

	bool bIsDashStop = false;

	bool bIsMovable;

	bool bIsRightArmOnFire = false;

	bool bIsLeftArmOnFire = false;

	/** Can Shoot Trigger */ 
	bool bIsShootable;

	/** TimerHandle for shootTerm */ 
	UPROPERTY()
	FTimerHandle ShootHandle;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AA_Projectile> TempProjectile;

	UPROPERTY()
	FVector ProjectileRespawnLocation;

	UPROPERTY()
	FRotator ProjectileShootRotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float TotalWeight = 0.0f;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MaxWeight = 0.0f;

	UPROPERTY(VisibleAnywhere)
	bool bIsRepairing = false;

	UPROPERTY()
	TMap<FString, EModular> DamageMap;

	UFUNCTION(BlueprintCallable)
	void SetCharacterState(ECharacterState NewState);

	virtual void Jump() override;

	UFUNCTION()
	virtual void CheckJumpInput(float DeltaTime) override;

	UFUNCTION()
	virtual void ResetJumpState() override;

	/** Mapping BoneName FString to EModular */ 
	UFUNCTION()
	void InitDamageMap();

	/** Add Each Part Struct to PartsData Array */ 
	UFUNCTION()
	void InitPartsData();

	/**
	 * If Part's HP is Zero, Damage Spread to other parts. Spread Damage will Calculate in Func
	 * @param CurPart	Origin DamagePoint
	 * @param Damage	Origin's taken Damage
	 */
	UFUNCTION()
	float DamageSpreadToOtherParts(const EModular& CurPart, const float& Damage, const FPointDamageEvent& PointDamageEvent);

	UFUNCTION(BlueprintCallable, Category = "Caracter Status")
	FLinearColor SetHPStatColor(EModular Part);

	UFUNCTION()
	float GetMaxBoostOverHeatCoolTime();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DoRepair(EModular Part, float RepairValue);
	void DoRepair_Implementation(EModular Part, float RepairValue);

	/** Impulse When Dead status */
	UFUNCTION()
	virtual void ImpulseToRagdoll(const FPointDamageEvent& PointDamageEvent);
	
private:
	/** Store Modular Items what Character Equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterModularData", meta = (AllowPrivateAccess = true))
	TArray<FDataTableRowHandle> ModularInventory;

	UPROPERTY()
	ECharacterState CurCharacterState;


	UPROPERTY(BlueprintReadOnly, Category = "ShootTest", meta = (AllowPrivateAccess = true))
	EAttackState CurAttackState;

	
	bool bIsDash = false;
	float DashCoolTime = 0.3f;
	float CanDashTimeLimit = 0.5f;
	float CanDashTimeCount = 0.0f;
	int32 DashMaxCount = 2;
	int32 DashCount = 0;
	float OriginalMaxWalkSpeed;
	float TempMaxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Status", meta = (AllowPrivateAccess = true))
	float MaxBoostHeat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status", meta = (AllowPrivateAccess = true))
	float CurrentBoostHeat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status", meta = (AllowPrivateAccess = true))
	bool bIsBoostOverHeat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Status", meta = (AllowPrivateAccess = true))
	float MaxBoostOverHeatCoolTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status", meta = (AllowPrivateAccess = true))
	float RemainingBoostOverHeatCoolTime;
	
	UFUNCTION(BlueprintCallable, Category = "Character Status", meta = (AllowPrivateAccess = true))
	FLinearColor SetBoostHeatBarColor() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Status", meta = (AllowPrivateAccess = true))
	float BoostHeatReduceRate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status", meta = (AllowPrivateAccess = true))
	float CurrentLeftGunHeat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status", meta = (AllowPrivateAccess = true))
	float RemainingLeftGunOverHeatCoolTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status", meta = (AllowPrivateAccess = true))
	bool bLeftGunIsOverHeat = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status", meta = (AllowPrivateAccess = true))
	float CurrentRightGunHeat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status", meta = (AllowPrivateAccess = true))
	float RemainingRightGunOverHeatCoolTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Status", meta = (AllowPrivateAccess = true))
	bool bRightGunIsOverHeat = false;

	UFUNCTION()
	void CoolingGunHeat(const EModular& Part, const float& DeltaTime);
	
	UPROPERTY()
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY()
	FVector CurMovementDirection = FVector(0.0f, 0.0f, 0.0f);

	TArray<FModularItemStatData*> PartsData;

	UPROPERTY()
	FModularItemStatData HeadData;
	UPROPERTY()
	FModularItemStatData UpperBodyData;
	UPROPERTY()
	FModularItemStatData LowerBodyData;
	UPROPERTY()
	FModularItemStatData LeftArmData;
	UPROPERTY()
	FModularItemStatData RightArmData;

	UPROPERTY()
	float HeadDefense;
	UPROPERTY()
	float HeadGunHeatCoolingRate;

	UPROPERTY()
	float UpperBodyDefense;

	UPROPERTY()
	float LowerBodyDefense;

	UPROPERTY()
	float LeftArmDamage;
	UPROPERTY()
	float LeftArmShootTerm;

	UPROPERTY()
	float RightArmDamage;
	UPROPERTY()
	float RightArmShootTerm;

	UPROPERTY()
	float TotalDefense = 0.0f;

	/** LifeSpanTime for Hidden or Destroy when Projectile is not conflict to object */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float ProjectileLifeSpanTime = 5.0f;

	/**
	 * Use for BP_Widget RepairLoading's ProgressBar
	 * CurRepairProgressTime / SelectedRepairItemTime
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Repair", meta = (AllowPrivateAccess = true))
	float CurRepairProgressTime = 0.0f;

	/**
	 * Use for BP_Widget RepairLoading's ProgressBar
	 * CurRepairProgressTime / SelectedRepairItemTime
	 * Variable get value when click 'Use Item' Button from BP_Widget ActionMenu
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Repair", meta = (AllowPrivateAccess = true))
	float SelectedRepairItemTime = 0.0f;

	/**
	 * Repair Modular Part Data's HP
	 * Variable get value when click 'Use Item' Button from BP_Widget ActionMenu
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Repair", meta = (AllowPrivateAccess = true))
	float SelectedRepairItemValue = 0.0f;

	/**
	 * Modular Part what want to repair
	 * Variable get value when click 'RepairPopupButton' from BP_Widget RepairStatusPopup
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EModular SelectedRepairPart;

	/** Check Each modular part's HP and fix performance  */
	UFUNCTION()
	void CheckEachPartPerformance();

	UFUNCTION()
	void SetProjectileData(AA_Projectile*& Projectile, const EModular& ShootingSide, const bool& IsPoolProjectile);

	/** Relate with Gun Cooling */
	UFUNCTION()
	void GunCooling(const float& DeltaTime);

	/** Actor physics change to RagDoll */
	UFUNCTION()
	void DoRagDoll();

	UFUNCTION()
	void DeadTest();
};
