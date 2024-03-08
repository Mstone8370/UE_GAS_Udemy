// Copyright 


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "AuraGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AAuraCharacterBase::AAuraCharacterBase()
    : bDead(false)
    , CharacterClass(ECharacterClass::Warrior)
    , MinionCount(0)
{
    PrimaryActorTick.bCanEverTick = false;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetCapsuleComponent()->SetGenerateOverlapEvents(false);
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
    GetMesh()->SetGenerateOverlapEvents(true);

    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
    Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
    Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
    BurnDebuffComponent->SetupAttachment(GetRootComponent());
    BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
}

void AAuraCharacterBase::BeginPlay()
{
    Super::BeginPlay();
    
    OnASCRegistered.AddUObject(this, &AAuraCharacterBase::OnAbilitySystemComponentRegistered);
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
    if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
    {
        return Weapon->GetSocketLocation(WeaponTipSocketName);
    }
    if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
    {
        return GetMesh()->GetSocketLocation(RightHandSocketName);
    }
    if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
    {
        return GetMesh()->GetSocketLocation(LeftHandSocketName);
    }
    if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
    {
        return GetMesh()->GetSocketLocation(TailSocketName);
    }

    return FVector::ZeroVector;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
    return HitReactMontage;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
    Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
    MulticastHandleDeath(DeathImpulse);
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
    return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
    return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontage_Implementation()
{
    return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
    return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag MontageTag)
{
    for (const FTaggedMontage& TaggedMontage : AttackMontages)
    {
        if (TaggedMontage.MontageTag == MontageTag)
        {
            return TaggedMontage;
        }
    }
    return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
    return MinionCount;
}

void AAuraCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
    MinionCount += Amount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
    return CharacterClass;
}

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate()
{
    return OnASCRegistered;
}

FOnDeath& AAuraCharacterBase::GetOnDeathDelegate()
{
    return OnDeath;
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
    return Weapon;
}

void AAuraCharacterBase::OnAbilitySystemComponentRegistered(UAbilitySystemComponent* ASC)
{
    ASC->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(
        this, &AAuraCharacterBase::StunTagChanged
    );
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    bIsStunned = NewCount > 0;
    GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
    UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

    Weapon->SetSimulatePhysics(true);
    Weapon->SetEnableGravity(true);
    Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetEnableGravity(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    GetCapsuleComponent()->SetEnableGravity(false);

    Dissolve();

    bDead = true;
    
    OnDeath.Broadcast(this);
}

void AAuraCharacterBase::OnRep_IsStunned(bool bOldIsStunned) {}

void AAuraCharacterBase::InitAbilityActorInfo() {}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
    check(GetAbilitySystemComponent());
    check(GameplayEffectClass);
    
    FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    ContextHandle.AddSourceObject(this);
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
        GameplayEffectClass, Level, ContextHandle);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
    ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
    ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
    ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
    // 서버에서 승인 받아야 함
    if (!HasAuthority())
    {
        return;
    }

    UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
    AuraASC->AddCharacterAbilities(StartupAbilities);
    AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::Dissolve()
{
    if (IsValid(DissolveMaterialInstance))
    {
        UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
        GetMesh()->SetMaterial(0, DynamicMatInst);
        StartDissolveTimeline(DynamicMatInst);
    }
    if (IsValid(WeaponDissolveMaterialInstance))
    {
        UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
        Weapon->SetMaterial(0, DynamicMatInst);
        StartWeaponDissolveTimeline(DynamicMatInst);
    }
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
