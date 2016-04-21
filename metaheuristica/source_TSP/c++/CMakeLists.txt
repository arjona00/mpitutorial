CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(trajectory)
ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
)
find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})
SET(SRC
  TSP.cc)
add_executable(main main.cc ${SRC})
target_link_libraries(main ${ARMADILLO_LIBRARIES})
add_executable(test_seq test.cc ${SRC})
target_link_libraries(test_seq ${ARMADILLO_LIBRARIES})
add_executable(random random.cc ${SRC})
target_link_libraries(random ${ARMADILLO_LIBRARIES})
