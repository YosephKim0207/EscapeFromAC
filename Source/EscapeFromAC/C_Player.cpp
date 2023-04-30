// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Player.h"

#include "ACGameInstance.h"
#include "A_Pickup.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/RepLayout.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

AC_Player::AC_Player()
{
	PrimaryActorTick.bCanEverTick = true;

	/** Set SpringArm */
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->AddRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SpringArmComponent->TargetArmLength = 250.0f;
	SpringArmComponent->AddRelativeRotation(FRotator(-22.0f, 0.0f, 0.0f));
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 20.0f;
	SpringArmComponent->CameraLagMaxDistance = 200.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	AiPerceptionStimuliSourceComponent->SetAutoActivate(true);
	
	/** Set Camera */
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArmComponent);
	Camera->AddRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	Camera->AddRelativeRotation(FRotator(15.0f, 0.0f, 0.0f));
	
	Camera->bUsePawnControlRotation = false;

	bIsLineTrace = false;
	bItemEmpasis = false;
}

void AC_Player::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->Possess(this);
	PlayerController = GetController();
	// EObjectFlags ObjectFlags;
	// CollectGarbage(ObjectFlags);
}

/** Set bIsLineTrace For Interaction with Item */
void AC_Player::SetLineTrace(bool NewState)
{
	bIsLineTrace = NewState;
}

