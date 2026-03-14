#ifndef NBUI_SRC_WIDGETS_TOOLBAR_HPP
#define NBUI_SRC_WIDGETS_TOOLBAR_HPP

#include "IWidget.hpp"
#include "Alghorithm.hpp"

namespace Widgets
{
    class ToolBar : public Widgets::IWidget
    {
    public:
        
        ToolBar() noexcept 
            : IWidget({})
        {

        }

        constexpr static const char* CLASS_NAME = "ToolBar";

        bool hitTest(const NbPoint<int>& pos) override;
        const char* getClassName() const override;

        void addToolBarWidget(std::shared_ptr<IWidget> widget, int size) noexcept;


        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            int totalWidth = 0;
            int maxHeight = 0;

            for (auto& child : childrens) {
                // Просим каждого ребенка измерить себя
                NbSize<int> childSize = child->measure(maxSize);
                totalWidth += childSize.width; 
                maxHeight = (nbstl::max)(maxHeight, childSize.height);
            }

            // Сохраняем и возвращаем результат
            measuredSize = { totalWidth, maxHeight };
            return measuredSize;
        }

        // РЕАЛИЗАЦИЯ РАССТАНОВКИ
        void layout(const NbRect<int>& rect) noexcept override
        {
            this->rect = rect; // Сохраняем свой размер
            
            int currentX = rect.x;

            for (auto& child : childrens) {
                // Берем размер, который ребенок насчитал в measure
                NbSize<int> childSize = child->getMeasuredSize();
                
                // Даем ребенку его прямоугольник внутри тулбара
                NbRect<int> childRect = { 
                    currentX, 
                    rect.y, 
                    childSize.width, 
                    rect.height // Кнопки обычно растягиваются по высоте тулбара
                };
                
                child->layout(childRect);
                
                // Сдвигаемся вправо для следующего элемента
                currentX += childSize.width;
            }
        }






    };

};


#endif