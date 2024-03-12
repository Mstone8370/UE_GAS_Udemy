// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraAbilityTypes.h"
#include "AuraProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class USoundBase;
class UNiagaraSystem;

USTRUCT()
struct FHomingParam
{
	GENERATED_BODY()

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	UPROPERTY()
	bool bIsHoming = false;

	UPROPERTY()
	float HomingAcceleration = 0.f;

	UPROPERTY()
	bool bIsHomingToSceneComp = false;

	UPROPERTY()
	FVector_NetQuantize HomingSceneCompLocation;

	TWeakObjectPtr<USceneComponent> HomingTarget = nullptr;

	~FHomingParam()
	{
		// HomingTarget = nullptr;
		if (HomingTarget.IsValid())
		{
			HomingTarget.Reset();
		}
	}
};

template<>
struct TStructOpsTypeTraits<FHomingParam> : public TStructOpsTypeTraitsBase2<FHomingParam>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	//~ Begin Homing Field
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateHomingTarget();

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_HomingParam)
	FHomingParam HomingParam;

	UFUNCTION()
	void OnRep_HomingParam(FHomingParam OldHomingParam);

	bool bIsHomingToSceneComp = false;

	FVector HomingSceneCompLocation = FVector::ZeroVector;
	//~ End Homing Field

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	void OnHit();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	bool bHit = false;

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

public:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FDamageEffectParams DamageEffectParams;

};
