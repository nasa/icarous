############################################################
#
# Icarous Base container
#
############################################################

FROM ubuntu:16.04
MAINTAINER Swee Balachandran (swee.balachandran@nianet.org)
LABEL icarous-base version="1.0"

############################################################
#
# Ubuntu dependencies installation
#
############################################################

RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update \
    && apt-get upgrade -yq \
    && apt-get install -yq --no-install-recommends \
       build-essential \
       g++-multilib \
       sudo \
       wget \
       ca-certificates \
       openjdk-8-jdk \
       cmake \
       zlib1g-dev \
       git \
       gdb \
       python \
       vim \
       emacs \
       python-matplotlib \
       python-serial \
       python-wxgtk3.0 \
       python-wxtools \
       python-lxml \
       python-scipy \
       python-opencv \
       ccache \
       gawk \
       python-pip \
       python-pexpect \
       python-dev \
       libcanberra-gtk-module \ 
       ruby-full 
        
RUN gem install jekyll bundler

# Python libraries
RUN pip install --upgrade pip
RUN pip install setuptools
RUN pip install future

RUN git config --global url."https://".insteadOf git://

##############################
# Ardupilot
RUN git clone --recurse-submodules git://github.com/ArduPilot/ardupilot.git /ardupilot

##############################
# Mavlink
RUN git clone --recurse-submodules https://github.com/ArduPilot/mavlink.git mavlink

##############################
# MAXProxy
RUN git clone https://github.com/ArduPilot/MAVProxy.git /MAVProxy
RUN cd /MAVProxy && python setup.py install

#############################
# PolyCARP
RUN git clone https://github.com/NASA/PolyCARP.git /PolyCARP
RUN export PYTHONPATH=$PYTHONPATH:/PolyCARP/Python

############################################################
# Download ICAROUS
RUN git clone --recurse-submodules https://github.com/nasa/icarous.git /icarous


