// Copyright 


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystemBlueprintLibrary.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAuraHUD*& OutAuraHUD)
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD());
        if (OutAuraHUD)
        {
            AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
            UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
            UAttributeSet* AS = PS->GetAttributeSet();

            OutWCParams.AbilitySystemComponent = ASC;
            OutWCParams.AttributeSet = AS;
            OutWCParams.PlayerController = PC;
            OutWCParams.PlayerState = PS;
            
            return true;
        }
    }
    return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
    FWidgetControllerParams WCParams;
    AAuraHUD* AuraHUD = nullptr;
    if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
    {
        return AuraHUD->GetOverlayWidgetController(WCParams);
    }
    return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
    const UObject* WorldContextObject)
{
    FWidgetControllerParams WCParams;
    AAuraHUD* AuraHUD = nullptr;
    if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
    {
        return AuraHUD->GetAttributeMenuWidgetController(WCParams);
    }
    return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
    FWidgetControllerParams WCParams;
    AAuraHUD* AuraHUD = nullptr;
    if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
    {
        return AuraHUD->GetSpellMenuWidgetController(WCParams);
    }
    return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
    const AActor* AvatarActor = ASC->GetAvatarActor();
    FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
    ContextHandle.AddSourceObject(AvatarActor);

    const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    
    const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
    const FGameplayEffectSpecHandle PrimaryAttributeSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, ContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributeSpecHandle.Data.Get());

    const FGameplayEffectSpecHandle SecondaryAttributeSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, ContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributeSpecHandle.Data.Get());

    const FGameplayEffectSpecHandle VitalAttributeSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, ContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributeSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
    const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    if (!CharacterClassInfo)
    {
        return;
    }

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterClassInfo->CommonAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        ASC->GiveAbility(AbilitySpec);
    }
    
    const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
    for(const TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
    {
        if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
        {
            FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
            ASC->GiveAbility(AbilitySpec);
        }
    }
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
    const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (!AuraGameMode)
    {
        return nullptr;
    }

    return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
    const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (!AuraGameMode)
    {
        return nullptr;
    }

    return AuraGameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    const FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(Context))
    {
        return AuraContext->IsBlockedHit();
    }
    return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    const FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(Context))
    {
        return AuraContext->IsCriticalHit();
    }
    return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
    const FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(Context))
    {
        return AuraContext->IsSuccessfulDebuff();
    }
    return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
    const FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(Context))
    {
        return AuraContext->GetDebuffDamage();
    }
    return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
    const FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(Context))
    {
        return AuraContext->GetDebuffDuration();
    }
    return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
    const FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(Context))
    {
        return AuraContext->GetDebuffFrequency();
    }
    return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
    const FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(Context))
    {
        if (AuraContext->GetDamageType().IsValid())
        {
            return *AuraContext->GetDamageType();
        }
    }
    return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
    const FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(Context))
    {
        return AuraContext->GetDeathImpulse();
    }
    return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle,
    const bool bInIsBlockedHit)
{
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Context))
    {
        AuraContext->SetIsBlockedHit(bInIsBlockedHit);
    }
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
    const bool bInIsCriticalHit)
{
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Context))
    {
        AuraContext->SetIsCriticalHit(bInIsCriticalHit);
    }
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsSuccessfulDebuff)
{
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Context))
    {
        AuraContext->SetIsSuccessfulDebuff(bInIsSuccessfulDebuff);
    }
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, const float InDebuffDamage)
{
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Context))
    {
        AuraContext->SetDebuffDamage(InDebuffDamage);
    }
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, const float InDebuffDuration)
{
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Context))
    {
        AuraContext->SetDebuffDuration(InDebuffDuration);
    }
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, const float InDebuffFrequency)
{
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Context))
    {
        AuraContext->SetDebuffFrequency(InDebuffFrequency);
    }
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Context))
    {
        AuraContext->SetDamageType(InDamageType);
    }
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InImpulse)
{
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Context))
    {
        AuraContext->SetDeathImpulse(InImpulse);
    }
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin)
{
    // UGameplayStatics::ApplyRadialDamageWithFalloff Âü°í
    FCollisionQueryParams SphereParams;
    SphereParams.AddIgnoredActors(ActorsToIgnore);

    TArray<FOverlapResult> Overlaps;
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
    }

    for (const FOverlapResult& Overlap : Overlaps)
    {
        if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
        {
            OutOverlappingActors.AddUnique(Overlap.GetActor());
        }
    }
}

bool UAuraAbilitySystemLibrary::IsEnemy(AActor* FirstActor, AActor* SecondActor)
{
    const bool bBothArePlayer = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
    const bool bBothAreEnemy = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
    return !(bBothArePlayer || bBothAreEnemy);
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(FDamageEffectParams Params)
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

    FGameplayEffectContextHandle ContextHandle = Params.SourceAbilitySystemComponent->MakeEffectContext();
    ContextHandle.AddSourceObject(Params.SourceAbilitySystemComponent->GetAvatarActor());
    UAuraAbilitySystemLibrary::SetDeathImpulse(ContextHandle, Params.DeathImpulse);

    FGameplayEffectSpecHandle SpecHandle = Params.SourceAbilitySystemComponent->MakeOutgoingSpec(Params.DamageGameplayEffectClass, Params.AbilityLevel, ContextHandle);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Params.DamageType, Params.BaseDamage);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Param_Chance, Params.DebuffChance);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Param_Damage, Params.DebuffDamage);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Param_Frequency, Params.DebuffFrequency);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Param_Duration, Params.DebuffDuration);
    
    Params.SourceAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), Params.TargetAbilitySystemComponent);

    return ContextHandle;
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
    const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    if (!CharacterClassInfo)
    {
        return 0;
    }

    const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
    const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

    return int32(XPReward);
}
