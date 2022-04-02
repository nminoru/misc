Docker registry で Token-based authentication を試行

# この試行は失敗。registry は OAuth2 token サーバーを含まないのでそれをどこからか持ってくる必要がある。

1. 証明書の作成

openssl req -newkey rsa:4096 -nodes -keyout ./server.key -x509 -days 3650 -out ./server.pem -subj "/C=JP/ST=Kanagawa/L=Kawasaki/O=nminoru/CN=localhost"
