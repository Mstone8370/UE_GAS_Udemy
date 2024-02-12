// Copyright 


#include "AbilitySystem/Data/CharacterClassInfo.h"

const FCharacterClassDefaultInfo& UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass) const
{
    return CharacterClassInformation.FindChecked(CharacterClass);
}
