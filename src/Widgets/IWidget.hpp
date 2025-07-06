#ifndef SRC_WIDGETS_IWIDGET_HPP
#define SRC_WIDGETS_IWIDGET_HPP

#include "../Core.hpp"
#include "../IIndexable.hpp"

#include <functional>

namespace Widgets
{
    class IWidget : public IIndexable
    {
    public:
        
        IWidget(NbRect<int> rect) : rect(rect) {}
        virtual ~IWidget() = default;
        virtual void onClick() 
        {
            if(onClickCallback == nullptr) return;
            onClickCallback();
        };

        virtual void onButtonClicked(const wchar_t symbol) {};
        virtual void onSymbolButtonClicked(const wchar_t symbol) {};
        virtual bool hitTest(const NbPoint<int>& pos) = 0;

        inline void setSize(const NbSize<int>& newSize) { rect.width = newSize.width; rect.height = newSize.height; }
        
        inline const NbRect<int>& getRect() const { return rect; }
        inline void setRect(const NbRect<int> &rect) { this->rect = rect; };

        inline const NbColor& getColor() const { return color; }
        inline const NbColor& getHoverColor() const { return hoverColor; }
        
        inline const bool getIsHover() const { return isHover; }
        inline void setIsHover(const bool isHover) { this->isHover = isHover; }

        virtual const char* getClassName() const = 0;

        inline void setOnClickCallback(const std::function<void()>& onClickCallback) { this->onClickCallback = onClickCallback; }

        inline bool getIsFocused() const noexcept { return isFocused; }
        inline void setFocused() noexcept { isFocused = true; }
        inline void setUnfocused() noexcept { isFocused = false; }
        
    protected:

        NbRect<int>             rect                = { 0, 0, 0, 0 };
        NbColor                 color               = { 200, 200, 200 };
        NbColor                 hoverColor          = { 255, 255, 255 };

        std::function<void()>   onClickCallback;

    // state
        bool                    isHover             = false;
        bool                    isFocused           = false;
    };
}

#endif