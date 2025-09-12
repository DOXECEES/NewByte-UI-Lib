#ifndef NBUI_SRC_DOCKTREE_HPP
#define NBUI_SRC_DOCKTREE_HPP

#include "WindowInterface/IWindow.hpp"
#include "../../nbstl/src/NTree.hpp"

#include <array>
#include <unordered_set>

#include <Debug.hpp>

enum class DockPlacement
{
    LEFT        = 0,
    RIGHT       = 1,
    BOT         = 2,
    TOP         = 3,
    CENTER      = 4,
    NONE        = 5,
};




// class DockNode
// {
//     enum class NodeType
//     {
//         CONTAINER,
//         WINDOW
//     };

// public:
//     DockNode() = default;
//     DockNode(WindowInterface::IWindow* window, DockPlacement placement = DockPlacement::NONE)
//     {
//         this->window = window;
//         this->placement = placement;
//     }

//     static DockNode* createContainerNode(const NbRect<int>& rect, DockPlacement placement)
//     {
//         DockNode* newNode = new DockNode(nullptr, placement);
//         newNode->type = NodeType::CONTAINER;
        
//         newNode->rect = { rect.x + 5, rect.y + 35, rect.width, rect.height };
//         return newNode;
//     }

//     static DockNode* createWindowNode(WindowInterface::IWindow* window, DockPlacement placement)
//     {
//         DockNode* newNode = new DockNode(window, placement);
//         newNode->type = NodeType::WINDOW;
//         return newNode;

//     }

//     bool isContainer() const noexcept { return type == NodeType::CONTAINER; }
//     bool isWindow() const noexcept { return type == NodeType::WINDOW; }

//     DockNode* findNodeByWindow(WindowInterface::IWindow* lookupWindow)
//     {
//         //dfs

//         if(this->isContainer())
//         {
//             for (int i = 0; i < (int)DockPlacement::NONE; i++)
//             {
//                 if(children[i] != nullptr)
//                 {
//                     if(children[i]->findNodeByWindow(lookupWindow) != nullptr)
//                     {
//                         return children[i];
//                     }
//                 }
//             }
//         }

//         return nullptr;
//     }
    
//     DockNode* addChild(WindowInterface::IWindow* window, DockPlacement placement)
//     {
//         if(this->isContainer())
//         {
//             children[(int)placement] = createWindowNode(window, placement);
//         }
//         else
//         {
//             //DockNode *newContainer = createContainerNode(this->window);
//             //children[(int)placement] = newContainer->addChild(window, placement);

//         }

        
//         return children[(int)placement];
//     }


//     DockNode* getChild(DockPlacement placement) { return children[(int)placement]; }
//     DockPlacement getPlacement() const noexcept { return placement; }


// //private:
//     NodeType                    type            = NodeType::CONTAINER;
//     uint8_t                     countOfChilds   = 0;

//     NbRect<int>                 rect            = {0, 0, 0, 0};

//     WindowInterface::IWindow*   window          = nullptr;  
//     DockPlacement               placement       = DockPlacement::NONE;
//     std::array<DockNode*, 5>    children        = { nullptr, nullptr, nullptr, nullptr, nullptr };
// };


class DockNode : public nbstl::TreeNode<5>
{
public:

    explicit DockNode(const NbRect<int>& rect, DockPlacement placement, const std::shared_ptr<DockNode>& parent = nullptr)
        : rect(rect)
        , placement(placement)
        , TreeNode(parent)
    {}

    std::shared_ptr<DockNode> getChild(DockPlacement placementParam) const noexcept
    {
        if(childs[(size_t)placementParam] == nullptr)
            return nullptr;

        return std::dynamic_pointer_cast<DockNode>(childs[(size_t)placementParam]);
    }

    DockPlacement getPlacement() const noexcept { return placement; }
    void setPlacement(DockPlacement placement) noexcept { this->placement = placement; }
    const NbRect<int>& getRect() const noexcept { return rect; }
    void setRect(const NbRect<int>& rect) noexcept
    {
        this->rect = rect;
    }

    virtual bool isContainer() const noexcept = 0;
    virtual bool isWindow() const noexcept = 0;

