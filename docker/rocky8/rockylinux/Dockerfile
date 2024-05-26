FROM rockylinux:8

RUN dnf install -y epel-release
RUN dnf install -y java-1.8.0-openjdk java-1.8.0-openjdk-devel java-1.8.0-openjdk-headless
RUN dnf install -y initscripts unzip less wget which strace telnet net-tools perl curl fontconfig ImageMagick
RUN dnf install -y sudo bash rsyslog openssl openssh openssh-clients nmap-ncat util-linux shadow-utils
RUN dnf install -y python3 python3-pip python3-devel

RUN dnf install -y gcc gcc-c++ make file kernel-headers patch

RUN pip3 install nbconvert docker-squash
RUN pip3 install awscli --upgrade --user

EXPOSE 22 80 443 5432 8888 8983

VOLUME [ "/sys/fs/cgroup"]

CMD ["/usr/sbin/init"]

