// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Projectile.h"

#include "ACGameInstance.h"
#include "A_EnemyDefault.h"
#include "A_PoolManager.h"
#include "C_Player.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AA_Projectile::AA_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create SphereComponent for Projectile's Collision and RootComponent */
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(SphereRadius);
	RootComponent = SphereComponent;

	// TODO PoolTEST
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	/** Set ProjectileMovementComponent for Projectile's move */
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	// TODO Speed For Aim Test
	ProjectileMovementComponent->InitialSpeed = 1200.0f;
	ProjectileMovementComponent->MaxSpeed = 1200.0f;
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;
	ProjectileMovementComponent->bShouldBounce = false;

	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AA_Projectile::SetProjectieDamage(float NewDamage)
{
	ProjectileDamage = NewDamage;
}

float AA_Projectile::GetProjectileDamage()
{
	return ProjectileDamage;
}

// Called when the game starts or when spawned
void AA_Projectile::BeginPlay()
{
	Super::BeginPlay();
}

void AA_Projectile::SetActorHiddenInGame(bool IsNewHiddenState)
{
	UE_LOG(LogTemp, Log, TEXT("%s : Enter SetActorHiddenInGame"), *GetName());

	if(!IsNewHiddenState)
	{
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AActor* Owner = GetOwner();
		if(Cast<AC_Player>(Owner))
		{
			SphereComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile_Player"));
			UE_LOG(LogTemp, Log, TEXT("Set %s's Collisioin Profile : %s"), *GetName(), *StaticMeshComponent->BodyInstance.GetCollisionProfileName().ToString());
		}
		else if(Cast<AA_EnemyDefault>(Owner))
		{
			SphereComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile_Enemy"));
			UE_LOG(LogTemp, Log, TEXT("Set %s's Collisioin Profile : %s"), *GetName(), *StaticMeshComponent->BodyInstance.GetCollisionProfileName().ToString());
		}
		else
		{
			SetActorHiddenInGame(true);
		}
	}
	else
	{
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
		SetActorLocation(FVector::ZeroVector);
		ProjectileMovementComponent->Velocity = FVector::ZeroVector;
		GetWorldTimerManager().ClearTimer(LifeSpanTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("%s is Hidden!"), *GetName());
	}
	
	Super::SetActorHiddenInGame(IsNewHiddenState);
}

void AA_Projectile::SetLifeSpan(float InLifespan)
{
	Super::SetLifeSpan(InLifespan);
}

void AA_Projectile::SetLifeSpan(const float& InLifespan, const bool& IsPoolMember)
{
	// If Actor is Pool member, Actor will hidden after InLifespan Time
	if(IsPoolMember)
	{
		GetWorldTimerManager().SetTimer(LifeSpanTimerHandle, FTimerDelegate::CreateUObject(this, &AA_Projectile::SetActorHiddenInGame, true), InLifespan, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(LifeSpanTimerHandle, this, &AA_Projectile::DestroyProjectile, InLifespan, false, InLifespan);
;	}
}


void AA_Projectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                              bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	UE_LOG(LogTemp, Log, TEXT("%s Hit!"), *GetName());
	
	FHitResult HitResult = Hit;

	if(HitResult.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("Projectile is Hit to %s"), *HitResult.GetActor()->GetName());
		UE_LOG(LogTemp, Log, TEXT("Projectile Hit BoneName : %s"), *HitResult.BoneName.ToString());

		AA_Character* Character = Cast<AA_Character>(HitResult.GetActor());

		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		if(Character)
		{
			FPointDamageEvent PointDamageEvent;
			PointDamageEvent.Damage = ProjectileDamage;
			PointDamageEvent.HitInfo = HitResult;

			FVector ProjectileDirection = GetVelocity();
			ProjectileDirection.Normalize();
			PointDamageEvent.ShotDirection = ProjectileDirection;
			if(GetInstigator())
			{
				UE_LOG(LogTemp, Log, TEXT("Projectile : Instigator is %s"), *GetInstigator()->GetName());
				
				AController* Controller = GetInstigator()->GetController();
				if(Controller)
				{
					Character->TakeDamage(ProjectileDamage, PointDamageEvent, Controller, this);
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Projectile : Don't have Instigator controller"));
					return;
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Projectile : Don't have Instigator"));
				return;
			}
		}
	}
	
	
	SetActorHiddenInGame(true);
	UE_LOG(LogTemp, Log, TEXT("Projectile : %s is Hidden"), *GetName());
}

// Called every frame
void AA_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Init Projectile's Speed by Shoot Direction
void AA_Projectile::FireDirection(const FVector& ShootDir)
{
	if(ProjectileMovementComponent)
	{
		if(ProjectileMovementComponent->HasStoppedSimulation())
		{			
			ProjectileMovementComponent->SetActive(true);
			UE_LOG(LogTemp, Log, TEXT("Projectile : %s's ProjectileMovementCompoment Do SetActive true"), *GetName());

			ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
			UE_LOG(LogTemp, Log, TEXT("Projectile : %s's ProjectileMovementCompoment Set UpdatedComponet SphereComponent"), *GetName());
			
			if(ProjectileMovementComponent->UpdatedComponent == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Projectile : %s's ProjectileMovementCompoment UpdateComponent is nullptr"), *GetName());
			}

			if(ProjectileMovementComponent->IsActive() == false)
			{
				UE_LOG(LogTemp, Error, TEXT("Projectile : %s's ProjectileMovementCompoment IsActive is false"), *GetName());
			}

		}
		
		ProjectileMovementComponent->Velocity = ShootDir * ProjectileMovementComponent->InitialSpeed;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ProjectileMovementComponent is NULL"));
	}
}

void AA_Projectile::DestroyProjectile()
{
	GetWorldTimerManager().ClearTimer(LifeSpanTimerHandle);
	Destroy();
}

