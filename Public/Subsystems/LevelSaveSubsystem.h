// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/LevelSaveObject.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/WorldSubsystem.h"
#include "LevelSaveSubsystem.generated.h"

/**
 * 
 */
UCLASS(Abstract, NotBlueprintType)
class SAVESYSTEM_API ULevelSaveSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:

	ULevelSaveSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void UpdateActors(AActor* SavedActor, bool bInteracted);

	virtual void UpdateMovedActors(TObjectPtr<AActor> SavedActor, FTransform Transform);
	
	UFUNCTION(BlueprintCallable)
	void SaveData();

	UFUNCTION(BlueprintCallable)
	void LoadData();
	
protected:
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

	FString LevelSaveSlot = "LevelSlot";
	
private:


	UPROPERTY()
	TObjectPtr<ULevelSaveObject> LevelSaveObject;
	
	
};
