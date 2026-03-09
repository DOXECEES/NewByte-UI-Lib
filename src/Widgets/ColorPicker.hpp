#pragma once

#include "IWidget.hpp"
#include "SpinBox.hpp"
#include "Button.hpp"

#include <Alghorithm.hpp>
#include <Array.hpp>

#include <Color.hpp>

#include "GlobalWidgetContext.hpp"

namespace Widgets
{

class SVArea : public IWidget
{
public:
    SVArea(const NbRect<int>& rect) : IWidget(rect)
    {
    }

    const char* getClassName() const override
    {
        return "SVArea";
    }

    bool hitTest(const NbPoint<int>& pos) override
    {
       bool isInside = rect.isInside(pos);
    
       return isInside;
    }

    bool hitTestClick(const NbPoint<int>& pos) noexcept override
    {
        bool isInside = rect.isInside(pos);
        if (isInside)
        {
            updateFromMouse(pos);
        }
        return isInside;
    }

    void onClick() override
    {
        dragging = true;
        nbui::GlobalWidgetContext::captureWidget(this);
    }

    void onRelease() noexcept override
    {
        IWidget::onRelease();
        dragging = false;
        nbui::GlobalWidgetContext::releaseWidget(this);
    }

    void onMouseMove(const NbPoint<int>& pos) noexcept override
    {
        updateFromMouse(pos);
    }

    void updateFromMouse(const NbPoint<int>& pos)
    { 
        float s = float(pos.x - rect.x) / float(rect.width);
        float v = 1.0f - float(pos.y - rect.y) / float(rect.height);

        s = nbstl::clamp(s, 0.0f, 1.0f);
        v = nbstl::clamp(v, 0.0f, 1.0f);

        onSVChanged.emit(s, v);
    }

    const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
    {
        int side = std::min(maxSize.width, maxSize.height);

        if (side <= 0)
        {
            side = 1;
        }

        measuredSize = {side, side};
        return measuredSize;
    }

    void layout(const NbRect<int>& rect) noexcept override
    {
        setRect(rect);
    }

public:
    Signal<void(float, float)> onSVChanged;

private:
    NbSize<int> measuredSize;

    bool dragging = false;
};


class HueBar : public IWidget
{
public: 

    HueBar(const NbRect<int>& rect) : IWidget(rect)
    {
    }

    const char* getClassName() const override
    {
        return "HueBar";
    }

    bool hitTest(const NbPoint<int>& pos) override
    {
        bool isInside = rect.isInside(pos);
        
        return isInside;
    }

    bool hitTestClick(const NbPoint<int>& pos) noexcept override
    {
        bool isInside = rect.isInside(pos);
        if (isInside)
        {
            updateFromMouse(pos);
        }
        return isInside;
    }

    void onMouseMove(const NbPoint<int>& pos) noexcept override
    {
        updateFromMouse(pos);
    }

    void onClick() override
    {
        dragging = true;
        nbui::GlobalWidgetContext::captureWidget(this);
    }

    void onRelease() noexcept override
    {
        IWidget::onRelease();
        dragging = false;
        nbui::GlobalWidgetContext::releaseWidget(this);
    }


    void updateFromMouse(const NbPoint<int>& pos)
    {

        float h = float(pos.y - rect.y) / float(rect.height);
        h = std::clamp(h, 0.0f, 1.0f);

        onHueChanged.emit(h * 360.0f);
    }

    const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
    {
        const int fixedWidth = 20;

        int height = maxSize.height;

        if (height <= 0)
        {
            height = 1;
        }

        measuredSize = {fixedWidth, height};
        return measuredSize;
    }

    void layout(const NbRect<int>& rect) noexcept override
    {
        setRect(rect);
    }

public:
    Signal<void(float)> onHueChanged;

private:
    NbSize<int> measuredSize;

    bool dragging = false;
};


class ColorBar : public IWidget
{
public:
    enum class Channel
    {
        Red,
        Green,
        Blue,
        Alpha
    };

public:
    ColorBar(
        const NbRect<int>& rect,
        Channel channel
    ) noexcept
        : IWidget(rect),
          channel(channel)
    {

    }

    const char* getClassName() const override
    {
        return "ColorBar";
    }

    Channel getChannel() const noexcept
    {
        return channel;
    }

    uint8_t getValue() const noexcept
    {
        return value;
    }

    void setValue(uint8_t val) noexcept
    {
        value = val;
        //onColorChanged.emit(channel, value);
    }

