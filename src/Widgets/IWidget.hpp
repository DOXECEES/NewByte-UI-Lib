#ifndef NBUI_SRC_WIDGETS_IWIDGET_HPP
#define NBUI_SRC_WIDGETS_IWIDGET_HPP

#define DECLARE_WIDGET_CLASS_NAME(ClassName) constexpr static const char* CLASS_NAME = #ClassName

#include "../Core.hpp"
#include "../IIndexable.hpp"
#include "Signal.hpp"

#include "Core/ZIndex.hpp"
#include "WidgetSizePolicy.hpp"

#include "WidgetStyle.hpp"
#include "Theme.hpp"


#include <functional>
#include "MouseState.hpp"


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

        const NbSize<int> getMeasuredSize() const noexcept
        {
            return measuredSize;
        }

        void setMeasuredSize(const NbSize<int>& size) noexcept
        {
            measuredSize = size;
        }

    protected: 
        NbSize<int> measuredSize;

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
        
        
        virtual void onClick(); 
        virtual void onRelease() noexcept
        {
            if (state == WidgetState::DISABLE)
            {
                return;
            }

            onReleasedSignal.emit();
        }

        virtual void onUnfocus() noexcept
        {

        }

        virtual void onButtonClicked(const wchar_t symbol, SpecialKeyCode specialCode = SpecialKeyCode::NONE) {};
        virtual void onSymbolButtonClicked(const wchar_t symbol) {};
        virtual void onTimer() {};

        virtual void onMouseMove(const MouseState& pos) noexcept {};
        virtual void onMouseWheel(
            const NbPoint<int>& pos,
            int                 delta
        )
        {

        }

        virtual bool hitTest(const NbPoint<int>& pos) = 0;
        virtual bool hitTestClick(const NbPoint<int>& pos) noexcept 
        {
            for (auto it = childrens.rbegin(); it != childrens.rend(); ++it)
            {
                IWidget* child = (*it).get();

                if (child->isHide())
                {
                    continue;
                }

                if (child->hitTest(pos))
                {
                    if (child->hitTestClick(pos))
                    {
                        child->onClick();
                        return true;
                    }

                    return true;
                }
            }

            return false;
        } // temporary non abstract

        virtual bool hitTestRightClick(const NbPoint<int>& pos) noexcept
        {
            return false;
        };


        inline void setSize(const NbSize<int>& newSize) 
        {
            rect.width = newSize.width;
            rect.height = newSize.height; 
            isSizeChange = true; 
            onSizeChangedSignal.emit(rect);
        }
        
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
        void hide(bool flag) noexcept;

        bool isHover() const noexcept;
		bool isActive() const noexcept;
		bool isDisable() const noexcept;
		bool isDefault() const noexcept;
        bool isHide() const noexcept;

        void disableHoverState(bool flag) noexcept;
        bool isHoverStateDisable() const noexcept;

        virtual const char* getClassName() const = 0;

        inline void setOnClickCallback(const std::function<void()>& onClickCallback) { this->onClickCallback = onClickCallback; }

        inline bool getIsFocused() const noexcept { return isFocused; }
        void setFocused() noexcept;
        void setUnfocused() noexcept;

        virtual NbRect<int> getRequestedSize() const noexcept;

        void addChildrenWidget(std::shared_ptr<IWidget> widget) noexcept;
        NB_NODISCARD const std::vector<std::shared_ptr<IWidget>>& getChildrens() const noexcept;
        
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

        void addMenu(IWidget* menu) noexcept
        {
            contextMenu = menu;
        }

        bool hasMenu() noexcept
        {
            return contextMenu != nullptr;
        }

        IWidget* getMenu()
        {
            return contextMenu;
        }

        virtual void onRightClick(const NbPoint<int>& point) noexcept;
        


        
    public:
        Signal<void(const NbRect<int>&)> onSizeChangedSignal;
        Signal<void()> onPressedSignal;
        Signal<void()> onReleasedSignal;
        Signal<void()> onUnfocusedSignal;
        Signal<void()> onFocusSignal;
        std::function<void()> onClickCallback;

    protected:

        IWidget* contextMenu = nullptr;

        std::vector<std::shared_ptr<IWidget>> childrens;
        NbRect<int>             rect                = { 0, 0, 0, 0 };

        Core::ZIndex            zIndex;


        WidgetStyle             style               = ThemeManager::getCurrent().widgetStyle;
        WidgetSizePolicy        sizePolicy          = { SizePolicy::EXPANDING, SizePolicy::EXPANDING };
        WidgetState             state               = WidgetState::DEFAULT;
        
        bool                    isHover_            = false;
        bool                    isFocused           = false;
        bool                    isHide_             = false;

        bool                    isDisableHoverState = false;
    

    public:
        bool isSizeChange = true;
    };

};

#endif