#ifndef NBUI_SRC_WIDGETS_SPACER_HPP
#define NBUI_SRC_WIDGETS_SPACER_HPP

#include <NbCore.hpp>

#include "Widgets/IWidget.hpp"

namespace Widgets
{

    class Spacer : public Widgets::IWidget
    {
    public:


        Spacer() noexcept;
        ~Spacer() noexcept override = default;

        NB_NON_COPYMOVABLE(Spacer);

        bool hitTest(const NbPoint<int>& pos) override;
        const char* getClassName() const override;
        
        const NbSize<int>& measure(const NbSize<int>& max) noexcept override;
        void layout(const NbRect<int>& rect) noexcept override;
        
        NB_NODISCARD const NbSize<int>& getSize() const noexcept;

    private:

        NbSize<int> size;
    };


};

#endif