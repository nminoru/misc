# Amazon S3 へのアクセスのテスト

```
mvn compile
```

## 実行

環境変数を設定する。

- AMAZON_S3_BUCKET
- AMAZON_S3_REGION
- AMAZON_S3_ACCESS_KEY_ID
- AMAZON_S3_SECRET_KEY

以下はオプションで指定する。

- AMAZON_S3_ENDPOINT

```
mvn exec:java
```
