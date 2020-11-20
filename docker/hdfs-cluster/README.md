# Get started

```sh
sudo docker-compose up -d
```

# Test


```sh
sudo docker-compose exec namenode /bin/bash
```

```sh

```

# WebHDFS でアクセスする場合

```sh
su - alice
kinit alice
curl -v "http://127.0.0.1:9870/webhdfs/v1/?doas=alice&op=LISTSTATUS"
```
