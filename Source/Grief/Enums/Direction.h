#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EDirection : uint8
{
	None	= 0b00000000,
	Up		= 0b00000001,
	Down	= 0b00000010,
	Left	= 0b00000100,
	Right	= 0b00001000,

	UpLeft		= Up | Left,
	UpRight		= Up | Right,
	DownLeft	= Down | Left,
	DownRight	= Down | Right
};
ENUM_CLASS_FLAGS(EDirection)