    bool hitTest(const NbPoint<int>& pos) override
    {
        bool inside = rect.isInside(pos);
        return inside;
    }

    bool hitTestClick(const NbPoint<int>& pos) noexcept override
    {
        bool isInside = rect.isInside(pos);
        if (isInside)
        {
            updateFromMouse(pos);
        }
        return isInside;
    }

    void onMouseMove(const NbPoint<int>& pos) noexcept override
    {
        updateFromMouse(pos);
    }

    void onClick() override
    {
        dragging = true;
        nbui::GlobalWidgetContext::captureWidget(this);
    }

    void onRelease() noexcept override
    {
        dragging = false;
        nbui::GlobalWidgetContext::releaseWidget(this);    
    }

    void updateFromMouse(const NbPoint<int>& pos)
    {
        float t = float(pos.x - rect.x) / float(rect.width);
        t = std::clamp(t, 0.0f, 1.0f);

        value = static_cast<int>(t * 255.0f);
        onColorChanged.emit(channel, value);
    }

    const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
    {
        measuredSize = {maxSize.width, 20};
        return measuredSize;
    }

    void layout(const NbRect<int>& rect) noexcept override
    {
        setRect(rect);
    }

public:
    Signal<void(Channel, int)> onColorChanged;

private:
    bool dragging = false;
    NbSize<int> measuredSize;

    Channel channel;
    uint8_t value;
};


class ColorPicker : public IWidget
{
public:

    static constexpr const char* CLASS_NAME = "ColorPicker";

    ColorPicker(const NbRect<int>& rect) : IWidget(rect)
    {
        disableHoverState(true);

        layoutChildren();

        subscribe(*svArea, &SVArea::onSVChanged, 
            [this](float s, float v)
            {
                hsv.saturation = s;
                hsv.value = v;

                emitColor();
            }
        );

        subscribe(*hueBar, &HueBar::onHueChanged,
            [this](float h)
            {
                hsv.hue = h;
                emitColor();
            }
        );


        subscribe(
            *redBar, &ColorBar::onColorChanged,
            [this](ColorBar::Channel channel, int value)
            {
                redColorSpinbox->setValue(value);
                nb::Color col = nb::Color::fromRgba(
                    redBar->getValue(), greenBar->getValue(), blueBar->getValue(), alphaBar->getValue()
                );
                hsv = col.toHsv();
            }
        );

        subscribe(
            *greenBar, &ColorBar::onColorChanged,
            [this](ColorBar::Channel channel, int value)
            {
                greenColorSpinbox->setValue(value);
                nb::Color col = nb::Color::fromRgba(
                    redBar->getValue(), greenBar->getValue(), blueBar->getValue(), alphaBar->getValue()
                );
                hsv = col.toHsv();
            }
        );

        subscribe(
            *blueBar, &ColorBar::onColorChanged,
            [this](ColorBar::Channel channel, int value)
            {
                blueColorSpinbox->setValue(value);
                nb::Color col = nb::Color::fromRgba(
                    redBar->getValue(), greenBar->getValue(), blueBar->getValue(), alphaBar->getValue()
                );
                hsv = col.toHsv();
            }
        );

          subscribe(
            *alphaBar, &ColorBar::onColorChanged,
            [this](ColorBar::Channel channel, int value)
            {
                alphaColorSpinbox->setValue(value);
                nb::Color col = nb::Color::fromRgba(
                    redBar->getValue(), greenBar->getValue(), blueBar->getValue(), alphaBar->getValue()
                );
                hsv = col.toHsv();
            }
        );

        okButton->setText(L"OK");
        closeButton->setText(L"Close");

   
        addChildrenWidget(svArea);
        addChildrenWidget(hueBar);
        addChildrenWidget(redBar);
        addChildrenWidget(greenBar);
        addChildrenWidget(blueBar);
        addChildrenWidget(alphaBar);

        addChildrenWidget(redColorSpinbox);
        addChildrenWidget(greenColorSpinbox);
        addChildrenWidget(blueColorSpinbox);
        addChildrenWidget(alphaColorSpinbox);
        
        addChildrenWidget(okButton);
        addChildrenWidget(closeButton);


        redColorSpinbox->bind(
            [this]()
            {
                return redBar->getValue();
            },
            [this](int value)
            {
                redBar->setValue(uint8_t(value));
            }
        );

        greenColorSpinbox->bind(
            [this]()
            {
                return greenBar->getValue();
            },
            [this](int value)
            {
                greenBar->setValue(uint8_t(value));
            }
        );

        blueColorSpinbox->bind(
            [this]()
            {
                return blueBar->getValue();
            },
            [this](int value)
            {
                blueBar->setValue(uint8_t(value));
            }
        );

        alphaColorSpinbox->bind(
            [this]()
            {
                return alphaBar->getValue();
            },
            [this](int value)
            {
                alphaBar->setValue(uint8_t(value));
            }
        );


        subscribe(*okButton, &Widgets::IWidget::onReleasedSignal, [this]()
        {
            onOkButtonPressed.emit(getColor());
        });

        subscribe(
            *closeButton, &Widgets::IWidget::onReleasedSignal,
            [this]()
        {
            onCancelButtonPressed.emit();
        });
    }

