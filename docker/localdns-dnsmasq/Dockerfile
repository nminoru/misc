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

RUN echo -e "root:root" | chpasswd

RUN yum install -y rsyslog dnsmasq bind-utils less

RUN echo address=/.host.example.com/127.0.0.1 >> /etc/dnsmasq.conf
RUN echo resolv-file=/etc/dnsmasq.resolv.conf >> /etc/dnsmasq.conf
RUN echo "nameserver 8.8.8.8" > /etc/dnsmasq.resolv.conf

RUN systemctl enable rsyslog.service
RUN systemctl enable dnsmasq.service

VOLUME [ "/sys/fs/cgroup"]

CMD ["/usr/sbin/init"]
