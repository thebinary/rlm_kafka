# rlm_kafka
## Metadata
<dl>
  <dt>category</dt><dd>io</dd>
</dl>

## Summary
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

1. Case-1: If you are installing all FreeRADIUS from the same source directory
```sh
make && make install
```

2. Case-2: If you are just installing the module to already installed FreeRADIUS instance. If the `rlm` modules are located in `/usr/lib/freeradius`
```sh
export RLM_MODULES_DIR=/usr/lib/freeradius
make && cp build/lib/.libs/rlm_kafka.so $RLM_MODULES_DIR/
```