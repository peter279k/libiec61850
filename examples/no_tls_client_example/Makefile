LIBIEC_HOME=../..

PROJECT_BINARY_NAME = no_tls_client_example
PROJECT_SOURCES = no_tls_client_example.c
PROJECT_SOURCES += $(LIBIEC_HOME)/third_party/sqlite/sqlite3.c

include $(LIBIEC_HOME)/make/target_system.mk
include $(LIBIEC_HOME)/make/stack_includes.mk

all:	$(PROJECT_BINARY_NAME)

include $(LIBIEC_HOME)/make/common_targets.mk

CFLAGS += -I$(LIBIEC_HOME)/third_party/sqlite
CFLAGS += -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DHAVE_USLEEP

LDLIBS += -lm
CFLAGS += -std=gnu99

$(PROJECT_BINARY_NAME):	$(PROJECT_SOURCES) $(LIB_NAME)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROJECT_BINARY_NAME) $(PROJECT_SOURCES) $(INCLUDES) $(LIB_NAME) $(LDLIBS)

clean:
	rm -f $(PROJECT_BINARY_NAME)
