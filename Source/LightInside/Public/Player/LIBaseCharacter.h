// Project "Lignt Inside" Game. RedBuff Studios copyright.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LIBaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class LIGHTINSIDE_API ALIBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALIBaseCharacter();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	const float RotationRate = 5.0f;
	const float DeadZone = 0.1f;

	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void RotationControl(float& DeltaTime);
};
