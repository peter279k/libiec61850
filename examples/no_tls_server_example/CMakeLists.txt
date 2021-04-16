include(FindJSON-C.cmake)

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
find_package(JSON-C)
add_executable(no_tls_server_example
  ${example_SRCS}
)

IF(CURL_FOUND)
    target_link_libraries(no_tls_server_example ${CURL_LIBRARY} iec61850)
ELSE(CURL_FOUND)
    message(FATAL_ERROR "CURL library not found")
ENDIF(CURL_FOUND)

IF(JSONC_FOUND)
    target_link_libraries(no_tls_server_example ${CURL_LIBRARY} ${JSONC_LIBRARIES} iec61850)
ELSE(CURL_FOUND)
    message(FATAL_ERROR "JSON-C library not found")
ENDIF(CURL_FOUND)
