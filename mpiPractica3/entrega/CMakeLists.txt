PROJECT(sec)
SET(CMAKE_BUILD_TYPE Debug)
SET(SRCSEQ
  sec_multmatrixseq)
SET(SRC
  sec_multmatrix)

ADD_EXECUTABLE(multmatrix ${SRC})
ADD_EXECUTABLE(multmatrixseq ${SRCSEQ})

TARGET_LINK_LIBRARIES(multmatrix)
TARGET_LINK_LIBRARIES(multmatrixseq)

FIND_PACKAGE(MPI)
include_directories(${MPI_INCLUDE_PATH})

SET(SRCMPI
  mpi_multmatrixseq)

ADD_EXECUTABLE(mpi_multmatrix ${SRCMPI})
TARGET_LINK_LIBRARIES(mpi_multmatrix ${MPI_C_LIBRARIES} -lm)
