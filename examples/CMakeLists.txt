include_directories( ../include )

# set(CMAKE_DEBUG_POSTFIX "")

function(CompileExample name)
    add_executable(${name}  ${name}.cpp )
    target_link_libraries(${name} ${PICKUP_LIBRARY} pickup )
endfunction()

function(CompileHeaderOnlyExample name)
    add_executable(${name}  ${name}.cpp )
endfunction()

CompileHeaderOnlyExample("FactoryDemo")
CompileHeaderOnlyExample("StopWatchDemo")
CompileHeaderOnlyExample("unusedDemo")
CompileExample("DateTimeDemo")
# CompileHeaderOnlyExample("PacketBufferDemo")