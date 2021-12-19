# project-lyd

Det bedste projekt

## Project layout

### Library
`/lib`

`lib` contains the `libdtmf` library that the `slaveapp` and `masterapp` makes use of. 
They depend on this library.

#### Application layer
`/lib/layer-application`

Application layer of `libdtmf`. 
This is the layer that will be exposed to the applications using it.

#### Data Transfer Layer 
`/lib/layer-data-transfer`

This layer turns a received sequence of bits into a frame that the application layer can work with. 
Likewise it turns a frame that has to be sent into bits that the physical layer turn into sounds.

#### DTMF Layer
`/lib/layer-physical`

This layer is the physical layer. 
It records incoming sounds and transforms them into data that the data transfer layer can work with.
It also turns data from the data transport layer into sounds that it can play via speakers.

### Master application
`/masterapp`

The master application is supposed to be on a PC where it can control what the slave application should do.

### Slave application
`slaveapp`

The slave application resides on the robot and responds to requests coming from the master application. 
These requests may be actions to perform or inquiry about data that is of interest for the master application.

---
## How to build

### libdtmf

Change directory to `lib`
```bash
cd lib
```
Ready the build configuration
```bash
mkdir -p build && cmake ..
```
Then compile the library `libdtmf`
```
make -j$(nproc)
```
To install the library on the current machine, do*
```bash
sudo make install
```
(*) This is not required for development.

### Master application
Since the master application depends on `libdtmf`, a script `create-master-app.sh` has been made.
This will prepare all dependencies from `libdtmf` and compile the application in the `masterapp/build` directory.

To update the app without recompiling `libdtmf` (after running `create-master-app.sh`):
```bash
cd masterapp/build
```
When in the correct `build` directory
```bash
make -j$(nproc)
```
To run the application
```bash
./masterapp
```

### Slave application
The steps here are almost equivalent to the steps for the `masterapp`.
Since the slave application also depends on `libdtmf`, a script `create-slave-app.sh` has been made.
This will prepare all dependencies from `libdtmf` and compile the application in the `slaveapp/build` directory.

By default the slave application is compiled for `TEST_MODE`. This can be changed in `create-slave-app.sh`.

To update the app without recompiling `libdtmf` (after running `create-slave-app.sh`):
```bash
cd slaveapp/build
```
When in the correct `build` directory
```bash
make -j$(nproc)
```
To run the application
```bash
./slaveapp
```
