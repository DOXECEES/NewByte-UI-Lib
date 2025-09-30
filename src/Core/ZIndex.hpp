#ifndef NBUI_SRC_CORE_ZINDEX_HPP
#define NBUI_SRC_CORE_ZINDEX_HPP

#include <NbCore.hpp>

#include <cstdint>
#include <compare>


namespace Core
{
    // 4 HI bits represents layer type;
    // other 12 represents draw order inside layer
    // that means MAIN is lower than any other Ztype
    // But at the same time if we have 2 popups they must diff by 12 bits 
    // draw starts from 0 to uint16_t max (max value of zbuffer in one layer is 4096)
    // processing in acsending order

    class ZIndex
    {
    public:
        enum class ZType : uint16_t
        {
            MAIN = 0,
            WIDGET = 1,
            POPUP = 2,
            MENU = 3,
            // other reserved for a while
        };

        constexpr ZIndex(ZType type, uint16_t order) noexcept
            : index((static_cast<uint16_t>(type) << OFFSET_TYPE) | (order & ORDER_MASK))
        {
        }


        NB_NODISCARD constexpr ZIndex::ZType getType() const noexcept
        {
            return static_cast<ZType>((index >> OFFSET_TYPE) & TYPE_MASK_BITS);
        }


        NB_NODISCARD constexpr uint16_t getOrder() const noexcept
        {
            return index & ORDER_MASK;
        }


        NB_NODISCARD constexpr bool isLayer(ZType t) const noexcept
        {
            return getType() == t;
        }

        constexpr auto operator<=>(const ZIndex&) const noexcept = default;

    private:
        uint16_t                  index;

        static constexpr uint16_t ORDER_MASK        = 0x0FFF; 
        static constexpr uint16_t TYPE_MASK         = 0xF000; 
        static constexpr uint16_t TYPE_MASK_BITS    = 0x000F;
        static constexpr uint8_t  OFFSET_TYPE       = 12;
    };
};



#endif