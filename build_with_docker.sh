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
    make sekigon/crkbd_ec:tcy:uf2

docker run \
    --rm \
    -v $PWD:/qmk_firmware \
    -v $PWD/.build:/qmk_firmware/.build\
    -ti qmkfm/qmk_cli:corne_ec\
    make mc2s/rev1:default

docker run \
    --rm \
    -v $PWD:/qmk_firmware \
    -v $PWD/.build:/qmk_firmware/.build\
    -ti qmkfm/qmk_cli:corne_ec\
    make mc2s/rev1:tcy
