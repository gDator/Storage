add_library(sqllitecpp INTERFACE)
target_include_directories(sqllitecpp INTERFACE .)
target_link_directories(sqllitecpp INTERFACE .)
#find_library(LIBSQLCPP  .)
target_link_libraries(sqllitecpp INTERFACE libSQLiteCpp.a)