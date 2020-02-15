FROM centos:7

MAINTAINER nminoru1975@gmail.com

ENV container docker
RUN (cd /lib/systemd/system/sysinit.target.wants/; for i in *; do [ $i == \
    systemd-tmpfiles-setup.service ] || rm -f $i; done); \
#   rm -f /lib/systemd/system/multi-user.target.wants/*;\
    rm -f /etc/systemd/system/*.wants/*;\
    rm -f /lib/systemd/system/local-fs.target.wants/*; \
    rm -f /lib/systemd/system/sockets.target.wants/*udev*; \
    rm -f /lib/systemd/system/sockets.target.wants/*initctl*; \
    rm -f /lib/systemd/system/basic.target.wants/*;\
    rm -f /lib/systemd/system/anaconda.target.wants/*;

COPY ./core-site.xml       /tmp/core-site.xml
COPY ./hdfs-site.xml       /tmp/hdfs-site.xml 
COPY ./build.sh            /tmp/build.sh
COPY ./user_hdfs.sh        /tmp/user_hdfs.sh
COPY ./hdfs.service        /tmp/hdfs.service
COPY ./hadoop-3.2.1.tar.gz /tmp/hadoop-3.2.1.tar.gz

RUN  /tmp/build.sh

USER hdfs

RUN  /tmp/user_hdfs.sh

USER root

# HDFS
EXPOSE 50010 50020 50070 50075 50090 8020 9000

VOLUME [ "/sys/fs/cgroup"]

CMD ["/usr/sbin/init"]

