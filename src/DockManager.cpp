// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "DockManager.hpp"

#include <queue>
#include <stack>

DockManager::DockManager(WindowInterface::IWindow *window)
    : window(window)
    , tree(new DockTree(window->getClientRect()))
{

}


void DockManager::show()
{
    // load from file

    //



}

void DockManager::addWindow(WindowInterface::IWindow *parent, WindowInterface::IWindow *dockWindow, DockPlacement placement)
{
    std::shared_ptr<DockNode> parentNode = tree->find(parent);
    
    
    if(parentNode == nullptr)
    {
        parentNode = tree->getRoot();
    }

    std::shared_ptr<DockNode> insertNode = std::make_shared<DockWindow>(dockWindow, placement, parentNode);
    std::shared_ptr<DockNode> parentOfInsertedNode = tree->insert(parentNode, insertNode);

    update(parentOfInsertedNode, placement);

    splitterManager.addSplitter(parentNode, insertNode);

}

void DockManager::update(const std::shared_ptr<DockNode>& parent, DockPlacement placement)
{
    // parent 
    // placement
    parent->calculate(placement);
    //tree->calculateLayout(tree->getRoot());

    std::stack<std::shared_ptr<DockNode>> nodes;
    nodes.push(tree->getRoot());
    
    while(!nodes.empty())
    {
        std::shared_ptr<DockNode> current = nodes.top();
        nodes.pop();

        if(current->isWindow())
        {
            std::shared_ptr<DockWindow> currentWindow = std::dynamic_pointer_cast<DockWindow>(current);
            currentWindow->getWindow()->setClientRect(currentWindow->getRect());
            Debug::debug(currentWindow->getWindow()->getTitle());
        }

        auto childs = current->getChilds();
        for (const auto& i : childs)
        {
            if (i)
            {
                nodes.push(std::dynamic_pointer_cast<DockNode>(i));
            }
        }
    }
}

void DockManager::onSize(const NbRect<int>& newRect)
{
    const NbRect<int> parentRect = tree->getRoot()->getRect();
    const NbSize<float> scaleFactor = { (float)newRect.width / (float)parentRect.width, (float)newRect.height / (float)parentRect.height };
    //tree->rect = newRect;
    rescale(scaleFactor);

    std::stack<std::shared_ptr<DockNode>> nodes;
    nodes.push(tree->getRoot());
    
    while(!nodes.empty())
    {
        std::shared_ptr<DockNode> current = nodes.top();
        nodes.pop();

        if(current->isWindow())
        {
            std::shared_ptr<DockWindow> currentWindow = std::dynamic_pointer_cast<DockWindow>(current);
            currentWindow->getWindow()->setClientRect(currentWindow->getRect());
            Debug::debug(currentWindow->getWindow()->getTitle());
        }

        auto childs = current->getChilds();
        for (const auto& i : childs)
        {
            if (i)
            {
                nodes.push(std::dynamic_pointer_cast<DockNode>(i));
            }
        }
    }
    // for (auto& i : splitterList)
    // {
    //     i->update();
    // }
}


void DockManager::rescale(const NbSize<float>& scaleFactor)
{
    if (!tree->getRoot()) return; 

    rescaleNode(tree->getRoot(), scaleFactor);
}

void DockManager::rescaleNode(const std::shared_ptr<DockNode>& node, const NbSize<float>& scaleFactor)
{
    if (!node) return;

    NbRect<int> origRect = node->getRect();

    NbRect<int> newRect = { };
    newRect.x = static_cast<int>(std::lround(origRect.x * scaleFactor.width));
    newRect.y = static_cast<int>(std::lround(origRect.y * scaleFactor.height));
    newRect.width  = static_cast<int>(std::lround(origRect.width * scaleFactor.width));
    newRect.height = static_cast<int>(std::lround(origRect.height * scaleFactor.height));

    node->setRect(newRect);

    if (node->isContainer())
    {
        for (auto& child : node->getChilds())
        {
            if (child)
            {
                auto c = std::static_pointer_cast<DockNode>(child);
                rescaleNode(c, scaleFactor);
            }
        }
    }
}



void DockManager::addSplitter(const std::shared_ptr<DockNode>& insertedNode)
{
    std::shared_ptr<DockNode> parent = std::dynamic_pointer_cast<DockNode>(insertedNode->getParent());

    for(const auto& child : parent->getChilds())
    {
        if(!child)
        {
            continue;
        }

        
    }
}

// SPLITTER CLASS

Splitter::Placement Splitter::toSplitterPlacement(DockPlacement placement) noexcept
{
    return static_cast<Splitter::Placement>(placement); 
}

[[nodiscard]] Splitter::Placement Splitter::getOpposite(Splitter::Placement placement) noexcept
{
    switch (placement)
    {
        case Placement::LEFT:
            return Placement::RIGHT;
        case Placement::RIGHT:
            return Placement::LEFT;
        case Placement::TOP:
            return Placement::BOT;
        case Placement::BOT:
            return Placement::TOP;
    }
}



void Splitter::linkNode(const std::shared_ptr<DockNode> &node) noexcept
{
    linkedNodes.push_back(node);
    recalculateRect();
}

void Splitter::recalculateRect() noexcept
{
    for(const auto& node : linkedNodes)
    {
        const NbRect<int> nodeRect = node->getRect();
        rect.x = (std::min)(rect.x, nodeRect.x);
        rect.y = (std::min)(rect.y, nodeRect.y);
        
        rect.width += nodeRect.width;
        rect.height += nodeRect.height;
    }
}

//