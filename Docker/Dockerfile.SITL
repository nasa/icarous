############################################################
#
# Icarous Software-In-The-Loop container
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

RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update \
    && apt-get upgrade -yq \
    && apt-get install -yq --no-install-recommends \
       build-essential \
       libc6-dev-i386 \
       g++-multilib \
       sudo \
       wget \
       git \
       ca-certificates \
       python-matplotlib python-serial python-wxgtk3.0 python-wxtools python-lxml python-scipy python-opencv ccache gawk python-pip python-pexpect python-dev libcanberra-gtk-module


############################################################
#
# SITL
#
############################################################

##############################
# Python libraries
RUN pip install --upgrade pip
RUN pip install setuptools
RUN pip install future


##############################
# Ardupilot
RUN git clone git://github.com/ArduPilot/ardupilot.git /ardupilot
RUN cd /ardupilot && git submodule update --init --recursive


##############################
# Mavlink
# RUN git clone https://github.com/ArduPilot/mavlink.git /tools/mavlink


##############################
# ICAROUS

RUN git clone https://github.com/nasa/icarous.git


##############################
# MAXProxy
RUN git clone https://github.com/ArduPilot/MAVProxy.git /MAVProxy


##############################
# Icarous tools

RUN cd /ardupilot/modules/mavlink/pymavlink && python setup.py install
RUN pip install MAVProxy
RUN cd /icarous/Python/CustomModules && bash SetupMavProxy.sh /

##############################
# Precompile Arducopter SITL
RUN cd /ardupilot/ArduCopter && echo | /ardupilot/Tools/autotest/sim_vehicle.py -b bin/arducopter -l 37.101783,-76.382997,0,270
