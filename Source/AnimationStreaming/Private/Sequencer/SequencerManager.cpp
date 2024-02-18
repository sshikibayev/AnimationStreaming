// Fill out your copyright notice in the Description page of Project Settings.


#include "Sequencer/SequencerManager.h"

#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "MovieScene.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieSceneSkeletalAnimationTrack.h"

#include "Sections/MovieScene3DTransformSection.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneDoubleChannel.h"

#include "GameFramework/Character.h"


ULevelSequence* USequencerManager::GetLevelSequencer(const FString& Path, bool& bOutSuccess)
{
    ULevelSequence* LevelSequence = Cast<ULevelSequence>(StaticLoadObject(ULevelSequence::StaticClass(), nullptr, *Path));
    if (!IsValid(LevelSequence))
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("GetLevelSequencer is failed: Level sequence is not valid"));

        return nullptr;
    }

    return LevelSequence;
}

FGuid USequencerManager::GetActorGuidFromLevelSequence(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess)
{
    if (!IsValid(Actor))
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("GetActorGuidFromLevelSequence is failed: Actor is not valid"));

        return FGuid();
    }

    ULevelSequence* LevelSequence{ GetLevelSequencer(SequencerPath, bOutSuccess) };
    if (!IsValid(LevelSequence))
    {
        bOutSuccess = false;

        return FGuid();
    }

    FGuid Result{ LevelSequence->FindBindingFromObject(Actor, Actor->GetWorld()) };

    if (!Result.IsValid())
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("GetActorGuidFromLevelSequence is failed: Guid is not valid"));

        return FGuid();
    }

    return Result;
}

FGuid USequencerManager::AddActorToLevelSequence(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess)
{
    if (IsActorInSequence(Actor, SequencerPath, bOutSuccess))
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("AddActorToLevelSequence is failed: ActorID is already exist"));

        return FGuid();
    }

    if (!IsValid(Actor))
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("AddActorToLevelSequence is failed: Actor is not valid"));

        return FGuid();
    }

    ULevelSequence* LevelSequence{ GetLevelSequencer(SequencerPath, bOutSuccess) };
    if (!IsValid(LevelSequence))
    {
        bOutSuccess = false;

        return FGuid();
    }

    FGuid ActorID{ Cast<UMovieSceneSequence>(LevelSequence)->CreatePossessable(Actor) };

    bOutSuccess = ActorID.IsValid();
    if (!bOutSuccess)
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("AddActorToLevelSequence is failed: ActorID is not valid"));

        return FGuid();
    }

    return ActorID;
}

UMovieScene3DTransformTrack* USequencerManager::GetTransformTrackFromActor(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess)
{
    FGuid ActorID{ GetActorGuidFromLevelSequence(Actor, SequencerPath, bOutSuccess) };

    if (!ActorID.IsValid())
    {
        bOutSuccess = false;

        return nullptr;
    }

    ULevelSequence* LevelSequence{ GetLevelSequencer(SequencerPath, bOutSuccess) };
    if (!IsValid(LevelSequence))
    {
        bOutSuccess = false;

        return nullptr;
    }

    UMovieScene3DTransformTrack* TransformTrack{ LevelSequence->MovieScene->FindTrack<UMovieScene3DTransformTrack>(ActorID) };

    if (!IsValid(TransformTrack))
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("GetTransformTrackFromActor is failed: TransformTrack is not valid"));

        return nullptr;
    }

    return TransformTrack;
}

UMovieScene3DTransformTrack* USequencerManager::AddTransformTrackToActor(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess)
{
    if (IsTransformTrackInSequence(Actor, SequencerPath, bOutSuccess))
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("AddTransformTrackToActor is failed: TransformTrack is already added"));

        return nullptr;
    }

    FGuid ActorID{ GetActorGuidFromLevelSequence(Actor, SequencerPath, bOutSuccess) };

    if (!ActorID.IsValid())
    {
        bOutSuccess = false;

        return nullptr;
    }

    ULevelSequence* LevelSequence{ GetLevelSequencer(SequencerPath, bOutSuccess) };
    if (!IsValid(LevelSequence))
    {
        bOutSuccess = false;

        return nullptr;
    }

    UMovieScene3DTransformTrack* TransformTrack = LevelSequence->MovieScene->AddTrack<UMovieScene3DTransformTrack>(ActorID);
    bOutSuccess = true;

    return TransformTrack;
}

