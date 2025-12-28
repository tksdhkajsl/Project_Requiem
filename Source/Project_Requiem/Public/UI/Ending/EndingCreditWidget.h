#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingCreditWidget.generated.h"

class UScrollBox;
class UStringTable;

UCLASS()
class PROJECT_REQUIEM_API UEndingCreditWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void StartCredits();

    virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ScrollSpeed = 10.f;
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Credits")
    TObjectPtr<UStringTable> CreditStringTable;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UScrollBox> CreditScrollBox;

    UPROPERTY(EditDefaultsOnly, Category = "Credits")
    float SpawnInterval = 0.5f;

private:
    void SpawnNextLine();

    FTimerHandle LineSpawnTimerHandle;
    TArray<FString> CreditLines;
    int32 CurrentLineIndex = 0;
};
