set(val_SRCS
    LogTest.cpp
    div.cpp    
)

add_executable(log_test ${val_SRCS})
target_link_libraries(log_test dlog_logger)
target_link_libraries(log_test pthread)
