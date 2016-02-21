// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "leveldb/db.h"

#include "LevelDBPluginBPLibrary.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FLevelDBOptions
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDB")
		bool bCreateDbIfMissing = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDB")
		bool bUseSnappyCompression = true;

	FLevelDBOptions()
	{

	}
};

UCLASS(BlueprintType)
class ULevelDBObject : public UObject
{
	GENERATED_BODY()
public:
	leveldb::DB* db;

	virtual void BeginDestroy() override;
};


UCLASS()
class ULevelDBPluginBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Open Database"), Category = "LevelDB")
		static bool OpenLevelDB(UObject* WorldContextObject, FString DataBaseName, FLevelDBOptions Options, ULevelDBObject* & DatabaseObject);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write String To Database"), Category = "LevelDB")
		static bool WriteStringToLevelDB(ULevelDBObject* DatabaseObject, FString Key, FString Value);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write Object To Database"), Category = "LevelDB")
		static bool WriteObjectToLevelDB(ULevelDBObject* DatabaseObject, FString Key, UObject* Value);


	UFUNCTION(BlueprintPure, meta = (DisplayName = "Read StringFrom Database"), Category = "LevelDB")
		static bool ReadStringFromLevelDB(ULevelDBObject* DatabaseObject, FString Key, FString & Value);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Read Object From Database"), Category = "LevelDB")
		static bool ReadObjectFromLevelDB(ULevelDBObject* DatabaseObject, FString Key, UObject* & Value);
};
