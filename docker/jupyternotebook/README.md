```sh
sudo docker run -p 8888:8888 -e GRANT_SUDO=yes --user root --log-driver syslog --log-opt syslog-facility=daemon --log-opt tag=myjupyter/{{.Name}}/{{.ID}} myjupyter
```
