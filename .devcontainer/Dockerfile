FROM mcr.microsoft.com/devcontainers/base:ubuntu-24.04

ARG SMING_REPO=https://github.com/SmingHub/Sming
ARG SMING_BRANCH=6.0.0
ARG INSTALL_ARGS=all

RUN git clone $SMING_REPO -b $SMING_BRANCH /opt/sming

RUN /opt/sming/Tools/install.sh ${INSTALL_ARGS}
RUN echo ". /opt/sming/Tools/export.sh" >> ~/.bashrc
RUN echo "pnpm config set store-dir ~/.pnpm-store" >> ~/.bashrc