// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "MultiplayerSessions/Public/StartMapHUD.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeStartMapHUD() {}
// Cross Module References
	MULTIPLAYERSESSIONS_API UClass* Z_Construct_UClass_AStartMapHUD_NoRegister();
	MULTIPLAYERSESSIONS_API UClass* Z_Construct_UClass_AStartMapHUD();
	ENGINE_API UClass* Z_Construct_UClass_AHUD();
	UPackage* Z_Construct_UPackage__Script_MultiplayerSessions();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	UMG_API UClass* Z_Construct_UClass_UUserWidget_NoRegister();
// End Cross Module References
	void AStartMapHUD::StaticRegisterNativesAStartMapHUD()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AStartMapHUD);
	UClass* Z_Construct_UClass_AStartMapHUD_NoRegister()
	{
		return AStartMapHUD::StaticClass();
	}
	struct Z_Construct_UClass_AStartMapHUD_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ServerRoomSlotClass_MetaData[];
#endif
		static const UECodeGen_Private::FClassPropertyParams NewProp_ServerRoomSlotClass;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AStartMapHUD_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AHUD,
		(UObject* (*)())Z_Construct_UPackage__Script_MultiplayerSessions,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AStartMapHUD_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Rendering Actor Input Replication" },
		{ "IncludePath", "StartMapHUD.h" },
		{ "ModuleRelativePath", "Public/StartMapHUD.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AStartMapHUD_Statics::NewProp_ServerRoomSlotClass_MetaData[] = {
		{ "Category", "StartMapHUD" },
		{ "ModuleRelativePath", "Public/StartMapHUD.h" },
	};
#endif
	const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_AStartMapHUD_Statics::NewProp_ServerRoomSlotClass = { "ServerRoomSlotClass", nullptr, (EPropertyFlags)0x0014000000000001, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AStartMapHUD, ServerRoomSlotClass), Z_Construct_UClass_UUserWidget_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(Z_Construct_UClass_AStartMapHUD_Statics::NewProp_ServerRoomSlotClass_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AStartMapHUD_Statics::NewProp_ServerRoomSlotClass_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AStartMapHUD_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AStartMapHUD_Statics::NewProp_ServerRoomSlotClass,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AStartMapHUD_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AStartMapHUD>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_AStartMapHUD_Statics::ClassParams = {
		&AStartMapHUD::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_AStartMapHUD_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_AStartMapHUD_Statics::PropPointers),
		0,
		0x009002ACu,
		METADATA_PARAMS(Z_Construct_UClass_AStartMapHUD_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AStartMapHUD_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AStartMapHUD()
	{
		if (!Z_Registration_Info_UClass_AStartMapHUD.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AStartMapHUD.OuterSingleton, Z_Construct_UClass_AStartMapHUD_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_AStartMapHUD.OuterSingleton;
	}
	template<> MULTIPLAYERSESSIONS_API UClass* StaticClass<AStartMapHUD>()
	{
		return AStartMapHUD::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(AStartMapHUD);
	struct Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_StartMapHUD_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_StartMapHUD_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_AStartMapHUD, AStartMapHUD::StaticClass, TEXT("AStartMapHUD"), &Z_Registration_Info_UClass_AStartMapHUD, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AStartMapHUD), 1550268022U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_StartMapHUD_h_1588869240(TEXT("/Script/MultiplayerSessions"),
		Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_StartMapHUD_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_StartMapHUD_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
