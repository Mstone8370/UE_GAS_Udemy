// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */
struct FAuraGameplayTags
{
public:
    static const FAuraGameplayTags& Get() { return GameplayTags; }
    static void InitializeNativeGameplayTags();

    // Primary Attributes
    FGameplayTag Attributes_Primary_Strength;
    FGameplayTag Attributes_Primary_Intelligence;
    FGameplayTag Attributes_Primary_Resilience;
    FGameplayTag Attributes_Primary_Vigor;

    // Secondary Attributes
    FGameplayTag Attributes_Secondary_Armor;
    FGameplayTag Attributes_Secondary_ArmorPenetration;
    FGameplayTag Attributes_Secondary_BlockChance;
    FGameplayTag Attributes_Secondary_CriticalHitChance;
    FGameplayTag Attributes_Secondary_CriticalHitDamage;
    FGameplayTag Attributes_Secondary_CriticalHitResistance;
    FGameplayTag Attributes_Secondary_HealthRegeneration;
    FGameplayTag Attributes_Secondary_ManaRegeneration;
    FGameplayTag Attributes_Secondary_MaxHealth;
    FGameplayTag Attributes_Secondary_MaxMana;

    // Resistance
    FGameplayTag Attributes_Resistance_Fire;
    FGameplayTag Attributes_Resistance_Lightning;
    FGameplayTag Attributes_Resistance_Arcane;
    FGameplayTag Attributes_Resistance_Physical;

    // Meta Attributes
    FGameplayTag Attributes_Meta_IncomingXP;

    // Input Tags
    FGameplayTag InputTag_LMB;
    FGameplayTag InputTag_RMB;
    FGameplayTag InputTag_1;
    FGameplayTag InputTag_2;
    FGameplayTag InputTag_3;
    FGameplayTag InputTag_4;
    FGameplayTag InputTag_Passive_1;
    FGameplayTag InputTag_Passive_2;

    // Damage Types
    FGameplayTag Damage;
    FGameplayTag Damage_Fire;
    FGameplayTag Damage_Lightning;
    FGameplayTag Damage_Arcane;
    FGameplayTag Damage_Physical;

    TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;

    // Debuff
    FGameplayTag Debuff_Burn;
    FGameplayTag Debuff_Stun;
    FGameplayTag Debuff_Arcane;
    FGameplayTag Debuff_Physical;

    TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;

    FGameplayTag Debuff_Param_Chance;
    FGameplayTag Debuff_Param_Damage;
    FGameplayTag Debuff_Param_Frequency;
    FGameplayTag Debuff_Param_Duration;

    // Effects
    FGameplayTag Effects_HitReact;

    // Abilities
    FGameplayTag Abilities_Attack;
    FGameplayTag Abilities_Summon;
    FGameplayTag Abilities_Fire_FireBolt;
    FGameplayTag Abilities_Fire_FireBlast;
    FGameplayTag Abilities_Lightning_Electrocute;
    FGameplayTag Abilities_Arcane_ArcaneShards;

    FGameplayTag Abilities_Passive_HaloOfProtection;
    FGameplayTag Abilities_Passive_LifeSiphon;
    FGameplayTag Abilities_Passive_ManaSiphon;

    FGameplayTag Abilities_HitReact;

    FGameplayTag Abilities_Status_Locked;
    FGameplayTag Abilities_Status_Eligible;
    FGameplayTag Abilities_Status_Unlocked;
    FGameplayTag Abilities_Status_Equipped;

    FGameplayTag Abilities_Type_Offensive;
    FGameplayTag Abilities_Type_Passive;
    FGameplayTag Abilities_Type_None;

    // Cooldowns
    FGameplayTag Cooldown_Fire_FireBolt;

    // Combat Sockets
    FGameplayTag CombatSocket_Weapon;
    FGameplayTag CombatSocket_RightHand;
    FGameplayTag CombatSocket_LeftHand;
    FGameplayTag CombatSocket_Tail;

    // Montage Tags
    FGameplayTag Montage_Attack_1;
    FGameplayTag Montage_Attack_2;
    FGameplayTag Montage_Attack_3;
    FGameplayTag Montage_Attack_4;

    // Block Tags
    FGameplayTag Player_Block_InputPressed;
    FGameplayTag Player_Block_InputHeld;
    FGameplayTag Player_Block_InputReleased;
    FGameplayTag Player_Block_CursorTrace;

private:
    static FAuraGameplayTags GameplayTags;
};