UMovieScene3DTransformSection* USequencerManager::GetTransformSectionFromActor(AActor* Actor, const FString& SequencerPath, const int SectionIndex, bool& bOutSuccess)
{
    UMovieScene3DTransformTrack* TransformTrack{ GetTransformTrackFromActor(Actor, SequencerPath, bOutSuccess) };

    if (!IsValid(TransformTrack))
    {
        bOutSuccess = false;

        return nullptr;
    }

    TArray<UMovieSceneSection*> AllSections = TransformTrack->GetAllSections();

    if (SectionIndex < 0 || SectionIndex >= AllSections.Num())
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("GetTransformSectionFromActor is failed: Section index is out of range"));

        return nullptr;
    }

    bOutSuccess = true;

    return Cast<UMovieScene3DTransformSection>(AllSections[SectionIndex]);
}

UMovieScene3DTransformSection* USequencerManager::AddTransformSectionToActor(AActor* Actor, const FString& SequencerPath, const int StartFrame, const int EndFrame, EMovieSceneBlendType BlendType, bool& bOutSuccess)
{
    UMovieScene3DTransformTrack* TransformTrack{ GetTransformTrackFromActor(Actor, SequencerPath, bOutSuccess) };

    if (!IsValid(TransformTrack))
    {
        bOutSuccess = false;

        return nullptr;
    }

    UMovieScene3DTransformSection* TransformSection{ Cast<UMovieScene3DTransformSection>(TransformTrack->CreateNewSection()) };

    if (!IsValid(TransformSection))
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("AddTransformSectionToActor is failed: TransformSection is not valid"));

        return nullptr;
    }

    ULevelSequence* LevelSequence{ GetLevelSequencer(SequencerPath, bOutSuccess) };
    if (!IsValid(LevelSequence))
    {
        bOutSuccess = false;

        return nullptr;
    }
    const int TickPerFrame = LevelSequence->MovieScene->GetTickResolution().AsDecimal() / LevelSequence->MovieScene->GetDisplayRate().AsDecimal();

    TransformSection->SetRange(TRange<FFrameNumber>(FFrameNumber(StartFrame * TickPerFrame), FFrameNumber(EndFrame * TickPerFrame)));

    TransformSection->SetBlendType(BlendType);
    int RowIndex{ -1 };
    for (UMovieSceneSection* ExistingSection : TransformTrack->GetAllSections())
    {
        RowIndex = FMath::Max(RowIndex, ExistingSection->GetRowIndex());
    }
    TransformSection->SetRowIndex(RowIndex + 1);

    TransformTrack->AddSection(*TransformSection);

    bOutSuccess = true;

    return TransformSection;
}

