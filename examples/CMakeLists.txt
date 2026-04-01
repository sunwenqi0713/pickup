# ============================================================
# Examples
# ============================================================

# Example that links against the pickup library
function(add_pickup_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
endfunction()

# Header-only example (no library linkage needed)
function(add_header_only_example name)
    add_executable(${name} ${name}.cpp)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    # 确保 MSVC 以 UTF-8 解析含中文注释的头文件
    if(MSVC)
        target_compile_options(${name} PRIVATE /utf-8)
    endif()
endfunction()

# Header-only examples
add_header_only_example(FactoryDemo)
add_header_only_example(StopWatchDemo)
add_header_only_example(unusedDemo)

# Examples requiring the library
add_pickup_example(DateTimeDemo)
# add_header_only_example(PacketBufferDemo)