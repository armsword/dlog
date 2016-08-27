set(config_SRCS
  ConfigParser.cpp
  )

add_library(dlog_config ${common_SRCS})
set_target_properties(dlog_config PROPERTIES COMPILE_FLAGS "-std=c++0x")
install(TARGETS dlog_config DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/dlog/config)
