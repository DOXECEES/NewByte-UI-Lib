// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "LayoutBuilder.hpp"

#include <Error/ErrorManager.hpp>
#include "Layout.hpp"
#include "Widgets/IWidget.hpp"
#include "Widgets/Spacer.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/TreeView.hpp"
#include "Widgets/ToolBar.hpp"
#include <Color.hpp>

namespace nbui
{
    LayoutBuilder LayoutBuilder::widget(Widgets::IWidget* w)
    {
        LayoutBuilder b;
        b.node = std::make_unique<NNsLayout::LayoutWidget>(w);
        b.currentNode = b.node.get();
        return b;
    }

    LayoutBuilder LayoutBuilder::label(const std::wstring& text)
    {
        LayoutBuilder b;
        b.node = std::make_unique<NNsLayout::LayoutWidget>(new Widgets::Label(text));
        b.currentNode = b.node.get();
        return b;
    }

    LayoutBuilder LayoutBuilder::hBox()
    {
        LayoutBuilder b;
        b.node = std::make_unique<NNsLayout::HLayout>();
        b.currentNode = b.node.get();
        return b;
    }

    LayoutBuilder LayoutBuilder::vBox()
    {
        LayoutBuilder b;
        b.node = std::make_unique<NNsLayout::VLayout>();
        b.currentNode = b.node.get();
        return b;
    }

    LayoutBuilder LayoutBuilder::grid(int columns)
    {
        LayoutBuilder b;
        b.node = std::make_unique<NNsLayout::GridLayout>(columns);
        b.currentNode = b.node.get();
        return b;
    }

    LayoutBuilder LayoutBuilder::flow()
    {
        LayoutBuilder b;
        b.node = std::make_unique<NNsLayout::FlowLayout>();
        b.currentNode = b.node.get();
        return b;
    }

    LayoutBuilder LayoutBuilder::spacer()
    {
        LayoutBuilder b;
        b.node = std::make_unique<NNsLayout::LayoutWidget>(new Widgets::Spacer());
        b.currentNode = b.node.get();
        b.currentNode->style.widthSizeType = NNsLayout::SizeType::RELATIVE;
        b.currentNode->style.width = 1.0f;
        b.currentNode->style.heightSizeType = NNsLayout::SizeType::RELATIVE;
        b.currentNode->style.height = 1.0f;
        return b;
    }

    LayoutBuilder LayoutBuilder::toolbar()
    {
        LayoutBuilder b;
        auto tb = new Widgets::ToolBar();
        b.node = std::make_unique<NNsLayout::LayoutWidget>(tb);
        b.currentNode = b.node.get();
        b.currentNode->style.widthSizeType = NNsLayout::SizeType::RELATIVE;
        b.currentNode->style.width = 1.0f;
        b.currentNode->style.heightSizeType = NNsLayout::SizeType::ABSOLUTE;
        b.currentNode->style.height = 32.0f;

        b.currentNodeWidget = tb; 
        return b;
    }

    LayoutBuilder LayoutBuilder::treeView()
    {
        LayoutBuilder b;
        b.node = std::make_unique<NNsLayout::LayoutWidget>(new Widgets::TreeView({}));
        b.currentNode = b.node.get();
        return b;
    }

