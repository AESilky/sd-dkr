# Library: Control Bus Master operation Commands (shell) - TEMPLATE (Library/Interface)
add_library(dbusm_cmd INTERFACE)

target_sources(dbusm_cmd INTERFACE
  cmds.c
)

target_include_directories(dbusm_cmd INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/app/shell/include
	${CMAKE_CURRENT_LIST_DIR}/../include
)

target_link_libraries(dbusm_cmd INTERFACE
    cmd
)
