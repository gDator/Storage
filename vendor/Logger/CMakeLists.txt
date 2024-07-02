add_library(logger STATIC logger.cpp)
target_include_directories(logger PUBLIC .)
target_link_directories(logger INTERFACE .)
target_link_libraries(logger boost_header boost_log-mt-x64.a
        boost_thread-mt-x64.a
        boost_regex-mt-x64.a
        boost_filesystem-mt-x64.a
        boost_log_setup-mt-x64.a
        boost_atomic-mt-x64.a
        boost_chrono-mt-x64.a)