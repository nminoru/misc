Docker registry の調査


# ビルド

```
mvn package -DskipTests
```

# 実行方法
## 環境の変数

Docker registry の情報を環境変数として設定する

```
export DOCKER_REGISTRY_URL=(Docker registry のURL)
export DOCKER_REGISTRY_USER=(認証が必要な場合のユーザー名)
export DOCKER_REGISTRY_PASSWORD=(認証が必要な場合のパスワード)
```

Docker registry の URL は http:// または https:// を指定する。

```
export DOCKER_REGISTRY_URL=http://registry.example.com/
```

# 実行方法

タグ一覧の取得

```
java -jar target/docker_registry_test-0.0.1.jar --list-tags
```

タグの削除

```
java -jar target/docker_registry_test-0.0.1.jar --delete-tags < (削除したいタグのリスト)
```
