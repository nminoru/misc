#! /bin/sh

sudo docker run -ti \
     --tmpfs /run \
     --tmpfs /run/lock \
     -v /sys/fs/cgroup:/sys/fs/cgroup:ro \
     -p 13306:3306 \
     -p 15432:5432 \
     -p 18080:8080 \
     --name presto \
     nminoru/presto
