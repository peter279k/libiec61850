find_package(CURL)

IF(CURL_FOUND)
    target_link_libraries(no_tls_server_example ${CURL_LIBRARY})
ELSE(CURL_FOUND)
    message(FATAL_ERROR "CURL library not found")
ENDIF(CURL_FOUND)

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

add_executable(no_tls_server_example
  ${example_SRCS}
)

target_link_libraries(no_tls_server_example
    iec61850
)
