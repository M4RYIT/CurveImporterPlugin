// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeCategories.h"
#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CurveImporter.generated.h"

/**
 * 
 */
UCLASS()
class WORLDPLUGIN_API UCurveImporter : public UFactory
{
	GENERATED_BODY()

private:
	const TCHAR* LoadingMessage = TEXT("Loading ...");

public:
	UCurveImporter();

	UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
							   EObjectFlags Flags, const FString& Filename,
							   const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
							  UObject* Context, FFeedbackContext* Warn) override;
	FText GetDisplayName() const override
	{
		return FText::FromString(TEXT("Custom Curve Float"));
	}

	uint32 GetMenuCategories() const override
	{
		return EAssetTypeCategories::Basic;
	}
};
