FROM debian:buster-slim

LABEL "description"="Docker image for building and running a generic app"
LABEL "author"="nicolae.natea"

ENV CLEANUP "set -xe && apt-get autoclean && apt-get clean && rm -rf /var/lib/apt/lists/*"

# Cleanup
RUN set -xe && apt-get update && apt-get -y upgrade && ${CLEANUP}

RUN apt-get install -y make build-essential g++ cmake curl gdb gdbserver cmake && ${CLEANUP}

RUN apt-get install -y libssl-dev wget && ${CLEANUP}

RUN apt-get install -y libboost-all-dev pkg-config libgtest-dev libpthread-stubs0-dev && ${CLEANUP}

# Built gtest_main
RUN cd /usr/src/gtest/ && cmake -DBUILD_SHARED_LIBS=ON && make && cp *.so /usr/lib && ${CLEANUP}

# Setting the command to launch
CMD ["bash", "-l"]
