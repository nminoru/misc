FROM centos:7
ENV container docker
RUN echo 'root:root' | chpasswd
RUN (cd /lib/systemd/system/sysinit.target.wants/; for i in *; do [ $i == \
    systemd-tmpfiles-setup.service ] || rm -f $i; done); \
#   rm -f /lib/systemd/system/multi-user.target.wants/*;\
    rm -f /etc/systemd/system/*.wants/*;\
    rm -f /lib/systemd/system/local-fs.target.wants/*; \
    rm -f /lib/systemd/system/sockets.target.wants/*udev*; \
    rm -f /lib/systemd/system/sockets.target.wants/*initctl*; \
    rm -f /lib/systemd/system/basic.target.wants/*;\
    rm -f /lib/systemd/system/anaconda.target.wants/*;

RUN rm  -f /etc/yum.repos.d/CentOS-*.repo
COPY ./settings/nexus.repo /etc/yum.repos.d/nexus.repo

COPY ./settings/pip.conf /etc/pip.conf
COPY ./settings/npmrc    /etc/npmrc
# COPY ./requirements.txt  /tmp/requirements.txt

# epel-release は明示的にインストールした上で無効化する
RUN yum install -y epel-release
RUN yum-config-manager --disable epel

RUN yum install -y yum-plugin-priorities
RUN yum install -y yum-utils
RUN yum install -y createrepo

RUN yum clean all
RUN yum makecache -y

RUN yum install -y openssh-server openssh-clients
RUN yum install -y java-1.8.0-openjdk java-1.8.0-openjdk-devel java-1.8.0-openjdk-headless
RUN yum install -y postgresql96 postgresql96-server postgresql96-libs postgresql96-docs
RUN yum install -y initscripts less wget which strace telnet net-tools
RUN yum install -y ImageMagick
RUN yum install -y nginx
RUN yum install -y golang
RUN yum install -y kong
RUN yum install -y python-pip
RUN yum install -y npm

# RUN npm install -g npm
# RUN npm init -y
# RUN npm 
# RUN pip install nbconvert
# RUN npm -g install yarn
# RUN yarn install && yarn build

RUN systemctl enable sshd
RUN systemctl enable postgresql-9.6
RUN systemctl enable nginx

RUN echo "export LANG=en_US.UTF-8" >> /root/.bash_profile

EXPOSE 22 80 443 8081
    
VOLUME [ "/sys/fs/cgroup"]

CMD ["/usr/sbin/init"]
