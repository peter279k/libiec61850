/*
 *  no_tls_server_example.c
 *
 *  How to configure a no TLS server
 */

#include "iec61850_server.h"
#include "hal_thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <curl/curl.h>

#include "static_model.h"

#define MAX_CHAR_SIZE 300

static char INVERTER_GET_INFO[MAX_CHAR_SIZE];
static char INVERTER_GET_STATUS[MAX_CHAR_SIZE];
static char INVERTER_SET[MAX_CHAR_SIZE];

struct memory_struct {
    char *memory;
    size_t size;
};

/* import IEC 61850 device model created from SCL-File */
extern IedModel iedModel;

static int running = 0;
static IedServer iedServer = NULL;

static size_t receive_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct memory_struct *mem = (struct memory_struct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        /* out of memory! */
        fprintf(stderr, "receive_callback(): not enough memory!\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

void fetch_inverter_info() {
    CURL *curl = curl_easy_init();
    CURLcode res;
    struct memory_struct chunk;

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 

    if (!curl) {
        fprintf(stderr, "libcurl is not loaded correctly!\n");
    }

    // set cURL setting
    curl_easy_setopt(curl, CURLOPT_URL, INVERTER_GET_INFO);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receive_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK) {
        fprintf(stderr, "fetch_inverter_info(): curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
        fflush(stdout);
        printf("repsonse string: %s\n", chunk.memory);
        fflush(stdout);
    }

    curl_easy_cleanup(curl);
    free(chunk.memory);
    curl_global_cleanup();
    curl = NULL;

    printf("response string: %s\n", response_string);
    fflush(stdout);
    printf("response header string: %s\n", header_string);
    fflush(stdout);
}

void fetch_inverter_status() {
}

void send_inverter_set() {
}

void read_config_file() {
    strcpy(INVERTER_GET_INFO, "");
    strcpy(INVERTER_GET_STATUS, "");
    strcpy(INVERTER_SET, "");

    char config_path[1000] = "/home/iec61850/config";
    FILE *fp;
    char buffer1[250], buffer2[250];

    fp = fopen(config_path, "rb");
    if (!fp) {
        fprintf(stderr, "Cannot open %s file", config_path);
        exit(1);
    }

    if(fgets(buffer1, sizeof(buffer1), fp)) {
        if(1 == sscanf(buffer1, "inverter_on_off_api=%s", buffer2)) {
            strcpy(INVERTER_SET, buffer2);
        }
    }

    if(fgets(buffer1, sizeof(buffer1), fp)) {
        if(1 == sscanf(buffer1, "inverter_status_api=%s", buffer2)) {
            strcpy(INVERTER_GET_STATUS, buffer2);
        }
    }

    if(fgets(buffer1, sizeof(buffer1), fp)) {
        if(1 == sscanf(buffer1, "inverter_info_api=%s", buffer2)) {
            strcpy(INVERTER_GET_INFO, buffer2);
        }
    }

    fclose(fp);
}

void
sigint_handler(int signalId)
{
    running = 0;
}

static MmsDataAccessError
writeAccessHandler (DataAttribute* dataAttribute, MmsValue* value, ClientConnection connection, void* parameter)
{
    if (dataAttribute == IEDMODEL_GenericIO_GGIO1_NamPlt_vendor) {
        char* newValue = MmsValue_toString(value);
        printf("New value for OutVarSet_setMag_f = %s\n", newValue);
        fflush(stdout);
        return DATA_ACCESS_ERROR_SUCCESS;
    }

    return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
}

static ControlHandlerResult
controlHandlerForBinaryOutput(ControlAction action, void* parameter, MmsValue* value, bool test)
{
    if (test)
        return CONTROL_RESULT_FAILED;

    if (MmsValue_getType(value) == MMS_BOOLEAN) {
        printf("received binary control command: ");
        fflush(stdout);

        if (MmsValue_getBoolean(value)) {
            printf("on\n");
            fflush(stdout);
        }
        else {
            printf("off\n");
            fflush(stdout);
        }
    }
    else
        return CONTROL_RESULT_FAILED;

    uint64_t timeStamp = Hal_getTimeInMs();

    if (parameter == IEDMODEL_GenericIO_GGIO1_SPCSO1) {
        IedServer_updateUTCTimeAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_SPCSO1_t, timeStamp);
        IedServer_updateAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_SPCSO1_stVal, value);
    }

    if (parameter == IEDMODEL_GenericIO_GGIO1_SPCSO2) {
        IedServer_updateUTCTimeAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_SPCSO2_t, timeStamp);
        IedServer_updateAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_SPCSO2_stVal, value);
    }

    if (parameter == IEDMODEL_GenericIO_GGIO1_SPCSO3) {
        IedServer_updateUTCTimeAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_SPCSO3_t, timeStamp);
        IedServer_updateAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_SPCSO3_stVal, value);
    }

    if (parameter == IEDMODEL_GenericIO_GGIO1_SPCSO4) {
        IedServer_updateUTCTimeAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_SPCSO4_t, timeStamp);
        IedServer_updateAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_SPCSO4_stVal, value);
    }

    return CONTROL_RESULT_OK;
}

