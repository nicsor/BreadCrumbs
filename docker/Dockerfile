FROM debian:buster-slim

LABEL "description"="Docker image for building and running a generic app"
LABEL "author"="nicolae.natea"

ENV CLEANUP "set -xe && apt-get autoclean && apt-get clean && rm -rf /var/lib/apt/lists/*"

# Cleanup
RUN set -xe && apt-get update && apt-get -y upgrade && ${CLEANUP}

RUN apt-get install -y make build-essential g++ cmake curl gdb gdbserver cmake && ${CLEANUP}

RUN apt-get install -y libssl-dev wget && ${CLEANUP}

RUN apt-get install -y libboost-all-dev pkg-config libgtest-dev libpthread-stubs0-dev && ${CLEANUP}

# OpenGL dependencies
RUN apt-get install -y freeglut3-dev libglfw3-dev && ${CLEANUP}

# Setting the command to launch
CMD ["bash", "-l"]