    LayoutBuilder&& LayoutBuilder::child(LayoutBuilder&& childBuilder)&&
    {
        if (currentNodeWidget)
        {
            if (auto widgetLayout = dynamic_cast<NNsLayout::LayoutWidget*>(childBuilder.node.get()))
            {
                currentNodeWidget->addChildrenWidget(widgetLayout->getWidget());
            }


            //return std::move(*this);
        }

        if (currentNode && childBuilder.node)
        {
            currentNode->addChild(std::move(childBuilder.node));
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::background(
        const NbColor& color,
        StateStyle stateStyle
    ) &&
    {
        if (currentNode && currentNode->getOwner())
        {
            WidgetStyle& style = currentNode->getOwner()->getStyle();
            switch (stateStyle)
            {
            case nbui::LayoutBuilder::StateStyle::ACTIVE:
                style.baseColor = color;
                break;
            case nbui::LayoutBuilder::StateStyle::HOVER:
                style.hoverColor = color;
                break;
            case nbui::LayoutBuilder::StateStyle::DISABLE:
                style.disableColor = color;
                break;
            default:
                break;
            }
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::color(const NbColor& color)&&
    {
        if (currentNode && currentNode->getOwner())
        {
            currentNode->getOwner()->getStyle().baseTextColor = color;
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::border(
        int width,
        Border::Style style,
        const NbColor& color
    )&&
    {
        if (currentNode)
        {
            currentNode->style.border = {
                .style = style,
                .width = width,
                .color = color 
            };
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::margin(const Margin<int>& margin)&&
    {
        if (currentNode) 
        {
            currentNode->style.margin = margin.to<float>();
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::padding(const Padding<int>& padding)&&
    {
        if (currentNode)
        {
            currentNode->style.padding = padding.to<float>();
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::relativeWidth(float w)&&
    {
        if (currentNode)
        {
            currentNode->style.widthSizeType = NNsLayout::SizeType::RELATIVE;
            currentNode->style.width = w;
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::relativeHeight(float h)&&
    {
        if (currentNode) 
        {
            currentNode->style.heightSizeType = NNsLayout::SizeType::RELATIVE;
            currentNode->style.height = h;
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::absoluteWidth(int w)&&
    {
        if (currentNode) {
            currentNode->style.widthSizeType = NNsLayout::SizeType::ABSOLUTE;
            currentNode->style.width = static_cast<float>(w);
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::absoluteHeight(int h)&&
    {
        if (currentNode)
        {
            currentNode->style.heightSizeType = NNsLayout::SizeType::ABSOLUTE;
            currentNode->style.height = static_cast<float>(h);
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::autoWidth() &&
    {
        if (currentNode)
        {
            currentNode->style.widthSizeType = NNsLayout::SizeType::AUTO;
            //currentNode->style.width = static_cast<float>(w);
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::autoHeight() &&
    {
        if (currentNode)
        {
            currentNode->style.heightSizeType = NNsLayout::SizeType::AUTO;
            // currentNode->style.width = static_cast<float>(w);
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::text(const std::wstring& t)&&
    {
        auto owner = currentNode->getOwner();
        if (auto w = dynamic_cast<Widgets::Button*>(owner)) 
        {
            w->setText(t);
        }
        else if (auto w = dynamic_cast<Widgets::Label*>(owner))
        {
            w->setText(t);
        }
        else if (auto w = dynamic_cast<Widgets::CheckBox*>(owner))
        {
            w->setText(t);
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::style(std::function<void(NNsLayout::LayoutStyle&)> f)&&
    {
        if (currentNode)
        {
            f(currentNode->style);
        }
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::checked(bool state)&&
    {
        nb::Error::ErrorManager::instance()
            .report(nb::Error::Type::INFO, "Checked not implemented");
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::fontSize(int size)&&
    {
        nb::Error::ErrorManager::instance()
            .report(nb::Error::Type::INFO, "Font size not implemented");
        return std::move(*this);
    }

    LayoutBuilder&& LayoutBuilder::textAlign(Widgets::TextAlign align)&&
    {
        nb::Error::ErrorManager::instance()
            .report(nb::Error::Type::INFO, "TextAlign not implemented");
        return std::move(*this);
    }

    std::unique_ptr<NNsLayout::LayoutNode> LayoutBuilder::build()&&
    {
        return std::move(node);
    }

    std::shared_ptr<Widgets::IWidget> LayoutBuilder::buildRawWidget()
    {
        if (auto widgetLayout = dynamic_cast<NNsLayout::LayoutWidget*>(currentNode))
        {
            return widgetLayout->getWidget();
        }
        else
        {
            nb::Error::ErrorManager::instance()
                .report(nb::Error::Type::WARNING, "Failed to build raw widget from ui");
            return nullptr;
        }
        
        
    }
}