// Project "Lignt Inside" Game. RedBuff Studios copyright.

#include "Components/LIHealthComponent.h"

ULIHealthComponent::ULIHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float ULIHealthComponent::GetHealth() const
{
	return Health;
}

void ULIHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}
