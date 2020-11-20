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

curl が成功した後で klist を叩くと `krbtgt/EXAMPLE.COM@EXAMPLE.COM` と `HTTP/www.example.com@EXAMPLE.COM` の両方が表示される。

```
$ klist
Ticket cache: FILE:/tmp/krb5cc_1000
Default principal: alice@EXAMPLE.COM

Valid starting     Expires            Service principal
11/20/20 12:16:07  11/21/20 12:16:07  krbtgt/EXAMPLE.COM@EXAMPLE.COM
11/20/20 12:16:16  11/21/20 12:16:07  HTTP/www.example.com@EXAMPLE.COM
```