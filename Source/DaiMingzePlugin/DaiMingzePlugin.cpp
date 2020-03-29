// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

// #include "DaiMingzePlugin.h"
// /* 做为插件使用 */
// #define LOCTEXT_NAMESPACE "FDaiMingzePluginModule"

// void FDaiMingzePluginModule::StartupModule()
// {
// 	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
// }

// void FDaiMingzePluginModule::ShutdownModule()
// {
// 	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
// 	// we call this function before unloading the module.
// }

// #undef LOCTEXT_NAMESPACE
	
// IMPLEMENT_MODULE(FDaiMingzePluginModule, DaiMingzePlugin)
/* 做为插件使用 */


/* 作为游戏模块使用 */
#include "DaiMingzePlugin.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, DaiMingzePlugin, "DaiMingzePlugin" );

/* 作为游戏模块使用 */

