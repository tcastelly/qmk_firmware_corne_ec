FROM qmkfm/qmk_cli

RUN apt-get update -y && \
    apt install -y libstdc++-arm-none-eabi-newlib

COPY .gitconfig /root/.gitconfig

VOLUME /qmk_firmware

WORKDIR /qmk_firmware

CMD qmk compile -kb all -km default
