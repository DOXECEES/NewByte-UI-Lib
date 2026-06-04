#pragma once

#include "IWidget.hpp"
#include <memory>
#include <vector>

namespace Widgets
{
    class Node : public IWidget
    {
    public:
        DECLARE_WIDGET_CLASS_NAME(Node);

         Node(
            const std::string& title,
            const NbRect<int>& rect
        ) noexcept
            : IWidget(rect)
            , m_title(title)
        {
        }

        bool hitTest(const NbPoint<int>& pos) override
        {
            return rect.isInside(pos);
        }

        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        const NbSize<int>& measure(const NbSize<int>& available) noexcept override
        {
            // Return the current size of the node
            m_measuredSize = {rect.width, rect.height};
            return m_measuredSize;
        }

        void layout(const NbRect<int>& bounds) noexcept override
        {
            rect = bounds;
        }

        const std::string& getTitle() const noexcept
        {
            return m_title;
        }

        void setPosition(const NbPoint<int>& pos) noexcept
        {
            rect.x = pos.x;
            rect.y = pos.y;
        }

    private:
        std::string m_title;
        NbSize<int> m_measuredSize{150, 80};
    };
}



namespace Widgets
{
    class ShaderCanvas : public IWidget
    {
    public:
        DECLARE_WIDGET_CLASS_NAME(ShaderCanvas);

        ShaderCanvas() noexcept : IWidget({})
        {
        }

        bool hitTest(const NbPoint<int>& pos) override
        {
            return rect.isInside(pos);
        }

        bool hitTestClick(const NbPoint<int>& pos) noexcept override
        {
            bool inside = rect.isInside(pos);
            if (inside)
            {
                // Hit test nodes in reverse order to find the top-most clicked node
                for (auto it = m_nodes.rbegin(); it != m_nodes.rend(); ++it)
                {
                    if ((*it)->hitTest(m_lastLocalMousePos))
                    {
                        m_draggedNode = it->get();

                        const auto& nodeRect = m_draggedNode->getRect();
                        m_dragOffset.x       = m_lastLocalMousePos.x - nodeRect.x;
                        m_dragOffset.y       = m_lastLocalMousePos.y - nodeRect.y;
                        return true;
                    }
                }

                addNode("Mew", pos);     
            }
            return inside;
        }


        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        const NbSize<int>& measure(const NbSize<int>& available) noexcept override
        {
            return {400, 400};
        }

        void layout(const NbRect<int>& bounds) noexcept override
        {
            rect = bounds;

            // Optional: If nodes are children in a layout hierarchy,
            // you would call node->layout() here.
        }

        // Add a new node to the canvas
        void addNode(
            const std::string&  title,
            const NbPoint<int>& position
        )
        {
            // Create a rectangle from the point with a default size (e.g., 150x80)
            NbRect<int> nodeRect;
            nodeRect.x      = position.x;
            nodeRect.y      = position.y;
            nodeRect.width  = 150;
            nodeRect.height = 80;

            m_nodes.push_back(std::make_shared<Node>(title, nodeRect));
        }

        // Retrieve the list of nodes for rendering or interaction
        const std::vector<std::shared_ptr<Node>>& getNodes() const noexcept
        {
            return m_nodes;
        }

        // Call this when a mouse button is pressed
        void onMouseMove(const MouseState& mouseState) noexcept override
        {
            // Translate global/window mouse positions to local canvas coordinates.
            // Adjust 'mouseState.position' to match whatever property name your MouseState struct
            // uses.
            m_lastLocalMousePos = {mouseState.position.x - rect.x, mouseState.position.y - rect.y};

            // If a node is active, update its position relative to the initial drag offset
            if (m_draggedNode)
            {
                NbRect<int> nodeRect = m_draggedNode->getRect();
                nodeRect.x           = m_lastLocalMousePos.x - m_dragOffset.x;
                nodeRect.y           = m_lastLocalMousePos.y - m_dragOffset.y;
                m_draggedNode->setRect(nodeRect);
            }
        }

        void onClick() override
        {
            // Call the base click callback if one is registered
            if (onClickCallback)
            {
                onClickCallback();
            }

            
        }

        void onRelease() noexcept override
        {
            // Stop dragging
            m_draggedNode = nullptr;

            // Carry out standard base releases (triggers onReleasedSignal)
            IWidget::onRelease();
        }


    private:
        std::vector<std::shared_ptr<Node>> m_nodes;
        Node*                              m_draggedNode = nullptr;
        NbPoint<int>                       m_dragOffset{0, 0};
        NbPoint<int>                       m_lastLocalMousePos{0, 0};


    };
} // namespace Widgets