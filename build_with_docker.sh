#!/bin/bash

# git submodule update --init --recursive

r=`docker image inspect qmkfm/qmk_cli:corne_ec 2>/dev/null`

if [ "$r" == [] ]; then
    docker build -t qmkfm/qmk_cli:corne_ec .
fi

docker run \
    --rm \
    -v $PWD:/qmk_firmware \
    -v $PWD/.build:/qmk_firmware/.build\
    -ti qmkfm/qmk_cli:corne_ec\
    make SKIP_GIT=1 QMK_BIN=bin/qmk sekigon/crkbd_ec:tcy:uf2
