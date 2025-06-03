
cmake_minimum_required(VERSION 3.25.0)

project(NewByte_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)



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

        src/Renderer/FactorySingleton.cpp
        src/Renderer/FactorySingleton.hpp
        src/Renderer/Renderer.cpp
        src/Renderer/Renderer.hpp
)



if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()


if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32 )
endif()
