/*
 *  server_example_write_handler.c
 */

#include "iec61850_server.h"
#include "hal_thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include "../server_example_write_handler/static_model.h"

/* import IEC 61850 device model created from SCL-File */
extern IedModel iedModel;

static int running = 0;
static IedServer iedServer = NULL;

void sigint_handler(int signalId)
{
	running = 0;
}

static MmsDataAccessError
writeAccessHandler (DataAttribute* dataAttribute, MmsValue* value, ClientConnection connection, void* parameter)
{
    if (dataAttribute == IEDMODEL_GenericIO_GGIO1_NamPlt_vendor) {
        char newValue[] = MmsValue_toString(value);
        printf("New value for OutVarSet_setMag_f = %s\n", newValue);

        return DATA_ACCESS_ERROR_SUCCESS;
    }

    return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
}

int main(int argc, char** argv) {

    int port_number = 8103;
    if (argc > 1)
        port_number = atoi(argv[1]);

    printf("Using libIEC61850 version %s\n", LibIEC61850_getVersionString());
    printf("libIEC61850 IedServer server will listen on %d\n", port_number);

    iedServer = IedServer_create(&iedModel);

	/* MMS server will be instructed to start listening to client connections. */
	IedServer_start(iedServer, port_number);

    /* Don't allow access to SP variables by default */    
    IedServer_setWriteAccessPolicy(iedServer, IEC61850_FC_SP, ACCESS_POLICY_DENY);

    /* Allow access to DC variable */
    IedServer_setWriteAccessPolicy(iedServer, IEC61850_FC_DC, ACCESS_POLICY_ALLOW);

	/* Instruct the server that we will be informed if a clients writes to a
	 * certain variables we are interested in.
	 */
	IedServer_handleWriteAccess(iedServer, IEDMODEL_GenericIO_GGIO1_NamPlt_vendor, writeAccessHandler, NULL);

	if (!IedServer_isRunning(iedServer)) {
		printf("Starting server failed! Exit.\n");
		IedServer_destroy(iedServer);
		exit(-1);
	}

	running = 1;

	signal(SIGINT, sigint_handler);

	while (running) {
		Thread_sleep(1);
	}

	/* stop MMS server - close TCP server socket and all client sockets */
	IedServer_stop(iedServer);

	/* Cleanup - free all resources */
	IedServer_destroy(iedServer);
} /* main() */
