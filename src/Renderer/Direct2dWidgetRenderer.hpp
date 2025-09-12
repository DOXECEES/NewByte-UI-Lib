#ifndef NBUI_SRC_RENDERER_DIRECT2DWIDGETRENDERER_HPP
#define NBUI_SRC_RENDERER_DIRECT2DWIDGETRENDERER_HPP

#include "Direct2dWrapper.hpp"

#include "Widgets/IWidgetRenderer.hpp"
#include "Widgets/TreeView.hpp"

#include "Debug.hpp"

namespace Renderer
{
    using namespace Widgets;
    class Direct2dWidgetRenderer : public IWidgetRenderer
    {
    public:
        Direct2dWidgetRenderer(Direct2dHandleRenderTarget* renderTarget);
        void render(IWidget* widget) override;
    
    private:

        void renderButton(IWidget* widget);
        void renderTextEdit(IWidget* widget);
        void renderTreeView(IWidget* widget);
        void renderLabel(IWidget* widget);

        template<typename T>
        T* castWidget(IWidget* widget) const noexcept
        {
            T* castWidget = dynamic_cast<T*>(widget);

            if (!castWidget)
            {
                Debug::debug("The widget name is already registered or the type could not be cast.");
                abort(); // temp 
            }

            return castWidget;
        }

        void createTextLayoutForWidget(IWidget* widget, const std::wstring& data = L"");

        void createTextLayoutForTreeView(Widgets::TreeView* treeView);

        Direct2dHandleRenderTarget* renderTarget = nullptr;

    };


};

#endif