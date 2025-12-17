#include "DockTree.hpp"
// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

std::shared_ptr<DockNode> DockTree::insert(const std::shared_ptr<DockNode> &parent,
                      const std::shared_ptr<DockNode> &insertNode) noexcept
{
    DockPlacement childPlacement = insertNode->getPlacement();
    if (parent->getChild(childPlacement) == nullptr && parent->isContainer())
    {
        parent->setChild(static_cast<size_t>(childPlacement), insertNode);
        return parent;
    }
    else
    {
        std::shared_ptr<DockNode> parentOfParent = std::dynamic_pointer_cast<DockNode>(parent->getParent());
        NbRect<int> rc = parent->getRect();
        std::shared_ptr<DockNode> container = std::make_shared<DockContainer>(rc, parent->getPlacement(), parentOfParent);

        DockTree::replaceNode(parent, container);
        parent->setPlacement(DockPlacement::CENTER);

        this->insert(container, parent);
        this->insert(container, insertNode);

        return container;
    }

}

std::shared_ptr<DockNode> DockTree::find(const WindowInterface::IWindow *lookupWindow) noexcept
{
    if(!root) 
    {
        return nullptr;
    }

    std::stack<std::shared_ptr<DockNode>> nodes;
    nodes.push(root);
    
    while(!nodes.empty())
    {
        std::shared_ptr<DockNode> current = nodes.top();
        nodes.pop();

        if(current->isWindow())
        {
            std::shared_ptr<DockWindow> window = std::dynamic_pointer_cast<DockWindow>(current);
            if(window->getWindow() == lookupWindow)
            {
                return current;
            }   
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

    return nullptr;
}

