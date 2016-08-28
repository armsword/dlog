set(common_SRCS
  TimeUtility.cpp
  LoopThread.cpp
  Lock.cpp
  Thread.cpp
  FileUtil.cpp
  )

add_library(dlog_common ${common_SRCS})
set_target_properties(dlog_common PROPERTIES COMPILE_FLAGS "-std=c++0x")
install(TARGETS dlog_common DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/dlog/common)
