# 1. 初期化用のSQL作成

```sh
mkdir -p ./pginit ./pgdata
chmod -R +x ./pginit ./pgdata
docker run --rm guacamole/guacamole /opt/guacamole/bin/initdb.sh --postgresql > ./pginit/initdb.sql
```

# 2. Docker Compose の機動

```sh
docker compose up -d
```

# 3. 動作確認

```
http://<IPアドレス or ホスト名＞/guacamole/
```

```
user: guacadmin
password: guacadmin
```
