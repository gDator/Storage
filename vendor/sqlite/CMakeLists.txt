add_library(_sqlite3 INTERFACE)
target_include_directories(_sqlite3 INTERFACE .)
target_link_directories(_sqlite3 INTERFACE .)
target_link_libraries(_sqlite3 INTERFACE libsqlite3.a)