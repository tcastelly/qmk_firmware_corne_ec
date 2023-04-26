#!/bin/bash

# git submodule update --init --recursive

r=`docker image inspect qmkfm/qmk_cli:corne_ec 2>/dev/null`

if [ "$r" == [] ]; then
    docker build -t qmkfm/qmk_cli:corne_ec .
fi

docker run -v $PWD:/qmk_firmware -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli:corne_ec make sekigon/crkbd_ec:tcy:uf2
