include_directories(
   .
   /usr/include/x86_64-linux-gnu
)

link_directories(/usr/lib/x86_64-linux-gnu)

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
