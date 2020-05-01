#pragma once

#include "GripState.generated.h"

UENUM(BlueprintType)
enum class EGripState : uint8
{
    EGS_Open UMETA(DisplayName = "Open"),
    EGS_CanGrab UMETA(DisplayName = "CanGrab"),
    EGS_Grab UMETA(DisplayName = "Grab"),

    EGS_Max UMETA(Hidden)
};

