// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Evaluation/Blending/MovieSceneBlendType.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SequencerManager.generated.h"

class ULevelSequence;
class UMovieScene3DTransformTrack;
class UMovieSceneSkeletalAnimationTrack;
class UMovieScene3DTransformSection;


UCLASS()
class ANIMATIONSTREAMING_API USequencerManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    //----------------------ACTOR----------------------------
    static bool IsActorInSequence(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess);
    static bool IsTransformTrackInSequence(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess);

    static ULevelSequence* GetLevelSequencer(const FString& Path, bool& bOutSuccess);

    UFUNCTION(BlueprintCallable, Category = Sequencer)
    static FGuid GetActorGuidFromLevelSequence(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess);

    UFUNCTION(BlueprintCallable, Category = Sequencer)
    static FGuid AddActorToLevelSequence(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess);

    //-------------------TRACK-------------------------
    UFUNCTION(BlueprintCallable, Category = Sequencer)
    static UMovieScene3DTransformTrack* GetTransformTrackFromActor(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess);

    UFUNCTION(BlueprintCallable, Category = Sequencer)
    static UMovieScene3DTransformTrack* AddTransformTrackToActor(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess);

    //------------------------SECTION--------------------
    UFUNCTION(BlueprintCallable, Category = Sequencer)
    static UMovieScene3DTransformSection* GetTransformSectionFromActor(AActor* Actor, const FString& SequencerPath, const int SectionIndex, bool& bOutSuccess);

    UFUNCTION(BlueprintCallable, Category = Sequencer)
    static UMovieScene3DTransformSection* AddTransformSectionToActor(AActor* Actor, const FString& SequencerPath, const int StartFrame, const int EndFrame, EMovieSceneBlendType BlendType, bool& bOutSuccess);

    //---------------Keyframe--------------------
    UFUNCTION(BlueprintCallable, Category = Sequencer)
    static void AddTransformKeyframe(AActor* Actor, const FString& SequencerPath, const int SectionIndex, const int Frame, const FTransform& Transform, int KeyInterpolation, bool& bOutSuccess);

    UFUNCTION(BlueprintCallable, Category = Sequencer)
    static void AddKeyframeToDoubleChannel(UMovieSceneSection* Section, const int ChannelIndex, const int Frame, double Value, int KeyInterpolation, bool& bOutSuccess);

};
