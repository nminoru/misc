# Kong を PostgreSQL で動く

```
sudo /usr/local/bin/kong migrations bootstrap -c kong.conf
```

```
sudo /usr/local/bin/kong migrations up -c kong.conf
```

```
sudo /usr/local/bin/kong start -c kong.conf
```

# Kong の DB-less で動かす & 仮想ホストのルーティング実験

[Kong(v1.1.x) DB-less and Declarative Configuration](https://docs.konghq.com/1.1.x/db-less-and-declarative-config/)を参考にしている。

```
sudo /usr/local/bin/kong start -c kong.conf
```
