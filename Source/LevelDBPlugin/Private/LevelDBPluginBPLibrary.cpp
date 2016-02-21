// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LevelDBPluginPrivatePCH.h"
#include "LevelDBPluginBPLibrary.h"


void ULevelDBObject::BeginDestroy()
{
	//this->RemoveFromRoot();
	delete(db);

	Super::BeginDestroy();
}

bool ULevelDBPluginBPLibrary::OpenLevelDB(UObject* WorldContextObject, FString DataBaseName, FLevelDBOptions Options, ULevelDBObject* & DatabaseObject)
{
	//Create a new UObject that will take care of our LevelDB
	ULevelDBObject* dbObject = NewObject<ULevelDBObject>(WorldContextObject, ULevelDBObject::StaticClass(), "LevelDBContext");

	//Check if our Object was successfully created
	if (IsValid(dbObject))
	{
		//Prevent it from beeing GarbageCollected
		//dbObject->AddToRoot();

		//Level DB Declarative stuff
		leveldb::DB* levelDB_db;
		leveldb::Options levelDB_options;

		FString FinalDatabasePath = FPaths::GameDir().Append("\\").Append(DataBaseName);

		//Set options
		levelDB_options.create_if_missing = Options.bCreateDbIfMissing;
		levelDB_options.compression = Options.bUseSnappyCompression ? leveldb::CompressionType::kSnappyCompression : leveldb::CompressionType::kNoCompression;

		//open database connection - TCHAR_TO_UTF8(*DataBaseName) is marshalling FString to std::String
		leveldb::Status status = leveldb::DB::Open(levelDB_options, TCHAR_TO_UTF8(*FinalDatabasePath), &levelDB_db);

		//check if opening the db was successfull, if not delete the db Context object
		if (false == status.ok())
		{
			dbObject->RemoveFromRoot();
			dbObject->ConditionalBeginDestroy();

			return false;
		}

		//set db var in the dbContext
		dbObject->db = levelDB_db;

		//update the ref param
		DatabaseObject = dbObject;

		return true;
	}

	return false;
}


bool ULevelDBPluginBPLibrary::WriteStringToLevelDB(ULevelDBObject* DatabaseObject, FString Key, FString Value)
{
	leveldb::WriteOptions writeOptions;

	if (DatabaseObject->db != NULL)
		DatabaseObject->db->Put(writeOptions, TCHAR_TO_UTF8(*Key), TCHAR_TO_UTF8(*Value));
	else
		return false;

	return true;
}

bool ULevelDBPluginBPLibrary::WriteObjectToLevelDB(ULevelDBObject* DatabaseObject, FString Key, UObject* Value)
{
	if (IsValid(Value))
	{
		//Vars
		TArray<uint8> ObjectBytes;
		FMemoryWriter MemoryWriter(ObjectBytes, true);

		//Write Object Class name so we can deserialize it one day
		FString ObjectClassName = Value->GetClass()->GetName();
		MemoryWriter << ObjectClassName;

		//Serialize Object and put into the byte array as well
		FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
		Value->Serialize(Ar);

		//marshall pointer to first array entry
		auto ptr = reinterpret_cast<const char*>(ObjectBytes.GetData());

		//level db options
		leveldb::WriteOptions writeOptions;

		//create a slice to store
		leveldb::Slice data = leveldb::Slice(ptr, ObjectBytes.Num());

		//Smash it into the db
		if (DatabaseObject->db != NULL)
			DatabaseObject->db->Put(writeOptions, TCHAR_TO_UTF8(*Key), data);
		else
			return false;
		return true;
	}
	return false;
}

bool ULevelDBPluginBPLibrary::ReadStringFromLevelDB(ULevelDBObject* DatabaseObject, FString Key, FString & Value)
{
	if (DatabaseObject->db != NULL)
	{
		leveldb::ReadOptions readOptions;

		std::string result;
		leveldb::Status s = DatabaseObject->db->Get(leveldb::ReadOptions(), TCHAR_TO_UTF8(*Key), &result);

		if (false == s.ok())
			return false;

		Value = result.c_str();

		return true;
	}

	return false;
}

bool ULevelDBPluginBPLibrary::ReadObjectFromLevelDB(ULevelDBObject* DatabaseObject, FString Key, UObject* & Value)
{
	if (DatabaseObject->db != NULL)
	{
		UObject* FinalObject = NULL;
		TArray<uint8> ObjectBytes;

		std::string result;
		leveldb::Status s = DatabaseObject->db->Get(leveldb::ReadOptions(), TCHAR_TO_UTF8(*Key), &result);
		ObjectBytes.Append((uint8*)result.data(), result.size());

		FMemoryReader MemoryReader = FMemoryReader(ObjectBytes, true);

		FString ObjectClassName;
		MemoryReader << ObjectClassName;

		// Try and find it, and failing that, load it
		UClass* ObjectClass = FindObject<UClass>(ANY_PACKAGE, *ObjectClassName);
		if (ObjectClass == NULL)
		{
			ObjectClass = LoadObject<UClass>(NULL, *ObjectClassName);
		}

		// If we have a class, try and load it.
		if (ObjectClass != NULL)
		{
			Value = NewObject<UObject>(GetTransientPackage(), ObjectClass);

			FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
			Value->Serialize(Ar);

			if (IsValid(Value))
				return true;
		}
	}

	return false;
}