void AC_Player::CheckFieldItem()
{
	// Check Turn On / Off the Line Trace
	if(bIsLineTrace)
	{
		FHitResult LineTraceOut;
		FVector Location;
		FRotator Rotator;
		if(PlayerController)
		{
			PlayerController->GetPlayerViewPoint(Location, Rotator);
			FVector Start = Location;
			FCollisionQueryParams TraceParams;
			
			// Ref : https://forums.unrealengine.com/t/third-person-shooter-aiming-at-crosshair/129180/6
			FVector TraceStart = UKismetMathLibrary::ProjectPointOnToPlane(Location, ProjectileRespawnLocation, Rotator.Vector());
			FVector TraceEnd = TraceStart + (Rotator.Vector() * ItemCheckRange);
		
			GetWorld()->LineTraceSingleByChannel(LineTraceOut, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 2.0f);

			AA_Item* tempItem = Cast<AA_Item>(LineTraceOut.GetActor());
			// If 
			if(Item && !tempItem)
			{
				Item->SetIsInRange(false);
				Item = nullptr;

				return;
			}
		
			Item = tempItem;
		
			if(Item)
			{
				// UE_LOG(LogTemp, Log, TEXT("Chek Field Item : %s"), *Item->GetName());

				//TODO
				if(!bItemEmpasis)
				{
					// Item에 protected 변수로 IsBag을 만들어두고 true면 Blueprint에서 Bag Invetory 재생
					//false면 아이템 습득
					Item->SetIsInRange(true);
					bIsBag = Item->GetIsBag();
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s : CheckFieldItem PlayerControlelr is nullPtr!"));
		}
		
	}
	
}

void AC_Player::RightArmFire()
{
	if(SKM_UpperBody->GetSocketByName(TEXT("RightHandProjectileRespawnLocation")))
	{
		ProjectileRespawnLocation = SKM_UpperBody->GetSocketLocation(TEXT("RightHandProjectileRespawnLocation"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player : No Projectile Respawn Socket"));
	}
	
	// Shoot LineTrace for Shoot Aim
	if(PlayerController)
	{
		FVector TempLocationCamera;
		FVector TempLocationTraceEnd;
		FRotator TempRotatorCamera;

		PlayerController->GetPlayerViewPoint(TempLocationCamera, TempRotatorCamera);

		TempLocationTraceEnd = TempLocationCamera + (TempRotatorCamera.Vector() * ShootRange);

		// DrawDebugLine(GetWorld(), TempLocation_Camera, TempLocation_TraceEnd, FColor::Purple, false, 5.0f);
		
		// Ref : https://forums.unrealengine.com/t/third-person-shooter-aiming-at-crosshair/129180/6
		FVector TraceStart = UKismetMathLibrary::ProjectPointOnToPlane(TempLocationCamera, ProjectileRespawnLocation, TempRotatorCamera.Vector());
		FVector TraceEnd = TraceStart + (TempRotatorCamera.Vector() * ShootRange);
		
			
		FHitResult LineTraceOut;
		FCollisionQueryParams TraceParams;
		GetWorld()->LineTraceSingleByChannel(LineTraceOut, TempLocationCamera, TempLocationTraceEnd, ECC_Visibility, TraceParams);
		// DrawDebugLine(GetWorld(), TempLocation_Camera, TempLocation_TraceEnd, FColor::Orange, false, 2.0f);

		GetWorld()->LineTraceSingleByChannel(LineTraceOut, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 5.0f);
		DrawDebugLine(GetWorld(), ProjectileRespawnLocation, LineTraceOut.Location, FColor::Red, false, 2.0f);
		
		
		if(LineTraceOut.bBlockingHit)
		{
			FVector TraceImpactPoint = LineTraceOut.ImpactPoint;
			ProjectileShootRotator = (TraceImpactPoint - ProjectileRespawnLocation).Rotation();
		}
		else
		{
			ProjectileShootRotator = (TempLocationTraceEnd - ProjectileRespawnLocation).Rotation();
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : RightArmFire PlayerControlelr is nullPtr!"));
	}
	Super::RightArmFire();
}

void AC_Player::LeftArmFire()
{
	if(SKM_UpperBody->GetSocketByName(TEXT("LeftHandProjectileRespawnLocation")))
	{
		ProjectileRespawnLocation = SKM_UpperBody->GetSocketLocation(TEXT("LeftHandProjectileRespawnLocation"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player : No Projectile Respawn Socket"));
	}
	
	
	// Shoot LineTrace for Shoot Aim
	if(PlayerController)
	{
		FVector TempLocation_Camera;
		FVector TempLocation_TraceEnd;
		FRotator TempRotator_Camera;

		PlayerController->GetPlayerViewPoint(TempLocation_Camera, TempRotator_Camera);

		TempLocation_TraceEnd = TempLocation_Camera + (TempRotator_Camera.Vector() * ShootRange);

		// Ref : https://forums.unrealengine.com/t/third-person-shooter-aiming-at-crosshair/129180/6
		FVector TraceStart = UKismetMathLibrary::ProjectPointOnToPlane(TempLocation_Camera, ProjectileRespawnLocation, TempRotator_Camera.Vector());
		FVector TraceEnd = TraceStart + (TempRotator_Camera.Vector() * ShootRange);
			
		FHitResult LineTraceOut;
		FCollisionQueryParams TraceParams;
		// GetWorld()->LineTraceSingleByChannel(LineTraceOut, TempLocation_Camera, TempLocation_TraceEnd, ECC_Visibility, TraceParams);
		// DrawDebugLine(GetWorld(), TempLocation_Camera, TempLocation_TraceEnd, FColor::Orange, false, 2.0f);

		GetWorld()->LineTraceSingleByChannel(LineTraceOut, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
		// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 2.0f);
		// DrawDebugLine(GetWorld(), ProjectileRespawnLocation, LineTraceOut.Location, FColor::Red, false, 2.0f);
		
		
		if(LineTraceOut.bBlockingHit)
		{
			FVector TraceImpactPoint = LineTraceOut.ImpactPoint;
			ProjectileShootRotator = (TraceImpactPoint - ProjectileRespawnLocation).Rotation();
		}
		else
		{
			ProjectileShootRotator = (TempLocation_TraceEnd - ProjectileRespawnLocation).Rotation();
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : LeftArmFire PlayerControlelr is nullPtr!"));
	}
	
	Super::LeftArmFire();
}

void AC_Player::DoAfterDead(const float& DeltaSecond)
{
	if(bControllerIsAttached)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : Dead"), *GetName());
		
		bControllerIsAttached = false;
		GetMovementComponent()->StopMovementImmediately();
		DetachFromControllerPendingDestroy();
	}
	else
	{
		RespawnTime -= DeltaSecond;

		if(RespawnTime < 0.0f)
		{
			if(PlayerIsDead.IsBound())
			{
				PlayerIsDead.Broadcast();
			}

			UACGameInstance* ACGameInstance = Cast<UACGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if(ACGameInstance)
			{
				TSubclassOf<AC_Player> Player = ACGameInstance->GetPlayer();

				if(Player)
				{
					AActor* PlayerStart = GetWorld()->GetAuthGameMode()->FindPlayerStart(GetWorld()->GetFirstPlayerController());
					if(PlayerStart)
					{
						FVector RespawnLocation = PlayerStart->GetActorLocation();
						FRotator RespawnRotator = PlayerStart->GetActorRotation();
						FActorSpawnParameters ActorSpawnParameters;
						AC_Player* PlayerActor = GetWorld()->SpawnActor<AC_Player>(Player, RespawnLocation, RespawnRotator, ActorSpawnParameters);
						GetWorld()->GetFirstPlayerController()->Possess(PlayerActor);
						Destroy();
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("%s : DoAfterDead, PlayerStart Not Found!"));
					}
					
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("%s : DoAfterDead, Player Not Found!"));
				}
				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s : DoAfterDead, Cast UACGameInstance Fail!"));
			}
		}
	}
	
	
	
	
}

// void AC_Player::SetOverlapBagInventory(TArray<FPickupItemData> &NewBagInventory)
// {
// 	OverlapBagInventory = &NewBagInventory;
// }

// void AC_Player::PostInitializeComponents()
// {
// 	Super::PostInitializeComponents();
// }

void AC_Player::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bIsLineTrace)
	{
		CheckFieldItem();
	}
	else if(Item)
	{
		Item = nullptr;
	}
	
}
