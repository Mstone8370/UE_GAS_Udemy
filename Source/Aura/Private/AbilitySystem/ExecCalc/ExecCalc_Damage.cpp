// Copyright 


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Aura/Aura.h"

struct AuraDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
    
    DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

    TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
    
    AuraDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

        /* 엔진 로딩중에 이 구조체가 생성되는데 이때는 에셋 매니저에서 게임플레이 태그를 생성하기 전이어서 Map이 제대로 초기화되지 않음.
         * 런타임에 DamageStatics() 함수 대신 AuraDamageStatics()를 이용하면 임시로 구조체를 생성해서 Map을 초기화 할 수 있긴 하지만,
         * 에셋 매니저에서 게임플레이 태그를 생성한 후에 Map을 초기화 하는 방식을 사용했음.
         * 이렇게 하면 엔진 로딩중에 한번만 초기화 됨.
        const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);

        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
        */
    }

    void InitializeTagsToCaptureDefMap()
    {
        const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
        
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);

        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
        TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
    }
};

static AuraDamageStatics& DamageStatics()
{
    static AuraDamageStatics DStatics;
    return DStatics;
}

void StaticInitTagsToCaptureDefMap()
{
    DamageStatics().InitializeTagsToCaptureDefMap();
}

UExecCalc_Damage::UExecCalc_Damage()
{
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
    RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

    RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
    int32 SourcePlayerLevel = 1;
    if (SourceAvatar->Implements<UCombatInterface>())
    {
        SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
    }
    int32 TargetPlayerLevel = 1;
    if (TargetAvatar->Implements<UCombatInterface>())
    {
        TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
    }

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    // Debuff
    DetermineDebuff(ExecutionParams, Spec, EvaluationParameters);

    // Get Damage Set by Caller Magnitude
    float Damage = 0.f;
    for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToResistances)
    {
        const FGameplayTag DamageTypeTag = Pair.Key;
        const FGameplayTag ResistanceTag = Pair.Value;
        
        checkf(DamageStatics().TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs dosen't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
        const FGameplayEffectAttributeCaptureDefinition CaptureDef = DamageStatics().TagsToCaptureDefs[ResistanceTag];

        float Resistance = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
        Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
        
        float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
        if (FMath::IsNearlyZero(DamageTypeValue) || DamageTypeValue <= 0.f)
        {
            continue;
        }

        if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
        {
            /**
             * 범위 데미지는 거리가 멀어짐에 따라 데미지가 감소함.
             * 거리에 따른 데미지 감소는 언리얼에서 계산해주는 함수가 있으니 그걸 이용함.
             * 그러기 위해서 아래의 과정을 거침.
             * 
             * 1. 액터의 TakeDamage 함수를 override해서 Super::TakeDamage를 통해 감소된 데미지 값을 받음.
             * 2. OnDamageSignature라는 델리게이트를 생성해서 TakeDamage 함수에서 감소된 데미지 값을 Broadcast 함.
             * 3. OnDamageSignature 델리게이트에 Lambda 함수 연결.
             * 4. 델리게이트에 함수를 연결한 뒤에 UGameplayStatics::ApplyRadialDamageWithFalloff 함수 호출.
             * 5. Lambda 함수에서 감소된 데미지 값을 받아서 기존의 데미지값을 변경.
             */
            if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
            {
                CombatInterface->GetOnDamageDelegate().AddLambda(
                    [&](float DamageAmount)
                    {
                        DamageTypeValue = DamageAmount;
                    }
                );
            }
            UGameplayStatics::ApplyRadialDamageWithFalloff(
                TargetAvatar,
                DamageTypeValue,
                1.f,
                UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
                UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
                UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
                1.f,
                UDamageType::StaticClass(),
                TArray<AActor*>(),
                SourceAvatar
            );
        }

        Damage += DamageTypeValue * (100.f - Resistance) / 100.f;
    }
    
    // Capture BlockChance on Target, and determine in there was a successful Block
    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
    // If Block, halve the damage.
    const bool bBlocked = FMath::FRandRange(0.f, 100.f) <= TargetBlockChance;
    UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
    if (!FMath::IsNearlyZero(TargetBlockChance) && bBlocked)
    {
        Damage = FMath::RoundHalfToZero(Damage / 2);
    }
    
    // ArmorPenetration ignores a percentage of the Target's Armor
    float TargetArmor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
    TargetArmor = FMath::Max<float>(TargetArmor, 0.f);
    
    float SourceArmorPenetration = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
    SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

    const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
    
    const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
    const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
    const float EffectiveArmor = TargetArmor * FMath::Clamp<float>((100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f, 0.f, 1.f);

    // Armor ignores a percentage of incoming damage
    const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
    const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);
    Damage *= FMath::Clamp<float>((100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f, 0.f, 1.f);

    // Critical Hit
    float SourceCriticalHitChance = 0.f;
    float SourceCriticalHitDamage = 0.f;
    float TargetCriticalHitResistance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
    SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
    SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);
    TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

    const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
    const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

    // Critical Hit Resistance reduces Critical Hit Chance by a certain percentage
    const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
    const bool bCriticalHit = FMath::FRandRange(0.f, 100.f) <= EffectiveCriticalHitChance;
    UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
    if (!FMath::IsNearlyZero(SourceCriticalHitChance) && bCriticalHit)
    {
        Damage = Damage * 2 + SourceCriticalHitDamage;
    }
    
    // Set Final Damage
    // 다양한 어트리뷰트에 접근해서 값을 조정할 수 있음.
    const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters) const
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

    for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageTypesToDebuffs)
    {
        const FGameplayTag& DamageType = Pair.Key;
        const FGameplayTag& DebuffType = Pair.Value;

        const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
        if (TypeDamage < 0.f)
        {
            continue;
        }

        // Determine if there was a successful debuff
        const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Param_Chance, false, -1.f);

        float TargetDebuffResistance = 0.f;
        const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageType];
        const FGameplayEffectAttributeCaptureDefinition CaptureDef = DamageStatics().TagsToCaptureDefs[ResistanceTag];
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, TargetDebuffResistance);
        TargetDebuffResistance = FMath::Max(TargetDebuffResistance, 0.f);
        const float EffectiveDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100.f;

        const bool bDebuff = FMath::FRandRange(0.f, 100.f) <= EffectiveDebuffChance;
        if (bDebuff)
        {
            UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(EffectContextHandle, true);
            UAuraAbilitySystemLibrary::SetDamageType(EffectContextHandle, DamageType);

            const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Param_Damage, false, -1.f);
            const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Param_Duration, false, -1.f);
            const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Param_Frequency, false, -1.f);

            UAuraAbilitySystemLibrary::SetDebuffDamage(EffectContextHandle, DebuffDamage);
            UAuraAbilitySystemLibrary::SetDebuffDuration(EffectContextHandle, DebuffDuration);
            UAuraAbilitySystemLibrary::SetDebuffFrequency(EffectContextHandle, DebuffFrequency);
        }
    }
}
