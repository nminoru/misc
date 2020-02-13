#! /bin/sh

sudo docker run -ti \
     --tmpfs /run \
     --tmpfs /run/lock \
     -v /sys/fs/cgroup:/sys/fs/cgroup:ro \
     --name localdns-bind \
     localdns-bind
