cmake_minimum_required(VERSION 3.0)
project(vector_slicer VERSION 0.5.0)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

ADD_SUBDIRECTORY(bayesopt)
include_directories(${PROJECT_SOURCE_DIR}/bayesopt/include)
SET(EXT_LIBS bayesopt)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD C17)

configure_file(source/vector_slicer_config.h.in vector_slicer_config.h)
include_directories(${PROJECT_BINARY_DIR})

add_executable(Vector_Slicer main.cpp
        source/pattern/auxiliary/SimpleMathOperations.cpp
        source/pattern/auxiliary/SimpleMathOperations.h
        source/pattern/importing_and_exporting/TableReading.cpp
        source/pattern/importing_and_exporting/TableReading.h
        source/pattern/FilledPattern.cpp
        source/pattern/FilledPattern.h
        source/pattern/DesiredPattern.cpp
        source/pattern/DesiredPattern.h
        source/pattern/Path.cpp
        source/pattern/Path.h
        source/pattern/auxiliary/Perimeter.cpp
        source/pattern/auxiliary/Perimeter.h
        source/pattern/auxiliary/Geometry.cpp
        source/pattern/auxiliary/Geometry.h
        source/pattern/importing_and_exporting/OpenFiles.cpp
        source/pattern/importing_and_exporting/OpenFiles.h
        source/pattern/FillingPatterns.cpp
        source/pattern/FillingPatterns.h
        source/pattern/QuantifiedConfig.cpp
        source/pattern/QuantifiedConfig.h
        source/pattern/importing_and_exporting/Exporting.h
        source/pattern/importing_and_exporting/Exporting.cpp
        source/pattern/auxiliary/ValarrayOperations.cpp
        source/pattern/auxiliary/ValarrayOperations.h
        source/pattern/auxiliary/ProgressBar.cpp
        source/pattern/auxiliary/ProgressBar.h
        source/pattern/FillingConfig.cpp
        source/pattern/FillingConfig.h
        source/pattern/DirectorIndexedPath.cpp
        source/pattern/DirectorIndexedPath.h
        ExecutionConfig.h
        source/bayesian_optimisation.cpp
        source/bayesian_optimisation.h
        source/pattern/auxiliary/vector_operations.cpp
        source/pattern/auxiliary/vector_operations.h
        source/pattern/disagreement_weights.cpp
        source/pattern/disagreement_weights.h
        source/pattern/importing_and_exporting/importing_patterns.h
        source/pattern/importing_and_exporting/importing_patterns.cpp
        source/pattern/auxiliary/configuration_reading.cpp
        source/pattern/auxiliary/configuration_reading.h source/pattern/vector_sorted_paths.cpp source/pattern/vector_sorted_paths.h)

target_link_libraries(Vector_Slicer
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}  -ldl
        bayesopt
)