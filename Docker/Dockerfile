############################################################
#
# Icarous CFS SBD Installation
#
############################################################

FROM ubuntu:16.04
LABEL maintainer.name="Marco A. Feliu" \
      maintainer.email="marco.feliu@nianet.org" \
      version="0.0"


############################################################
#
# Ubuntu dependencies installation
#
############################################################

RUN dpkg --add-architecture i386
RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update \
    && apt-get upgrade -yq \
    && apt-get install -yq --no-install-recommends \
       build-essential \
       libc6-dev-i386 \
       g++-multilib \
       sudo \
       wget \
       ca-certificates \
       openjdk-8-jdk \
       cmake \
       zlib1g-dev \
       tcpd:i386 \
       git 


############################################################
# Download ICAROUS

RUN git clone https://github.com/nasa/icarous.git /icarous
RUN cd /icarous && git submodule update --init --recursive

############################################################
# Install wrap library

RUN cd /lib/i386-linux-gnu && ln -s libwrap.so.0  libwrap.so


############################################################
# Build Plexil compiler

RUN cd /icarous/Modules/Plexil && \
    PLEXIL_HOME="/icarous/Modules/Plexil" \
    make plexil-compiler


############################################################
# Build ICAROUS

RUN cd /icarous/cFS && mkdir build && cd build && \
    JAVA_HOME="/usr/lib/jvm/java-8-openjdk-amd64" \
    PLEXIL_HOME="/icarous/Modules/Plexil" \
    OSAL_HOME="/icarous/cFS/cFE/osal" \
    cmake ../.. -DSITL=on
RUN cd /icarous/cFS/build && cmake --build . --target cpu1-install
