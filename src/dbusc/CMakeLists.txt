# Library: dbusc - TEMPLATE (Library/Interface)
add_library(dbusc INTERFACE)

target_include_directories(dbusc INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/include
)

target_sources(dbusc INTERFACE
    dbusc.c
)

target_link_libraries(dbusc INTERFACE
    pico_stdlib
)

add_subdirectory(cmd)
