Python2 ではエラーが出ない

```sh
$ python calltest1.py
```

Python3 ではエラーが出る


```sh
$ python3 calltest1.py
Traceback (most recent call last):
  File "calltest1.py", line 8, in <module>
    import lib.callenv
  File "/home/nminoru/git/github/nminoru/misc/python/diff_2_3/lib/callenv.py", line 3, in <module>
    import calllogger
ModuleNotFoundError: No module named 'calllogger'
```

原因は Python3 のインポートの相対/絶対パスの方式が変わり、カレントディレクトリにある別ファイルの取り込むことができないようだ。


```diff
-import calllogger
+from . import calllogger
```

この書き方では、Python3 ではエラーが出ない。
ただし Python2 では動作しない。


```sh
$ python3 calltest2.py
```
