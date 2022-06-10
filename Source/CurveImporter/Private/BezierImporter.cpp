// Fill out your copyright notice in the Description page of Project Settings.

#include "BezierImporter.h"
#include "BezierData.h"
#include "ImageUtils.h"
#include "Misc/FeedbackContext.h"
#include "Misc/FileHelper.h"

UBezierImporter::UBezierImporter()
{
	bEditorImport = true;
	SupportedClass = UTexture2D::StaticClass();
	Formats.Add(FString("bz;Bz file format"));
}

UObject* UBezierImporter::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, const FString& Filename,
	const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UTexture2D* Texture = CreateBezierTexture(InParent, InName, Flags, Filename);
	if (!Texture)
	{
		bOutOperationCanceled = true;
	}
	return Texture;
}

UTexture2D* UBezierImporter::CreateBezierTexture(UObject* InParent, FName InName,
	EObjectFlags Flags, const FString& Filename)
{
	TArray<FString> Lines;
	FFileHelper::LoadFileToStringArray(Lines, *Filename);
	
	FIntVector Size;
	GetTextureSize(Lines[0], Size);

	TArray<FColor> Pixels;
	Pixels.AddZeroed(Size.X * Size.Y);
	int32 SamplesNumber = FCString::Atoi(*Lines[1]);

	TArray<FVector> ControlPoints;
	FVector MinRange;
	GetControlPoint(Lines[2], MinRange);
	ControlPoints.Add(MinRange);
	FVector MaxRange = MinRange;

	for (int32 Index = 3; Index < Lines.Num(); Index++)
	{
		FVector ControlPoint;
		GetControlPoint(Lines[Index], ControlPoint);
		ControlPoints.Add(ControlPoint);
		MinRange = MinRange.ComponentMin(ControlPoint);
		MaxRange = MaxRange.ComponentMax(ControlPoint);
	}

	TArray<FVector> Samples;
	FVector::EvaluateBezier(ControlPoints.GetData(), SamplesNumber, Samples);

	TArray<FIntVector> TexCoords;
	RemapPointsOnTexture(Size, MinRange, MaxRange, Samples, TexCoords);
	DrawPoints(Pixels, Size, TexCoords, FColor::White);

	for (int32 Index = 0; Index < 3; Index++)
	{
		Samples.Empty();
		TexCoords.Empty();
		TArray<FVector> Vs = { ControlPoints[Index], ControlPoints[Index], ControlPoints[Index + 1], ControlPoints[Index + 1] };
		FVector::EvaluateBezier(Vs.GetData(), SamplesNumber, Samples);
		RemapPointsOnTexture(Size, MinRange, MaxRange, Samples, TexCoords);
		DrawPoints(Pixels, Size, TexCoords, FColor::Green);
	}

	TexCoords.Empty();
	RemapPointsOnTexture(Size, MinRange, MaxRange, ControlPoints, TexCoords);
	DrawPoints(Pixels, Size, TexCoords, FColor::Red, 3);	

	UTexture2D* Texture = FImageUtils::CreateTexture2D(Size.X, Size.Y, Pixels, InParent, InName.ToString(), Flags, FCreateTexture2DParameters());
	UBezierData* BezierData = NewObject<UBezierData>(Texture);
	BezierData->ControlPoints = ControlPoints;
	Texture->AddAssetUserData(BezierData);
	return Texture;
}

void UBezierImporter::DrawPoints(TArray<FColor>& Pixels, const FIntVector& TexSize, const TArray<FIntVector>& TexCoords, FColor Color, int32 PointSize)
{
	for (const FIntVector& TexCoord : TexCoords)
	{
		DrawPoint(Pixels, TexSize, TexCoord, Color);

		for (int32 Index = 1; Index <= PointSize; Index++)
		{
			DrawPoint(Pixels, TexSize, FIntVector(TexCoord.X, TexCoord.Y - Index, 0), Color);
			DrawPoint(Pixels, TexSize, FIntVector(TexCoord.X + Index, TexCoord.Y - Index, 0), Color);
			DrawPoint(Pixels, TexSize, FIntVector(TexCoord.X + Index, TexCoord.Y, 0), Color);
			DrawPoint(Pixels, TexSize, FIntVector(TexCoord.X + Index, TexCoord.Y + Index, 0), Color);
			DrawPoint(Pixels, TexSize, FIntVector(TexCoord.X, TexCoord.Y + Index, 0), Color);
			DrawPoint(Pixels, TexSize, FIntVector(TexCoord.X - Index, TexCoord.Y + Index, 0), Color);
			DrawPoint(Pixels, TexSize, FIntVector(TexCoord.X - Index, TexCoord.Y, 0), Color);
			DrawPoint(Pixels, TexSize, FIntVector(TexCoord.X - Index, TexCoord.Y - Index, 0), Color);
		}
	}
}

void UBezierImporter::DrawPoint(TArray<FColor>& Pixels, FIntVector TexSize, FIntVector TexCoord, FColor Color)
{
	int32 Index = TexCoord.X + (TexCoord.Y * TexSize.X);
	if (FMath::IsWithin<int32>(Index, 0, TexSize.X * TexSize.Y))
	{
		Pixels[Index] = Color;
	}
}

float UBezierImporter::RemapValue(float Value, float SrcA, float SrcB, float DestA, float DestB)
{
	return DestA + (((FMath::Clamp(Value, SrcA, SrcB) - SrcA) / (SrcB - SrcA)) * (DestB - DestA));
}

void UBezierImporter::RemapPointsOnTexture(const FIntVector& TextureSize, const FVector& MinRange,
	const FVector& MaxRange, const TArray<FVector>& InPoints, TArray<FIntVector>& OutPoints)
{
	for (const FVector& Point : InPoints)
	{	
		int32 X = FMath::RoundToInt(RemapValue(Point.X, MinRange.X, MaxRange.X, 0.f, TextureSize.X-1));
		int32 Y = TextureSize.Y - 1 - FMath::RoundToInt(RemapValue(Point.Y, MinRange.Y, MaxRange.Y, 0.f, TextureSize.Y-1));
		OutPoints.Add(FIntVector(X, Y, 0));		
	}
}

bool UBezierImporter::GetControlPoint(const FString& InLine, FVector& OutPoint)
{
	TArray<FString> Point;
	if (InLine.ParseIntoArrayWS(Point) != 2)
	{
		return false;
	}

	OutPoint = FVector(FCString::Atof(*Point[0]), FCString::Atof(*Point[1]), 0.f);
	return true;
}

bool UBezierImporter::GetTextureSize(const FString& InLine, FIntVector& OutSize)
{
	TArray<FString> Size;
	if (InLine.ParseIntoArrayWS(Size) != 2)
	{
		return false;
	}

	OutSize = FIntVector(FCString::Atoi(*Size[0]), FCString::Atoi(*Size[1]), 0);
	return true;
}

UTexture2D* UBezierImporter::CreateRandomTexture(uint32 Width, uint32 Height, UObject* Outer, 
	FString TextureName, EObjectFlags Flags)
{
	TArray<FColor> Pixels;
	Pixels.AddZeroed(Width * Height);

	FCreateTexture2DParameters Params;

	for (int32 Index=0; Index < Pixels.Num(); Index++)
	{
		Pixels[Index].R = FMath::Rand() % 255;
		Pixels[Index].G = FMath::Rand() % 255;
		Pixels[Index].B = FMath::Rand() % 255;
		Pixels[Index].A = 255;
	}

	return FImageUtils::CreateTexture2D(Width, Height, Pixels, Outer, TextureName, Flags, Params);
}