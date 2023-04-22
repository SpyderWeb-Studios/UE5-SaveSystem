// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubsystem.generated.h"

class USaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDataLoaded, USaveGame*, PlayerSaveObject);

/**
 * 
 */
UCLASS(Abstract)
class SAVESYSTEM_API USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDataLoaded OnPlayerDataLoaded;

	UFUNCTION(BlueprintCallable)
	void StartNewSave();
	
	FString PlayerSaveSlot = "PlayerSlot";

	UFUNCTION(BlueprintCallable)
	void SaveData();

	UFUNCTION(BlueprintCallable)
	void LoadData();


#pragma region Event Listeners

	/**
	 * @brief 
	 * @param SlotName 
	 * @param UserIndex 
	 * @param SaveGame 
	 */
	UFUNCTION()
	virtual void OnAsyncLoadFinished(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame);
		
	UFUNCTION()
	virtual void OnAsyncSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	
	void SetSaveGameClass(TSubclassOf<USaveGame> SaveGameSubClass); 

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USaveGame> PlayerSaveObject;

	
private:

	TSubclassOf<USaveGame> SaveGameClass;

	bool bHasDoneInitialLoad;
	
};
