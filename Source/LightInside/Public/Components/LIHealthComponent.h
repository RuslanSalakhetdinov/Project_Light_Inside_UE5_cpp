// Project "Lignt Inside" Game. RedBuff Studios copyright.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LIHealthComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LIGHTINSIDE_API ULIHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULIHealthComponent();

	float GetHealth() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth = 100.0f;

	float Health = 0.0f;

	virtual void BeginPlay() override;
};
