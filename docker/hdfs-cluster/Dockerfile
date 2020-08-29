FROM nminoru/hadoop AS common

COPY ./build.sh  /tmp/build.sh
RUN  /tmp/build.sh

FROM common AS namenode

COPY ./core-site.xml          /opt/hadoop/hadoop-3.3.0/etc/hadoop/core-site.xml
COPY ./namenode/hdfs-site.xml /opt/hadoop/hadoop-3.3.0/etc/hadoop/hdfs-site.xml
COPY ./masters                /opt/hadoop/hadoop-3.3.0/etc/hadoop/masters
COPY ./slaves                 /opt/hadoop/hadoop-3.3.0/etc/hadoop/slaves

USER hadoop
USER root

FROM common AS datanode

COPY ./core-site.xml         /opt/hadoop/hadoop-3.3.0/etc/hadoop/core-site.xml
COPY ./datanodehdfs-site.xml /opt/hadoop/hadoop-3.3.0/etc/hadoop/hdfs-site.xml
