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

COPY ./app/ /app/

RUN /app/build-base.sh
RUN /app/build-postgresql.sh
RUN /app/build-mysql.sh
RUN /app/build-presto.sh

COPY /app/systemd/runonce.service /etc/systemd/system/runonce.service
RUN systemctl enable runonce.service

EXPOSE 80 443 3306 5432

VOLUME [ "/sys/fs/cgroup"]

CMD ["/usr/sbin/init"]
