# Presto サーバーの起動

Presto サーバーは HTTPS が必須なので運用環境のサーバー証明書がない場合は自己証明書を作る必要がある。

## サーバー証明書がある場合

以下のサーバー証明書があるとする。

- cert.pem (サーバ証明書(公開鍵))
- chain.pem (中間証明書)
- privkey.pem (秘密鍵)

```sh
openssl pkcs12 -export -in cert.pem -inkey privkey.pem -certfile chain.pem -out pkcs12.pfx -passout pass:changeit
```

```sh
keytool -importkeystore -srckeystore pkcs12.pfx -srcstoretype PKCS12 -srcstorepass changeit -destkeystore keystore.jks -deststoretype JKS -deststorepass changeit
```

## サーバー証明書がない場合

自己証明書を作成する。

- host.example.com で動作すると想定
- keystore.jks のパスワードは changeit

```sh
keytool -genkeypair -keystore keystore.jks -storepass changeit -keypass changeit -alias jetty -keyalg RSA -keysize 2048 -validity 5000 -dname "CN=host.example.com, OU=Unspecified, O=Unspecified, L=Unspecified, ST=Unspecified, C=JP" -ext "SAN=DNS:host.example.com" -ext "BC=ca:true"
```

以下のように presto コンテナを pull して起動する。
HTTP(8080)、HTTPS(7778)で起動する。

```sh
sudo docker run -ti \
     --tmpfs /run \
     --tmpfs /run/lock \
     -v /sys/fs/cgroup:/sys/fs/cgroup:ro \
     -v /home/nminoru/keystore.jks:/opt/presto-server/etc/keystore.jks \
     -p 80:80 \
     -p 443:443 \
     -p 3306:3306 \
     -p 5432:5432 \
     --name presto \
     nminoru/presto
```

# 諸元

## CentOS
root:root

## PrestoDB

Port: 80 (HTTP)
Port: 443 (HTTPS)

tester:k9yuDsfnMy$

## PostgreSQL
Port: 5432

tester:k9yuDsfnMy$

## MySQL
Port: 3306

root:3E6hW2qr2V$
tester:k9yuDsfnMy$
