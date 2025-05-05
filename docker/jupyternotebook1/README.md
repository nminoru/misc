# コマンド例

Jupyter Notebook を root で動かす場合は以下のように実行する

```sh
sudo docker run -p 18888:8888 -e GRANT_SUDO=yes --user root myjupyter
```

```sh
sudo docker run -p 8888:8888 -e GRANT_SUDO=yes --user root --log-driver syslog --log-opt syslog-facility=daemon --log-opt tag=myjupyter/{{.Name}}/{{.ID}} myjupyter
```
