// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_Projectile.generated.h"

UCLASS()
class ESCAPEFROMAC_API AA_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	/** Sets default values for this actor's properties */
	AA_Projectile();

	UFUNCTION()
	void SetProjectieDamage(float NewDamage);

	UFUNCTION()
	float GetProjectileDamage();

	/** Set Projectile Collision Channel using Projectile Owner */
	virtual void SetActorHiddenInGame(bool bNewHidden) override;

	virtual void SetLifeSpan(float InLifespan) override;
	virtual void SetLifeSpan(const float& InLifespan, const bool& IsPoolMember);

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	class USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float SphereRadius = 7.0f;
	
public:	
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Projectile's Movement Component */
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	/** Initialization Projectile's Speed by Shoot Direction */
	UFUNCTION()
	void FireDirection(const FVector& ShootDir);

private:
	UPROPERTY()
	float ProjectileDamage = 0.0f;

	FTimerHandle LifeSpanTimerHandle;

	UFUNCTION()
	void DestroyProjectile();
};
