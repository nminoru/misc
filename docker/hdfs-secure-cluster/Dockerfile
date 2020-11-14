FROM nminoru/hadoop AS common

RUN useradd alice
RUN useradd bob

RUN echo -e "alice:alice" | chpasswd
RUN echo -e "bob:bob"     | chpasswd

RUN yum install -y bind-utils krb5-server krb5-libs krb5-workstation krb5-libs dnsmasq

COPY ./kdc/krb5.conf      /etc/krb5.conf

COPY ./build.sh  /tmp/build.sh
RUN  /tmp/build.sh


#
#
#
FROM common AS kdc

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
RUN systemctl enable dnsmasq.service
RUN systemctl enable runonce.service

EXPOSE 88
EXPOSE 749

#
#
#
FROM common AS namenode

COPY ./core-site.xml          /opt/hadoop/hadoop-3.3.0/etc/hadoop/core-site.xml
COPY ./namenode/hdfs-site.xml /opt/hadoop/hadoop-3.3.0/etc/hadoop/hdfs-site.xml
COPY ./masters                /opt/hadoop/hadoop-3.3.0/etc/hadoop/masters
COPY ./slaves                 /opt/hadoop/hadoop-3.3.0/etc/hadoop/slaves

USER hadoop
USER root


#
#
#
FROM common AS datanode

COPY ./core-site.xml          /opt/hadoop/hadoop-3.3.0/etc/hadoop/core-site.xml
COPY ./datanode/hdfs-site.xml /opt/hadoop/hadoop-3.3.0/etc/hadoop/hdfs-site.xml