    void calculate(DockPlacement placementParam) noexcept
    {
        NbSize<float> splitRatio = { 0.0f, 0.0f };

        if(std::shared_ptr<DockNode> children = getChild(DockPlacement::CENTER); children != nullptr)
        {

            if(getCurrentCountOfChilds() == 1)
            {
                splitRatio = { 1.0f, 1.0f };
                std::shared_ptr<DockNode> children = getChild(placementParam);
                if (children)
                {
                    children->setRect(rect);
                }

                return;
            }

            splitRatio = { 0.25f, 0.5f };
            
            NbRect<int> centralRect = children->getRect();
            NbRect<int> insertRect = centralRect;

            switch (placementParam)
            {
            case DockPlacement::LEFT:
            {
                insertRect.width *= splitRatio.width;
                centralRect.x += insertRect.width;
                centralRect.width = centralRect.width - insertRect.width;

                Debug::debug("LEFT: {");

                Debug::debug("CENTRAL: " 
                    + std::to_string(centralRect.x) 
                    + " " + std::to_string(centralRect.y) 
                    + " " + std::to_string(centralRect.width) 
                    + " " + std::to_string(centralRect.height));

                Debug::debug("INSERT: " 
                    + std::to_string(insertRect.x) 
                    + " " + std::to_string(insertRect.y) 
                    + " " + std::to_string(insertRect.width) 
                    + " " + std::to_string(insertRect.height));

                Debug::debug("}");
               
                break;
            }
            case DockPlacement::RIGHT:
            {
                centralRect.width *= 1 - splitRatio.width;
                insertRect.x += centralRect.width;
                insertRect.width = insertRect.width - centralRect.width;


                Debug::debug("RIGHT: {");

                Debug::debug("CENTRAL: " 
                    + std::to_string(centralRect.x) 
                    + " " + std::to_string(centralRect.y) 
                    + " " + std::to_string(centralRect.width) 
                    + " " + std::to_string(centralRect.height));

                Debug::debug("INSERT: " 
                    + std::to_string(insertRect.x) 
                    + " " + std::to_string(insertRect.y) 
                    + " " + std::to_string(insertRect.width) 
                    + " " + std::to_string(insertRect.height));

                Debug::debug("}");
               
                break;
            }
            case DockPlacement::BOT:
            {
                centralRect.height *= splitRatio.height;
                insertRect.y += centralRect.height;
                insertRect.height = insertRect.height - centralRect.height;

                Debug::debug("BOT: {");

                Debug::debug("CENTRAL: " 
                    + std::to_string(centralRect.x) 
                    + " " + std::to_string(centralRect.y) 
                    + " " + std::to_string(centralRect.width) 
                    + " " + std::to_string(centralRect.height));

                Debug::debug("INSERT: " 
                    + std::to_string(insertRect.x) 
                    + " " + std::to_string(insertRect.y) 
                    + " " + std::to_string(insertRect.width) 
                    + " " + std::to_string(insertRect.height));

                Debug::debug("}");
               
                break;
            }
            case DockPlacement::TOP:
            {


                insertRect.height *= splitRatio.height;
                centralRect.y += insertRect.height;
                centralRect.height = centralRect.height - insertRect.height;

                Debug::debug("TOP: {");

                Debug::debug("CENTRAL: " 
                    + std::to_string(centralRect.x) 
                    + " " + std::to_string(centralRect.y) 
                    + " " + std::to_string(centralRect.width) 
                    + " " + std::to_string(centralRect.height));

                Debug::debug("INSERT: " 
                    + std::to_string(insertRect.x) 
                    + " " + std::to_string(insertRect.y) 
                    + " " + std::to_string(insertRect.width) 
                    + " " + std::to_string(insertRect.height));

                Debug::debug("}");
               
                break;
            }
            default:
                return;
            }
            


            children->setRect(centralRect);
            std::shared_ptr<DockNode> insertChildren = getChild(placementParam);
            if (insertChildren)
            {
                insertChildren->setRect(insertRect);
            }

            
        }
        else
        {
            splitRatio = { 0.50f, 0.50f };
        }
        
    }

protected:
    NbRect<int>     rect      = {0, 0, 0, 0};
    DockPlacement   placement = DockPlacement::NONE;
    

};

class DockWindow : public DockNode
{
public:
    explicit DockWindow(WindowInterface::IWindow* window, DockPlacement placement, const std::shared_ptr<DockNode>& parent = nullptr)
        : DockNode(window->getClientRect(), placement, parent)
        , window(window)
    {}

    virtual bool isContainer() const noexcept override { return false; }
    virtual bool isWindow() const noexcept override { return true; }

    WindowInterface::IWindow* getWindow() const noexcept { return window; }

private:
    WindowInterface::IWindow* window = nullptr;


};

class DockContainer : public DockNode
{
public:
    explicit DockContainer(const NbRect<int>& rect, DockPlacement placement, const std::shared_ptr<DockNode>& parent = nullptr)
        : DockNode(rect, placement, parent)
    {}

    virtual bool isContainer() const noexcept override { return true; }
    virtual bool isWindow() const noexcept override { return false; }

};


class DockTree : public nbstl::NTree<float, 5, DockNode>
{
public:

    explicit DockTree(const NbRect<int>& rect)
        : NTree(std::make_shared<DockContainer>(rect, DockPlacement::NONE, nullptr))
    {
    }

	std::shared_ptr<DockNode> insert(const std::shared_ptr<DockNode>& parent, const std::shared_ptr<DockNode>& insertNode) noexcept override;
	void insert(const std::shared_ptr<DockNode>& parent, const std::shared_ptr<DockNode>& insertNode, const size_t position) noexcept override { };
	void removeNodeRecursively(const std::shared_ptr<DockNode>& removeNode) noexcept override { };
	void removeNode(const std::shared_ptr<DockNode>& removeNode) noexcept override { };
    std::shared_ptr<DockNode> find(const std::shared_ptr<DockNode> &lookupNode) const noexcept override { return nullptr; };
    std::shared_ptr<DockNode> find(const WindowInterface::IWindow* lookupWindow) noexcept;
	//template<typename Predicate>
	//virtual std::shared_ptr<Node>& find(Predicate predicate) const noexcept = 0;
	virtual void inOrderTraversal()		noexcept override {};
	virtual void preOrderTraversal()	noexcept override {};
	virtual void postOrderTraversal()	noexcept override {};
	virtual void levelOrderTraversal()	noexcept override {};

    std::shared_ptr<DockNode> getRoot() const noexcept { return root; }


    void calculateLayout(const std::shared_ptr<DockNode>& node)
    {
        if (!node || node->isWindow())
        {
            return;
        }




    }
      
        
       


};

#endif