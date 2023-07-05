FROM qmkfm/qmk_cli

RUN apt-get update -y && \
    apt install -y libstdc++-arm-none-eabi-newlib

VOLUME /qmk_firmware

WORKDIR /qmk_firmware

RUN git config --global --add safe.directory '*'

CMD qmk compile -kb all -km default
