cmake_minimum_required(VERSION 3.5)
project(FaceTracker CXX)
SET(CMAKE_BUILD_TYPE Debug)
include (Common.cmake)
#
# now add each of the server builds 
#

	file(GLOB SOURCES Source/*.cpp)


        set(TFLite_LIBS -L/usr/local/lib
            libtensorflow-lite.a
            libruy_allocator.a
            libruy_apply_multiplier.a
            libruy_context.a
            libruy_context_get_ctx.a
            libruy_cpuinfo.a
            libruy_ctx.a
            libruy_denormal.a
            libruy_frontend.a
            libruy_have_built_path_for_avx.a
            libruy_have_built_path_for_avx2_fma.a
            libruy_have_built_path_for_avx512.a
            libruy_kernel_arm.a
            libruy_kernel_avx.a
            libruy_kernel_avx2_fma.a
            libruy_kernel_avx512.a
            libruy_pack_arm.a
            libruy_pack_avx.a
            libruy_pack_avx2_fma.a
            libruy_pack_avx512.a
            libruy_prepacked_cache.a
            libruy_prepare_packed_matrices.a
            libruy_profiler_instrumentation.a
            libruy_profiler_profiler.a
            libruy_system_aligned_alloc.a
            libruy_thread_pool.a
            libruy_trmul.a
            libruy_blocking_counter.a
            libruy_block_map.a
            libruy_tune.a
            libruy_wait.a
            libflatbuffers.a
            libXNNPACK.a
            libcpuinfo.a
            libfft2d_fftsg.a
            libfft2d_fftsg2d.a
            -lpthreadpool
            -lfarmhash

            )


        find_package(OpenCV REQUIRED)

        # Add include path
        include_directories(FaceTracker PRIVATE ${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/Source)

#
## Specify a name for the generated executable file

## 1. Compile...
	add_executable( FaceTracker  ${SOURCES} )

## 2. Link...
        target_link_libraries (FaceTracker
            PRIVATE ${OpenCV_LIBS}
            PRIVATE ${TFLite_LIBS}
            PRIVATE  ${wxWidgets_LIBRARIES}
    )


## 3. Install...
	install( TARGETS FaceTracker  RUNTIME DESTINATION bin )
#
#
## 3. Install...
install( TARGETS DESTINATION bin)


