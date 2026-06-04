#pragma once

#include "Button.hpp"
#include "IWidget.hpp"
#include "ListView.hpp"
#include "TextEdit.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

namespace Widgets
{
    class FilePicker : public IWidget
    {
    public:
        static constexpr const char* CLASS_NAME = "FilePicker";

        FilePicker(
            const NbRect<int>&              rect,
            const std::vector<std::string>& extensions = {}
        )
            : IWidget(rect)
            , m_allowedExtensions(extensions)
        {
            disableHoverState(true);

            // Подготовка фильтров расширений
            for (auto& ext : m_allowedExtensions)
            {
                if (!ext.empty() && ext[0] != '.')
                {
                    ext = "." + ext;
                }
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            }

            openButton->setText(L"Select");
            cancelButton->setText(L"Cancel");
            upButton->setText(L" ⮬ Up ");

            addChildrenWidget(pathTextBox);
            addChildrenWidget(upButton);
            addChildrenWidget(fileListView);
            addChildrenWidget(fileNameTextBox);
            addChildrenWidget(openButton);
            addChildrenWidget(cancelButton);

            // Кнопка "Вверх"
            subscribe(
                *upButton, &Widgets::IWidget::onReleasedSignal,
                [this]()
                {
                    std::filesystem::path current(m_currentPath);
                    if (current.has_parent_path())
                    {
                        refreshDirectory(current.parent_path().string());
                    }
                }
            );

            // Выбор в списке (Клик по элементу)
            subscribe(
                *fileListView, &Widgets::ListView::onItemSelectedSignal,
                [this](int index)
                {
                    std::string rawName = fileListView->getItemText(index);

                    // Ищем первый пробел после иконки (📁 или 📄)
                    size_t spacePos = rawName.find(' ');
                    if (spacePos == std::string::npos)
                    {
                        return;
                    }

                    std::string           cleanName = rawName.substr(spacePos + 1);
                    std::filesystem::path fullPath =
                        std::filesystem::path(m_currentPath) / cleanName;

                    if (std::filesystem::is_directory(fullPath))
                    {
                        // Мгновенный переход при выборе папки в списке
                        refreshDirectory(fullPath.string());
                    }
                    else
                    {
                        // Просто подставляем имя файла в поле ввода
                        fileNameTextBox->setData(Utils::toWstring(cleanName));
                    }
                }
            );

            // Кнопка Select (Подтверждение)
            subscribe(
                *openButton, &Widgets::IWidget::onReleasedSignal,
                [this]()
                {
                    std::wstring wFileName = fileNameTextBox->getData();
                    if (wFileName.empty())
                    {
                        return;
                    }

                    std::filesystem::path finalPath =
                        std::filesystem::path(m_currentPath) / wFileName;

                    // Если в поле ввода указана существующая папка — заходим в неё
                    if (std::filesystem::exists(finalPath) &&
                        std::filesystem::is_directory(finalPath))
                    {
                        refreshDirectory(finalPath.string());
                    }
                    else
                    {
                        // Если это файл (существующий или новый) — возвращаем путь
                        onFileSelected.emit(finalPath.string());
                    }
                }
            );

            // Кнопка Cancel
            subscribe(
                *cancelButton, &Widgets::IWidget::onReleasedSignal,
                [this]()
                {
                    onCancelButtonPressed.emit();
                }
            );

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

        void onMouseWheel(
            const NbPoint<int>& pos,
            int                 delta
        ) override
        {
            if (fileListView && fileListView->getRect().isInside(pos))
            {
                fileListView->onMouseWheel(pos, delta);
            }
        }

        void refreshDirectory(const std::string& path)
        {
            try
            {
                std::filesystem::path fsPath(path);
                m_currentPath = std::filesystem::absolute(fsPath).string();

                pathTextBox->setData(Utils::toWstring(m_currentPath));
                fileListView->clear();
                fileNameTextBox->setData(L"");

                std::vector<std::filesystem::directory_entry> dirs;
                std::vector<std::filesystem::directory_entry> files;

                for (const auto& entry : std::filesystem::directory_iterator(m_currentPath))
                {
                    if (entry.is_directory())
                    {
                        dirs.push_back(entry);
                    }
                    else
                    {
                        if (m_allowedExtensions.empty())
                        {
                            files.push_back(entry);
                        }
                        else
                        {
                            std::string ext = entry.path().extension().string();
                            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                            if (std::find(
                                    m_allowedExtensions.begin(), m_allowedExtensions.end(), ext
                                ) != m_allowedExtensions.end())
                            {
                                files.push_back(entry);
                            }
                        }
                    }
                }

                auto sortFunc = [](const auto& a, const auto& b)
                {
                    return a.path().filename() < b.path().filename();
                };
                std::sort(dirs.begin(), dirs.end(), sortFunc);
                std::sort(files.begin(), files.end(), sortFunc);

                for (const auto& d : dirs)
                {
                    fileListView->addItem("📁 " + d.path().filename().string());
                }
                for (const auto& f : files)
                {
                    fileListView->addItem("📄 " + f.path().filename().string());
                }
            }
            catch (...)
            {
                pathTextBox->setData(L"Error: Access Denied");
            }
        }

        // --- Геттеры для рендерера ---
        std::shared_ptr<TextEdit> getPathTextBox() const
        {
            return pathTextBox;
        }
        std::shared_ptr<Button> getUpButton() const
        {
            return upButton;
        }
        std::shared_ptr<ListView> getFileListView() const
        {
            return fileListView;
        }
        std::shared_ptr<TextEdit> getFileNameTextBox() const
        {
            return fileNameTextBox;
        }
        std::shared_ptr<Button> getOpenButton() const
        {
            return openButton;
        }
        std::shared_ptr<Button> getCancelButton() const
        {
            return cancelButton;
        }
        const std::string& getCurrentPath() const
        {
            return m_currentPath;
        }

        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            m_measuredSize.width  = std::clamp(500, 0, maxSize.width);
            m_measuredSize.height = std::clamp(600, 0, maxSize.height);
            return m_measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept override
        {
            setRect(rect);
            const int p = 12, s = 6, topH = 32, bottomH = 34, upBtnW = 65, actionBtnW = 90;

            pathTextBox->layout({rect.x + p, rect.y + p, rect.width - p * 2 - upBtnW - s, topH});
            upButton->layout({rect.x + rect.width - p - upBtnW, rect.y + p, upBtnW, topH});

            int bottomY = rect.y + rect.height - p - bottomH;
            cancelButton->layout(
                {rect.x + rect.width - p - actionBtnW, bottomY, actionBtnW, bottomH}
            );
            openButton->layout(
                {cancelButton->getRect().x - s - actionBtnW, bottomY, actionBtnW, bottomH}
            );
            fileNameTextBox->layout(
                {rect.x + p, bottomY, openButton->getRect().x - p - s, bottomH}
            );

            int listY = pathTextBox->getRect().y + topH + s;
            int listH = fileNameTextBox->getRect().y - listY - s;
            fileListView->layout({rect.x + p, listY, rect.width - p * 2, listH});
        }

    public:
        Signal<void(const std::string&)> onFileSelected;
        Signal<void()>                   onCancelButtonPressed;

    private:
        NbSize<int>              m_measuredSize;
        std::string              m_currentPath;
        std::vector<std::string> m_allowedExtensions;

        std::shared_ptr<TextEdit> pathTextBox     = std::make_shared<TextEdit>();
        std::shared_ptr<Button>   upButton        = std::make_shared<Button>();
        std::shared_ptr<ListView> fileListView    = std::make_shared<ListView>();
        std::shared_ptr<TextEdit> fileNameTextBox = std::make_shared<TextEdit>();
        std::shared_ptr<Button>   openButton      = std::make_shared<Button>();
        std::shared_ptr<Button>   cancelButton    = std::make_shared<Button>();
    };
} // namespace Widgets