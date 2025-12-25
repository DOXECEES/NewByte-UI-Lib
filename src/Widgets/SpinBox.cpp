#include "SpinBox.hpp"
#include <string>
#include <Debug.hpp>
#include "Utils/Validator.hpp"

namespace Widgets
{
    SpinBox::SpinBox(int initialValue)
        : IWidget({}), value(initialValue)
    {
        createInternalWidgets();
        setupHandlers();
        clampValue();
        syncText();
    }

    void SpinBox::setStep(int s) noexcept
    {
        step = s;
    }

    void SpinBox::setValue(int v) noexcept
    {
        value = clamp(v);
        syncText();
    }

    void SpinBox::createInternalWidgets()
    {
        input = std::make_unique<TextEdit>(NbRect<int>{0, 0, 0, 0});
        input->addValidator(Utils::Validator().required().numericOnly());
        upButton = std::make_unique<Button>(NbRect<int>{0, 0, 0, 0});
        upButton->setText(L"\u25B4");
        downButton = std::make_unique<Button>(NbRect<int>{0, 0, 0, 0});
        downButton->setText(L"\u25BE");
        addChildrenWidget(input.get());
        addChildrenWidget(upButton.get());
        addChildrenWidget(downButton.get());
    }

    void SpinBox::setupHandlers()
    {
        input->onPressedSignal.connect([this]() {
            onInputEdited();
        });

        input->onUnfocusedSignal.connect([this]() {
            input->setIsCaretVisible(false);
        });
      
        upButton->onPressedSignal.connect([&]() {
            onUpClicked();
        });

        downButton->onPressedSignal.connect([&]() {
            onDownClicked();
        });
        
    }

    void SpinBox::syncText()
    {
        input->setData(std::to_wstring(value));
    }

    void SpinBox::onInputEdited()
    {
        std::wstring txt = input->getData();

        if (txt.empty())
        {
            value = minValue;
            syncText();
            return;
        }

        try
        {
            int parsed = std::stoi(txt);
            value = clamp(parsed);
        }
        catch (...)
        {
            syncText();
            return;
        }
        syncText();
    }

    void SpinBox::onUpClicked()
    {
        value = clamp(value + step);
        syncText();
    }

    void SpinBox::onDownClicked()
    {
        value = clamp(value - step);
        syncText();
    }

    const NbSize<int>& SpinBox::measure(const NbSize<int>& maxSize) noexcept
    {
        auto inputSize = input->measure(maxSize);
        int btnWidth = inputSize.height;       
        int width = inputSize.width + btnWidth;
        int height = inputSize.height;

        measuredSize = { width, height };
        return measuredSize;
    }

    void SpinBox::layout(const NbRect<int>& rect) noexcept
    {
        setRect(rect);

        NbRect<int> inputRect = rect;
        int btnWidth = rect.height;

        inputRect.width = rect.width - btnWidth;

        NbRect<int> upRect = { rect.x + inputRect.width, rect.y, btnWidth, rect.height / 2 };
        NbRect<int> downRect = { rect.x + inputRect.width, rect.y + rect.height / 2,
                                 btnWidth, rect.height / 2 };

        input->layout(inputRect);
        upButton->layout(upRect);
        downButton->layout(downRect);
    }

    void SpinBox::onTimer()
    {
        input->setFocused();
        input->onTimer();
        syncText();
    }

    void SpinBox::onButtonClicked(const wchar_t symbol, SpecialKeyCode code)
    {
        input->onSymbolButtonClicked(symbol);
    }

    bool SpinBox::hitTest(const NbPoint<int>& pos)
    {
        const auto& r = getRect();
        bool childHit = true;
        if (upButton->hitTest(pos))
        {
            upButton->setHover();
            childHit = false;
        }
        else
        {
            upButton->setDefault();
        }

        if (downButton->hitTest(pos))
        {
            downButton->setHover();
            childHit = false;
        }
        else
        {
            downButton->setDefault();
        }

        if (!childHit)
        {
            return childHit;
        }
        return r.isInside(pos);
    }

    bool SpinBox::hitTestClick(const NbPoint<int>& pos) noexcept
    {
        input->hitTestClick(pos);
        upButton->hitTestClick(pos);
        downButton->hitTestClick(pos);
        return false;
    }

}