    const char* getClassName() const override
    {
        return "ColorPicker";
    }

    bool hitTest(const NbPoint<int>& pos) override
    {
        return rect.isInside(pos);
    }

    void setColor(const nb::Color& color)
    {
        hsv = color.toHsv();
        emitColor();
    }

    nb::Color getColor() const
    {
        return nb::Color::fromHsv(hsv);
    }

    SVArea* getSVArea()
    {
        return svArea.get();
    }

    HueBar* getHueBar()
    {
        return hueBar.get();
    }

    nb::HSV getHSV()
    {
        return hsv;
    }

    nbstl::Array<ColorBar*, 4> getRgbaBars()
    {
        return nbstl::Array<ColorBar*, 4>{
            redBar.get(),
            greenBar.get(),
            blueBar.get(),
            alphaBar.get()
        };
    }

    nbstl::Array<SpinBoxBase*, 4> getSpinboxes()
    {
        return nbstl::Array<SpinBoxBase*, 4>{
            redColorSpinbox.get(),
            greenColorSpinbox.get(),
            blueColorSpinbox.get(),
            alphaColorSpinbox.get()
        };
    }

    nbstl::Array<Button*, 2> getButtons()
    {
        return nbstl::Array<Button*, 2>{
            okButton.get(),
            closeButton.get()
        };
    }

   
    void onRelease() noexcept override
    {
        svArea->onRelease();
        hueBar->onRelease();

        redBar->onRelease();
        greenBar->onRelease();
        blueBar->onRelease();
        alphaBar->onRelease();


    }

    const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
    {
        const int hueWidth = 20;
        const int previewHeight = 25;

        const int outerPadding = 6;
        const int spacing = 6;
        const int spacingVertical = 6;

        const int rgbHeight = 30;
        const int rgbSpacing = 6;

        const int minSVSize = 120;

        int contentWidth = maxSize.width - outerPadding * 2;
        int contentHeight = maxSize.height - outerPadding * 2;

        int maxSVWidth = contentWidth - hueWidth - spacing;

        int maxSVHeight = maxSize.height - outerPadding * 2 - previewHeight - spacingVertical -
                          rgbHeight * 3 - rgbSpacing * 3;

        int svSize = std::max(minSVSize, std::min(maxSVWidth, maxSVHeight));

        if (svSize < 0)
        {
            svSize = minSVSize;
        }

        const int channelCount = 4;

        int totalHeight = svSize + previewHeight + spacingVertical + rgbHeight * channelCount +
                          rgbSpacing * (channelCount - 1) + outerPadding * 2;

        measuredSize.width = svSize + hueWidth + spacing + outerPadding * 2;

        measuredSize.height = totalHeight;

        return measuredSize;
    }

