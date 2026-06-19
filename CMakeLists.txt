cmake_minimum_required(VERSION 3.26)
project(cfc_toolkit VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Mandatory warnings — mirrors AMRC professional standard
add_compile_options(-Wall -Wextra -Wpedantic)

link_libraries(m)

include_directories(include)

# Sources grow each day — uncomment as files are created
set(C_SOURCES
    src/force_model.c
    # src/csv_reader.c     # Day 5
    # src/signal_proc.c    # Day 10
)

# Targets — uncomment as needed
# add_library(force_model_static STATIC ${C_SOURCES})   # Day 6
# add_library(force_model_shared SHARED ${C_SOURCES})   # Day 7
# add_executable(cfc_main src/main.c)                   # Day 6
# find_package(pybind11 REQUIRED)                        # Day 16
