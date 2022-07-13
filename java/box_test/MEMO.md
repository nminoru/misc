2022/07/14

1. アプリアクセスレベルをアプリ+Enterpirseアクセスにし、クライアントID、クライアントシークレット、エンタープライズIDでのアクセスには成功
  ただしファイルの内容は表示されない。
  
2. アプリアクセスレベルをアプリ+Enterpirseアクセスにし、クライアントID、クライアントシークレット、ユーザーIDでのアクセスは失敗

```
{"error":"invalid_grant","error_description":"Grant credentials are invalid"}
```

3. 2.の後にユーザーアクセストークンを生成する(Generate user access tokens)をチェックした場合には、ユーザーIDでアクセスしファイル内容を表示することができた。
この場合、再承認を忘れないこと。



