* 目的

Redis 7.2 系と Redis 8 系の動作の差を確認する


* テストの実行

Python の仮想環境を作成する。

```sh
python -m venv python-test
pip install requirements.txt
```

以降の実行で以下を入力する。


```sh
. python-test/bin/activeate
```

redis-client.py を修正しながら以下を実行

```sh
python redis-client.py
```

* 注意

redis コンテナは、127.0.0.1 以外からはリクエストを受け取らない・保護モードが有効になっている。
redis.conf を指定して解除する必要がある。

```
bind 0.0.0.0
protected-mode no
```

