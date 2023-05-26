// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/SaveSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "Interfaces/SaveObjectInterface.h"
#include "Kismet/GameplayStatics.h"

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USaveSubsystem::Deinitialize()
{
	OnPlayerDataLoaded.Clear();
	OnPlayerDataSaved.Clear();
	Super::Deinitialize();
}

void USaveSubsystem::StartNewSave(bool bLoad)
{
	if(UGameplayStatics::DoesSaveGameExist(GetPlayerSaveSlot(), 0))
	{
		UGameplayStatics::DeleteGameInSlot(GetPlayerSaveSlot(), 0);
	}
	if(bLoad)
	{
		LoadData();
	}
}


void USaveSubsystem::OnAsyncLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame)
{
	UE_LOG(LogSaveSystem, Display, TEXT("Async Loading Finished"));
	if(IsValid(SaveGame))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Save Game Pointer is Valid"));
		PlayerSaveObject = SaveGame;
		// Check if the Player Save Object implements the Save Object Interface
		if(PlayerSaveObject->GetClass()->ImplementsInterface(USaveObjectInterface::StaticClass()))
		{
			UE_LOG(LogSaveSystem, Display, TEXT("Save Game Object Implements Save Object Interface"));
			ISaveObjectInterface::Execute_OnObjectLoaded(PlayerSaveObject);
		}
		else
		{
			UE_LOG(LogSaveSystem, Warning, TEXT("Save Game Object Does NOT Implement Save Object Interface"));
		}
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

void USaveSubsystem::SetSaveGameClass(TSubclassOf<USaveGame> SaveGameSubClass, bool bResetSaveObject)
{
	_SaveGameClass = SaveGameSubClass;
	if(bResetSaveObject) StartNewSave();
}

FString USaveSubsystem::GetPlayerSaveSlot()
{
	return "";
}

USaveGame* USaveSubsystem::GetValidatedSaveGameObject(const TSubclassOf<USaveGame> SaveGameClass, bool& bIsValid)
{
	if(!SaveGameClass)
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Save Game Class is Invalid"));
		bIsValid = false;
		return nullptr;
	}

	USaveGame* SaveGame = GetSaveGameObject(SaveGameClass);
	
	bIsValid = IsValid(SaveGame) && SaveGame->IsA(SaveGameClass);
	return SaveGame;
}

USaveGame* USaveSubsystem::GetSaveGameObject(const TSubclassOf<USaveGame> SaveGameClass)
{
	if(!SaveGameClass)
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Save Game Class is Invalid"));
		return nullptr;
	}
	if(!IsValid(PlayerSaveObject))
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Player Save is NOT Valid"));
		return nullptr;
	}
	return PlayerSaveObject;
}

bool USaveSubsystem::AssignSaveGameObject(USaveGame* SaveGameObject)
{
	if(!IsValid(SaveGameObject))
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object is NOT Valid"));
		return false;
	}

	UE_LOG(LogSaveSystem, Display, TEXT("Assigning Save Game Object with: "), *GetNameSafe(SaveGameObject));
		
	PlayerSaveObject = SaveGameObject;
	return true;
}



void USaveSubsystem::SaveData(bool bAsync)
{
	UE_LOG(LogSaveSystem, Display, TEXT("Saving Player Data"));
	
	// If it isn't Valid, Create a new instance
	if(!PlayerSaveObject)
	{
		UE_LOG(LogSaveSystem, Warning, TEXT("Player Save is NOT Valid. Creating New Instance"));
		PlayerSaveObject = UGameplayStatics::CreateSaveGameObject(_SaveGameClass);
	}
	if(bAsync){
		FAsyncSaveGameToSlotDelegate asyncSaveDelegate;
		asyncSaveDelegate.BindUObject(this, &USaveSubsystem::OnAsyncSaveFinished);
		UGameplayStatics::AsyncSaveGameToSlot(PlayerSaveObject, GetPlayerSaveSlot(), 0, asyncSaveDelegate);
		UE_LOG(LogSaveSystem, Display, TEXT("Saving Player Data Asynchronously"));
	}
	else
	{
		UGameplayStatics::SaveGameToSlot(PlayerSaveObject, GetPlayerSaveSlot(), 0);
		UE_LOG(LogSaveSystem, Display, TEXT("Saving Player Data Synchronously"));
	}
}

void USaveSubsystem::LoadData()
{
	UE_LOG(LogSaveSystem, Display, TEXT("Attempting to Load Data from Slot: %s"),* GetPlayerSaveSlot());

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
		UE_LOG(LogSaveSystem, Warning, TEXT("No Player Save Data Exists. Creating New One with Class: %s"), *GetNameSafe(_SaveGameClass));
		OnAsyncLoadFinished(GetPlayerSaveSlot(), 0, UGameplayStatics::CreateSaveGameObject(_SaveGameClass));
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
