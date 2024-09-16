# ビルド方法

```sh
mvn package -DskipTests
```

target/active_directory_test-0.0.1.jar に格納される。

# コマンドライン

```sh
java -jar active_directory_test-0.0.1.jar [kerberos+smb | kerberos+ldap | ntlm | ldap]
```

# 環境変数

- ADT_DOMAIN       "example.com"
- ADT_USERNAME     "username"
- ADT_PASSWORD     "password"
- ADT_LDAP         "ldap.example.com"
- ADT_KDC          "kdc.example.com"

## ADT_METHOD が "SMB" の時のみ有効

- ADT_SMB_HOST
- ADT_SMB_SHARENAME

## ADT_METHOD が "LDAP" の時のみ有効

- ADT_LDAP_USER_DN  "dc=example,dc=com"
- ADT_LDAP_FILTER   "cn=username"
