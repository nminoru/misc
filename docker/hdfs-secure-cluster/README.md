# Get started

```sh
sudo docker-compose up -d
```

# Test


```sh
sudo docker-compose exec namenode /bin/bash
```

root アカウントから hadoop アカウントへ切り替えて、ディレクトリを作成する。

```sh
su - hadoop
kinit hadoop
hadoop fs -mkdir /foo
hadoop fs -chmod 777 /foo
```

次に alice アカウントでログインしてディレクトリの下にファイルを格納する。

```sh
su - alice
kinit alice
hadoop fs -touchz /foo/bar.txt
hadoop fs -ls /foo
```

# WebHDFS でアクセスする場合

```sh
sudo docker-compose exec namenode /bin/bash
```

```sh
su - alice
kinit alice
curl -vv --negotiate -u : -X GET "http://namenode.example.com:9870/webhdfs/v1/?op=LISTSTATUS"
```

curl が成功したら後で klist を叩くと `krbtgt/EXAMPLE.COM@EXAMPLE.COM` と `HTTP/namenode.example.com@EXAMPLE.COM` の両方が表示される。

```sh
$ klist
Ticket cache: FILE:/tmp/krb5cc_1000
Default principal: alice@EXAMPLE.COM

Valid starting     Expires            Service principal
11/20/20 12:16:07  11/21/20 12:16:07  krbtgt/EXAMPLE.COM@EXAMPLE.COM
11/20/20 12:16:16  11/21/20 12:16:07  HTTP/namenode.example.com@EXAMPLE.COM
```

# Account

|account|password|
|-------|--------|
|root   |root    |
|hdfs   |hdfs    |
|alice  |alice   |
|bob    |bob     |
