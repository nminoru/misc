CREATE EXTENSION IF NOT EXISTS exec_sql_twice;

CREATE TABLE testtable1 (key int4, value int4);
CREATE TABLE testtable2 (key int4, value int4);

ALTER TABLE testtable1 SET (AUTOVACUUM_ENABLED = FALSE, TOAST.AUTOVACUUM_ENABLED = FALSE);
ALTER TABLE testtable2 SET (AUTOVACUUM_ENABLED = FALSE, TOAST.AUTOVACUUM_ENABLED = FALSE);

INSERT INTO testtable1 (key, value) SELECT i, i % 2 FROM generate_series(1, 100) AS i;
INSERT INTO testtable2 (key, value) SELECT i, i % 3 FROM generate_series(1, 100) AS i;

SELECT testtable1.key AS key, testtable1.value AS value1, testtable2.value AS value2 FROM testtable1, testtable2 WHERE testtable1.key = testtable2.key LIMIT 10;

SELECT * FROM sql_exec_twice('SELECT testtable1.key AS key, testtable1.value AS value1, testtable2.value AS value2 FROM testtable1, testtable2 WHERE testtable1.key = testtable2.key LIMIT 10;') AS t(key int4, value1 int4, value2 int4);

DROP TABLE testtable1, testtable2;
