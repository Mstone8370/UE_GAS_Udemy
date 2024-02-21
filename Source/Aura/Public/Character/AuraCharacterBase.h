// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UGameplayAbility;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
    GENERATED_BODY()

public:
    AAuraCharacterBase();

protected:
    virtual void BeginPlay() override;

public:
    //~ Begin Combat Interface
    virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
    virtual UAnimMontage* GetHitReactMontage_Implementation() override;
    virtual void Die() override;
    virtual bool IsDead_Implementation() const override;
    virtual AActor* GetAvatar_Implementation() override;
    virtual TArray<FTaggedMontage> GetAttackMontage_Implementation() override;
    //~ End Combat Interface

    // NetMulticast: 서버와 클라이언트에서 모두 실행되고, 모든 클라이언트에 레플리케이트 됨. _Implementation 함수 작성해야함.
    UFUNCTION(NetMulticast, Reliable)
    virtual void MulticastHandleDeath();

    UPROPERTY(EditAnywhere, Category = "Combat")
    TArray<FTaggedMontage> AttackMontages;
    
protected:
    bool bDead;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<USkeletalMeshComponent> Weapon;
    UPROPERTY(EditAnywhere, Category = "Combat")
    FName WeaponTipSocketName;
    UPROPERTY(EditAnywhere, Category = "Combat")
    FName RightHandSocketName;
    UPROPERTY(EditAnywhere, Category = "Combat")
    FName LeftHandSocketName;
    
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;
    
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
    TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

    virtual void InitAbilityActorInfo();

    void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
    virtual void InitializeDefaultAttributes() const;

    void AddCharacterAbilities();

    /**
     * Dissolve Effects
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

    void Dissolve();
    
    UFUNCTION(BlueprintImplementableEvent)
    void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
    UFUNCTION(BlueprintImplementableEvent)
    void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

private:
    UPROPERTY(EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

    UPROPERTY(EditAnywhere, Category = "Combat")
    TObjectPtr<UAnimMontage> HitReactMontage;

public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }
};
