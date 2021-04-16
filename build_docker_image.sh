#!/bin/bash

green_color='\e[0;32m'
red_color='\e[0;31m'
yellow_color='\e[0;33m'
rest_color='\e[0m'
tag_name=$1

if [[ $tag_name == "--help" || $tag_name == "help" ]]; then
    echo -e "${green_color}Usage: ./build_docker_image.sh tag_name${rest_color}"
    echo -e "${green_color}Usage: ./build_docker_image.sh${rest_color}"
    exit 0;
fi;

if [[ $tag_name == "" ]]; then
    echo -e "${yellow_color}The tage name is empty and it will not run docker tag!${rest_color}"
fi;


echo -e "${green_color}Building Docker image is started....${rest_color}"

docker build -t libiec61850 . --no-cache

if [[ $? != 0 ]]; then
    echo -e "${red_color}Building Docker image is failed...${rest_color}"
    exit 1;
fi

echo -e "${green_color}Building Docker image is done....${rest_color}"

if [[ $tag_name != "" ]]; then
    echo -e "${yellow_color}The tage name is ${tag_name} and it will run docker tag!${rest_color}"
    docker tag libiec61850:latest libiec61850:"$tag_name"
fi;

echo -e "${green_color}Running Docker tag command is done....${rest_color}"
