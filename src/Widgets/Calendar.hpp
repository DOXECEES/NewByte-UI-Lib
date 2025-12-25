#ifndef NBUI_WIDGETS_CALENDAR_HPP
#define NBUI_WIDGETS_CALENDAR_HPP

#include "IWidget.hpp"
#include "../Localization/Calendars/ICalendar.hpp"
#include <string>
#include <vector>
#include <ctime>
#include <StdUtils.hpp>

namespace Widgets
{
    enum class CalendarViewMode { DAYS, MONTHS, YEARS };

    struct CalendarCell
    {
        Localization::CalendarDate date;
        bool isCurrentRange;
        bool isSelected;
        bool isToday;
        bool isHovered;
        NbRect<int> rect;
        std::wstring text;
    };

    class CalendarWidget : public IWidget
    {
    public:
        static constexpr const char* CLASS_NAME = "CalendarWidget";

        CalendarWidget(const NbRect<int>& rect, Localization::ICalendar* calendar, uint16_t zOrder = 0)
            : IWidget(rect, zOrder), calendar(calendar)
        {
            time_t now = time(nullptr);
            todayDate = calendar->timeToCalendarDate(now);
            viewDate = todayDate;
            selectedDate = todayDate;

            rebuildGrid();
        }

        virtual const char* getClassName() const override { return CLASS_NAME; }

        void goToNext()
        {
            if (viewMode == CalendarViewMode::DAYS)
            {
                if (++viewDate.month > 12)
                { 
                    viewDate.month = 1;
                    viewDate.year++;
                }
            }
            else if (viewMode == CalendarViewMode::MONTHS)
            {
                viewDate.year++;
            }
            else
            {
                viewDate.year += 10;
            }
            rebuildGrid();
        }

        void goToPrev()
        {
            if (viewMode == CalendarViewMode::DAYS)
            {
                if (--viewDate.month < 1)
                {
                    viewDate.month = 12;
                    viewDate.year--;
                }
            }
            else if (viewMode == CalendarViewMode::MONTHS)
            {
                viewDate.year--;
            }
            else
            {
                viewDate.year -= 10;
            }
            rebuildGrid();
        }

        virtual bool hitTest(const NbPoint<int>& pos) override
        {
            lastMousePos = pos;
            bool hit = rect.isInside(pos);
            if (hit)
            {
                for (auto& cell : m_grid) 
                {
                    cell.isHovered = cell.rect.isInside(pos);
                }
            }
            return hit;
        }

        virtual void onClick() override 
        {
            if (state == WidgetState::DISABLE) return;

            bool stateChanged = false;
            if (buttonPrevRect.isInside(lastMousePos)) 
            {
                goToPrev();
                stateChanged = true;
            }
            else if (buttonNextRect.isInside(lastMousePos))
            {
                goToNext();
                stateChanged = true;
            }
            else if (titleRect.isInside(lastMousePos)) 
            {
                if (viewMode == CalendarViewMode::DAYS)
                {
                    viewMode = CalendarViewMode::MONTHS;
                }
                else if (viewMode == CalendarViewMode::MONTHS)
                {
                    viewMode = CalendarViewMode::YEARS;
                }
                stateChanged = true;
            }
            else 
            {
                for (auto& cell : m_grid) 
                {
                    if (cell.rect.isInside(lastMousePos))
                    {
                        if (viewMode == CalendarViewMode::YEARS) 
                        {
                            viewDate.year = cell.date.year;
                            viewMode = CalendarViewMode::MONTHS;
                        }
                        else if (viewMode == CalendarViewMode::MONTHS)
                        {
                            viewDate.month = cell.date.month;
                            viewMode = CalendarViewMode::DAYS;
                        }
                        else 
                        {
                            selectedDate = cell.date;
                            onDateChangedSignal.emit(selectedDate);
                        }
                        stateChanged = true;
                        break;
                    }
                }
            }

            if (stateChanged)
            {
                rebuildGrid();
            }
            
            IWidget::onClick();
        }

        // --- Layout ---
        virtual void layout(const NbRect<int>& newRect) noexcept override 
        {
            setRect(newRect);
            rebuildGrid();
            isSizeChange = false;
        }

        void rebuildGrid() noexcept {
            m_grid.clear();
            if (rect.width <= 0 || rect.height <= 0)
            {
                return;
            }

            int headerH = rect.height / 8;
            buttonPrevRect = { rect.x, rect.y, headerH, headerH };
            buttonNextRect = { rect.x + rect.width - headerH, rect.y, headerH, headerH };
            titleRect = { rect.x + headerH, rect.y, rect.width - (headerH * 2), headerH };

            if (viewMode == CalendarViewMode::DAYS)
            {
                layoutDays(headerH);
            }
            else if (viewMode == CalendarViewMode::MONTHS)
            {
                layoutMonths(headerH);
            }
            else
            {
                layoutYears(headerH);
            }
        }

        const std::vector<CalendarCell>& getGrid() const { return m_grid; }
        CalendarViewMode getViewMode() const { return viewMode; }
        const NbRect<int>& getBtnPrevRect() const { return buttonPrevRect; }
        const NbRect<int>& getBtnNextRect() const { return buttonNextRect; }
        const NbRect<int>& getTitleRect() const { return titleRect; }

