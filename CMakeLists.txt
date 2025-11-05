
cmake_minimum_required(VERSION 3.25.0)

project(NewByte_UI_Lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(../Common NbCommon_build)


set (SRC 
        src/main.cpp
        
        src/Core.cpp
        src/Core.hpp
        src/Utils.cpp
        src/Utils.hpp
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
        src/Signal.hpp
        src/SubscribtionManager.cpp
        src/SubscribtionManager.hpp



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
        src/Renderer/Direct2dFont.cpp
        src/Renderer/Direct2dFont.hpp
        src/Renderer/Font.cpp
        src/Renderer/Font.hpp
        src/Renderer/GeometryFactory.cpp
        src/Renderer/GeometryFactory.hpp


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
        src/Widgets/Label.cpp
        src/Widgets/Label.hpp
        src/Widgets/CheckBox.cpp
        src/Widgets/CheckBox.hpp
        src/Widgets/Indentations.cpp
        src/Widgets/Indentations.hpp
        src/Widgets/Theme.cpp
        src/Widgets/Theme.hpp
        src/Widgets/ComboBox.cpp
        src/Widgets/ComboBox.hpp
        src/Widgets/IPopUp.cpp
        src/Widgets/IPopUp.hpp
        src/Widgets/SpinBox.cpp
        src/Widgets/SpinBox.hpp
        src/Widgets/WidgetSizePolicy.cpp
        src/Widgets/WidgetSizePolicy.hpp
        src/Widgets/SideBar.cpp
        src/Widgets/SideBar.hpp
        src/Widgets/ILayoutable.hpp

        src/Utils/Windows/WindowPositionQueue.cpp
        src/Utils/Windows/WindowPositionQueue.hpp

        src/Core/ZIndex.cpp
        src/Core/ZIndex.hpp
)


add_library(${PROJECT_NAME} STATIC ${SRC} "src/Widgets/SpinBox.cpp" "src/Widgets/WidgetSizePolicy.cpp" "src/Widgets/SideBar.cpp" "src/Utils/Windows/WindowPositionQueue.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ../Engine/src
    ../nbstl/src
)


if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()


if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32 PUBLIC NbCommon)
    
endif()

foreach(source IN LISTS SRC)
  get_filename_component(source_path "${source}" PATH)
  string(REPLACE "/" "\\" source_path_msvc "${source_path}")
  source_group("${source_path_msvc}" FILES "${source}")
endforeach()
