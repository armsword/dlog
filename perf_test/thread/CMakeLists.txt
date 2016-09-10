set(val_SRCS
    ThreadTest.cpp
)

add_executable(thread_test ${val_SRCS})
target_link_libraries(thread_test dlog_logger)
target_link_libraries(thread_test pthread)
