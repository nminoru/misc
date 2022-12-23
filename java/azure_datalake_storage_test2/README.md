# Azure Data Lake Storage Gen2 へのアクセスのテスト

```
mvn compile
```

## 実行
環境変数を設定する。

- ABFS_ACCOUNT_NAME
- ABFS_ACCOUNT_KEY
- ABFS_CONTAINER_NAME


プロキシが必要な場合は以下の環境変数も指定する。
ユーザー&パスワードの設定が必要なパターンには対応しない。

- PROXY_HOST
- PROXY_PORT

```
mvn exec:java
```
