FROM ubuntu:18.04

# Install required packages
RUN apt-get update \
&& useradd iec61850 -s /bin/bash -m \
&& export DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true && apt-get install -y --no-install-recommends --no-install-suggests tzdata \
# Set timezone to Asia/Taipei
&& export TZ=Asia/Taipei \
&& ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone \
# Install required packages for building C/C++
&& apt-get update \
&& apt-get install -y \
    ssh \
    libcurl4-openssl-dev \
    libjson-c-dev libjson-c3 \
    libpcre3 libpcre3-dev \
    wget \
    git \
    unzip \
    build-essential \
    pkg-config cmake-data \
    cmake \ 
    python3 \
    python3-pip \
&& rm -rf /var/lib/apt/lists/* \
&& export DEBIAN_FRONTEND="" \
# Clone libiec61850 repository
&& git config --global http.sslVerify false \
&& git clone https://github.com/peter279k/libiec61850.git \
# Download mbedtls
&& wget --no-check-certificate https://tls.mbed.org/download/mbedtls-2.16.0-apache.tgz \
&& wget --no-check-certificate https://www.sqlite.org/2021/sqlite-amalgamation-3340100.zip \
&& tar zxvf mbedtls-2.16.0-apache.tgz \
&& unzip sqlite-amalgamation-3340100.zip \
&& mv mbedtls-2.16.0 libiec61850/third_party/mbedtls/mbedtls-2.16 \
&& mv sqlite-amalgamation-3340100/* libiec61850/third_party/sqlite/ \
# Build libiec61850
&& cd libiec61850 && cmake . && make WITH_MBEDTLS=1 \
&& cd ../ \
# Remove unued files, folders and system packages
&& apt-get clean \
&& apt-get purge -y wget git unzip build-essential cmake pkg-config cmake-data \
&& apt-get autoremove -y \
&& mv ./libiec61850/examples/no_tls_server_example/ /home/iec61850/ \
&& mv ./libiec61850/examples/no_tls_client_example/ /home/iec61850/ \
&& mv ./libiec61850/examples/tls_server_example/ /home/iec61850/ \
&& mv ./libiec61850/examples/tls_client_example/ /home/iec61850/ \
&& chown -R iec61850:iec61850 /home/iec61850/ \
&& rm -rf ./libiec61850/ \
&& rm -f mbedtls-2.16.0-apache.tgz \
&& rm -f sqlite-amalgamation-3340100.zip

USER iec61850

WORKDIR /home/iec61850/

EXPOSE 8102 8103

ENTRYPOINT ["bash"]
