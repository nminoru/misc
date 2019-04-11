# ビルド

```
sudo docker build --rm -t nminoru/centos7-systemd .
```

# 起動

```
sudo docker run -ti --tmpfs /run --tmpfs /run/lock -v /sys/fs/cgroup:/sys/fs/cgroup:ro -p 10022:22 --name centos7-systemd nminoru/centos7-systemd
```

