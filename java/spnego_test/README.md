# はじめに

このテストは Java プログラムで SPNEGO 認証でアクセスを行うテストである。
ただし Java プログラムを動かすクライアントマシンは Kerberos ドメインには傘下せずに行う。

# テスト方法

## Apache HTTP Server under MIT Kerberos Server

[kerberos-http](../../docker/kerberos-http)を起動しておく。

これは MIT Kerberos Server と Apache HTTP Server が2のコンテナが起動するコンポーズである。

Kerberos Server は 127.0.0.1:88/{tcp,udp} で、Apache HTTP Server は 127.0.0.1:8080 にバインドしている。

```sh
export SPNEGO_TEST_DOMAIN=EXAMPLE.COM
export SPNEGO_TEST_USER=alice
export SPNEGO_TEST_PASSWORD=alice
export SPNEGO_TEST_KDC=127.0.0.1
export SPNEGO_TEST_URL=http://www.example.com:80/kerberos/
export SPNEGO_TEST_PROXYURL=http://127.0.0.1:8080/kerberos/
```

## HDFS Cluster under MIT Kerberos Server

[hdfs-secure-cluster](../../docker/hdfs-secure-cluster)を起動しておく。

これは MIT Kerberos Server と HDFS のネームノード、データノードのが3のコンテナが起動するコンポーズである。

Kerberos Server は 127.0.0.1:88/{tcp,udp} で、ネームノードの Web-console が 127.0.0.1:9870 にバインドしている。

```sh
export SPNEGO_TEST_DOMAIN=EXAMPLE.COM
export SPNEGO_TEST_USER=alice
export SPNEGO_TEST_PASSWORD=alice
export SPNEGO_TEST_KDC=127.0.0.1
export SPNEGO_TEST_URL=http://namenode.example.com:9870/
export SPNEGO_TEST_PROXYURL=http://127.0.0.1:9870/
```

# 事前の設定

/etc/krb5.conf から renew_lifetime をコメントアウトすること。

