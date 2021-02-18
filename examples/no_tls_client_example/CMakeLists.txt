
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../third_party/sqlite/sqlite3.h")
message("Found sqlite source code -> compile sqlite-log driver with static sqlite library")

include_directories(
   .
   ${CMAKE_SOURCE_DIR}/third_party/sqlite
)

set(example_SRCS
   no_tls_client_example.c
)

set(sqlite_SRCS
   ${CMAKE_CURRENT_LIST_DIR}/../../third_party/sqlite/sqlite3.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION")

IF(MSVC)
set_source_files_properties(${example_SRCS}
                                       PROPERTIES LANGUAGE CXX)
ENDIF(MSVC)

add_executable(no_tls_client_example
  ${example_SRCS}
  ${sqlite_SRCS}
)

target_link_libraries(no_tls_client_example
    iec61850
)

ELSE()

message("no_tls_client_example: sqlite3 library not found")

ENDIF()
