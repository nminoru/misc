--
-- 1998-01-01 からの日付が並んだテーブル
-- データマートの生成時に利用
-- 

CREATE TABLE IF NO EXISTS generated_days AS SELECT CAST(DATE '1998-01-01' + i * interval '1 day' AS date) AS day FROM generate_series(0, 3000) AS i;


