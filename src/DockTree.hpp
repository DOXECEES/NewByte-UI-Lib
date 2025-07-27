#ifndef NBUI_SRC_DOCKTREE_HPP
#define NBUI_SRC_DOCKTREE_HPP

#include "WindowInterface/IWindow.hpp"

#include <array>
#include <unordered_set>

enum class DockPlacement
{
    LEFT        = 0,
    RIGHT       = 1,
    BOT         = 2,
    TOP         = 3,
    CENTER      = 4,
    NONE        = 5,
};

class DockNode
{
    enum class NodeType
    {
        CONTAINER,
        WINDOW
    };

public:
    DockNode() = default;
    DockNode(WindowInterface::IWindow* window, DockPlacement placement = DockPlacement::NONE)
    {
        this->window = window;
        this->placement = placement;
    }

    static DockNode* createContainerNode(const NbRect<int>& rect, DockPlacement placement)
    {
        DockNode* newNode = new DockNode(nullptr, placement);
        newNode->type = NodeType::CONTAINER;
        
        newNode->rect = { rect.x + 5, rect.y + 35, rect.width, rect.height };
        return newNode;
    }

    static DockNode* createWindowNode(WindowInterface::IWindow* window, DockPlacement placement)
    {
        DockNode* newNode = new DockNode(window, placement);
        newNode->type = NodeType::WINDOW;
        return newNode;

    }

    bool isContainer() const noexcept { return type == NodeType::CONTAINER; }
    bool isWindow() const noexcept { return type == NodeType::WINDOW; }

    DockNode* findNodeByWindow(WindowInterface::IWindow* lookupWindow)
    {
        //dfs

        if(this->isContainer())
        {
            for (int i = 0; i < (int)DockPlacement::NONE; i++)
            {
                if(children[i] != nullptr)
                {
                    if(children[i]->findNodeByWindow(lookupWindow) != nullptr)
                    {
                        return children[i];
                    }
                }
            }
        }

        return nullptr;
    }
    
    DockNode* addChild(WindowInterface::IWindow* window, DockPlacement placement)
    {
        if(this->isContainer())
        {
            children[(int)placement] = createWindowNode(window, placement);
        }
        else
        {
            //DockNode *newContainer = createContainerNode(this->window);
            //children[(int)placement] = newContainer->addChild(window, placement);

        }

        
        return children[(int)placement];
    }


    DockNode* getChild(DockPlacement placement) { return children[(int)placement]; }
    DockPlacement getPlacement() const noexcept { return placement; }


//private:
    NodeType                    type        = NodeType::CONTAINER;

    NbRect<int>                 rect        = {0, 0, 0, 0};

    WindowInterface::IWindow*   window      = nullptr;
    DockPlacement               placement   = DockPlacement::NONE;
    std::array<DockNode*, 5>    children    = { nullptr, nullptr, nullptr, nullptr, nullptr };
};

#endif