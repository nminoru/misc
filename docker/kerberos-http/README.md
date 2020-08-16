Kerberos の実験のために以下の3つのコンテナを立てる。

- kdc (Kerberos の KDC サーバー)
- http (SPNEGO に対応した HTTP サーバー)
- client (HTTP サーバーへアクセスするためのクライアント)


```sh
sudo docker-compose up -d
```

```sh
sudo docker-compose exec client /bin/sh
```

```sh
su - alice
kinit alice
curl -vv --negotiate -u : "http://www.example.com/kerberos/"
```