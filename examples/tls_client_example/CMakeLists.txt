
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../third_party/sqlite/sqlite3.h")
message("Found sqlite source code -> compile sqlite-log driver with static sqlite library")

include_directories(
   .
   ${CMAKE_SOURCE_DIR}/third_party/sqlite
)

set(example_SRCS
   tls_client_example.c
)

set(sqlite_SRCS
   ${CMAKE_CURRENT_LIST_DIR}/../../third_party/sqlite/sqlite3.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION")

IF(MSVC)
set_source_files_properties(${example_SRCS}
                                       PROPERTIES LANGUAGE CXX)
ENDIF(MSVC)

configure_file(client1-key.pem client1-key.pem COPYONLY)
configure_file(client1.cer client1.cer COPYONLY)
configure_file(root.cer root.cer COPYONLY)

add_executable(tls_client_example
  ${example_SRCS}
  ${sqlite_SRCS}
)

target_link_libraries(tls_client_example
    iec61850
)

ELSE()

message("tls_client_example: sqlite3 library not found")

ENDIF()
