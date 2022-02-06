FROM debian:bullseye

LABEL "description"="Docker image for generating git statistics"
LABEL "author"="nicolae.natea"

RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get -y install git python2 python-setuptools

RUN mkdir -p /opt/bin/ && \
    cd /opt/bin/ && \
    git clone https://github.com/ejwa/gitinspector.git

ENTRYPOINT ["/usr/bin/python2", "/opt/bin/gitinspector/gitinspector.py"]
