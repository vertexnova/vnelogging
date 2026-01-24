#==============================================================================
# 05_multithreaded - Thread-safe logging from multiple threads
#==============================================================================

add_executable(05_Multithreaded main.cpp)

target_link_libraries(05_Multithreaded
    PRIVATE
        vne::logging
)

target_include_directories(05_Multithreaded
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(05_Multithreaded PUBLIC cxx_std_20)
