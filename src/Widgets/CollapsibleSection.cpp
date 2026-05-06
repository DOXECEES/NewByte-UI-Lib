//#include "CollapsibleSection.hpp"
//#include "Widgets/Button.hpp"
//
//namespace NNsLayout
//{
//    CollapsibleSection::CollapsibleSection(
//        std::shared_ptr<Widgets::IWidget> headerWidget,
//        std::unique_ptr<LayoutNode>       contentNode
//    ) noexcept
//        : LayoutWidget(
//              nullptr
//          ) // Сам по себе Section не рисует виджет, он управляет дочерними узлами
//        , m_contentLayout(std::move(contentNode))
//    {
//        // Создаем обертку для заголовка
//        m_headerLayout                      = std::make_shared<LayoutWidget>(headerWidget);
//        m_headerLayout->style.widthSizeType = SizeType::FLEX;
//
//        // Подписываемся на клик по заголовку (если заголовок — кнопка)
//        if (auto* btn = dynamic_cast<Widgets::Button*>(headerWidget.get()))
//        {
//            btn->onClickCallback = [this]()
//            {
//                this->toggle();
//            };
//        }
//
//        // Важно: контент должен знать, что мы его родитель
//        // Но мы не используем addChild, чтобы управлять measure/layout вручную
//    }
//
//    void CollapsibleSection::toggle() noexcept
//    {
//        setCollapsed(!m_collapsed);
//    }
//
//    void CollapsibleSection::setCollapsed(bool collapsed) noexcept
//    {
//        if (m_collapsed != collapsed)
//        {
//            m_collapsed = collapsed;
//            markDirty(); // Заставляем всю систему лейаутов пересчитать размеры
//        }
//    }
//
//    void CollapsibleSection::measure(const NbSize<int>& available) noexcept
//    {
//        // 1. Измеряем заголовок
//        m_headerLayout->measure(available);
//        NbSize<int> headerSize = m_headerLayout->getMeasuredSize();
//
//        int totalHeight = headerSize.height;
//        int maxWidth    = headerSize.width;
//
//        // 2. Если развернуто — измеряем контент
//        if (!m_collapsed && m_contentLayout)
//        {
//            m_contentLayout->measure(available);
//            NbSize<int> contentSize = m_contentLayout->getMeasuredSize();
//
//            totalHeight += contentSize.height;
//            maxWidth = (std::max)(maxWidth, contentSize.width);
//        }
//
//        measuredSize = {maxWidth, totalHeight};
//    }
//
//    void CollapsibleSection::layout(const NbRect<int>& bounds) noexcept
//    {
//        layoutRect = bounds;
//
//        // Размещаем заголовок сверху
//        NbSize<int> headerSize = m_headerLayout->getMeasuredSize();
//        NbRect<int> headerRect = {bounds.x, bounds.y, bounds.width, headerSize.height};
//        m_headerLayout->layout(headerRect);
//
//        // Размещаем контент под заголовком
//        if (m_contentLayout)
//        {
//            if (m_collapsed)
//            {
//                // Если свернуто, даем нулевой прямоугольник (или просто не вызываем layout)
//                m_contentLayout->layout({0, 0, 0, 0});
//            }
//            else
//            {
//                NbRect<int> contentRect = {
//                    bounds.x, bounds.y + headerSize.height, bounds.width,
//                    bounds.height - headerSize.height
//                };
//                m_contentLayout->layout(contentRect);
//            }
//        }
//    }
//} // namespace NNsLayout