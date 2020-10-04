#! /bin/sh

sudo docker run -ti \
     --tmpfs /run \
     --tmpfs /run/lock \
     -v /sys/fs/cgroup:/sys/fs/cgroup:ro \
     -v /home/nminoru/keystore.jks:/opt/presto-server/etc/keystore.jks \
     -v /home/nminoru/config.properties:/opt/presto-server/etc/config.properties \
     -p 7778:7778 \
     -p 8080:8080 \
     --name presto2 \
     nminoru/presto2
