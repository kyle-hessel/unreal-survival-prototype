// Fill out your copyright notice in the Description page of Project Settings.


#include "CampGameModeBase.h"

ACampGameModeBase::ACampGameModeBase()
{
	// Load default items data table created in-editor into C++.
	static ConstructorHelpers::FObjectFinder<UDataTable> ItemsDataObject(TEXT("DataTable'/Game/CampfiresTest/Blueprints/Interactables/DT_Items_Default.DT_Items_Default'"));

	// If a data table isn't specified in blueprint, load in this default data table into our Items data table.
	if (ItemsDataObject.Succeeded()) Items = ItemsDataObject.Object;

	// Only run this row creation on the constructor if there aren't any rows. I think this is a horrible place to be doing this.
	TArray<FItemStruct*> RowsArray;
	Items->GetAllRows("Fetch", RowsArray);
	
	// Just in case this constructor runs multiple times (increase as item list grows)
	if (RowsArray.Num() < 4)
	{
		PopulateItemsDataTable();
	}
}

void ACampGameModeBase::PopulateItemsDataTable() const
{
	// Populate default row item struct
	FItemStruct DefaultRowItem;
	DefaultRowItem.DisplayName = FText::FromString("Default World Item");
	DefaultRowItem.DisplayDescription = FText::FromString("Pseudo-item.");
	static ConstructorHelpers::FObjectFinder<UTexture2D> DefaultItemIcon(TEXT("Texture2D'/Game/CampfiresTest/Assets/Textures/InventoryIcons/7a4cc3c2b36f4b01ed6b90f7eed4ebb5.7a4cc3c2b36f4b01ed6b90f7eed4ebb5'"));
	if (DefaultItemIcon.Succeeded()) DefaultRowItem.Thumbnail = DefaultItemIcon.Object;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultStaticMesh(TEXT("StaticMesh'/Game/CampfiresTest/Assets/Test_Meshes/Tent.Tent'"));
	if (DefaultStaticMesh.Succeeded()) DefaultRowItem.Mesh = DefaultStaticMesh.Object;
	DefaultRowItem.bIsStackable = true;
	DefaultRowItem.ItemType = EItemType::ITEM_Food;
	DefaultRowItem.bEnergyItem = true;
	DefaultRowItem.EnergyDelta = 50.f;
	DefaultRowItem.bLifeForceItem = true;
	DefaultRowItem.LifeForceDelta = 25.f;

	// Pass in default row struct object to create a row
	Items->AddRow(FName(TEXT("Default")), DefaultRowItem);

		
	// Second row item struct
	FItemStruct SecondRowItem;
	SecondRowItem.DisplayName = FText::FromString("Wood");
	SecondRowItem.DisplayDescription = FText::FromString("Useful for constructing objects, and as fuel for a campfire.");
	static ConstructorHelpers::FObjectFinder<UTexture2D> SecondItemIcon(TEXT("Texture2D'/Game/CampfiresTest/Assets/Textures/InventoryIcons/65272156.65272156'"));
	if (SecondItemIcon.Succeeded()) SecondRowItem.Thumbnail = SecondItemIcon.Object;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SecondStaticMesh(TEXT("StaticMesh'/Game/CampfiresTest/Assets/Test_Meshes/Campfire.Campfire'"));
	if (SecondStaticMesh.Succeeded()) SecondRowItem.Mesh = SecondStaticMesh.Object;
	SecondRowItem.bIsStackable = true;
	SecondRowItem.ItemType = EItemType::ITEM_Resource;

	Items->AddRow(FName(TEXT("Wood")), SecondRowItem);

	// Third row item struct
	FItemStruct ThirdRowItem;
	ThirdRowItem.DisplayName = FText::FromString("Stone");
	ThirdRowItem.DisplayDescription = FText::FromString("Useful for constructing objects, such as fire pits.");
	static ConstructorHelpers::FObjectFinder<UTexture2D> ThirdItemIcon(TEXT("Texture2D'/Game/CampfiresTest/Assets/Textures/InventoryIcons/kyle-hessel-self-portrait.kyle-hessel-self-portrait'"));
	if (ThirdItemIcon.Succeeded()) ThirdRowItem.Thumbnail = ThirdItemIcon.Object;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ThirdStaticMesh(TEXT("StaticMesh'/Game/CampfiresTest/Assets/Test_Meshes/RockOrSomething.RockOrSomething'"));
	if (ThirdStaticMesh.Succeeded()) ThirdRowItem.Mesh = ThirdStaticMesh.Object;
	ThirdRowItem.bIsStackable = true;
	ThirdRowItem.ItemType = EItemType::ITEM_Resource;

	Items->AddRow(FName(TEXT("Stone")), ThirdRowItem);
	
	// Fourth
	FItemStruct FourthRowItem;
	FourthRowItem.DisplayName = FText::FromString("Cloth");
	FourthRowItem.DisplayDescription = FText::FromString("Useful for constructing objects, such as tents.");
	static ConstructorHelpers::FObjectFinder<UTexture2D> FourthItemIcon(TEXT("Texture2D'/Game/CampfiresTest/Assets/Textures/InventoryIcons/prof_crop2.prof_crop2'"));
	if (FourthItemIcon.Succeeded()) FourthRowItem.Thumbnail = FourthItemIcon.Object;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FourthStaticMesh(TEXT("StaticMesh'/Game/CampfiresTest/Assets/Test_Meshes/RockOrSomething.RockOrSomething'"));
	if (FourthStaticMesh.Succeeded()) FourthRowItem.Mesh = FourthStaticMesh.Object;
	FourthRowItem.bIsStackable = true;
	FourthRowItem.ItemType = EItemType::ITEM_Resource;

	Items->AddRow(FName(TEXT("Cloth")), FourthRowItem);
	
}
