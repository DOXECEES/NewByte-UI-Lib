// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "DockManager.hpp"

#include <queue>
#include <stack>

DockManager::DockManager(WindowInterface::IWindow *window)
    : window(window)
{
    tree = DockNode::createContainerNode(window->getClientRect(), DockPlacement::CENTER);
}

void DockManager::addWindow(WindowInterface::IWindow *parent, WindowInterface::IWindow *dockWindow, DockPlacement placement)
{
    if(parent == nullptr)
    {
        tree->addChild(dockWindow, placement);
        addSplitter(tree, tree->getChild(placement));
    }
    else
    {
        DockNode* parentNode = tree->findNodeByWindow(parent);
        parentNode->addChild(dockWindow, placement);
    }

    update();
}

void DockManager::update()
{
    if (tree->isContainer())
    {
        const NbSize<int> centralWindowMinSize = { tree->rect.width / 2, tree->rect.height / 2 };

        if (tree->children[(int)DockPlacement::CENTER] != nullptr)
        {
            tree->children[(int)DockPlacement::CENTER]->window->setClientRect(tree->rect);
        }

        NbRect<int> newTopRect = tree->getChild(DockPlacement::TOP) ? tree->getChild(DockPlacement::TOP)->window->getClientRect() : NbRect<int>();
        NbRect<int> newBotRect = tree->getChild(DockPlacement::BOT) ? tree->getChild(DockPlacement::BOT)->window->getClientRect() : NbRect<int>();
        NbRect<int> newLeftRect = tree->getChild(DockPlacement::LEFT) ? tree->getChild(DockPlacement::LEFT)->window->getClientRect() : NbRect<int>();
        NbRect<int> newRightRect = tree->getChild(DockPlacement::RIGHT) ? tree->getChild(DockPlacement::RIGHT)->window->getClientRect() : NbRect<int>();
        NbRect<int> newCenterRect = tree->getChild(DockPlacement::CENTER) ? tree->getChild(DockPlacement::CENTER)->window->getClientRect() : NbRect<int>();


        for (int i = 0; i < (int)DockPlacement::NONE; i++)
        {
            if (!tree->children[i])
                continue;

            if (i == (int)DockPlacement::CENTER)
                continue;

            switch (i)
            {
            case (int)DockPlacement::LEFT:
            {
                const float LEFT_SCALE_FACTOR = 0.25f;
                const NbSize<int> minLeftPanelSize = { (int)((float)tree->rect.width * LEFT_SCALE_FACTOR), tree->rect.height };

                if (minLeftPanelSize.width < newCenterRect.width)
                {
                    newCenterRect.width -= minLeftPanelSize.width;
                    newLeftRect.width = minLeftPanelSize.width;
                    newLeftRect.height = minLeftPanelSize.height;

                    newCenterRect.x = newLeftRect.x + newLeftRect.width;
                }
                
                break;
            }

            case (int)DockPlacement::RIGHT:
            {
                const float RIGHT_SCALE_FACTOR = 0.25f;
                const NbSize<int> minRightPanelSize = { (int)((float)tree->rect.width * RIGHT_SCALE_FACTOR), tree->rect.height };

                if (minRightPanelSize.width < newCenterRect.width)
                {
                    newCenterRect.width -= minRightPanelSize.width;
                    newRightRect.width = minRightPanelSize.width;
                    newRightRect.height = minRightPanelSize.height;

                    newRightRect.x = newCenterRect.x + newCenterRect.width;
                }
                
                break;
            }

            default:
                break;
            }

            if (!newRightRect.isEmpty())    tree->children[(int)DockPlacement::RIGHT]->window->setClientRect(newRightRect);
            if (!newLeftRect.isEmpty())     tree->children[(int)DockPlacement::LEFT]->window->setClientRect(newLeftRect);
            if (!newTopRect.isEmpty())      tree->children[(int)DockPlacement::TOP]->window->setClientRect(newTopRect);
            if (!newBotRect.isEmpty())      tree->children[(int)DockPlacement::BOT]->window->setClientRect(newBotRect);
            if (!newCenterRect.isEmpty())   tree->children[(int)DockPlacement::CENTER]->window->setClientRect(newCenterRect);
        }

        for (auto& i : splitterList)
        {
            i->update();
        }
    }
}

