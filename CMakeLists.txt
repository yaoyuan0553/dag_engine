cmake_minimum_required(VERSION 3.28)
project(dag_exec)

set(CMAKE_CXX_STANDARD 20)

add_executable(dag_exec main.cpp)

target_sources(dag_exec
    PUBLIC
    FILE_SET CXX_MODULES FILES
    meta.cpp utility.cpp graph.cpp
)

target_compile_definitions(dag_exec PRIVATE $<$<CONFIG:Debug>:DAG_DEBUG>)
