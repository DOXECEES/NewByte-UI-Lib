#ifndef SRC_WINDOWINTERFACE_WINDOWSTATE_HPP
#define SRC_WINDOWINTERFACE_WINDOWSTATE_HPP

#include "../Core.hpp"
#include "WindowCore.hpp"

namespace WindowInterface
{
    struct FrameSize
    {
        int right   = 5;
        int left    = 5;
        int top     = 35;
        int bot     = 5;
    };

    class WindowState
    {
    public:
        WindowState() = default;

        void setColor(const NbColor& newColor);
        void setFrameColor(const NbColor& newFrameColor);
        void setSize(const NbSize<int>& newSize);
        void setTitle(const std::wstring& newTitle);
        void setFontColor(const NbColor& newFontColor);
        void setMinSize(const NbSize<int>& newMinSize);
        void setFrameSize(const FrameSize& newFrameSize);

        // TODO: change to private
    public:
        NbColor         color       = { 45, 45, 45 };
        NbColor         frameColor  = { 35, 35, 35 };
        NbSize<int>     size        = { 800, 600 };
        NbSize<int>     clientSize  = { size.width - frameSize.left - frameSize.right
                                      , size.height - frameSize.top - frameSize.bot }; // calculated only automatically
        std::wstring    title       = L"Ui-Lib";
        NbColor         fontColor   = { 255, 255, 255 };
        NbSize<int>     minSize     = { 400, 300 };
        
        FrameSize       frameSize;
    };

};

#endif