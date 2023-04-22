// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/LevelSaveSubsystem.h"

#include "FunctionLibrary/DebugFunctionLibrary.h"
#include "Interfaces/LevelSaveInterface.h"
#include "Kismet/GameplayStatics.h"

ULevelSaveSubsystem::ULevelSaveSubsystem()
{
}

void ULevelSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LevelSaveSlot = GetWorld()->GetFName().ToString();

	UDebugFunctionLibrary::DebugLogWithObject(this, "Save Slot: " +LevelSaveSlot, EDebugType::DT_Log);

	GetWorld()->OnWorldBeginPlay.AddUObject(this, &ULevelSaveSubsystem::LoadData);
}

void ULevelSaveSubsystem::UpdateActors(AActor* SavedActor, bool bInteracted)
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Updating Save Data", EDebugType::DT_Log);
	if(LevelSaveObject && IsValid(SavedActor))
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Save Object and Actor are Valid", EDebugType::DT_Log);
		LevelSaveObject->InteractedWithActors.Add(SavedActor, bInteracted);
	}
}

void ULevelSaveSubsystem::UpdateMovedActors(TObjectPtr<AActor> SavedActor, FTransform Transform)
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Updating Save Data (Moved Actor)", EDebugType::DT_Log);
	if(LevelSaveObject && SavedActor)
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Save Object and Actor are Valid", EDebugType::DT_Log);
		LevelSaveObject->MovedActors.Add(SavedActor, Transform);
	}
}

void ULevelSaveSubsystem::OnAsyncLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame)
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Level Async Loading Finished", EDebugType::DT_Log);
	if(SaveGame)
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Level Save Game Pointer is Valid", EDebugType::DT_Log);
		LevelSaveObject = Cast<ULevelSaveObject>(SaveGame);

		// Use the Save Data to Affect which Actors have been interacted with
		for(auto SavedActor : LevelSaveObject->InteractedWithActors)
		{
			UDebugFunctionLibrary::DebugLogWithObject(this, "Attempting to Update Actor", EDebugType::DT_Log);
			if(IsValid(SavedActor.Key) && SavedActor.Key->Implements<ULevelSaveInterface>())
			{
				ILevelSaveInterface::Execute_UpdateActor(SavedActor.Key, SavedActor.Value);
			}
		}
	}
}

void ULevelSaveSubsystem::OnAsyncSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Level Async Saving Finished", EDebugType::DT_Log);
	if(bSuccess)
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Level Save was Successful", EDebugType::DT_Log);
	}
}

void ULevelSaveSubsystem::SaveData()
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Saving Level Data", EDebugType::DT_Log);
	// If it isn't Valid, Create a new instance
	if(!IsValid(LevelSaveObject))
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Player Save is NOT Valid. Creating New Instance", EDebugType::DT_Log);
		LevelSaveObject = Cast<ULevelSaveObject>(UGameplayStatics::CreateSaveGameObject(ULevelSaveObject::StaticClass()));
	}
	FAsyncSaveGameToSlotDelegate asyncSaveDelegate;
	asyncSaveDelegate.BindUObject(this, &ULevelSaveSubsystem::OnAsyncSaveFinished);
	UGameplayStatics::AsyncSaveGameToSlot(LevelSaveObject, LevelSaveSlot, 0, asyncSaveDelegate);

}

void ULevelSaveSubsystem::LoadData()
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Attempting to Load Level Data", EDebugType::DT_Log);

	// If a save game exists in a slot, then load it
	if(UGameplayStatics::DoesSaveGameExist(LevelSaveSlot, 0))
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Level Save Data Exists. Async Loading", EDebugType::DT_Log);
		
		FAsyncLoadGameFromSlotDelegate asyncLoadDelegate;
		asyncLoadDelegate.BindUObject(this, &ULevelSaveSubsystem::OnAsyncLoadFinished);
		UGameplayStatics::AsyncLoadGameFromSlot(LevelSaveSlot, 0, asyncLoadDelegate);
	}

	// Otherwise, create one
	else
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "No Player Save Data Exists. Creating New One", EDebugType::DT_Log);
		OnAsyncLoadFinished(LevelSaveSlot, 0, UGameplayStatics::CreateSaveGameObject(ULevelSaveObject::StaticClass()));
	}
}