static void
connectionHandler (IedServer self, ClientConnection connection, bool connected, void* parameter)
{
    if (connected) {
        printf("Connection opened\n");
        fflush(stdout);
    }
    else {
        printf("Connection closed\n");
        fflush(stdout);
    }
}

static void
printAppTitle(ItuObjectIdentifier* oid)
{
    int i;

    for (i = 0; i < oid->arcCount; i++) {
        printf("%i", oid->arc[i]);
        fflush(stdout);

        if (i != (oid->arcCount - 1)) {
            printf(".");
            fflush(stdout);
        }
    }
}

static bool
clientAuthenticator(void* parameter, AcseAuthenticationParameter authParameter, void** securityToken, IsoApplicationReference* appRef)
{
    printf("ACSE Authenticator:\n");
    fflush(stdout);
    
    printf("  client ap-title: "); printAppTitle(&(appRef->apTitle)); printf("\n");
    fflush(stdout);

    printf("  client ae-qualifier: %i\n", appRef->aeQualifier);
    fflush(stdout);

    printf("  auth-mechanism: %i\n", authParameter->mechanism);
    fflush(stdout);

    return true;
}

int
main(int argc, char** argv)
{
    printf("Reading config file...\n");
    fflush(stdout);
    read_config_file();

    printf("Read config file: first line is: %s\n", INVERTER_SET);
    fflush(stdout);

    printf("Read config file: second line is: %s\n", INVERTER_GET_STATUS);
    fflush(stdout);

    printf("Read config file: third line is: %s\n", INVERTER_GET_INFO);
    fflush(stdout);

    fetch_inverter_info();

    int port_number = 8102;
    if (argc > 1)
        port_number = atoi(argv[1]);

    printf("Using libIEC61850 version %s\n", LibIEC61850_getVersionString());
    fflush(stdout);

    printf("libIEC61850 IedServer server will listen on %d\n", port_number);
    fflush(stdout);

    // Create Model IED
    IedModel* myModel = IedModel_create("inverterModel");

    // Create Logical Device
    LogicalDevice* lDevice1 = LogicalDevice_create("SENSORS", myModel);

    // Create Logical Node
    LogicalNode* ttmp1 = LogicalNode_create("TTMP1", lDevice1);

    // Create Model Node
    CDC_ASG_create("TmpSp", (ModelNode*) ttmp1, 0, false);
    CDC_VSG_create("TmpSt", (ModelNode*) ttmp1, 0);

    // Create Data Object
    DataObject* do1 = DataObject_create("Temp1", (ModelNode*) ttmp1, 0);

    // Create Data Attribute
    DataAttribute* fl = DataAttribute_create("float", (ModelNode*) do1, IEC61850_FLOAT64, IEC61850_FC_MX, 0, 0, 0);
    DataAttribute* st = DataAttribute_create("string", (ModelNode*) do1, IEC61850_VISIBLE_STRING_255, IEC61850_FC_DC,0, 0, 0);

    // Create Server Connection
    iedServer = IedServer_create(&iedModel);

    // Install writer handler
    IedServer_handleWriteAccess(iedServer, IEDMODEL_GenericIO_GGIO1_NamPlt_vendor, writeAccessHandler, NULL);

    IedServer_setAuthenticator(iedServer, clientAuthenticator, NULL);

    IedServer_setWriteAccessPolicy(iedServer, IEC61850_FC_DC, ACCESS_POLICY_ALLOW);

    /* MMS server will be instructed to start listening to client connections. */
    IedServer_start(iedServer, port_number);

    if (!IedServer_isRunning(iedServer)) {
        fprintf(stderr, "Starting server failed! Exit.\n");
        IedServer_destroy(iedServer);
        exit(-1);
    }

    running = 1;

    signal(SIGINT, sigint_handler);

    float t = 0.f;

    while (running) {
        uint64_t timestamp = Hal_getTimeInMs();

        t += 0.1f;

        float watt = 258.1;
        float volt = 230.5;
        float electric = 1.12;
        float an4 = sinf(t + 3.f);

        IedServer_lockDataModel(iedServer);

        Timestamp iecTimestamp;

        Timestamp_clearFlags(&iecTimestamp);
        Timestamp_setTimeInMilliseconds(&iecTimestamp, timestamp);
        Timestamp_setLeapSecondKnown(&iecTimestamp, true);

        /* toggle clock-not-synchronized flag in timestamp */
        if (((int) t % 2) == 0)
            Timestamp_setClockNotSynchronized(&iecTimestamp, true);

        IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_AnIn1_t, &iecTimestamp);
        IedServer_updateFloatAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_AnIn1_mag_f, watt);

        IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_AnIn2_t, &iecTimestamp);
        IedServer_updateFloatAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_AnIn2_mag_f, volt);

        IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_AnIn3_t, &iecTimestamp);
        IedServer_updateFloatAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_AnIn3_mag_f, electric);

        IedServer_updateTimestampAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_AnIn4_t, &iecTimestamp);
        IedServer_updateFloatAttributeValue(iedServer, IEDMODEL_GenericIO_GGIO1_AnIn4_mag_f, an4);

        IedServer_unlockDataModel(iedServer);

        Thread_sleep(100);
    }

    /* stop MMS server - close TCP server socket and all client sockets */
    IedServer_stop(iedServer);

    /* Cleanup - free all resources */
    IedServer_destroy(iedServer);

} /* main() */
