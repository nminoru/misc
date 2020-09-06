FROM centos:7 AS builder

COPY ./hadoop-3.3.0.tar.gz /tmp/hadoop-3.3.0.tar.gz

WORKDIR /tmp

RUN mkdir -p /app /tmp/base1 /tmp/base2

RUN cd /tmp/base1 && \
    tar xvf /tmp/hadoop-3.3.0.tar.gz

RUN cd /tmp/base2 && \
    tar xvf /tmp/hadoop-3.3.0.tar.gz

RUN rm -rf /tmp/base1/hadoop-3.3.0/share/doc
RUN rm -rf /tmp/base1/hadoop-3.3.0/share/hadoop/common
RUN rm -rf /tmp/base1/hadoop-3.3.0/share/hadoop/client
RUN rm -rf /tmp/base1/hadoop-3.3.0/share/hadoop/tools
RUN rm -rf /tmp/base1/hadoop-3.3.0/share/hadoop/mapreduce
RUN rm -rf /tmp/base1/hadoop-3.3.0/share/hadoop/yarn

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

COPY ./hadoop.sh           /etc/profile.d/

COPY ./build1.sh           /tmp/build1.sh
COPY ./build2.sh           /tmp/build2.sh

COPY ./core-site.xml       /app/core-site.xml
COPY ./hdfs-site.xml       /app/hdfs-site.xml
COPY ./user_hdfs.sh        /app/user_hdfs.sh
COPY ./hdfs.service        /app/hdfs.service

RUN  /tmp/build1.sh

COPY --from=builder --chown=hadoop:hadoop /tmp/base1/hadoop-3.3.0                        /opt/hadoop/hadoop-3.3.0
COPY --from=builder --chown=hadoop:hadoop /tmp/base2/hadoop-3.3.0/share/doc              /opt/hadoop/hadoop-3.3.0/share/doc
COPY --from=builder --chown=hadoop:hadoop /tmp/base2/hadoop-3.3.0/share/hadoop/common    /opt/hadoop/hadoop-3.3.0/share/hadoop/common
COPY --from=builder --chown=hadoop:hadoop /tmp/base2/hadoop-3.3.0/share/hadoop/client    /opt/hadoop/hadoop-3.3.0/share/hadoop/client
COPY --from=builder --chown=hadoop:hadoop /tmp/base2/hadoop-3.3.0/share/hadoop/tools     /opt/hadoop/hadoop-3.3.0/share/hadoop/tools
COPY --from=builder --chown=hadoop:hadoop /tmp/base2/hadoop-3.3.0/share/hadoop/mapreduce /opt/hadoop/hadoop-3.3.0/share/hadoop/mapreduce
COPY --from=builder --chown=hadoop:hadoop /tmp/base2/hadoop-3.3.0/share/hadoop/yarn      /opt/hadoop/hadoop-3.3.0/share/hadoop/yarn

RUN  /tmp/build2.sh

USER hadoop

RUN  /app/user_hdfs.sh

USER root

# HDFS
EXPOSE 88 749 50010 50020 50070 50075 50090 8020 9000

VOLUME [ "/sys/fs/cgroup"]

CMD ["/usr/sbin/init"]
