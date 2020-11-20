FROM nminoru/hadoop AS common

COPY ./build.sh  /tmp/build.sh
RUN  /tmp/build.sh


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

EXPOSE 8020 9000 9870 9871 50100 50105
# EXPOSE 9868 9869

#
#
#
FROM common AS datanode

COPY ./core-site.xml          /opt/hadoop/hadoop-3.3.0/etc/hadoop/core-site.xml
COPY ./datanode/hdfs-site.xml /opt/hadoop/hadoop-3.3.0/etc/hadoop/hdfs-site.xml

EXPOSE 9864 9865 9866 9867 50475
