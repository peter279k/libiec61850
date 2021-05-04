/*
 * tls_client_exmaple.c
 *
 * This example shows how to configure TLS
 */

#define _GNU_SOURCE

#include "iec61850_client.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "hal_thread.h"

#include "sqlite3.h"

void
reportCallbackFunction(void* parameter, ClientReport report)
{
    MmsValue* dataSetValues = ClientReport_getDataSetValues(report);

    printf("received report for %s\n", ClientReport_getRcbReference(report));

    int i;
    for (i = 0; i < 4; i++) {
        ReasonForInclusion reason = ClientReport_getReasonForInclusion(report, i);

        if (reason != IEC61850_REASON_NOT_INCLUDED) {
            printf("  GGIO1.SPCSO%i.stVal: %i (included for reason %i)\n", i,
                MmsValue_getBoolean(MmsValue_getElement(dataSetValues, i)), reason);
        }
    }
}

int create_sqlite3_table() {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open_v2("/home/iec61850/databases/tls_libiec61850.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *sql = "CREATE TABLE IF NOT EXISTS inverter_info(id INTEGER PRIMARY KEY AUTOINCREMENT, watt TEXT, volt TEXT, electric TEXT, measured_date_time TEXT);"
                "CREATE TABLE IF NOT EXISTS writing_attribute_log(id INTEGER PRIMARY KEY AUTOINCREMENT, attribute TEXT, created_date_time TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);

        return 1;
    }

    sqlite3_close(db);
    return 0;
}

int insert_inverter_value(char *insert_reading_sql) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open_v2("/home/iec61850/databases/tls_libiec61850.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    rc = sqlite3_exec(db, insert_reading_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_close(db);

    return 0;
}

