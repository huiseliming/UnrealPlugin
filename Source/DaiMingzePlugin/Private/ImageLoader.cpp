﻿#include "..\Public\ImageLoader.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "RenderUtils.h"
#include "Engine.h"



void UImageLoader::LoadImageFromDiskAsync(UObject* Outer, UImageLoader* Loader, const FString& ImagePath)
{
	Async(EAsyncExecution::ThreadPool, [=]() {
		UTexture2D* Tex = UImageLoader::LoadImageFromDisk(Outer, ImagePath);
		AsyncTask(ENamedThreads::GameThread, [=]() { Loader->OnLoadCompleted().Broadcast(Tex); });
	});
}

// Change the UE_LOG log category name below to whichever log category you want to use.
#define UIL_LOG(Verbosity, Format, ...)	UE_LOG(LogTemp, Verbosity, Format, __VA_ARGS__)

UTexture2D* UImageLoader::LoadImageFromDisk(UObject* Outer, const FString& ImagePath)
{
	// Check if the file exists first
	if (!FPaths::FileExists(ImagePath))
	{
		UIL_LOG(Error, TEXT("File not found: %s"), *ImagePath);
		return nullptr;
	}

	// Load the compressed byte data from the file
	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *ImagePath))
	{
		UIL_LOG(Error, TEXT("Failed to load file: %s"), *ImagePath);
		return nullptr;
	}

	// Detect the image type using the ImageWrapper module
	EImageFormat ImageFormat = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper")).DetectImageFormat(FileData.GetData(), FileData.Num());
	if (ImageFormat == EImageFormat::Invalid)
	{
		UIL_LOG(Error, TEXT("Unrecognized image file format: %s"), *ImagePath);
		return nullptr;
	}

	// Create an image wrapper for the detected image format
	TSharedPtr<IImageWrapper>/*IImageWrapperPtr*/ ImageWrapper = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper")).CreateImageWrapper(ImageFormat);
	if (!ImageWrapper.IsValid())
	{
		UIL_LOG(Error, TEXT("Failed to create image wrapper for file: %s"), *ImagePath);
		return nullptr;
	}

	// Decompress the image data
	const TArray<uint8>* RawData = nullptr;
	ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num());
	ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData);
	if (RawData == nullptr)
	{
		UIL_LOG(Error, TEXT("Failed to decompress image file: %s"), *ImagePath);
		return nullptr;
	}

	// Create the texture and upload the uncompressed image data
	FString TextureBaseName = TEXT("Texture_") + FPaths::GetBaseFilename(ImagePath);
	return CreateTexture(Outer, *RawData, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), EPixelFormat::PF_B8G8R8A8, FName(*TextureBaseName));
}

UTexture2D* UImageLoader::CreateTexture(UObject* Outer, const TArray<uint8>& PixelData, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat, FName BaseName)
{
	// Shamelessly copied from UTexture2D::CreateTransient with a few modifications
	if (InSizeX <= 0 || InSizeY <= 0 ||
		(InSizeX % GPixelFormats[InFormat].BlockSizeX) != 0 ||
		(InSizeY % GPixelFormats[InFormat].BlockSizeY) != 0)
	{
		UIL_LOG(Warning, TEXT("Invalid parameters specified for UImageLoader::CreateTexture()"));
		return nullptr;
	}

	// Most important difference with UTexture2D::CreateTransient: we provide the new texture with a name and an owner
	FName TextureName = MakeUniqueObjectName(Outer, UTexture2D::StaticClass(), BaseName);
	UTexture2D* NewTexture = NewObject<UTexture2D>(Outer, TextureName, RF_Transient);

	NewTexture->PlatformData = new FTexturePlatformData();
	NewTexture->PlatformData->SizeX = InSizeX;
	NewTexture->PlatformData->SizeY = InSizeY;
	NewTexture->PlatformData->PixelFormat = InFormat;

	// Allocate first mipmap and upload the pixel data
	int32 NumBlocksX = InSizeX / GPixelFormats[InFormat].BlockSizeX;
	int32 NumBlocksY = InSizeY / GPixelFormats[InFormat].BlockSizeY;
	//FTexture2DMipMap* Mip = new(NewTexture->PlatformData->Mips) FTexture2DMipMap();
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	NewTexture->PlatformData->Mips.Add(Mip);
	//NewTexture->PlatformData->Mips.Add(Mip);
	Mip->SizeX = InSizeX;
	Mip->SizeY = InSizeY;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	void* TextureData = Mip->BulkData.Realloc(NumBlocksX * NumBlocksY * GPixelFormats[InFormat].BlockBytes);
	FMemory::Memcpy(TextureData, PixelData.GetData(), PixelData.Num());
	Mip->BulkData.Unlock();

	NewTexture->UpdateResource();
	return NewTexture;
}
