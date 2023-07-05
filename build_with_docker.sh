#!/bin/bash

# git submodule update --init --recursive

r=`docker image inspect qmkfm/qmk_cli:quantizer_mini 2>/dev/null`

if [ "$r" == [] ]; then
    docker build -t qmkfm/qmk_cli:quantizer_mini .
fi

docker run \
    --rm\
    -v $PWD:/qmk_firmware\
    -v $PWD/.build:/qmk_firmware/.build\
    -it qmkfm/qmk_cli make sekigon/keyboard_quantizer/mini:tcy
