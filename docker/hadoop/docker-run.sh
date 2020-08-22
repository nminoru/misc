#! /bin/sh

sudo docker network create mynetwork

sudo docker run -ti \
     --net   mynetwork \
     --tmpfs /run \
     --tmpfs /run/lock \
     -v /sys/fs/cgroup:/sys/fs/cgroup:ro \
     -p 50010:50010 \
     -p 50020:50020 \
     -p 50070:50070 \
     -p 50075:50075 \
     -p 50090:50090 \
     -p 8020:8020 \
     -p 9000:9000 \
     --name hdfs-server \
     nminoru/hadoop
