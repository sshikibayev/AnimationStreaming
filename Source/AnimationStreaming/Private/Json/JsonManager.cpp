// Fill out your copyright notice in the Description page of Project Settings.


#include "Json/JsonManager.h"

#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"


void UJsonManager::LoadStringFromFile(const FString& FilePath, bool& bOutSuccess, FString& OutInfoMessage, FString& OutFileContents)
{
    bOutSuccess = FFileHelper::LoadFileToString(OutFileContents, *FilePath);

    if (!bOutSuccess)
    {
        OutInfoMessage = FString::Printf(TEXT("Load Json failed: - '%s'"), *FilePath);
    }
}

FKeyframes UJsonManager::LoadJsonToStruct(const FString& FilePath, bool& bOutSuccess, FString& OutInfoMessage)
{
    // Load file from disk
    FString JsonString{};
    LoadStringFromFile(FilePath, bOutSuccess, OutInfoMessage, JsonString);
    if (!bOutSuccess)
    {
        return FKeyframes();
    }

    // Convert file content to generic json
    TSharedPtr<FJsonObject> ResultJson;
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), ResultJson))
    {
        bOutSuccess = false;
        OutInfoMessage = FString::Printf(TEXT("Read Json Failed - Was not able to deserialize the json string. Is it the right format? - '%s'"), *JsonString);
        return FKeyframes();
    }

    // Convert FJsonObject to desired FExampleStruct
    FKeyframes ResultKeyframes;
    bOutSuccess = FJsonObjectConverter::JsonObjectToUStruct<FKeyframes>(ResultJson.ToSharedRef(), &ResultKeyframes);
    if (!bOutSuccess)
    {
        OutInfoMessage = FString::Printf(TEXT("Read Struct Json Failed - Was not able to convert the json object to your desired structure. Is it the right format / struct? - '%s'"), *FilePath);
        return FKeyframes();
    }
    OutInfoMessage = FString::Printf(TEXT("Read Struct Json Succeeded - '%s'"), *FilePath);

    return ResultKeyframes;
}

TArray<FKeyframes> UJsonManager::LoadJsonArrayToStruct(const FString& FilePath, bool& bOutSuccess, FString& OutInfoMessage)
{
    // Load file from disk
    FString JsonString{};
    LoadStringFromFile(FilePath, bOutSuccess, OutInfoMessage, JsonString);
    if (!bOutSuccess)
    {
        return TArray<FKeyframes>();
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    //Check the JsonObject for validation after
    //Deserialize, because it's outer object.
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON"));
        return TArray<FKeyframes>();
    }

    // Access the "global_ctrl" object directly
    const TSharedPtr<FJsonObject>& GlobalCtrlObject = JsonObject->GetObjectField("global_ctrl");
    // Iterate over each key-value pair in the "global_ctrl" object
    if (!GlobalCtrlObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find 'global_ctrl' object in JSON"));
        return TArray<FKeyframes>();
    }

    //If all validation is success
    //proceed further with parsing data.
    TArray<FKeyframes> Keyframes{};
    for (auto& Pair : GlobalCtrlObject->Values)
    {
        const FString& Key = Pair.Key;
        const TSharedPtr<FJsonObject>& SubObject = Pair.Value->AsObject();
        int Frame = FCString::Atoi(*Pair.Key);

        // Get parsed Transform
        FTransform Coordinates
        {
            GetParsedRotation(SubObject).Quaternion(),
            GetParsedLocation(SubObject),
            GetParsedScale(SubObject)
        };

        FKeyframes FinalKeyframe{ GetInitializedKeyFrameStruct(Coordinates, Frame) };
        Keyframes.Emplace(FinalKeyframe);
    }
    PrintKeyframeData(Keyframes);

    return Keyframes;
}

void UJsonManager::PrintKeyframeData(const TArray<FKeyframes>& Keyframes)
{
    for (const auto& Keyframe : Keyframes)
    {
        UE_LOG(LogTemp, Warning, TEXT("Frame: (%i)"), Keyframe.Frame);

        UE_LOG(LogTemp, Warning, TEXT("Rotation: (%f, %f, %f)"), Keyframe.Coordinates.GetRotation().X, Keyframe.Coordinates.GetRotation().Y, Keyframe.Coordinates.GetRotation().Z);

        UE_LOG(LogTemp, Warning, TEXT("Location: (%f, %f, %f)"), Keyframe.Coordinates.GetLocation().X, Keyframe.Coordinates.GetLocation().Y, Keyframe.Coordinates.GetLocation().Z);

        UE_LOG(LogTemp, Warning, TEXT("Scale: (%f, %f, %f)"), Keyframe.Coordinates.GetScale3D().X, Keyframe.Coordinates.GetScale3D().Y, Keyframe.Coordinates.GetScale3D().Z);
    }
}

float UJsonManager::SafeConvertToFloat(const double ValueToConvert)
{
    const double MinValidValue = -DBL_MAX; // Minimum valid value
    const double MaxValidValue = DBL_MAX; // Maximum valid value

    if (ValueToConvert >= MinValidValue && ValueToConvert <= MaxValidValue)
    {
        return static_cast<float>(ValueToConvert); // Convert safely to float
    }
    else
    {
        // Handle invalid value or out of range error
        UE_LOG(LogTemp, Error, TEXT("Invalid value encountered during conversion to float."));
        return 0.0f; // or any default value you prefer
    }
}

int UJsonManager::GetLastFrame(const TMap<FString, TSharedPtr<FJsonValue>>& JsonData)
{
    int LastFrame;
    for (auto& Pair : JsonData)
    {
        LastFrame = FCString::Atoi(*Pair.Key);
    }

    return LastFrame;
}

FRotator UJsonManager::GetParsedRotation(const TSharedPtr<FJsonObject>& SubObject)
{
    const TSharedPtr<FJsonObject>& RotationJson = SubObject->GetObjectField("rotation");
    const FRotator Rotation
    {
        SafeConvertToFloat(RotationJson->GetNumberField("y")),
        SafeConvertToFloat(RotationJson->GetNumberField("z")),
        SafeConvertToFloat(RotationJson->GetNumberField("x"))
    };

    return Rotation;
}

FVector UJsonManager::GetParsedLocation(const TSharedPtr<FJsonObject>& SubObject)
{
    const TSharedPtr<FJsonObject>& TranslationJson = SubObject->GetObjectField("translation");
    const FVector Location
    {
        SafeConvertToFloat(TranslationJson->GetNumberField("x")),
        SafeConvertToFloat(TranslationJson->GetNumberField("y")),
        SafeConvertToFloat(TranslationJson->GetNumberField("z"))
    };

    return Location;
}

FVector UJsonManager::GetParsedScale(const TSharedPtr<FJsonObject>& SubObject)
{
    const TSharedPtr<FJsonObject>& ScaleJson = SubObject->GetObjectField("scale");
    const FVector Scale
    {
        SafeConvertToFloat(ScaleJson->GetNumberField("x")),
        SafeConvertToFloat(ScaleJson->GetNumberField("y")),
        SafeConvertToFloat(ScaleJson->GetNumberField("z"))
    };

    UE_LOG(LogTemp, Warning, TEXT("Scale: %s"), *Scale.ToString());

    return Scale;
}

FKeyframes UJsonManager::GetInitializedKeyFrameStruct(const FTransform& Coordinates, const int Frame)
{
    FKeyframes Keyframe;
    Keyframe.Frame = Frame;
    Keyframe.Coordinates = Coordinates;

    return Keyframe;
}
