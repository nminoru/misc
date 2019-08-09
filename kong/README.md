# Kong の DB-less で動かす & 仮想ホストのルーティング実験

[Kong(v1.1.x) DB-less and Declarative Configuration](https://docs.konghq.com/1.1.x/db-less-and-declarative-config/)を参考にしている。

```
sudo /usr/local/bin/kong start -c kong.conf
```

## Kong のリトライをオフにする

kong.conf の以下の設定は API のリトライ設定を切る。

```
nginx_proxy_proxy_next_upstream = off
```
