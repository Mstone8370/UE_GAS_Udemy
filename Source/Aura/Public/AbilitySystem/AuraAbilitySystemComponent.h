// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilityGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);

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

    bool bStartupAbilitiesGiven = false;
    
    void AbilityActorInfoSet();

    void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
    void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

    void AbilityInputTagHeld(const FGameplayTag& InputTag);
    void AbilityInputTagReleased(const FGameplayTag& InputTag);

    void ForEachAbility(const FForEachAbility& Delegate);

    static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
    static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

    void UpgradeAttribute(const FGameplayTag& AttributeTag);

    UFUNCTION(Server, Reliable)
    void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

protected:
    UFUNCTION(Client, Reliable)
    void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

    /**
     * OverlayWidget�� Auraĳ������ ��� ������ ��ų�� ǥ���ϱ����� ����.
     * ĳ���Ͱ� �����Ƽ�� ���� �޾ƾ� UI�� �� �����Ƽ�� �������� ��� �� �����Ƿ� ������� �����ؾ� ��.
     * ������ GiveAbility�� ���������� �۵��ϹǷ� Ŭ���̾�Ʈ������ ĳ���Ͱ� ���� �����Ƽ�� �޴��� ��.
     * ���� �������ִ� �����Ƽ ������ �������ִ� ActivatableAbilities ������ Ŭ���̾�Ʈ�� ���ø�����Ʈ�Ǵ� ���� �����.
     */
    virtual void OnRep_ActivateAbilities() override;
};
