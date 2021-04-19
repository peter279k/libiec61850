#!/bin/bash

green_color='\e[0;32m'
red_color='\e[0;31m'
yellow_color='\e[0;33m'
rest_color='\e[0m'

mode=$1
ip=$2

docker exec "$mode-iec61850-server" bash -c "cd no_tls_client_example/ && ./no_tls_client_example $ip 8102 test_string2"

if [[ $? != 0 ]]; then
    echo -e "${red_color}The $mode-iec61850-server container is failed to run.${rest_color}"
    exit 1;
fi;
