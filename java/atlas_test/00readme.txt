○ 登録

curl -s -u admin:admin -s -X POST -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/types/typedefs" -d "@./typedef-entities.json"

○ 削除

curl -s -u admin:admin -s -X DELETE -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/types/typedefs" -d "@./typedef-entities.json"

○ 登録した test_path のタイプの表示

curl -u admin:admin -s -X GET  -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/types/typedef/name/test_path"
curl -u admin:admin -s -X GET  -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/types/entitydef/name/test_path"

○ test_path のエンティティの登録

curl -s -u admin:admin -s -X POST -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/entity/bulk" -d "@./entity-sample.json"
curl -s -u admin:admin -s -X POST -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/entity/bulk" -d "@./proceses-sample.json"

○ 登録された tetstapth のエンティティの検索

curl -u admin:admin -s -X GET  -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/search/basic?typeName=test_path"

curl -u admin:admin -s -X POST -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/search/basic" -d '{"typeName":"test_path", "excludeDeletedEntities" : true}'

curl -u admin:admin -s -X GET  -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/search/dsl?typeName=test_path"

○ 登録された tetstapth  のエンティティの表示

curl -u admin:admin -s -X GET -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/entity/guid/4a2215b1-1fd7-473a-82aa-8a78a6c04662"

curl -u admin:admin -s -X GET -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/entity/uniqueAttribute/type/test_path?attr:qualifiedName=nop@0"

