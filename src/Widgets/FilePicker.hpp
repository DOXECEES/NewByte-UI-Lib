#pragma once

#include "Button.hpp"
#include "IWidget.hpp"
#include "Label.hpp"    // Предполагаемый заголовок для текста
#include "ListView.hpp" // Предполагаемый заголовок для списка
#include "TextEdit.hpp"  // Предполагаемый заголовок для текстового поля

#include <Alghorithm.hpp>
#include <Array.hpp>
#include <String.hpp> // Предположим, у вас есть свой nbstl::String или используем std::string
#include <filesystem> // Для работы с файловой системой

#include "GlobalWidgetContext.hpp"
#include "MouseState.hpp"
#include "Utils.hpp"

namespace Widgets
{

    class FilePicker : public IWidget
    {
    public:
        static constexpr const char* CLASS_NAME = "FilePicker";

        FilePicker(const NbRect<int>& rect) : IWidget(rect)
        {
            disableHoverState(true);

            // Инициализация кнопок
            openButton->setText(L"Open");
            cancelButton->setText(L"Cancel");
            upButton->setText(L"Up");

            // Добавление дочерних элементов
            addChildrenWidget(pathTextBox);
            addChildrenWidget(upButton);
            addChildrenWidget(fileListView);
            addChildrenWidget(fileNameTextBox);
            addChildrenWidget(openButton);
            addChildrenWidget(cancelButton);

            // Подписка на события

            // Кнопка "Вверх" (переход в родительскую директорию)
            subscribe(
                *upButton, &Widgets::IWidget::onReleasedSignal,
                [this]()
                {
                    std::filesystem::path current(currentPath);
                    if (current.has_parent_path())
                    {
                        refreshDirectory(current.parent_path().string());
                    }
                }
            );

            subscribe(
                *fileListView, &Widgets::ListView::onItemSelectedSignal,
                [this](int index)
                {
                    std::string rawName = fileListView->getItemText(index);

                    std::string cleanName = (rawName.size() > 6) ? rawName.substr(6) : rawName;

                    std::filesystem::path fullPath = std::filesystem::path(currentPath) / cleanName;

                    if (std::filesystem::is_directory(fullPath))
                    {
                        refreshDirectory(fullPath.string());
                    }
                    else
                    {
                        fileNameTextBox->setData(Utils::toWstring(cleanName));
                    }
                }
            );


            subscribe(
                *openButton, &Widgets::IWidget::onReleasedSignal,
                [this]()
                {
                    std::string finalPath =
                        (std::filesystem::path(currentPath) / fileNameTextBox->getData()).string();
                    onFileSelected.emit(finalPath);
                }
            );

            subscribe(
                *cancelButton, &Widgets::IWidget::onReleasedSignal,
                [this]()
                {
                    onCancelButtonPressed.emit();
                }
            );

            // Установка начальной директории
            refreshDirectory(std::filesystem::current_path().string());
        }

        const char* getClassName() const override
        {
            return CLASS_NAME;
        }

        bool hitTest(const NbPoint<int>& pos) override
        {
            return rect.isInside(pos);
        }

        void refreshDirectory(const std::string& path)
        {
            currentPath = path;
            pathTextBox->setData(Utils::toWstring(path));
            fileListView->clear();
            fileNameTextBox->setData(L"");

            try
            {
                for (const auto& entry : std::filesystem::directory_iterator(path))
                {
                    std::string prefix = entry.is_directory() ? "[DIR] " : "      ";
                    fileListView->addItem(prefix + entry.path().filename().string());
                }
            }
            catch (...)
            {
                // Обработка ошибок доступа
            }
        }

        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            // Минимальные желаемые размеры
            measuredSize.width  = std::clamp(400, 0, maxSize.width);
            measuredSize.height = std::clamp(500, 0, maxSize.height);
            return measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept override
        {
            setRect(rect);

            const int padding         = 10;
            const int spacing         = 5;
            const int topBarHeight    = 30;
            const int bottomBarHeight = 40;
            const int buttonWidth     = 80;

            // 1. Верхняя панель: Путь + Кнопка "Вверх"
            NbRect<int> upBtnRect = {
                rect.x + rect.width - padding - 40, rect.y + padding, 40, topBarHeight
            };
            NbRect<int> pathRect = {
                rect.x + padding, rect.y + padding, rect.width - padding * 2 - 40 - spacing,
                topBarHeight
            };

            // 2. Нижняя панель: Имя файла + Кнопки OK/Cancel
            int bottomY = rect.y + rect.height - padding - bottomBarHeight;

            NbRect<int> cancelBtnRect = {
                rect.x + rect.width - padding - buttonWidth, bottomY, buttonWidth, bottomBarHeight
            };

            NbRect<int> openBtnRect = {
                cancelBtnRect.x - spacing - buttonWidth, bottomY, buttonWidth, bottomBarHeight
            };

            NbRect<int> fileNameRect = {
                rect.x + padding, bottomY, openBtnRect.x - rect.x - padding - spacing,
                bottomBarHeight
            };

            int         listY    = pathRect.y + pathRect.height + spacing;
            NbRect<int> listRect = {
                rect.x + padding, listY, rect.width - padding * 2, fileNameRect.y - listY - spacing
            };

            // Применяем Layout
            pathTextBox->layout(pathRect);
            upButton->layout(upBtnRect);
            fileListView->layout(listRect);
            fileNameTextBox->layout(fileNameRect);
            openButton->layout(openBtnRect);
            cancelButton->layout(cancelBtnRect);
        }

        
        std::shared_ptr<TextEdit> getPathTextBox()
        {
            return pathTextBox;
        }
        std::shared_ptr<TextEdit> getFileNameTextBox()
        {
            return fileNameTextBox;
        }
        std::shared_ptr<Button> getUpButton()
        {
            return upButton;
        }
        std::shared_ptr<Button> getOpenButton()
        {
            return openButton;
        }
        std::shared_ptr<Button> getCancelButton()
        {
            return cancelButton;
        }
        std::shared_ptr<ListView> getFileListView()
        {
            return fileListView;
        }


    public:
        Signal<void(const std::string&)> onFileSelected;
        Signal<void()>                   onCancelButtonPressed;

    private:
        NbSize<int> measuredSize;
        std::string currentPath;

        std::shared_ptr<TextEdit> pathTextBox = std::make_shared<TextEdit>();
        std::shared_ptr<Button>  upButton    = std::make_shared<Button>();

        std::shared_ptr<ListView> fileListView = std::make_shared<ListView>();

        std::shared_ptr<TextEdit> fileNameTextBox = std::make_shared<TextEdit>();
        std::shared_ptr<Button>  openButton      = std::make_shared<Button>();
        std::shared_ptr<Button>  cancelButton    = std::make_shared<Button>();
    };

} // namespace Widgets