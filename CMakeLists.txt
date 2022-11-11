cmake_minimum_required(VERSION 3.0)
project(Vector_Slicer)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 14)

add_executable(Vector_Slicer main.cpp
        source/auxiliary/SimpleMathOperations.cpp
        source/auxiliary/SimpleMathOperations.h
        source/importing_and_exporting/TableReading.cpp
        source/importing_and_exporting/TableReading.h
        source/pattern/FilledPattern.cpp
        source/pattern/FilledPattern.h
        source/pattern/DesiredPattern.cpp
        source/pattern/DesiredPattern.h
        source/pattern/Path.cpp
        source/pattern/Path.h
        source/auxiliary/Perimeter.cpp
        source/auxiliary/Perimeter.h
        source/auxiliary/Geometry.cpp
        source/auxiliary/Geometry.h
        source/pattern/StartingPoint.cpp
        source/pattern/StartingPoint.h
        source/importing_and_exporting/OpenFiles.cpp
        source/importing_and_exporting/OpenFiles.h
        source/pattern/FillingPatterns.cpp
        source/pattern/FillingPatterns.h
        source/pattern/QuantifyPattern.cpp
        source/pattern/QuantifyPattern.h
        source/importing_and_exporting/Exporting.h
        source/importing_and_exporting/Exporting.cpp
        source/auxiliary/ValarrayOperations.cpp
        source/auxiliary/ValarrayOperations.h
        source/auxiliary/ProgressBar.cpp
        source/auxiliary/ProgressBar.h
        source/auxiliary/FillingConfig.cpp
        source/auxiliary/FillingConfig.h
        source/high_level/ConfigDisagreement.cpp
        source/high_level/ConfigDisagreement.h
        source/pattern/IndexedPath.cpp
        source/pattern/IndexedPath.h
        source/high_level/FillingOptimization.cpp
        source/high_level/FillingOptimization.h
        source/high_level/ConfigGeneration.cpp
        source/high_level/ConfigGeneration.h
        ExecutionConfig.h
        source/importing_and_exporting/ReadingFromOutside.cpp
        source/importing_and_exporting/ReadingFromOutside.h)

target_link_libraries(Vector_Slicer
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}  -ldl
)