    void layout(const NbRect<int>& rect) noexcept override
    {
        setRect(rect);

        const int hueWidth = 30;
        const int previewHeight = 25;

        const int outerPadding = 6;
        const int spacing = 6;
        const int spacingVertical = 6;

        const int spinBoxWidth = 40;

        const int rgbHeight = 20;
        const int rgbSpacing = 6;

        NbRect<int> content = {
            rect.x + outerPadding, rect.y + outerPadding, rect.width - outerPadding * 2,
            rect.height - outerPadding * 2
        };

        int maxSVWidth = content.width - hueWidth - spacing;

        int maxSVHeight =
            content.height - previewHeight - spacingVertical - rgbHeight * 3 - rgbSpacing * 3;

        int svSize = std::max(0, std::min(maxSVWidth, maxSVHeight));

        NbRect<int> svRect = {content.x, content.y, svSize, svSize};

        NbRect<int> hueRect = {svRect.x + svRect.width + spacing, content.y, hueWidth, svSize};

        int y = svRect.y + svRect.height + spacingVertical;

        NbRect<int> previewRect = {content.x, y, content.width, previewHeight};

        y += previewHeight + rgbSpacing;

        NbRect<int> redRect = {content.x, y, int(content.width * 0.8f), rgbHeight};

        NbRect<int> redSpinboxRect = {content.x + redRect.width + spacing, y, content.width - redRect.width - spacing, rgbHeight};

        y += rgbHeight + rgbSpacing;

        NbRect<int> greenRect = {content.x, y, int(content.width * 0.8f), rgbHeight};

        NbRect<int> greenSpinboxRect = {content.x + greenRect.width + spacing, y, content.width - greenRect.width - spacing, rgbHeight};

        y += rgbHeight + rgbSpacing;

        NbRect<int> blueRect = {content.x, y, int(content.width * 0.8f), rgbHeight};

        NbRect<int> blueSpinboxRect = {content.x + blueRect.width + spacing, y, content.width - blueRect.width - spacing, rgbHeight};

        y += rgbHeight + rgbSpacing;

        NbRect<int> alphaRect = {content.x, y, int(content.width * 0.8f), rgbHeight};

        NbRect<int> alphaSpinboxRect = {
            content.x + alphaRect.width + spacing, y, content.width - alphaRect.width - spacing,
            rgbHeight
        };

         y += rgbHeight + rgbSpacing;

        NbRect<int> okButtonRect = {
             content.width - int(content.width * 0.4f),
            y, int(content.width * 0.2f),
            40
        };

        NbRect<int> closeButtonRect = {
            content.width - int(content.width * 0.2f) + spacing,
            y,
            int(content.width * 0.2f),
            40
        };

        
        svArea->layout(svRect);
        hueBar->layout(hueRect);
        redBar->layout(redRect);
        greenBar->layout(greenRect);
        blueBar->layout(blueRect);
        alphaBar->layout(alphaRect);

        redColorSpinbox->layout(redSpinboxRect);
        greenColorSpinbox->layout(greenSpinboxRect);
        blueColorSpinbox->layout(blueSpinboxRect);
        alphaColorSpinbox->layout(alphaSpinboxRect);

        okButton->layout(okButtonRect);
        closeButton->layout(closeButtonRect);
    }


private:
    void layoutChildren()
    {
        int hueWidth = 20;

        svArea->setRect({rect.x, rect.y, rect.width - hueWidth - 4, rect.height});

        hueBar->setRect({rect.x + rect.width - hueWidth, rect.y, hueWidth, rect.height});
    }

    void emitColor()
    {
        nb::Color rgb = nb::Color::fromHsv(hsv);
        nb::RGBA rgba = rgb.toRgba();

        redColorSpinbox->setValue(rgba.r);
        greenColorSpinbox->setValue(rgba.g);
        blueColorSpinbox->setValue(rgba.b);
        alphaColorSpinbox->setValue(rgba.alpha);


        onColorChanged.emit(rgb);
    }



public:
    Signal<void(const nb::Color&)> onColorChanged;
    Signal<void(const nb::Color&)> onOkButtonPressed;
    Signal<void()> onCancelButtonPressed;


private:
    NbSize<int> measuredSize;
    nb::HSV hsv;

    std::shared_ptr<SVArea> svArea = std::make_shared<SVArea>(NbRect<int>{0, 0, 0, 0});
    std::shared_ptr<HueBar> hueBar = std::make_shared<HueBar>(NbRect<int>{0, 0, 0, 0});

    std::shared_ptr<ColorBar> redBar = std::make_shared<ColorBar>(NbRect<int>{0, 0, 0, 0}, ColorBar::Channel::Red);
    std::shared_ptr<ColorBar> greenBar= std::make_shared<ColorBar>(NbRect<int>{0, 0, 0, 0}, ColorBar::Channel::Green);
    std::shared_ptr<ColorBar> blueBar= std::make_shared<ColorBar>(NbRect<int>{0, 0, 0, 0}, ColorBar::Channel::Blue);;
    std::shared_ptr<ColorBar> alphaBar= std::make_shared<ColorBar>(NbRect<int>{0, 0, 0, 0}, ColorBar::Channel::Alpha);

    std::shared_ptr<IntSpinBox> redColorSpinbox;
    std::shared_ptr<IntSpinBox> greenColorSpinbox;
    std::shared_ptr<IntSpinBox> blueColorSpinbox;
    std::shared_ptr<IntSpinBox> alphaColorSpinbox;

    std::shared_ptr<Button> okButton;
    std::shared_ptr<Button> closeButton;
};

};
