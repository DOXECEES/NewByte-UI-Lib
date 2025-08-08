
cmake_minimum_required(VERSION 3.25.0)

project(NewByte_UI_Lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (SRC 
        src/main.cpp
        src/IWindow.cpp
        src/IWindow.hpp
        src/Core.cpp
        src/Core.hpp
        src/MainWindow.cpp
        src/MainWindow.hpp
        src/Utils.cpp
        src/Utils.hpp
        src/DoceTree.cpp
        src/DoceTree.hpp
        src/ChildWindow.cpp
        src/ChildWindow.hpp
        src/CaptionButtons.cpp
        src/CaptionButtons.hpp
        src/Layout.cpp
        src/Layout.hpp
        src/IIndexable.cpp
        src/IIndexable.hpp
        src/DockManager.cpp
        src/DockManager.hpp
        src/DockTree.cpp
        src/DockTree.hpp



        src/Win32Window/Win32Window.cpp
        src/Win32Window/Win32Window.hpp
        src/Win32Window/Win32EventLoop.cpp
        src/Win32Window/Win32EventLoop.hpp
        src/Win32Window/Win32ModalWindow.cpp
        src/Win32Window/Win32ModalWindow.hpp
        src/Win32Window/Win32ChildWindow.cpp
        src/Win32Window/Win32ChildWindow.hpp

        src/WindowInterface/IWindow.cpp
        src/WindowInterface/IWindow.hpp
        src/WindowInterface/IEventLoop.cpp
        src/WindowInterface/IEventLoop.hpp
        src/WindowInterface/IPlatformMessage.hpp
        src/WindowInterface/WindowMapper.hpp
        src/WindowInterface/IWindowRenderer.hpp
        src/WindowInterface/IWindowRenderer.cpp
        src/WindowInterface/WindowState.hpp
        src/WindowInterface/WindowState.cpp
        src/WindowInterface/WindowStyle.cpp
        src/WindowInterface/WindowStyle.hpp
      

        src/WindowInterface/WindowCore.hpp

        src/Renderer/FactorySingleton.cpp
        src/Renderer/FactorySingleton.hpp
        src/Renderer/Renderer.cpp
        src/Renderer/Renderer.hpp
        src/Renderer/Direct2dRenderer.cpp
        src/Renderer/Direct2dRenderer.hpp
        src/Renderer/Direct2dWrapper.cpp
        src/Renderer/Direct2dWrapper.hpp
        src/Renderer/Direct2dCaptionButtonRenderer.cpp
        src/Renderer/Direct2dCaptionButtonRenderer.hpp
        src/Renderer/Direct2dWidgetRenderer.cpp
        src/Renderer/Direct2dWidgetRenderer.hpp
        src/Renderer/Direct2dGlobalWidgetMapper.cpp
        src/Renderer/Direct2dGlobalWidgetMapper.hpp


        src/Widgets/IWidget.cpp
        src/Widgets/IWidget.hpp
        src/Widgets/Button.cpp
        src/Widgets/Button.hpp
        src/Widgets/IWidgetRenderer.cpp
        src/Widgets/IWidgetRenderer.hpp
        src/Widgets/TextEdit.cpp
        src/Widgets/TextEdit.hpp
        src/Widgets/TreeView.cpp
        src/Widgets/TreeView.hpp
        src/Widgets/WidgetStyle.cpp
        src/Widgets/WidgetStyle.hpp
)

add_library(${PROJECT_NAME} STATIC ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ../Engine/src
)


if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()


if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32 )
endif()

foreach(source IN LISTS SRC)
  get_filename_component(source_path "${source}" PATH)
  string(REPLACE "/" "\\" source_path_msvc "${source_path}")
  source_group("${source_path_msvc}" FILES "${source}")
endforeach()
