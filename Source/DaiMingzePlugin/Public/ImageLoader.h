// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PixelFormat.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ImageLoader.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DAIMINGZEPLUGIN_API UImageLoader : public UObject
{
	GENERATED_BODY()
public:
	//异步加载图片资源
	//Outer    : 被引用的父类对象，New对象GC使用，防止内存泄漏。
	//Loader   : 被引用的UImageLoader对象，在传入以前必须先绑定好加载完成的回调代理LoadCompleted。
	//ImagePath: 引擎外部图片路径
	UFUNCTION(BlueprintCallable, Category = ImageLoader, meta = (HidePin = "Outer", DefaultToSelf = "Outer"))
	static void LoadImageFromDiskAsync(UObject* Outer, UImageLoader* Loader, const FString& ImagePath);

	/**
	Loads an image file from disk into a texture. This will block the calling thread until completed.
	@return A texture created from the loaded image file.
	*/
	//同步加载图片资源
	//Outer    : 被引用的父类对象，New对象GC使用，防止内存泄漏。
	//ImagePath: 引擎外部图片路径
	//ReturnVal: 返回一个图片的UTexture2D资源
	UFUNCTION(BlueprintCallable, Category = ImageLoader, meta = (HidePin = "Outer", DefaultToSelf = "Outer"))
	static UTexture2D* LoadImageFromDisk(UObject* Outer, const FString& ImagePath);

public:
	/**
	Declare a broadcast-style delegate type, which is used for the load completed event.
	Dynamic multicast delegates are the only type of event delegates that Blueprint scripts can bind to.
	*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImageLoadCompleted, UTexture2D*, Texture);

	/** This accessor function allows C++ code to bind to the event. */
	FOnImageLoadCompleted& OnLoadCompleted()
	{
		return LoadCompleted;
	}

private:
	/** Helper function to dynamically create a new texture from raw pixel data. */
	static UTexture2D* CreateTexture(UObject* Outer, const TArray<uint8>& PixelData, int32 InSizeX, int32 InSizeY, EPixelFormat PixelFormat = EPixelFormat::PF_B8G8R8A8, FName BaseName = NAME_None);

private:
	/**
	Holds the load completed event delegate.
	Giving Blueprint access to this private variable allows Blueprint scripts to bind to the event.
	*/
	UPROPERTY(BlueprintAssignable, Category = ImageLoader, meta = (AllowPrivateAccess = true))
	FOnImageLoadCompleted LoadCompleted;

	/** Holds the future value which represents the asynchronous loading operation. */
	TFuture<UTexture2D*> Future;
};
