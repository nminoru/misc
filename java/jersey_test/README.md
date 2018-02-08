* Jersey のテスト兼マルチパートのテスト

[Q]
HTTP マルチパートで指定された boundary によるパートの区切りとパート内にある
Content-Length の指定とどちらが優先されるか。

[A]
multipart/form-data の中のパートの中では boundary のみ有効。
Content-Length 無視される。


** Jersey サーバーのテスト

    $ mvn exec:java

** Jersey クライアント

  Content-Type は正しいが、パート中の boundary 文字列が出現

    $ http_request.py localhost 8080 < http_request2.txt
    HTTP/1.1 400 Bad Request

  Content-Type がパートよりも小さく、パート中には boundary 文字列が存在しない

    $ http_request.py localhost 8080 < http_request3.txt
    HTTP/1.1 200 OK

  ※ Content-Type が無視されている

  Content-Type は正しく、パート中には boundary 文字列が存在しない

    $ http_request.py localhost 8080 < http_request4.txt
    HTTP/1.1 200 OK

