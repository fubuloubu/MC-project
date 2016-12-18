FROM ubuntu:14.04
USER root
RUN apt-get update && \
    apt-get install -y \
        linux-headers-$(uname -r) \
        build-essential
USER llbmc
