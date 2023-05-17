# rlm_kafka

FreeRADIUS 3.x module to send accounting messages to Kafka Queue.

## Dependency
This module is dependent on [librdkafka](https://github.com/confluentinc/librdkafka) (tested with version `1.7.0`).  So, `librdkafka` needs to be installed in the system prior to compiling and using this module.

## Compilation and Installation

The following steps assumes that you have FreeRADIUS source code in path `/usr/local/src/freeradius-server-3.0.23/`.

- Prepare some env vars to reference later
```sh
export FRADIUS_SRC="/usr/local/src/freeradius-server-3.0.23/"
```

- Clone the repo to FreeRADIUS source path inside `src/modules/`.    
```sh
cd $FRADIUS_SRC/src/modules/ && git clone https://github.com/thebinary/rlm_kafka.git
```

- Move back to the root of FreeRADIUS source code path
```sh
cd $FRADIUS_SRC && ./configure --with-modules=rlm_kafka
```

- Compile and Install
```sh
make && make install
```