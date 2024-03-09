// Copyright 


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
    bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
    Super::BeginPlay();

    ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
    if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
    {
        AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
    }
    else if (CombatInterface)
    {
        CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(
            this,
            [this](UAbilitySystemComponent* InASC)
            {
                if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(InASC))
                {
                    AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
                }
            }
        );
    }

    if (CombatInterface)
    {
        CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UPassiveNiagaraComponent::OnOwnerDeath);
    }
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
    if (AbilityTag.MatchesTagExact(PassiveSpellTag))
    {
        if (bActivate)
        {
            Activate();
        }
        else
        {
            Deactivate();
        }
    }
}

void UPassiveNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
    Deactivate();
}
