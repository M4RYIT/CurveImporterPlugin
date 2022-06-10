// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "BezierImporter.generated.h"

/**
 * 
 */
UCLASS()
class WORLDPLUGIN_API UBezierImporter : public UFactory
{
	GENERATED_BODY()

public:
	UBezierImporter();

	UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
		EObjectFlags Flags, const FString& Filename,
		const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

	UTexture2D* CreateRandomTexture(uint32 Width, uint32 Height, UObject* Outer, FString TextureName, EObjectFlags Flags);

	UTexture2D* CreateBezierTexture(UObject* InParent, FName InName,
		EObjectFlags Flags, const FString& Filename);

	bool GetTextureSize(const FString& InLine, FIntVector& OutSize);

	bool GetControlPoint(const FString& InLine, FVector& OutPoint);

	void RemapPointsOnTexture(const FIntVector& TextureSize, const FVector& MinRange, 
		const FVector& MaxRange, const TArray<FVector>& InPoints, TArray<FIntVector>& OutPoints);

	float RemapValue(float Value, float SrcA, float SrcB, float DestA, float DestB);

	void DrawPoints(TArray<FColor>& Pixels, const FIntVector& TexSize, const TArray<FIntVector>& TexCoords, FColor Color, int32 PointSize = 0);

	void DrawPoint(TArray<FColor>& Pixels, FIntVector TexSize, FIntVector TexCoord, FColor Color);
};
