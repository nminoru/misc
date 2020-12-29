#! /bin/sh

sudo docker run -ti \
     --tmpfs /run \
     --tmpfs /run/lock \
     -v /sys/fs/cgroup:/sys/fs/cgroup:ro \
     -p 10080:80 \
     -p 10443:443 \     
     -p 13306:3306 \
     -p 15432:5432 \
     --name presto \
     nminoru/presto
     
