FROM centos:7 AS builder

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

RUN useradd alice
RUN useradd bob
RUN echo -e "root:root"   | chpasswd
RUN echo -e "alice:alice" | chpasswd
RUN echo -e "bob:bob"     | chpasswd

RUN yum install -y httpd httpd-tools mailcap apr apr-util mod_auth_kerb rsyslog bind-utils krb5-server krb5-libs krb5-workstation krb5-libs dnsmasq less curl


#
#
#
FROM builder AS kdc

COPY ./kdc/init-kdc.sh          /init-kdc.sh
COPY ./kdc/krb5.conf            /etc/krb5.conf
COPY ./kdc/kdc.conf             /var/kerberos/krb5kdc/kdc.conf
COPY ./kdc/dnsmasq.conf         /etc/dnsmasq.conf
COPY ./kdc/dnsmasq.resolv.conf  /etc/dnsmasq.resolv.conf
COPY ./kdc/runonce.service      /etc/systemd/system/runonce.service
COPY ./kdc/runonce.sh           /runonce.sh

RUN /init-kdc.sh
RUN systemctl enable krb5kdc
RUN systemctl enable kadmin
RUN systemctl enable rsyslog.service
RUN systemctl enable dnsmasq.service
RUN systemctl enable runonce.service

EXPOSE 88
EXPOSE 749

VOLUME [ "/sys/fs/cgroup"]

CMD ["/usr/sbin/init"]


#
#
#
FROM builder AS http

COPY ./kdc/krb5.conf      /etc/krb5.conf
COPY ./http/kerberos.conf /etc/httpd/conf.d/
RUN rm /etc/httpd/conf.d/welcome.conf
RUN mkdir /var/www/html/kerberos/

RUN systemctl enable rsyslog.service
RUN systemctl enable httpd.service

EXPOSE 80
EXPOSE 443

VOLUME [ "/sys/fs/cgroup"]

CMD ["/usr/sbin/init"]


#
#
#
FROM builder AS client

COPY ./kdc/krb5.conf      /etc/krb5.conf

VOLUME [ "/sys/fs/cgroup"]

CMD ["/usr/sbin/init"]
