// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilityGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FAbilityStatusChanged, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
    FEffectAssetTags EffectAssetTags;
    FAbilityGiven AbilitiesGivenDelegate;
    FAbilityStatusChanged AbilityStatusChanged;

    bool bStartupAbilitiesGiven = false;
    
    void AbilityActorInfoSet();

    void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
    void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

    void AbilityInputTagHeld(const FGameplayTag& InputTag);
    void AbilityInputTagReleased(const FGameplayTag& InputTag);

    void ForEachAbility(const FForEachAbility& Delegate);

    static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);

    const FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

    void UpgradeAttribute(const FGameplayTag& AttributeTag);

    void UpdateAbilityStatus(int32 Level);

    UFUNCTION(Server, Reliable)
    void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

protected:
    UFUNCTION(Client, Reliable)
    void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

    UFUNCTION(Client, Reliable)
    void ClientUpdateAbilityState(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag);

    /**
     * OverlayWidget�� Auraĳ������ ��� ������ ��ų�� ǥ���ϱ����� ����.
     * ĳ���Ͱ� �����Ƽ�� ���� �޾ƾ� UI�� �� �����Ƽ�� �������� ��� �� �����Ƿ� ������� �����ؾ� ��.
     * ������ GiveAbility�� ���������� �۵��ϹǷ� Ŭ���̾�Ʈ������ ĳ���Ͱ� ���� �����Ƽ�� �޴��� ��.
     * ���� �������ִ� �����Ƽ ������ �������ִ� ActivatableAbilities ������ Ŭ���̾�Ʈ�� ���ø�����Ʈ�Ǵ� ���� �����.
     */
    virtual void OnRep_ActivateAbilities() override;
};
