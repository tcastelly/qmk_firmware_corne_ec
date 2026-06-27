FROM qmkfm/qmk_cli

RUN apt-get update -y && \
    apt install -y libstdc++-arm-none-eabi-newlib

COPY .gitconfig /root/.gitconfig
COPY requirements.txt /tmp/requirements.txt
RUN /opt/uv/tools/qmk/bin/python3 -m pip install -r /tmp/requirements.txt

VOLUME /qmk_firmware

WORKDIR /qmk_firmware

CMD ["qmk", "compile", "-kb", "all", "-km", "default"]
