#ifndef NBUI_SRC_WIDGETS_IWIDGET_HPP
#define NBUI_SRC_WIDGETS_IWIDGET_HPP

#include "../Core.hpp"
#include "../IIndexable.hpp"
#include "Signal.hpp"

#include "Core/ZIndex.hpp"
#include "WidgetSizePolicy.hpp"

#include "WidgetStyle.hpp"
#include "Theme.hpp"

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

    class IMeasureLayout
    {
    public:
        virtual const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept { return {}; };
        virtual void layout(const NbRect<int>& rect) noexcept {};
    };

    // INTERFACE JUST PEICE OF SHIT /
    // TODO: REWRITE
    // CHILDRENS DO NOT WORKS 
    class IWidget : public IIndexable, public IMeasureLayout
    {
    public:
    
        IWidget(const NbRect<int>& rect, const uint16_t zIndexOrder = 0) 
            : rect(rect)
            , zIndex(Core::ZIndex::ZType::WIDGET, zIndexOrder) {}
        virtual ~IWidget() = default;
        
        
        virtual void onClick() 
        {
            if (state == WidgetState::DISABLE)
            {
                return;
            }
            onPressedSignal.emit();
            //onClickCallback();
        };

        virtual void onRelease() noexcept
        {
            if (state == WidgetState::DISABLE)
            {
                return;
            }

            onReleasedSignal.emit();
        }

        virtual void onButtonClicked(const wchar_t symbol, SpecialKeyCode specialCode = SpecialKeyCode::NONE) {};
        virtual void onSymbolButtonClicked(const wchar_t symbol) {};
        virtual void onTimer() {};

        virtual bool hitTest(const NbPoint<int>& pos) = 0;
        virtual bool hitTestClick(const NbPoint<int>& pos) noexcept 
        {
            for (auto& children : childrens)
            {
                if (children->hitTest(pos))
                {
                    children->setFocused();
                    //Debug::debug("On unfocused in spinbox");
                }
                else
                {
                    children->setUnfocused();
                }
            }
            return false;
        } // temporary non abstract

        inline void setSize(const NbSize<int>& newSize) { rect.width = newSize.width; rect.height = newSize.height; isSizeChange = true; onSizeChangedSignal.emit(rect); }
        
        inline const NbRect<int>& getRect() const { return rect; }
		inline void setRect(const NbRect<int>& rect)
        {
			this->rect = rect;
            isSizeChange = true;
            onSizeChangedSignal.emit(rect);
		};

        
        virtual WidgetStyle& getStyle() noexcept 
        {
            return style;
        }

        virtual const WidgetStyle& getStyle() const noexcept
        {
            return style;
        }

        inline WidgetState getState() const noexcept { return state; }
        
        void show() noexcept;
        void hide() noexcept;

        inline void setHover() noexcept     { state = WidgetState::HOVER; }
        inline void setActive() noexcept    { state = WidgetState::ACTIVE; }
        inline void setDisable() noexcept   { state = WidgetState::DISABLE; }
        inline void setDefault() noexcept   { state = WidgetState::DEFAULT; }

        bool isHover() const noexcept;
		bool isActive() const noexcept;
		bool isDisable() const noexcept;
		bool isDefault() const noexcept;
        bool isHide() const noexcept;

        virtual const char* getClassName() const = 0;

        inline void setOnClickCallback(const std::function<void()>& onClickCallback) { this->onClickCallback = onClickCallback; }

        inline bool getIsFocused() const noexcept { return isFocused; }
        void setFocused() noexcept;
        void setUnfocused() noexcept;

        virtual NbRect<int> getRequestedSize() const noexcept;

        void addChildrenWidget(IWidget* widget) noexcept;
        NB_NODISCARD const std::vector<IWidget*>& getChildrens() const noexcept;
        
        NB_NODISCARD const Core::ZIndex& getZIndex() const noexcept;

        NB_NODISCARD void setSizePolicy(SizePolicy horiz, SizePolicy vert) noexcept
        {
            sizePolicy.horizontal = horiz;
            sizePolicy.vertical = vert;
        }

        NB_NODISCARD const WidgetSizePolicy& getSizePolicy() const noexcept
        {
            return sizePolicy;
        }

        virtual NbSize<int> computeContentSize() const noexcept {
            return {};
        }

        
    public:
        Signal<void(const NbRect<int>&)> onSizeChangedSignal;
        Signal<void()> onPressedSignal;
        Signal<void()> onReleasedSignal;
        Signal<void()> onUnfocusedSignal;
        Signal<void()> onFocusSignal;

    protected:


        std::vector<IWidget*>   childrens;
        NbRect<int>             rect                = { 0, 0, 0, 0 };

        Core::ZIndex            zIndex;

        std::function<void()>   onClickCallback;

        WidgetStyle             style               = ThemeManager::getCurrent().widgetStyle;
        WidgetSizePolicy        sizePolicy          = { SizePolicy::EXPANDING, SizePolicy::EXPANDING };
        WidgetState             state               = WidgetState::DEFAULT;
        
        bool                    isHover_            = false;
        bool                    isFocused           = false;
        bool                    isHide_             = false;
    
    public:
        bool isSizeChange = true;
    };

};

#endif