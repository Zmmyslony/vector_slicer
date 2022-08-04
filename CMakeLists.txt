cmake_minimum_required(VERSION 3.0)
project(Vector_Slicer)

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 14)

add_executable(Vector_Slicer main.cpp libs/auxiliary/SimpleMathOperations.cpp libs/auxiliary/TableReading.cpp libs/pattern/FilledPattern.cpp libs/pattern/FilledPattern.h libs/pattern/DesiredPattern.cpp libs/pattern/DesiredPattern.h libs/pattern/Path.cpp libs/pattern/Path.h libs/auxiliary/Perimeter.cpp libs/auxiliary/Geometry.cpp libs/pattern/StartingPoint.cpp libs/pattern/StartingPoint.h libs/high_level/OpenFiles.cpp libs/high_level/OpenFiles.h libs/pattern/FillingPatterns.cpp libs/auxiliary/SimpleMathOperations.h libs/auxiliary/TableReading.h libs/auxiliary/Perimeter.h libs/auxiliary/Geometry.h libs/pattern/FillingPatterns.h libs/pattern/QuantifyPattern.cpp libs/pattern/QuantifyPattern.h libs/auxiliary/Exporting.h libs/auxiliary/Exporting.cpp libs/auxiliary/ValarrayOperations.cpp libs/auxiliary/ValarrayOperations.h libs/obsolete/AutomaticPathGeneration.cpp libs/obsolete/AutomaticPathGeneration.h libs/obsolete/PathFillingOrder.cpp libs/obsolete/PathFillingOrder.h libs/auxiliary/ProgressBar.cpp libs/auxiliary/ProgressBar.h libs/auxiliary/FillingConfig.cpp libs/auxiliary/FillingConfig.h libs/high_level/ConfigDisagreement.cpp libs/high_level/ConfigDisagreement.h libs/auxiliary/GCodeFile.cpp libs/auxiliary/GCodeFile.h libs/pattern/IndexedPath.cpp libs/pattern/IndexedPath.h libs/high_level/FillingOptimization.cpp libs/high_level/FillingOptimization.h libs/high_level/ConfigGeneration.cpp libs/high_level/ConfigGeneration.h ExecutionConfig.h libs/high_level/ReadingFromOutside.cpp libs/high_level/ReadingFromOutside.h libs/auxiliary/Hyrel.cpp libs/auxiliary/Hyrel.h)
