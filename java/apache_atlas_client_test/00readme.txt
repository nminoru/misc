○  登録

curl -s -u admin:admin -s -X POST -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/types/typedefs" -d "@./typedef-entities.json"

○ 削除

curl -s -u admin:admin -s -X DELETE -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/types/typedefs" -d "@./typedef-entities.json"

○ 登録した  test_path  のタイプの表示

curl -u admin:admin -s -X GET  -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/types/typedef/name/test_path" | ~/bin/json_pp.py

○ test_path のエンティティの登録

curl -s -u admin:admin -s -X POST -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/entity/bulk" -d "@./entity-sample.json"

curl -s -u admin:admin -s -X POST -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/entity/bulk" -d "@./proceses-sample.json"

○  登録された etst_path  のエンティティの表示

curl -u admin:admin -s -X GET  -H "Content-Type: application/json" -H "Accept: application/json" "http://127.0.0.1:21000/api/atlas/v2/search/dsl?typeName=test_path" | ~/bin/json_pp.py



