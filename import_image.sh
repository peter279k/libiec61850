#!/bin/bash

archive_file_name=$1

if [[ $archive_file_name == "" ]]; then
    echo "Please specify Docker image name!"
    exit 1;
fi;

if [[ ! -f $archive_file_name ]]; then
    echo "$archive_file_name is not found."
    exit 1;
fi;

echo "Importing the $archive_file_name..."

docker load -i $archive_file_name

if [[ $? != 0 ]]; then
    echo "Importing the $archive_file_name has been failed."
    exit 1;
fi;

echo "Importing $archive_file_name has been done."
