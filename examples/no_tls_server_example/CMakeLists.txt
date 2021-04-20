include_directories(
    .
)

set(example_SRCS
   no_tls_server_example.c
   static_model.c
)

IF(MSVC)
set_source_files_properties(${example_SRCS}
                                       PROPERTIES LANGUAGE CXX)
ENDIF(MSVC)

find_package(CURL)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(JSON-C)

add_executable(no_tls_server_example
  ${example_SRCS}
)

IF(CURL_FOUND)
    target_link_libraries(no_tls_server_example ${CURL_LIBRARY} iec61850)
ELSE(CURL_FOUND)
    message(FATAL_ERROR "CURL library not found")
ENDIF(CURL_FOUND)

IF(JSON-C_FOUND)
    target_link_libraries(no_tls_server_example ${CURL_LIBRARY} ${JSON-C_LIBRARIES} iec61850)
ELSE(JSON-C_FOUND)
    message(FATAL_ERROR "JSON-C library not found")
ENDIF(JSON-C_FOUND)
