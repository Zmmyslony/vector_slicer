cmake_minimum_required(VERSION 3.0)
project(Vector_Slicer)

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 14)

add_executable(Vector_Slicer main.cpp libs/auxiliary/SimpleMathOperations.cpp libs/auxiliary/TableReading.cpp libs/pattern/FilledPattern.cpp libs/pattern/FilledPattern.h libs/pattern/DesiredPattern.cpp libs/pattern/DesiredPattern.h libs/pattern/Path.cpp libs/pattern/Path.h libs/auxiliary/PerimeterGeneration.cpp libs/auxiliary/PerimeterChecking.cpp libs/auxiliary/Geometry.cpp libs/pattern/StartingPoint.cpp libs/pattern/StartingPoint.h libs/high_level/OpenFiles.cpp libs/high_level/OpenFiles.h libs/pattern/FillingPatterns.cpp libs/auxiliary/SimpleMathOperations.h libs/auxiliary/TableReading.h libs/auxiliary/PerimeterGeneration.h libs/auxiliary/PerimeterChecking.h libs/auxiliary/Geometry.h libs/pattern/FillingPatterns.h libs/pattern/QuantifyPattern.cpp libs/pattern/QuantifyPattern.h libs/auxiliary/ValarrayConversion.cpp libs/auxiliary/ValarrayConversion.h libs/auxiliary/Exporting.h libs/auxiliary/Exporting.cpp libs/auxiliary/ValarrayOperations.cpp libs/auxiliary/ValarrayOperations.h libs/high_level/AutomaticPathGeneration.cpp libs/high_level/AutomaticPathGeneration.h libs/pattern/GcodeGenerator.cpp libs/pattern/GcodeGenerator.h libs/high_level/ProgressBar.cpp libs/high_level/ProgressBar.h libs/auxiliary/FillingConfig.cpp libs/auxiliary/FillingConfig.h libs/high_level/OptimizedFilling.cpp libs/high_level/OptimizedFilling.h)
