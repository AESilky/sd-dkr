# Library: dbusm - TEMPLATE (Library/Interface)
add_library(dbusm INTERFACE)

pico_generate_pio_header(dbusm ${CMAKE_CURRENT_LIST_DIR}/dbusm.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_include_directories(dbusm INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/include
)

target_sources(dbusm INTERFACE
    dbusm.c
)

target_link_libraries(dbusm INTERFACE
    pico_stdlib
)

add_subdirectory(cmd)
