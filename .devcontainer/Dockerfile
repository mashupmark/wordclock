FROM mcr.microsoft.com/devcontainers/base:ubuntu

RUN apt-get -y update \
    && DEBIAN_FRONTEND=noninteractive \
    TZ=Europe/Berlin \
    apt-get install -y \
    python3.10-venv \
    avr-libc

ARG SMING_REPO=https://github.com/SmingHub/Sming
ARG SMING_BRANCH=6.0.0
ARG INSTALL_ARGS=all

RUN git clone $SMING_REPO -b $SMING_BRANCH /opt/sming

RUN /opt/sming/Tools/install.sh ${INSTALL_ARGS}
RUN echo ". /opt/sming/Tools/export.sh" >> ~/.bashrc