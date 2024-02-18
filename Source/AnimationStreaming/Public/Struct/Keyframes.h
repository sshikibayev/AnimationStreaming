// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Keyframes.generated.h"


USTRUCT(BlueprintType, Category = KeyFrames)
struct ANIMATIONSTREAMING_API FKeyframes : public FTableRowBase
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Frame{0};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform Coordinates;
};
