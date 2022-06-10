// Fill out your copyright notice in the Description page of Project Settings.

#include "CurveImporter.h"
#include "Misc/FeedbackContext.h"
#include "Misc/FileHelper.h"

UCurveImporter::UCurveImporter()
{
	bEditorImport = true;
	bCreateNew = true;
	SupportedClass = UCurveFloat::StaticClass();
	Formats.Add(FString("crv;Crv file format"));
}

UObject* UCurveImporter::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, const FString& Filename,
	const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UCurveFloat* Curve = NewObject<UCurveFloat>(InParent, InName, Flags);
	
	TArray<FString> Lines;
	FFileHelper::LoadFileToStringArray(Lines, *Filename);

	for (int32 I = 0; I < Lines.Num(); I++)
	{
		TArray<FString> Keys;
		Lines[I].ParseIntoArray(Keys, TEXT(" "));
		if (Keys.Num() != 2) continue;

		Curve->FloatCurve.AddKey(FCString::Atof(*Keys[0]), FCString::Atof(*Keys[1]));
		Warn->StatusForceUpdate(I, Lines.Num(), FText::FromString(LoadingMessage));
	}

	return Curve;
}

UObject* UCurveImporter::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UCurveFloat>(InParent, InName, Flags);
}