void USequencerManager::AddTransformKeyframe(AActor* Actor, const FString& SequencerPath, const int SectionIndex, const int Frame, const FTransform& Transform, int KeyInterpolation, bool& bOutSuccess)
{
    UMovieScene3DTransformSection* Section{ GetTransformSectionFromActor(Actor, SequencerPath, SectionIndex, bOutSuccess) };

    if (!IsValid(Section))
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("AddTransformKeyframe is failed: Section is not valid"));
        return;
    }

    //Location
    AddKeyframeToDoubleChannel(Section, 0, Frame, Transform.GetLocation().X, KeyInterpolation, bOutSuccess);
    AddKeyframeToDoubleChannel(Section, 1, Frame, Transform.GetLocation().Y, KeyInterpolation, bOutSuccess);
    AddKeyframeToDoubleChannel(Section, 2, Frame, Transform.GetLocation().Z, KeyInterpolation, bOutSuccess);

    //Rotation
    AddKeyframeToDoubleChannel(Section, 3, Frame, Transform.Rotator().Roll, KeyInterpolation, bOutSuccess);
    AddKeyframeToDoubleChannel(Section, 4, Frame, Transform.Rotator().Pitch, KeyInterpolation, bOutSuccess);
    AddKeyframeToDoubleChannel(Section, 5, Frame, Transform.Rotator().Yaw, KeyInterpolation, bOutSuccess);

    //Scale
    AddKeyframeToDoubleChannel(Section, 6, Frame, Transform.GetScale3D().X, KeyInterpolation, bOutSuccess);
    AddKeyframeToDoubleChannel(Section, 7, Frame, Transform.GetScale3D().Y, KeyInterpolation, bOutSuccess);
    AddKeyframeToDoubleChannel(Section, 8, Frame, Transform.GetScale3D().Z, KeyInterpolation, bOutSuccess);

    bOutSuccess = true;
}

void USequencerManager::AddKeyframeToDoubleChannel(UMovieSceneSection* Section, const int ChannelIndex, const int Frame, double Value, int KeyInterpolation, bool& bOutSuccess)
{
    if (!IsValid(Section))
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("AddKeyframeToDoubleChannel is failed: Section is not valid"));

        return;
    }

    FMovieSceneDoubleChannel* Channel{ Section->GetChannelProxy().GetChannel<FMovieSceneDoubleChannel>(ChannelIndex) };

    if (Channel == nullptr)
    {
        bOutSuccess = false;
        UE_LOG(LogTemp, Error, TEXT("AddKeyframeToDoubleChannel is failed: Channel is not valid"));

        return;
    }

    ULevelSequence* LevelSequence{ Cast<ULevelSequence>(Section->GetOutermostObject()) };
    if (!IsValid(LevelSequence))
    {
        bOutSuccess = false;

        return;
    }

    const int TickPerFrame = LevelSequence->MovieScene->GetTickResolution().AsDecimal() / LevelSequence->MovieScene->GetDisplayRate().AsDecimal();

    FFrameNumber FrameNumber{ FFrameNumber(Frame * TickPerFrame) };

    if (KeyInterpolation == 0)
    {
        Channel->AddCubicKey(FrameNumber, Value);
    }
    else if (KeyInterpolation == 1)
    {
        Channel->AddLinearKey(FrameNumber, Value);
    }
    else
    {
        Channel->AddConstantKey(FrameNumber, Value);
    }

    Section->Modify();

    bOutSuccess = true;
}

bool USequencerManager::IsActorInSequence(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess)
{
    ULevelSequence* LevelSequence{ GetLevelSequencer(SequencerPath, bOutSuccess) };
    if (!IsValid(LevelSequence))
    {
        bOutSuccess = false;

        return false;
    }

    if (!IsValid(Actor))
    {
        bOutSuccess = false;

        return false;
    }

    FGuid Result{ LevelSequence->FindBindingFromObject(Actor, Actor->GetWorld()) };

    return Result.IsValid();
}

bool USequencerManager::IsTransformTrackInSequence(AActor* Actor, const FString& SequencerPath, bool& bOutSuccess)
{
    FGuid ActorID{ GetActorGuidFromLevelSequence(Actor, SequencerPath, bOutSuccess) };

    if (!ActorID.IsValid())
    {
        bOutSuccess = false;

        return false;
    }

    ULevelSequence* LevelSequence{ GetLevelSequencer(SequencerPath, bOutSuccess) };
    if (!IsValid(LevelSequence))
    {
        bOutSuccess = false;

        return false;
    }

    UMovieScene3DTransformTrack* TransformTrack{ LevelSequence->MovieScene->FindTrack<UMovieScene3DTransformTrack>(ActorID) };
    bOutSuccess = true;

    return IsValid(TransformTrack);
}
