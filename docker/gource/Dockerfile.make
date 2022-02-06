FROM debian:bullseye

RUN apt-get update && apt-get -y upgrade
RUN apt-get update && apt-get -y install build-essential autoconf automake cmake libtool git

RUN apt-get -y install build-essential autoconf automake cmake libtool git \      
    checkinstall nasm yasm libass-dev libfreetype6-dev libsdl2-dev p11-kit \           
    libva-dev libvdpau-dev libvorbis-dev libxcb1-dev libxcb-shm0-dev \                 
    libxcb-xfixes0-dev pkg-config texinfo wget zlib1g-dev libchromaprint-dev \         
    frei0r-plugins-dev gnutls-dev ladspa-sdk libcaca-dev libcdio-paranoia-dev \        
    libcodec2-dev libfontconfig1-dev libfreetype6-dev libfribidi-dev libgme-dev \      
    libgsm1-dev libjack-dev libmodplug-dev libmp3lame-dev libopencore-amrnb-dev \      
    libopencore-amrwb-dev libopenjp2-7-dev libopenmpt-dev libopus-dev \                
    libpulse-dev librsvg2-dev librubberband-dev librtmp-dev libshine-dev \             
    libsmbclient-dev libsnappy-dev libsoxr-dev libspeex-dev libssh-dev \               
    libtesseract-dev libtheora-dev libtwolame-dev libv4l-dev libvo-amrwbenc-dev \      
    libvorbis-dev libvpx-dev libwavpack-dev libwebp-dev libx264-dev libx265-dev \      
    libxvidcore-dev libxml2-dev libzmq3-dev libzvbi-dev liblilv-dev \    
    libopenal-dev opencl-dev libjack-dev

RUN mkdir -p /opt/src/aom && \
    cd /opt/src/aom && \
    git clone https://aomedia.googlesource.com/aom && \
    cmake aom/ -DBUILD_SHARED_LIBS=1 && \
    make && \
    make install && \
    cd / && \
    rm -rf /opt/src/aom

RUN apt-get install -y libcunit1 libcunit1-doc libcunit1-dev

RUN mkdir -p /opt/src/mysopha && \
    cd /opt/src/mysopha && \
    git clone https://github.com/hoene/libmysofa && \
    mkdir -p /opt/src/mysopha/libmysofa/build && \
    cd /opt/src/mysopha/libmysofa/build && \
    cmake .. && \
    make && \
    make install && \
    cd / && \
    rm -rf /opt/src/mysopha

RUN apt-get -y install libbluray-dev
RUN mkdir -p /opt/src/libfdk-aac && \
    cd /opt/src/libfdk-aac && \
    wget http://ftp.de.debian.org/debian/pool/non-free/f/fdk-aac/libfdk-aac2_2.0.1-1_amd64.deb && \
    dpkg -i libfdk-aac2_2.0.1-1_amd64.deb && \
    wget http://ftp.de.debian.org/debian/pool/non-free/f/fdk-aac/libfdk-aac-dev_2.0.1-1_amd64.deb && \
    dpkg -i libfdk-aac-dev_2.0.1-1_amd64.deb && \
    cd / && \
    rm -rf /opt/src/libfdk-aac

RUN apt-get install -y libdrm-dev

RUN mkdir -p /opt/src/ffmpeg && \
    cd /opt/src/ffmpeg && \
    wget https://ffmpeg.org/releases/ffmpeg-5.0.tar.bz2 && \
    tar -xf ffmpeg-5.0.tar.bz2 && \
    rm ffmpeg-5.0.tar.bz2 && \
    cd ffmpeg-5.0 && \
    ./configure --enable-gpl --enable-version3 --disable-static --enable-shared --enable-small --enable-chromaprint --enable-frei0r --enable-gmp --enable-gnutls --enable-ladspa --enable-libaom --enable-libass --enable-libcaca --enable-libcdio --enable-libcodec2 --enable-libfontconfig --enable-libfreetype --enable-libfribidi --enable-libgme --enable-libgsm --enable-libjack --enable-libmodplug --enable-libmp3lame --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libopencore-amrwb --enable-libopenjpeg --enable-libopenmpt --enable-libopus --enable-libpulse --enable-librsvg --enable-librubberband --enable-librtmp --enable-libshine --enable-libsmbclient --enable-libsnappy --enable-libsoxr --enable-libspeex --enable-libssh --enable-libtesseract --enable-libtheora --enable-libtwolame --enable-libv4l2 --enable-libvo-amrwbenc --enable-libvorbis --enable-libvpx --enable-libwebp --enable-libx264 --enable-libx265 --enable-libxvid --enable-libxml2 --enable-libzmq --enable-libzvbi --enable-lv2 --enable-libmysofa --enable-openal --enable-opencl --enable-opengl --enable-libdrm && \
    make && \
    make install && \
    cd / && \
    rm -rf /opt/src/ffmpeg

# skip --enable-libwavpack  --enable-avisynth

RUN apt-get install -y gource
RUN apt-get install -y xvfb screen

ADD runner.sh /home/runner.sh
ENTRYPOINT ["/home/runner.sh"]
