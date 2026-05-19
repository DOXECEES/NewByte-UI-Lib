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

            subscribe(
                *fileListView, &Widgets::ListView::onItemSelectedSignal,
                [this](int index)
                {
                    std::string rawName = fileListView->getItemText(index);

                    size_t spacePos = rawName.find(' ');
                    if (spacePos == std::string::npos)
                    {
                        return;
                    }

                    std::string cleanName = rawName.substr(spacePos + 1);

                    std::filesystem::path fullPath =
                        std::filesystem::path(m_currentPath) / cleanName;

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
                    std::wstring wFileName = fileNameTextBox->getData();
                    if (wFileName.empty())
                    {
                        return;
                    }


                    std::filesystem::path finalPath =
                        std::filesystem::path(m_currentPath) / wFileName;

                    if (std::filesystem::exists(finalPath))
                    {
                        if (!std::filesystem::is_directory(finalPath))
                        {
                            onFileSelected.emit(finalPath.string());
                        }
                        else
                        {
                            refreshDirectory(finalPath.string());
                        }
                    }
                }
            );

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
            m_currentPath = path;
            pathTextBox->setData(Utils::toWstring(path));
            fileListView->clear();
            fileNameTextBox->setData(L"");

            try
            {
                std::vector<std::filesystem::directory_entry> dirs;
                std::vector<std::filesystem::directory_entry> files;

                for (const auto& entry : std::filesystem::directory_iterator(path))
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

                            auto it = std::find(
                                m_allowedExtensions.begin(), m_allowedExtensions.end(), ext
                            );
                            if (it != m_allowedExtensions.end())
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

        const NbSize<int>& measure(const NbSize<int>& maxSize) noexcept override
        {
            measuredSize.width  = std::clamp(500, 0, maxSize.width);
            measuredSize.height = std::clamp(600, 0, maxSize.height);
            return measuredSize;
        }

        void layout(const NbRect<int>& rect) noexcept override
        {
            setRect(rect);

            const int p          = 12; 
            const int s          = 6;  
            const int topH       = 32;
            const int bottomH    = 34;
            const int upBtnW     = 65;
            const int actionBtnW = 90;

            NbRect<int> upBtnRect = {rect.x + rect.width - p - upBtnW, rect.y + p, upBtnW, topH};
            NbRect<int> pathRect  = {rect.x + p, rect.y + p, rect.width - p * 2 - upBtnW - s, topH};

            int bottomY = rect.y + rect.height - p - bottomH;

            NbRect<int> cancelBtnRect = {
                rect.x + rect.width - p - actionBtnW, bottomY, actionBtnW, bottomH
            };
            NbRect<int> openBtnRect = {
                cancelBtnRect.x - s - actionBtnW, bottomY, actionBtnW, bottomH
            };
            NbRect<int> fileNameRect = {
                rect.x + p, bottomY, openBtnRect.x - rect.x - p - s, bottomH
            };

            int         listY    = pathRect.y + pathRect.height + s;
            int         listH    = fileNameRect.y - listY - s;
            NbRect<int> listRect = {rect.x + p, listY, rect.width - p * 2, listH};

            pathTextBox->layout(pathRect);
            upButton->layout(upBtnRect);
            fileListView->layout(listRect);
            fileNameTextBox->layout(fileNameRect);
            openButton->layout(openBtnRect);
            cancelButton->layout(cancelBtnRect);
        }

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