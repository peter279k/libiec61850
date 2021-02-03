
set(example_SRCS
   no_tls_client_example.c
)

IF(MSVC)
set_source_files_properties(${example_SRCS}
                                       PROPERTIES LANGUAGE CXX)
ENDIF(MSVC)

add_executable(no_tls_client_example
  ${example_SRCS}
)

target_link_libraries(no_tls_client_example
    iec61850
)
