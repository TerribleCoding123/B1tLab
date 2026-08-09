cmake_minimum_required(VERSION 3.31)

project(B1tLab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(${PROJECT_NAME})

#for replxx 
include(FetchContent)

FetchContent_Declare(
    
    replxx 
    URL 
    "https://github.com/AmokHuginnsson/replxx/archive/refs/tags/release-0.0.4.zip"
)

FetchContent_MakeAvailable(replxx)

target_sources(${PROJECT_NAME}
    PRIVATE
    
	view/view.cpp
	source/main.cpp
	conversion_modules/binary/binary.cpp
	built_ins/built_ins.cpp

)

target_include_directories(${PROJECT_NAME}
    
    PRIVATE
	view
	conversion_modules/binary
	built_ins
)

target_link_libraries(${PROJECT_NAME}  replxx )