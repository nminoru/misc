# Get started

```sh
sudo docker-compose up -d
```

# Test


```sh
sudo docker-compose exec namenode /bin/bash
```

まず hdfs アカウントでログインして作業

```sh
su - hdfs
kinit hdfs
hadoop fs -mkdir /foo
hadoop fs -chmod 777 /foo
```

次に alice アカウントでログインして作業

```sh
su - alice
kinit alice
hadoop fs -touchz /foo/bar.txt
hadoop fs -ls /foo
```

# Account

|account|password|
|-------|--------|
|root   |root    |
|hdfs   |hdfs    |
|alice  |alice   |
|bob    |bob     |
