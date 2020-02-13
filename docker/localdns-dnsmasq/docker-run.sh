#! /bin/sh

sudo docker run -ti \
     --cap-add NET_ADMIN \
     --hostname host \
     --domainname example.com \
     --add-host "*.host.example.com:127.0.0.1" \
     --dns=127.0.0.1 \
     --tmpfs /run \
     --tmpfs /run/lock \
     -v /sys/fs/cgroup:/sys/fs/cgroup:ro \
     --name localdns-dnsmasq \
     localdns-dnsmasq
