if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../third_party/sqlite/sqlite3.h")
message("Found sqlite source code -> compile sqlite-log driver with static sqlite library")

include_directories(
   .
   ${CMAKE_SOURCE_DIR}/third_party/sqlite
)

set(sqlite_SRCS
   ${CMAKE_CURRENT_LIST_DIR}/../../third_party/sqlite/sqlite3.c
)

set(example_SRCS
   no_tls_client_example.c
)

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