        std::wstring getHeaderText() const
        {
            if (viewMode == CalendarViewMode::DAYS)
            {
                return nbstl::toWString(calendar->getMonthKey(viewDate.year, viewDate.month)) + L" " + std::to_wstring(viewDate.year);
            }
            if (viewMode == CalendarViewMode::MONTHS)
            {
                return std::to_wstring(viewDate.year);
            }
            int start = (viewDate.year / 10) * 10;
            return std::to_wstring(start) + L" - " + std::to_wstring(start + 9);
        }

        std::wstring getDayName(int col) const { return nbstl::toWString(calendar->getDayOfWeekKey(col)); }
        int getWeekLength() const { return calendar->getDaysInWeek(); }

    public:
        Signal<void(const Localization::CalendarDate&)> onDateChangedSignal;

    private:
        void layoutDays(int headerH)
        {
            int dowH = rect.height / 12; 
            int daysInWeek = calendar->getDaysInWeek();
            int cellW = rect.width / daysInWeek;
            int gridTop = rect.y + headerH + dowH;
            int cellH = (rect.y + rect.height - gridTop) / 6;

            long firstDayFixed = calendar->dateToFixed(viewDate.year, viewDate.month, 1);
            int offset = calendar->getDayOfWeekIndex(firstDayFixed);
            long currentFixed = firstDayFixed - offset;

            for (int i = 0; i < (daysInWeek * 6); ++i) 
            {
                Localization::CalendarDate d = calendar->fixedToDate(currentFixed);
                CalendarCell cell;
                cell.date = d;
                cell.isCurrentRange = (d.month == viewDate.month);
                cell.isSelected = (d.year == selectedDate.year && d.month == selectedDate.month && d.day == selectedDate.day);
                cell.isToday = (d.year == todayDate.year && d.month == todayDate.month && d.day == todayDate.day);
                cell.isHovered = false;
                cell.text = std::to_wstring(d.day);

                int col = i % daysInWeek;
                int row = i / daysInWeek;
                cell.rect = { rect.x + (col * cellW), gridTop + (row * cellH), cellW, cellH };
                m_grid.push_back(cell);
                currentFixed++;
            }
        }

        void layoutMonths(int headerH) {
            // ѕолучаем реальное количество мес€цев дл€ текущего года
            int totalMonths = calendar->getMonthsInYear(viewDate.year);

            // –ассчитываем сетку. 
            // ƒл€ 12 мес€цев удобно 4 столбца и 3 р€да.
            // ƒл€ 13 мес€цев (≈врейский) Ч нужно либо 4 р€да, либо мен€ть плотность.
            int cols = 4;
            int rows = (totalMonths + cols - 1) / cols; // ќкругление вверх (дл€ 13 мес€цев будет 4 р€да)

            int cellW = rect.width / cols;
            int cellH = (rect.height - headerH) / rows;

            for (int i = 0; i < totalMonths; ++i) {
                CalendarCell cell;
                // ћес€цы в наших календар€х начинаютс€ с 1
                int monthNum = i + 1;

                cell.date = { viewDate.year, monthNum, 1 };
                cell.isCurrentRange = true;
                cell.isSelected = (selectedDate.year == viewDate.year && selectedDate.month == monthNum);
                cell.isToday = (todayDate.year == viewDate.year && todayDate.month == monthNum);
                cell.isHovered = false;

                // ѕолучаем правильный ключ (дл€ ≈врейского календар€ тут вернетс€ јдар I или II)
                cell.text = nbstl::toWString(calendar->getMonthKey(viewDate.year, monthNum));

                int col = i % cols;
                int row = i / cols;
                cell.rect = {
                    rect.x + col * cellW,
                    rect.y + headerH + row * cellH,
                    cellW,
                    cellH
                };

                m_grid.push_back(cell);
            }
        }

        void layoutYears(int headerH) 
        {
            int cellW = rect.width / 4;
            int cellH = (rect.height - headerH) / 3;
            int startYear = (viewDate.year / 10) * 10 - 1;
            for (int i = 0; i < 12; ++i)
            {
                int curYear = startYear + i;
                CalendarCell cell;
                cell.date = { curYear, 1, 1 };
                cell.isCurrentRange = (i > 0 && i < 11);
                cell.isSelected = (selectedDate.year == curYear);
                cell.isToday = (todayDate.year == curYear);
                cell.isHovered = false;
                cell.text = std::to_wstring(curYear);
                cell.rect = { rect.x + (i % 4) * cellW, rect.y + headerH + (i / 4) * cellH, cellW, cellH };
                m_grid.push_back(cell);
            }
        }

        

    private:
        Localization::ICalendar*    calendar;
        Localization::CalendarDate  viewDate;
        Localization::CalendarDate  selectedDate;
        Localization::CalendarDate  todayDate;
        CalendarViewMode            viewMode        = CalendarViewMode::DAYS;
        std::vector<CalendarCell>   m_grid;
        NbRect<int>                 buttonPrevRect;
        NbRect<int>                 buttonNextRect;
        NbRect<int>                 titleRect;
        NbPoint<int>                lastMousePos;
        NbSize<int>                 measuredSize    = { 0, 0 };
    };
}
#endif