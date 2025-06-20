#ifndef SRC_WINDOWINTERFACE_WINDOWSTYLE_HPP
#define SRC_WINDOWINTERFACE_WINDOWSTYLE_HPP

namespace WindowInterface
{
    class WindowStyle
    {
    public:
        void setBorderRadius(const int radius) noexcept;
        int getBorderRadius() const noexcept;

    private:
        int borderRadius = 15;



    };
};

#endif