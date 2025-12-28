#include "UI/Ending/EndingCreditWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTableRegistry.h"
#include "Core/PRGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UEndingCreditWidget::StartCredits()
{
    if (!CreditScrollBox || !CreditStringTable) return;

    CreditScrollBox->ClearChildren();

    FStringTableConstRef TableRef = CreditStringTable->GetStringTable();
    TableRef->EnumerateSourceStrings([&](const FString& Key, const FString& SourceString) -> bool
        {
            UTextBlock* NewLine = NewObject<UTextBlock>(this);
            if (NewLine)
            {
                NewLine->SetText(FText::FromString(SourceString));
                NewLine->SetJustification(ETextJustify::Center);
                CreditScrollBox->AddChild(NewLine);
            }
            return true;
        });

    CreditScrollBox->ScrollToStart();
}
void UEndingCreditWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    if (!CreditScrollBox) return;

    float CurrentOffset = CreditScrollBox->GetScrollOffset();
    float NewOffset = CurrentOffset + ScrollSpeed * DeltaTime;
    CreditScrollBox->SetScrollOffset(NewOffset);

    if (NewOffset >= CreditScrollBox->GetScrollOffsetOfEnd()) {
        UPRGameInstance* GI = Cast<UPRGameInstance>(GetGameInstance());
        if (GI) {
            GI->bHasPlayedIntroVideo = false;

            FName MainMenuLevelName = TEXT("MainMenuLevel");
            UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
        }
    }
}