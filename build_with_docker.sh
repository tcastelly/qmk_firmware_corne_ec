#!/bin/bash

git submodule update --init --recursive

r=`docker image inspect qmkfm/qmk_cli 2>/dev/null`

if [ "$r" == [] ]; then
    docker build -t qmkfm/qmk_cli .
fi

docker run -v $PWD:/qmk_firmware -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli make sekigon/crkbd_ec:tcy:uf2
