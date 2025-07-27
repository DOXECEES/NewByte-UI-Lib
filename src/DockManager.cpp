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
    if (!tree->isContainer()) return;

    // Инициализируем центральный прямоугольник
    NbRect<int> centerRect = tree->rect;

    // Сначала обрабатываем горизонтальные панели (TOP/BOT)
    if (auto topNode = tree->getChild(DockPlacement::TOP))
    {
        const float heightFactor = 0.2f;
        int height = max(50, static_cast<int>(centerRect.height * heightFactor));

        topNode->window->setClientRect({
            centerRect.x,
            centerRect.y,
            centerRect.width,
            height
            });

        centerRect.y += height;
        centerRect.height -= height;
    }

    if (auto botNode = tree->getChild(DockPlacement::BOT))
    {
        const float heightFactor = 0.2f;
        int height = max(50, static_cast<int>(centerRect.height * heightFactor));

        botNode->window->setClientRect({
            centerRect.x,
            centerRect.y + centerRect.height - height,
            centerRect.width,
            height
            });

        centerRect.height -= height;
    }

    // Затем обрабатываем вертикальные панели (LEFT/RIGHT)
    if (auto leftNode = tree->getChild(DockPlacement::LEFT))
    {
        const float widthFactor = 0.25f;
        int width = max(50, static_cast<int>(centerRect.width * widthFactor));

        leftNode->window->setClientRect({
            centerRect.x,
            centerRect.y,
            width,
            centerRect.height
            });

        centerRect.x += width;
        centerRect.width -= width;
    }

    if (auto rightNode = tree->getChild(DockPlacement::RIGHT))
    {
        const float widthFactor = 0.25f;
        int width = max(50, static_cast<int>(centerRect.width * widthFactor));

        rightNode->window->setClientRect({
            centerRect.x + centerRect.width - width,
            centerRect.y,
            width,
            centerRect.height
            });

        centerRect.width -= width;
    }

    // Центральная панель получает оставшееся пространство
    if (auto centerNode = tree->getChild(DockPlacement::CENTER))
    {
        centerNode->window->setClientRect(centerRect);
    }

    // Обновляем сплиттеры
    for (auto& splitter : splitterList) {
        splitter->update();
    }
}

void DockManager::onSize(const NbRect<int>& newRect)
{
    const NbSize<float> scaleFactor = { (float)newRect.width / (float)tree->rect.width, (float)newRect.height / (float)tree->rect.height };
    tree->rect = newRect;
    rescale(scaleFactor);
    for (auto& i : splitterList)
    {
        i->update();
    }
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
            static WindowInterface::FrameSize frameSize = WindowInterface::FrameSize();
            rect.x -= frameSize.left;
            rect.y -= frameSize.top;
            rect.scale(scaleFactor);
            rect.x += frameSize.left;
            rect.y += frameSize.top;
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


