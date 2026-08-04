cmake_minimum_required(VERSION 3.31)

project(B1tLab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(${PROJECT_NAME})

add_subdirectory(external/replxx_master)

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
	external/replxx_master/include

)

target_link_libraries(${PROJECT_NAME}  replxx )