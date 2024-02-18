// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Struct/Keyframes.h"

#include "JsonManager.generated.h"


UCLASS()
class ANIMATIONSTREAMING_API UJsonManager : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    static void LoadStringFromFile(const FString& FilePath, bool& bOutSuccess, FString& OutInfoMessage, FString& OutFileContents);

    UFUNCTION(BlueprintCallable, Category = Json)
    static FKeyframes LoadJsonToStruct(const FString& FilePath, bool& bOutSuccess, FString& OutInfoMessage);

    UFUNCTION(BlueprintCallable, Category = Json)
    static TArray<FKeyframes> LoadJsonArrayToStruct(const FString& FilePath, bool& bOutSuccess, FString& OutInfoMessage);

    UFUNCTION()
    static void PrintKeyframeData(const TArray<FKeyframes>& Keyframes);

private:
    static float SafeConvertToFloat(const double ValueToConvert);
    static int GetLastFrame(const TMap<FString, TSharedPtr<FJsonValue>>& JsonData);
    static FRotator GetParsedRotation(const TSharedPtr<FJsonObject>& SubObject);
    static FVector GetParsedLocation(const TSharedPtr<FJsonObject>& SubObject);
    static FVector GetParsedScale(const TSharedPtr<FJsonObject>& SubObject);
    static FKeyframes GetInitializedKeyFrameStruct(const FTransform& Coordinates, const int Frame);
};
