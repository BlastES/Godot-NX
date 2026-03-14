FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update && \
    apt install --yes --no-install-recommends wget apt-transport-https ca-certificates gpg-agent
RUN mkdir -p /usr/local/share/keyring/ && \
    wget -O /usr/local/share/keyring/devkitpro-pub.gpg https://apt.devkitpro.org/devkitpro-pub.gpg && \
    echo "deb [signed-by=/usr/local/share/keyring/devkitpro-pub.gpg] https://apt.devkitpro.org stable main" > /etc/apt/sources.list.d/devkitpro.list
RUN apt-get update && \
    apt-get --yes --no-install-recommends install devkitpro-pacman
RUN ln -sf /proc/mounts /etc/mtab
RUN dkp-pacman -S --noconfirm --noprogressbar switch-dev switch-portlibs
ENV DEVKITPRO=/opt/devkitpro