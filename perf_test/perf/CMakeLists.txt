set(val_SRCS
    PerfTest.cpp
)

add_executable(perf_test ${val_SRCS})
target_link_libraries(perf_test dlog_logger)
target_link_libraries(perf_test pthread)
