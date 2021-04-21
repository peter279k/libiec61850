#!/bin/bash

green_color='\e[0;32m'
red_color='\e[0;31m'
yellow_color='\e[0;33m'
rest_color='\e[0m'

mode=$1
ip=$2
write_data_attr="inverter_status"

if [[ $write_data_attr == "" ]]; then
    echo -e "${red_color}Please fill the data attribute string${rest_color}"
    exit 1;
fi;

echo -e "${yellow_color}The inverter status loop has been started.${rest_color}"
echo -e "${yellow_color}It will poll this program every 15 seconds.${rest_color}"
echo -e "${yellow_color}Please press ctrl+C to terminate this loop.${rest_color}"

running=1
while [[ $running == 1 ]];
do
    if [[ $mode == "no_tls" ]]; then
        docker exec "$mode-iec61850-server" bash -c "cd no_tls_client_example/ && ./no_tls_client_example $ip 8102 $write_data_attr"
    else
        docker exec "$mode-iec61850-server" bash -c "cd tls_client_example/ && ./tls_client_example $ip 8102 $write_data_attr"
    fi;

    if [[ $? != 0 ]]; then
        echo -e "${red_color}The $mode-iec61850-server container is failed to run.${rest_color}"
        exit 1;
    fi;

    sleep 15;
done;
