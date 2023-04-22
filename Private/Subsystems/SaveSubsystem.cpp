// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/SaveSubsystem.h"

#include "FunctionLibrary/DebugFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	LoadData();
}

void USaveSubsystem::StartNewSave()
{
	if(UGameplayStatics::DoesSaveGameExist(PlayerSaveSlot, 0)) UGameplayStatics::DeleteGameInSlot(PlayerSaveSlot, 0);

	LoadData();
}


void USaveSubsystem::OnAsyncLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame)
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Async Loading Finished", EDebugType::DT_Log);
	if(SaveGame)
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Save Game Pointer is Valid", EDebugType::DT_Log);
		OnPlayerDataLoaded.Broadcast(SaveGame);
	}
	
}

void USaveSubsystem::OnAsyncSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Async Saving Finished", EDebugType::DT_Log);
	if(bSuccess)
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Save was Successful", EDebugType::DT_Log);
	}
}

void USaveSubsystem::SetSaveGameClass(TSubclassOf<USaveGame> SaveGameSubClass)
{
	SaveGameClass = SaveGameSubClass;
}

void USaveSubsystem::SaveData()
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Saving Player Data", EDebugType::DT_Log);
	// If it isn't Valid, Create a new instance
	if(!PlayerSaveObject)
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Player Save is NOT Valid. Creating New Instance", EDebugType::DT_Log);
		PlayerSaveObject = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
	}
	FAsyncSaveGameToSlotDelegate asyncSaveDelegate;
	asyncSaveDelegate.BindUObject(this, &USaveSubsystem::OnAsyncSaveFinished);
	UGameplayStatics::AsyncSaveGameToSlot(PlayerSaveObject, PlayerSaveSlot, 0, asyncSaveDelegate);
}

void USaveSubsystem::LoadData()
{
	UDebugFunctionLibrary::DebugLogWithObject(this, "Attempting to Load Data", EDebugType::DT_Log);

	// If a save game exists in a slot, then load it
	if(UGameplayStatics::DoesSaveGameExist(PlayerSaveSlot, 0))
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "Player Save Data Exists. Async Loading", EDebugType::DT_Log);
		
		FAsyncLoadGameFromSlotDelegate asyncLoadDelegate;
		asyncLoadDelegate.BindUObject(this, &USaveSubsystem::OnAsyncLoadFinished);
		UGameplayStatics::AsyncLoadGameFromSlot(PlayerSaveSlot, 0, asyncLoadDelegate);
	}

	// Otherwise, create one
	else
	{
		UDebugFunctionLibrary::DebugLogWithObject(this, "No Player Save Data Exists. Creating New One", EDebugType::DT_Log);
		
		OnAsyncLoadFinished(PlayerSaveSlot, 0, UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	}
}