void DockManager::onSize(const NbRect<int>& newRect)
{
    //resetAllClientSize(tree);
    const NbSize<float> scaleFactor = { (float)newRect.width / (float)tree->rect.width, (float)newRect.height / (float)tree->rect.height };
    tree->rect = newRect;
    rescale(scaleFactor);
    for (auto& i : splitterList)
    {
        i->update();
    }
    //update();
}

NbSize<int> DockManager::getAvailableSize(DockNode *node, DockPlacement exclution)
{
     if(node->isContainer())
     {
         NbSize<int> availiableSize = {node->rect.width, node->rect.height};
         for(int i = 0; i < (int)DockPlacement::NONE; i++)
         {
             if (i == (int)exclution)
             {
                 continue;
             }

             if(node->children[i] != nullptr)
             {
                 availiableSize -= node->children[i]->window->getClientSize();
             }
         }
         return availiableSize;
     }

    return {0, 0};
}
void DockManager::resetAllClientSize(DockNode* root)
{
    DockNode* currentNode = nullptr;
    std::stack<DockNode*> nodes;
    nodes.push(root);

    while (!nodes.empty())
    {
        currentNode = nodes.top();
        nodes.pop();

        if (currentNode->isWindow())
        {
            static WindowInterface::FrameSize frameSize = WindowInterface::FrameSize();
            currentNode->window->setClientRect({frameSize.left, frameSize.top, 0 , 0});
            continue;
        }

        currentNode->rect = {};
    
        for (int i = 0; i < 5; i++)
        {
            DockNode* child = currentNode->getChild((DockPlacement)i);
            if (child != nullptr)
            {
                nodes.push(child);
            }
        }
    }

   


}

void DockManager::rescale(const NbSize<float>& scaleFactor)
{
    if (tree->isContainer())
    {
        for (int i = 0; i < 5; i++)
        {
            DockNode* child = tree->getChild((DockPlacement)i);
            
            if (!child)
                continue;

            NbRect<int> rect = child->window->getClientRect();
            rect.scale(scaleFactor);
            child->window->setClientRect(rect);
        }
    }
}

void DockManager::addSplitter(DockNode* parent, DockNode* node)
{
    DockPlacement pl = node->getPlacement();
    
    if (pl == DockPlacement::LEFT)
    {
        if (parent->getChild(DockPlacement::CENTER) != nullptr)
        {
            splitterList.push_back(new Splitter(parent->getChild(DockPlacement::CENTER), node));
        }
        else if (parent->getChild(DockPlacement::RIGHT) != nullptr)
        {
            splitterList.push_back(new Splitter(parent->getChild(DockPlacement::RIGHT), node));
        }
    }
    else if (pl == DockPlacement::RIGHT)
    {
        if (parent->getChild(DockPlacement::CENTER) != nullptr)
        {
            splitterList.push_back(new Splitter(parent->getChild(DockPlacement::CENTER), node));
        }
        else if (parent->getChild(DockPlacement::LEFT) != nullptr)
        {
            splitterList.push_back(new Splitter(parent->getChild(DockPlacement::LEFT), node));
        }
    }
}
// void DockManager::update(DockNode* parent)
// {
//     const NbRect<int>& parentRect = parent->window->getClientRect();

//     for (int i = 0; i < (int)DockPlacement::NONE; i++)
//     {
//         if(parent->children[i] != nullptr)
//         {
//             parent->children[i]->window->setClientRect({parentRect.x, parentRect.y, int((float)parentRect.width * SCALE_FACTORS[i]), (int)((float)parentRect.height * SCALE_FACTORS[i])});
//             update(parent->children[i]);
//         }
//     }
// }

// void DockManager::updateTree()
// {
//     //bfs
//     std::queue<DockTree::DockNode*> nodes;
//     nodes.push(tree->getRoot());
    
//     const NbRect<int>& availiableRect = nodes.front()->window->getClientRect();


//     while(!nodes.empty())
//     {
//         DockTree::DockNode* node = nodes.front();
//         nodes.pop();

//         for(int i = 0; i < (int)DockPlacement::NONE; i++)
//         {
//             if(node->children[i] != nullptr)
//             {
//                 nodes.push(node->children[i]);
//             }
//         }

//     }
// }