int insert_writing_attr(char insert_attr_sql[]) {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open_v2("/home/iec61850/databases/tls_libiec61850.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    rc = sqlite3_exec(db, insert_attr_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_close(db);

    return 0;
}

char* get_current_datetime() {
    int hours, minutes, seconds, day, month, year;
    char *datetime_str;
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    hours = local->tm_hour;
    hours = hours - 12;
    if (hours < 0) {
        hours += 12;
    }
    minutes = local->tm_min;
    seconds = local->tm_sec;

    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;

    asprintf(&datetime_str, "%02d-%02d-%0d %02d:%02d:%02d", year, month, day, hours, minutes, seconds);

    return datetime_str;
}

int main(int argc, char** argv) {

    printf("SQLite3 version is: %s\n", sqlite3_libversion());
    printf("Creating SQLite3 reading_value and writing_data_attribute tables....\n");
    int table_result = create_sqlite3_table();
    if (table_result != 0) {
        printf("Creating Table has been failed!\n");
        return 1;
    }

    char* hostname;
    char* attribute_string;
    int port_number = -1;

    if (argc > 3) {
        hostname = argv[1];
        port_number = atoi(argv[2]);
        attribute_string = argv[3];
    }
    else {
        hostname = "localhost";
        port_number = 8102;
        attribute_string = "libiec61850_itri_tls";
    }

    TLSConfiguration tlsConfig = TLSConfiguration_create();

    TLSConfiguration_setChainValidation(tlsConfig, true);
    TLSConfiguration_setAllowOnlyKnownCertificates(tlsConfig, false);

    if (!TLSConfiguration_setOwnKeyFromFile(tlsConfig, "client1-key.pem", NULL)) {
        printf("ERROR: Failed to load private key!\n");
        return 0;
    }

    if (!TLSConfiguration_setOwnCertificateFromFile(tlsConfig, "client1.cer")) {
        printf("ERROR: Failed to load own certificate!\n");
        return 0;
    }

    if (!TLSConfiguration_addCACertificateFromFile(tlsConfig, "root.cer")) {
        printf("ERROR: Failed to load root certificate\n");
        return 0;
    }

    IedClientError error;

    IedConnection con = IedConnection_createWithTlsSupport(tlsConfig);

    IedConnection_connect(con, &error, hostname, port_number);

    if (error == IED_ERROR_OK) {

        LinkedList serverDirectory = IedConnection_getServerDirectory(con, &error, false);

        if (error != IED_ERROR_OK)
            printf("failed to read server directory (error=%i)\n", error);

        if (serverDirectory)
            LinkedList_destroy(serverDirectory);

        /* read an analog measurement watt value from server */
        MmsValue* value = IedConnection_readObject(con, &error, "simpleIOGenericIO/GGIO1.AnIn1.mag.f", IEC61850_FC_MX);

        char watt_fval_str[100];
        char volt_fval_str[100];
        char electric_fval_str[100];


        if (value != NULL) {
            float watt_val = MmsValue_toFloat(value);
            printf("read watt float value: %0.1f\n", watt_val);
            printf("Try to store reading value...\n");
            printf("Today Date time is: %s\n", get_current_datetime());

            gcvt(watt_val, 6, watt_fval_str);
            MmsValue_delete(value);
        }
 
        /* read an analog measurement volt value from server */
        value = IedConnection_readObject(con, &error, "simpleIOGenericIO/GGIO1.AnIn2.mag.f", IEC61850_FC_MX);

        if (value != NULL) {
            float volt_val = MmsValue_toFloat(value);
            printf("read volt float value: %0.1f\n", volt_val);
            printf("Try to store reading value...\n");
            printf("Today Date time is: %s\n", get_current_datetime());

            gcvt(volt_val, 6, volt_fval_str);
            MmsValue_delete(value);
        }

        /* read an analog measurement electric value from server */
        value = IedConnection_readObject(con, &error, "simpleIOGenericIO/GGIO1.AnIn3.mag.f", IEC61850_FC_MX);

        if (value != NULL) {
            float electric_val = MmsValue_toFloat(value);
            printf("read electric float value: %0.1f\n", electric_val);
            printf("Try to store reading value...\n");
            printf("Today Date time is: %s\n", get_current_datetime());

            gcvt(electric_val, 6, electric_fval_str);
            MmsValue_delete(value);
        }

        /* Insert watt, volt and electric values to inverter_info */
        char *insert_reading_sql;
        asprintf(&insert_reading_sql, "INSERT INTO inverter_info(watt, volt, electric, measured_date_time) VALUES('%s', '%s', '%s', '%s');", watt_fval_str, volt_fval_str, electric_fval_str, get_current_datetime());
        insert_inverter_value(insert_reading_sql);

        /* write a variable to the server */
        value = MmsValue_newVisibleString(attribute_string);
        IedConnection_writeObject(con, &error, "simpleIOGenericIO/GGIO1.NamPlt.vendor", IEC61850_FC_DC, value);

        if (error != IED_ERROR_OK) {
            printf("Error code=%d", error);
            printf("failed to write simpleIOGenericIO/GGIO1.NamPlt.vendor!\n");
        } else {
            printf("Writing data attribute to server has been successful!\n");
            printf("Trying to insert data attribute to SQLite writing_data_attribute table...\n");
            char *insert_attr_sql;
            asprintf(&insert_attr_sql, "INSERT INTO writing_attribute_log(attribute, created_date_time) VALUES('%s', '%s');", attribute_string, get_current_datetime());
            insert_writing_attr(insert_attr_sql);
        }

        MmsValue_delete(value);

        /* read data set */
        ClientDataSet clientDataSet = IedConnection_readDataSetValues(con, &error, "simpleIOGenericIO/LLN0.Events", NULL);

        if (clientDataSet == NULL)
            printf("failed to read dataset\n");

        /* Read RCB values */
        ClientReportControlBlock rcb =
                IedConnection_getRCBValues(con, &error, "simpleIOGenericIO/LLN0.RP.EventsRCB01", NULL);


        bool rptEna = ClientReportControlBlock_getRptEna(rcb);

        printf("RptEna = %i\n", rptEna);

        /* Install handler for reports */
        IedConnection_installReportHandler(con, "simpleIOGenericIO/LLN0.RP.EventsRCB01",
                ClientReportControlBlock_getRptId(rcb), reportCallbackFunction, NULL);

        /* Set trigger options and enable report */
        ClientReportControlBlock_setTrgOps(rcb, TRG_OPT_DATA_UPDATE | TRG_OPT_INTEGRITY | TRG_OPT_GI);
        ClientReportControlBlock_setRptEna(rcb, true);
        ClientReportControlBlock_setIntgPd(rcb, 5000);
        IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_RPT_ENA | RCB_ELEMENT_TRG_OPS | RCB_ELEMENT_INTG_PD, true);

        if (error != IED_ERROR_OK)
            printf("report activation failed (code: %i)\n", error);

        Thread_sleep(1000);

        /* trigger GI report */
        ClientReportControlBlock_setGI(rcb, true);
        IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_GI, true);

        if (error != IED_ERROR_OK)
            printf("Error triggering a GI report (code: %i)\n", error);

        Thread_sleep(60000);

        /* disable reporting */
        ClientReportControlBlock_setRptEna(rcb, false);
        IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_RPT_ENA, true);

        if (error != IED_ERROR_OK)
            printf("disable reporting failed (code: %i)\n", error);

        ClientDataSet_destroy(clientDataSet);

        ClientReportControlBlock_destroy(rcb);

        close_connection:

        IedConnection_close(con);
    }
    else {
        printf("Failed to connect to %s\n", hostname);
    }

    IedConnection_destroy(con);

    TLSConfiguration_destroy(tlsConfig);
}


