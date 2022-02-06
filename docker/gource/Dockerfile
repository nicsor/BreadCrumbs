FROM debian:bullseye

LABEL "description"="Docker image for generating a gource video"
LABEL "author"="nicolae.natea"

RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get -y install ffmpeg gource xvfb screen git

ADD runner.sh /home/runner.sh
ENTRYPOINT ["/home/runner.sh"]
