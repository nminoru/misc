Python2 ではエラーが出ない

```sh
$ python calltest.py
```

Python3 ではエラーが出る


```sh
$ python3 calltest.py
Traceback (most recent call last):
  File "calltest.py", line 8, in <module>
    import lib.callenv
  File "/home/nminoru/git/github/nminoru/misc/python/diff_2_3/lib/callenv.py", line 3, in <module>
    import calllogger
ModuleNotFoundError: No module named 'calllogger'
```
