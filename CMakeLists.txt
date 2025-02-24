cmake_minimum_required(VERSION 3.16)
project(PEP_BLA)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD 17)

include_directories(src "lib/PEP-HPC/src" "lib/PEP-HPC/concurrentqueue")

find_package(Python 3.8 COMPONENTS Interpreter Development REQUIRED)

execute_process(
  COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE PYBIND11_DIR)
list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_DIR}")
find_package(pybind11 CONFIG REQUIRED)


add_executable (test_vector tests/test_vector.cc)
target_sources (test_vector PUBLIC src/vector.h src/expression.h)



find_package(LAPACK REQUIRED)
add_executable (test_lapack tests/test_lapack.cc)
target_link_libraries (test_lapack PUBLIC ${LAPACK_LIBRARIES})


pybind11_add_module(bla src/bind_bla.cpp)

install (TARGETS bla DESTINATION pepbla)
install (FILES src/vector.h DESTINATION pepbla/include)
install (FILES src/matrix.h DESTINATION pepbla/include)
