FROM debian

RUN apt-get update && apt-get install -y \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    git \
    build-essential \
    cmake \
    libgl1-mesa-dev \
    libfreetype6-dev \
    libudev-dev \
    libogg-dev \
    libvorbis-dev \
    libflac-dev

WORKDIR /app

COPY . .

RUN cmake -B build && \
    cd build && \
    make

ENTRYPOINT ["./build/bin/main"]