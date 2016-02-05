cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(kmeans)

find_package(CUDA REQUIRED)
find_package(MPI REQUIRED)

add_definitions(-DBLOCK_SHARED_MEM_OPTIMIZATION=1)

if (WIN32)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	add_library(winposix getopt.c gettimeofday.c)
endif()

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include_directories(${MPI_C_INCLUDE_PATH})
add_executable(seq_main seq_main.c seq_kmeans.c file_io.c wtime.c)
add_executable(omp_main omp_main.c omp_kmeans.c file_io.c)

if (MPI_FOUND)
	add_executable(mpi_main mpi_main.c mpi_kmeans.c mpi_io.c file_io.c)
	target_link_libraries(mpi_main winposix ${MPI_C_LIBRARIES})
endif()

# Pass options to NVCC
#set(CUDA_VERBOSE_BUILD ON)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} --ptxas-options=-v)
cuda_add_executable(cuda_main cuda_main.cu cuda_io.cu cuda_wtime.cu cuda_kmeans.cu getopt.c gettimeofday.c)

if (WIN32)
	target_link_libraries(seq_main winposix)
	target_link_libraries(omp_main winposix)
	target_link_libraries(cuda_main winposix)
endif()