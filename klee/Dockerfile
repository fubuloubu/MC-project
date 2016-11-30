FROM klee/klee:latest
USER root
RUN apt-get update && \
    apt-get install -y linux-headers-$(uname -r)
USER klee
