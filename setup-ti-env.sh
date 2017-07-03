#!/bin/bash
if [ "$0" = "$BASH_SOURCE" ]; then
    echo "Script needs to be sourced."
    exit 1
fi

export PRU_CGT=~/prg/ti-cgt-pru_2.1.4
export PRU_LIB=~/prg/ti-processor-sdk-linux-am335x-evm-03.03.00.04/example-applications/pru-icss-5.1.0
