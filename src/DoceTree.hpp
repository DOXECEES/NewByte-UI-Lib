// #ifndef SRC_DOCETREE_HPP
// #define SRC_DOCETREE_HPP

// #include <Windows.h>

// #include <vector>
// #include <stack>
// #include <array>

// #include "IWindow.hpp"


// enum class DocePlacement
// {
//     LEFT        = 0,
//     RIGHT       = 1,
//     BOT         = 2,
//     TOP         = 3,
//     CONTAINER   = 4,
// };

// class DoceNode
// {
// public:
//     DoceNode(DocePlacement placement)
//         :placement(placement)
//     {

//     }


// protected:
//     // IWindow*        window;
//     // DoceNode*       parent;
//     DocePlacement   placement;
// };

// class DoceContainer : public DoceNode
// {
//     friend class DoceTree;

// public:
//     DoceContainer() = default;

// private:
//     std::array<DoceNode*, 5> children;
// };

// class DoceWindow :  public DoceNode
// {
// public:
//     DoceWindow() = default;

// private:

// };

// class DoceTree
// {
// public:
   
//     enum class NodeType
//     {
//         LEAF,
//         SPLITTER,
//     };

//     DocePlacement invertPlacement(DocePlacement placement)
//     {
//         switch(placement)
//         {
//             case DocePlacement::LEFT:
//                 return DocePlacement::RIGHT;
//             case DocePlacement::RIGHT:
//                 return DocePlacement::LEFT;
//             case DocePlacement::BOT:
//                 return DocePlacement::TOP;
//             case DocePlacement::TOP:
//                 return DocePlacement::BOT;
//             default:
//                 return DocePlacement::LEFT;
//         }
//     }

//     // struct DoceTreeNode
//     // {

//     //     DoceTreeNode(IWindow* window, DocePlacement placement, NodeType nodeType, IWindow *parent = nullptr)
//     //         : window(window)
//     //         , placement(placement)
//     //         , parent(parent) 
//     //         , nodeType(nodeType)
//     //         , index(sIndex++)
//     //     {}

//     //     IWindow*        parent          = nullptr;
//     //     IWindow*        window          = nullptr;
//     //     DocePlacement   placement       = DocePlacement::LEFT;

//     //     DoceTreeNode*   left            = nullptr;
//     //     DoceTreeNode*   right           = nullptr;
//     //     NodeType        nodeType        = NodeType::LEAF;
        
//     //     float           splitRatio      = 0.5f;

//     //     int index;
//     //     inline static int sIndex = 0;
//     // };
    

// public:

    
//     DoceTree() = default;
//     DoceTree(IWindow* window)
//     {
//         root = new DoceContainer();
//     }


//     void addWindow(IWindow* wnd, DocePlacement placement, IWindow* parent = nullptr)
//     {
//         // left
//         if(placement != DocePlacement::CONTAINER)
//             return;

//         DoceNode* node = new DoceNode(placement);

//         DoceContainer* temp = root;
//         while(temp->children[(int)placement])
//         {
//             if(temp->children[(int)DocePlacement::CONTAINER] == nullptr)
//             {
//                 temp->children[(int)DocePlacement::CONTAINER] = new DoceContainer();
//             }
//             temp = static_cast<DoceContainer*>(temp->children[(int)DocePlacement::CONTAINER]); // garants that DocePlacement::CONTAINER contains DoceContainer*
//         }

//         temp->children[(int)placement] = node;

//         calculateWindowsSize();
//     }

//     bool isWindowInsideNode(IWindow *window)
//     {

//     }


//     void calculateWindowsSize()
//     {
//         DoceContainer* temp = root;
//         while(temp)
//         {
            
//         }
//     }

//     void calculateHeight()
//     {

//     }

//     void calculateWidth()
//     {

//     }

//     ~DoceTree() = default;

// private:
    

//     DoceContainer* root;
// };


// #endif