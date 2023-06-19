cmake_minimum_required(VERSION 3.0)
project(vector_slicer VERSION 0.5.0)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

ADD_SUBDIRECTORY(bayesopt)
include_directories(${PROJECT_SOURCE_DIR}/bayesopt/include)
include_directories(${PROJECT_SOURCE_DIR}/bayesopt/utils)
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
        source/pattern/auxiliary/simple_math_operations.cpp
        source/pattern/auxiliary/simple_math_operations.h
        source/pattern/importing_and_exporting/table_reading.cpp
        source/pattern/importing_and_exporting/table_reading.h
        source/pattern/filled_pattern.cpp
        source/pattern/filled_pattern.h
        source/pattern/desired_pattern.cpp
        source/pattern/desired_pattern.h
        source/pattern/path.cpp
        source/pattern/path.h
        source/pattern/auxiliary/perimeter.cpp
        source/pattern/auxiliary/perimeter.h
        source/pattern/auxiliary/geometry.cpp
        source/pattern/auxiliary/geometry.h
        source/pattern/importing_and_exporting/open_files.cpp
        source/pattern/importing_and_exporting/open_files.h
        source/pattern/filling_patterns.cpp
        source/pattern/filling_patterns.h
        source/pattern/quantified_config.cpp
        source/pattern/quantified_config.h
        source/pattern/importing_and_exporting/exporting.h
        source/pattern/importing_and_exporting/exporting.cpp
        source/pattern/auxiliary/valarray_operations.cpp
        source/pattern/auxiliary/valarray_operations.h
        source/pattern/auxiliary/progress_bar.cpp
        source/pattern/auxiliary/progress_bar.h
        source/pattern/filling_config.cpp
        source/pattern/filling_config.h
        source/pattern/director_indexed_path.cpp
        source/pattern/director_indexed_path.h
        source/bayesian_optimisation.cpp
        source/bayesian_optimisation.h
        source/pattern/auxiliary/vector_operations.cpp
        source/pattern/auxiliary/vector_operations.h
        source/pattern/disagreement_weights.cpp
        source/pattern/disagreement_weights.h
        source/pattern/importing_and_exporting/importing_patterns.h
        source/pattern/importing_and_exporting/importing_patterns.cpp
        source/pattern/auxiliary/configuration_reading.cpp
        source/pattern/auxiliary/configuration_reading.h source/pattern/vector_sorted_paths.cpp source/pattern/vector_sorted_paths.h source/pattern/auxiliary/repulsion.cpp source/pattern/auxiliary/repulsion.h source/pattern/auxiliary/line_operations.cpp source/pattern/auxiliary/line_operations.h)

target_link_libraries(Vector_Slicer
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}  -ldl
        bayesopt
)