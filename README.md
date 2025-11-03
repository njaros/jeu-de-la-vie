# Easiest way to play (only on Linux) #

1- install docker and X11

2- enter command on terminal: `chmod 777 run.sh`

3- enter command on terminal: `./run.sh`

# Second way to play: compile with your dev env (Linux and Windows) #

1- install cmake >= v3.28 on your machine

2- enter command on terminal: `cmake -B build` (Install each missing dependencies if needed)

### On Linux ###

3- enter command on terminal: `cd build`

4- enter command on terminal: `make`

5- execute the binary in build/bin folder

### On Windows ###

3- enter command on terminal: `cmake --build build; .\build\bin\Debug\main.exe`

### On Mac ###

why did you buy a mac ?