#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "TestCharacter.generated.h"

class UWidgetComponent;
class UEnemyCharacterHPWidget;


UCLASS()
class PROJECT_REQUIEM_API ATestCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(VisibleAnywhere, Category = "UI")
	UWidgetComponent* HPWidgetComponent;
	UPROPERTY() UEnemyCharacterHPWidget* HPWidget;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	ATestCharacter();

	UFUNCTION(BlueprintCallable)
	void ReceiveDamageTest();
};
