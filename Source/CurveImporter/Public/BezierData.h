// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "BezierData.generated.h"

/**
 * 
 */
UCLASS()
class WORLDPLUGIN_API UBezierData : public UAssetUserData
{
	GENERATED_BODY()

public :
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bezier")
	TArray<FVector> ControlPoints;
};
