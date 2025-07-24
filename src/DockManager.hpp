#ifndef NBUI_SRC_DOCKMANAGER_HPP
#define NBUI_SRC_DOCKMANAGER_HPP

#include "WindowInterface/IWindow.hpp"
#include "DockTree.hpp"
#include <array>

class Splitter
{
public:

    Splitter() = delete;

    Splitter(DockNode* node1, DockNode* node2)
        : firstNode(node1)
        , secondNode(node2)
        , rect()
    {}

    void onMove(const NbPoint<int>& shift)
    {
        DockPlacement placement = firstNode->getPlacement();
        if (canMoveVertically(firstNode) && canMoveVertically(secondNode))
        {
            NbRect<int> firstRect = firstNode->window->getClientRect();
            NbRect<int> secondRect = secondNode->window->getClientRect();

            if (firstRect.x < secondRect.x) 
            {
                int minWidth = 50;
                int newFirstWidth = firstRect.width + shift.x;
                int newSecondWidth = secondRect.width - shift.x;

                if (newFirstWidth < minWidth || newSecondWidth < minWidth)
                    return; 

                firstRect.width = newFirstWidth;

                secondRect.x += shift.x;
                secondRect.width = newSecondWidth;
            }
            else
            {
                int minWidth = 50;
                int newSecondWidth = secondRect.width + shift.x;
                int newFirstWidth = firstRect.width - shift.x;

                if (newFirstWidth < minWidth || newSecondWidth < minWidth)
                    return;

                secondRect.width = newSecondWidth;

                firstRect.x += shift.x;
                firstRect.width = newFirstWidth;
            }

            firstNode->window->setClientRect(firstRect);
            secondNode->window->setClientRect(secondRect);
            update();

        }
        else
        {

        }
    }

    void update()
    {
        const NbRect<int>& firstRect = firstNode->window->getClientRect();
        const NbRect<int>& secondRect = secondNode->window->getClientRect();

        if (firstRect.x > secondRect.x)  // left - right
        {
            rect = {
               secondRect.x + secondRect.width - 5,
               secondRect.y,
               50,
               firstRect.height
            };
        }
        else
        {
            rect = {
                firstRect.x + firstRect.width - 5,
                firstRect.y,
                50,
                secondRect.height
            };
        }
    }

    bool hitTest(const NbPoint<int>& pos)
    {
        return pos.x >= rect.x && pos.x < rect.x + rect.width && pos.y >= rect.y && pos.y < rect.y + rect.height;
    }

private:
    bool canMoveVertically(DockNode* node)
    {
        DockPlacement placement = node->getPlacement();

        return placement == DockPlacement::CENTER
            || placement == DockPlacement::LEFT
            || placement == DockPlacement::RIGHT;
    }

    bool canMoveHorizontlly(DockNode* node)
    {
        DockPlacement placement = node->getPlacement();

        return placement == DockPlacement::CENTER
            || placement == DockPlacement::TOP
            || placement == DockPlacement::BOT;
    }

public:

    
    NbRect<int> rect            = {};

    DockNode*   firstNode       = nullptr;
    DockNode*   secondNode      = nullptr;

};

class DockManager
{
    
public:
    const std::array<float, 5> SCALE_FACTORS = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f }; 

    DockManager(WindowInterface::IWindow *window);

    void addWindow(WindowInterface::IWindow *parent, WindowInterface::IWindow *dockWindow, DockPlacement placement);

    void update();
    void onSize(const NbRect<int>& newRect);
    NbSize<int> getAvailableSize(DockNode* node, DockPlacement exclution);
    void resetAllClientSize(DockNode* root);
    void rescale(const NbSize<float>& scaleFactor);

    //DockTree* getTree() { return tree; }
    void addSplitter(DockNode* parent, DockNode* node);

    inline static std::vector<Splitter*>      splitterList;
private:
    WindowInterface::IWindow*   window  = nullptr;
    DockNode*                   tree    = nullptr;
};

#endif