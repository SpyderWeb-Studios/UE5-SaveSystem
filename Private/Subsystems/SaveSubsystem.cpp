// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/SaveSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USaveSubsystem::StartNewSave()
{
	if(UGameplayStatics::DoesSaveGameExist(GetPlayerSaveSlot(), 0))
	{
		UGameplayStatics::DeleteGameInSlot(GetPlayerSaveSlot(), 0);
	}
	LoadData();
}


void USaveSubsystem::OnAsyncLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame)
{
	UE_LOG(LogSaveSystem, Display, TEXT("Async Loading Finished"));
	if(IsValid(SaveGame))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Save Game Pointer is Valid"));
		PlayerSaveObject = SaveGame;
		OnPlayerDataLoaded.Broadcast(SaveGame);
	}

#ifndef UE_BUILD_SHIPPING
	else
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Save Game Pointer is Invalid"));
	}
#endif
}

void USaveSubsystem::OnAsyncSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	UE_LOG(LogSaveSystem, Display, TEXT("Async Saving Finished"));
	OnPlayerDataSaved.Broadcast(bSuccess);
	if(!bSuccess)
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Save Failed for Slot: %s"), *SlotName);
		return;
	}
	UE_LOG(LogSaveSystem, Display, TEXT("Save was Successful"));
}

void USaveSubsystem::SetSaveGameClass(TSubclassOf<USaveGame> SaveGameSubClass)
{
	SaveGameClass = SaveGameSubClass;
	StartNewSave();
}

FString USaveSubsystem::GetPlayerSaveSlot()
{
	return "";
}

void USaveSubsystem::SaveData()
{
	
	UE_LOG(LogSaveSystem, Display, TEXT("Saving Player Data"));
	
	// If it isn't Valid, Create a new instance
	if(!PlayerSaveObject)
	{
		UE_LOG(LogSaveSystem, Warning, TEXT("Player Save is NOT Valid. Creating New Instance"));
		PlayerSaveObject = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
	}
	FAsyncSaveGameToSlotDelegate asyncSaveDelegate;
	asyncSaveDelegate.BindUObject(this, &USaveSubsystem::OnAsyncSaveFinished);
	UGameplayStatics::AsyncSaveGameToSlot(PlayerSaveObject, GetPlayerSaveSlot(), 0, asyncSaveDelegate);
}

void USaveSubsystem::LoadData()
{
	UE_LOG(LogSaveSystem, Display, TEXT("Attempting to Load Data"));

	// If a save game exists in a slot, then load it
	if(UGameplayStatics::DoesSaveGameExist(GetPlayerSaveSlot(), 0))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Player Save Data Exists. Async Loading"));
		
		FAsyncLoadGameFromSlotDelegate asyncLoadDelegate;
		asyncLoadDelegate.BindUObject(this, &USaveSubsystem::OnAsyncLoadFinished);
		UGameplayStatics::AsyncLoadGameFromSlot(GetPlayerSaveSlot(), 0, asyncLoadDelegate);
	}

	// Otherwise, create one
	else
	{
		UE_LOG(LogSaveSystem, Warning, TEXT("No Player Save Data Exists. Creating New One with Class: %s"), *GetNameSafe(SaveGameClass));
		OnAsyncLoadFinished(GetPlayerSaveSlot(), 0, UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	}
}

void USaveSubsystem::ClearSave()
{
	if(UGameplayStatics::DoesSaveGameExist(GetPlayerSaveSlot(), 0))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Deleting Save Data"));
		UGameplayStatics::DeleteGameInSlot(GetPlayerSaveSlot(), 0);
	}
	PlayerSaveObject = nullptr;
}
