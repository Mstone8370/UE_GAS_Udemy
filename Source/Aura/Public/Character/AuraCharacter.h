// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	//~ Begin Combat Interface
	virtual int32 GetPlayerLevel_Implementation() override;
	//~ End Combat Interface

	//~ Begine Player Interface
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const;
	virtual int32 GetXP_Implementation() const;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const;
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttribuetPorints) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual void LevelUp_Implementation() override;
	//~ End Player Interface

private:
	virtual void InitAbilityActorInfo() override;
};
