#ifndef NBUI_SRC_WIDGETS_IWIDGET_HPP
#define NBUI_SRC_WIDGETS_IWIDGET_HPP

#include "../Core.hpp"
#include "../IIndexable.hpp"
#include "Signal.hpp"

#include "Core/ZIndex.hpp"

#include "WidgetStyle.hpp"

#include <functional>

namespace Widgets
{
    enum class WidgetState
    {
        DEFAULT,
        ACTIVE,
        DISABLE,
        HOVER,
        FOCUS,
    };


    class IWidget : public IIndexable
    {
    public:
    
        IWidget(const NbRect<int>& rect, const uint16_t zIndexOrder = 0) 
            : rect(rect)
            , zIndex(Core::ZIndex::ZType::WIDGET, zIndexOrder) {}
        virtual ~IWidget() = default;
        virtual void onClick() 
        {
            if(onClickCallback == nullptr || state == WidgetState::DISABLE) return;
            onClickCallback();
        };

        virtual void onButtonClicked(const wchar_t symbol, SpecialKeyCode specialCode = SpecialKeyCode::NONE) {};
        virtual void onSymbolButtonClicked(const wchar_t symbol) {};
        virtual void onTimer() {};

        virtual bool hitTest(const NbPoint<int>& pos) = 0;
        virtual bool hitTestClick(const NbPoint<int>& pos) noexcept { return false; } // temporary non abstractr

        inline void setSize(const NbSize<int>& newSize) { rect.width = newSize.width; rect.height = newSize.height; isSizeChange = true; onSizeChangedSignal.emit(rect); }
        
        inline const NbRect<int>& getRect() const { return rect; }
		inline void setRect(const NbRect<int>& rect) {
			this->rect = rect;
            isSizeChange = true;
            onSizeChangedSignal.emit(rect);
		};

        //inline const NbColor& getColor() const { return color; }
        //inline const NbColor& getHoverColor() const { return hoverColor; }
        
        inline const WidgetStyle& getStyle() const noexcept { return style; }
        inline WidgetState getState() const noexcept { return state; }
        
        inline void setHover() noexcept     { state = WidgetState::HOVER; }
        inline void setActive() noexcept    { state = WidgetState::ACTIVE; }
        inline void setDisable() noexcept   { state = WidgetState::DISABLE; }
        inline void setDefault() noexcept   { state = WidgetState::DEFAULT; }

        bool isHover() const noexcept;
		bool isActive() const noexcept;
		bool isDisable() const noexcept;
		bool isDefault() const noexcept;


        virtual const char* getClassName() const = 0;

        inline void setOnClickCallback(const std::function<void()>& onClickCallback) { this->onClickCallback = onClickCallback; }

        inline bool getIsFocused() const noexcept { return isFocused; }
        inline void setFocused() noexcept { isFocused = true; }
        inline void setUnfocused() noexcept { isFocused = false; }

        virtual NbRect<int> getRequestedSize() const noexcept;

        void addChildrenWidget(IWidget* widget) noexcept;
        NB_NODISCARD const std::vector<IWidget*>& getChildrens() const noexcept;
        
        
    public:
        Signal<void(const NbRect<int>&)> onSizeChangedSignal;

    protected:

        std::vector<IWidget*>   childrens;
        NbRect<int>             rect                = { 0, 0, 0, 0 };
        //NbColor                 color               = { 30, 30, 30 };
        //NbColor                 hoverColor          = { 51, 51, 51 };

        Core::ZIndex            zIndex;

        std::function<void()>   onClickCallback;

        WidgetStyle             style;

        WidgetState             state               = WidgetState::DEFAULT;
    // state
        bool                    isHover_             = false;
        bool                    isFocused           = false;
    
    public:
        bool isSizeChange = true;
    };

}

#endif