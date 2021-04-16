#!/bin/bash

image_name=$1

if [[ $image_name == "" ]]; then
    echo "Please specify Docker image name!"
    exit 1;
fi;

archive_file_name="$(echo $image_name | sed -e "s/:/-/g").tar"
rm -f $archive_file_name
rm -f "$archive_file_name.gz"

docker save -o $archive_file_name $image_name

if [[ ! -f $archive_file_name ]]; then
    echo "Packaging $archive_file_name has been failed."
    exit 1;
fi;

echo "Archiving $archive_file_name..."

gzip -9 $archive_file_name

if [[ $? != 0 ]]; then
    echo "Archiving $archive_file_name has been failed."
    exit 1;
fi;

echo "Archiving $archive_file_name has been done."
