
cmake_minimum_required(VERSION 3.25.0)

project(NewByte_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB_RECURSE headers "src/*.h")
FILE(GLOB_RECURSE sources "src/*.cpp")

add_executable(${PROJECT_NAME} WIN32 
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

        src/Win32Window/Win32Window.cpp
        src/Win32Window/Win32Window.hpp
        src/Win32Window/Win32EventLoop.cpp
        src/Win32Window/Win32EventLoop.hpp

        src/WindowInterface/IWindow.cpp
        src/WindowInterface/IWindow.hpp
        src/WindowInterface/IEventLoop.cpp
        src/WindowInterface/IEventLoop.hpp
        src/WindowInterface/IPlatformMessage.hpp
        src/WindowInterface/WindowMapper.hpp
        src/WindowInterface/IWindowRenderer.hpp
        src/WindowInterface/IWindowRenderer.cpp
        
        src/WindowInterface/WindowCore.hpp
        src/WindowInterface/WindowState.hpp

        src/Renderer/FactorySingleton.cpp
        src/Renderer/FactorySingleton.hpp
        src/Renderer/Renderer.cpp
        src/Renderer/Renderer.hpp
        src/Renderer/Direct2dRenderer.cpp
        src/Renderer/Direct2dRenderer.hpp
        src/Renderer/Direct2dWrapper.cpp
        src/Renderer/Direct2dWrapper.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources} ${headers})


if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()


if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32 )
endif()
