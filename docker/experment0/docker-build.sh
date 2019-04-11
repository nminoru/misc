#! /bin/sh

# sudo docker build --rm --sysctl net.core.somaxconn=1024 --ulimit sigpending=62793 --ulimit nproc=131072 --ulimit nofile=60000 --ulimit core=0 -t nminoru/centos7-systemd .
sudo docker build --rm -t nminoru/centos7-systemd .
