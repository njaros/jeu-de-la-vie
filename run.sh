docker build -t jdlv .
docker run -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY jdlv