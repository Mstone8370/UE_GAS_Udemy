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
     * OverlayWidget에 Aura캐릭터의 사용 가능한 스킬을 표시하기위한 목적.
     * 캐릭터가 어빌리티를 먼저 받아야 UI에 그 어빌리티가 무엇인지 띄울 수 있으므로 순서대로 진행해야 함.
     * 하지만 GiveAbility는 서버에서만 작동하므로 클라이언트에서는 캐릭터가 언제 어빌리티를 받는지 모름.
     * 따라서 가지고있는 어빌리티 정보를 가지고있는 ActivatableAbilities 변수가 클라이언트로 레플리케이트되는 때를 사용함.
     */
    virtual void OnRep_ActivateAbilities() override;
};
