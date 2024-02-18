// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "MultiplayerSessions/Public/ServerRoomSlot.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeServerRoomSlot() {}
// Cross Module References
	MULTIPLAYERSESSIONS_API UClass* Z_Construct_UClass_UServerRoomSlot_NoRegister();
	MULTIPLAYERSESSIONS_API UClass* Z_Construct_UClass_UServerRoomSlot();
	UMG_API UClass* Z_Construct_UClass_UUserWidget();
	UPackage* Z_Construct_UPackage__Script_MultiplayerSessions();
	UMG_API UClass* Z_Construct_UClass_UTextBlock_NoRegister();
	UMG_API UClass* Z_Construct_UClass_UButton_NoRegister();
// End Cross Module References
	DEFINE_FUNCTION(UServerRoomSlot::execServerRoomSlot_JoinButtonClicked)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->ServerRoomSlot_JoinButtonClicked();
		P_NATIVE_END;
	}
	void UServerRoomSlot::StaticRegisterNativesUServerRoomSlot()
	{
		UClass* Class = UServerRoomSlot::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "ServerRoomSlot_JoinButtonClicked", &UServerRoomSlot::execServerRoomSlot_JoinButtonClicked },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UServerRoomSlot_ServerRoomSlot_JoinButtonClicked_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UServerRoomSlot_ServerRoomSlot_JoinButtonClicked_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/ServerRoomSlot.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UServerRoomSlot_ServerRoomSlot_JoinButtonClicked_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UServerRoomSlot, nullptr, "ServerRoomSlot_JoinButtonClicked", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UServerRoomSlot_ServerRoomSlot_JoinButtonClicked_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UServerRoomSlot_ServerRoomSlot_JoinButtonClicked_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UServerRoomSlot_ServerRoomSlot_JoinButtonClicked()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UServerRoomSlot_ServerRoomSlot_JoinButtonClicked_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UServerRoomSlot);
	UClass* Z_Construct_UClass_UServerRoomSlot_NoRegister()
	{
		return UServerRoomSlot::StaticClass();
	}
	struct Z_Construct_UClass_UServerRoomSlot_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ServerName_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_ServerName;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CurrentPlayers_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_CurrentPlayers;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_MaxPlayers_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_MaxPlayers;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_JoinButton_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_JoinButton;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UServerRoomSlot_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UUserWidget,
		(UObject* (*)())Z_Construct_UPackage__Script_MultiplayerSessions,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UServerRoomSlot_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UServerRoomSlot_ServerRoomSlot_JoinButtonClicked, "ServerRoomSlot_JoinButtonClicked" }, // 2369925369
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UServerRoomSlot_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "IncludePath", "ServerRoomSlot.h" },
		{ "ModuleRelativePath", "Public/ServerRoomSlot.h" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_ServerName_MetaData[] = {
		{ "BindWidget", "" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/ServerRoomSlot.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_ServerName = { "ServerName", nullptr, (EPropertyFlags)0x0010000000080008, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UServerRoomSlot, ServerName), Z_Construct_UClass_UTextBlock_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_ServerName_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_ServerName_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_CurrentPlayers_MetaData[] = {
		{ "BindWidget", "" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/ServerRoomSlot.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_CurrentPlayers = { "CurrentPlayers", nullptr, (EPropertyFlags)0x0010000000080008, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UServerRoomSlot, CurrentPlayers), Z_Construct_UClass_UTextBlock_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_CurrentPlayers_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_CurrentPlayers_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_MaxPlayers_MetaData[] = {
		{ "BindWidget", "" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/ServerRoomSlot.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_MaxPlayers = { "MaxPlayers", nullptr, (EPropertyFlags)0x0010000000080008, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UServerRoomSlot, MaxPlayers), Z_Construct_UClass_UTextBlock_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_MaxPlayers_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_MaxPlayers_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_JoinButton_MetaData[] = {
		{ "BindWidget", "" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/ServerRoomSlot.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_JoinButton = { "JoinButton", nullptr, (EPropertyFlags)0x0010000000080008, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UServerRoomSlot, JoinButton), Z_Construct_UClass_UButton_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_JoinButton_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_JoinButton_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UServerRoomSlot_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_ServerName,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_CurrentPlayers,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_MaxPlayers,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UServerRoomSlot_Statics::NewProp_JoinButton,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UServerRoomSlot_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UServerRoomSlot>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UServerRoomSlot_Statics::ClassParams = {
		&UServerRoomSlot::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UServerRoomSlot_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_UServerRoomSlot_Statics::PropPointers),
		0,
		0x00B010A0u,
		METADATA_PARAMS(Z_Construct_UClass_UServerRoomSlot_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UServerRoomSlot_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UServerRoomSlot()
	{
		if (!Z_Registration_Info_UClass_UServerRoomSlot.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UServerRoomSlot.OuterSingleton, Z_Construct_UClass_UServerRoomSlot_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UServerRoomSlot.OuterSingleton;
	}
	template<> MULTIPLAYERSESSIONS_API UClass* StaticClass<UServerRoomSlot>()
	{
		return UServerRoomSlot::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UServerRoomSlot);
	struct Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_ServerRoomSlot_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_ServerRoomSlot_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UServerRoomSlot, UServerRoomSlot::StaticClass, TEXT("UServerRoomSlot"), &Z_Registration_Info_UClass_UServerRoomSlot, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UServerRoomSlot), 33974252U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_ServerRoomSlot_h_2535855903(TEXT("/Script/MultiplayerSessions"),
		Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_ServerRoomSlot_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_DeathMatch_Plugins_MultiplayerSessions_Source_MultiplayerSessions_Public_ServerRoomSlot_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
