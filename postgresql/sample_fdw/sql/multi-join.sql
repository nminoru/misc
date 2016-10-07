CREATE EXTENSION IF NOT EXISTS sample_fdw;

CREATE SERVER server1 FOREIGN DATA WRAPPER sample_fdw;
CREATE SERVER server2 FOREIGN DATA WRAPPER sample_fdw;
CREATE USER MAPPING FOR CURRENT_USER SERVER server1;
CREATE USER MAPPING FOR CURRENT_USER SERVER server2;

CREATE TABLE t1 (c1 int, c2 int);
CREATE TABLE t2 (c1 int, c2 int);
CREATE TABLE t3 (c1 int, c2 int);
CREATE TABLE t4 (c1 int, c2 int);
CREATE TABLE t5 (c1 int, c2 int);
CREATE TABLE t6 (c1 int, c2 int);

INSERT INTO t1 (c1, c2) SELECT i, i % 2 FROM GENERATE_SERIES(1, 10) AS i;
INSERT INTO t2 (c1, c2) SELECT i, i % 3 FROM GENERATE_SERIES(1, 20) AS i;
INSERT INTO t3 (c1, c2) SELECT i, i % 4 FROM GENERATE_SERIES(1, 30) AS i;
INSERT INTO t4 (c1, c2) SELECT i, i % 5 FROM GENERATE_SERIES(1, 40) AS i;
INSERT INTO t5 (c1, c2) SELECT i, i % 6 FROM GENERATE_SERIES(1, 50) AS i;
INSERT INTO t6 (c1, c2) SELECT i, i % 6 FROM GENERATE_SERIES(1, 60) AS i;

CREATE FOREIGN TABLE ft1 (
       c1 int,
       c2 int
) SERVER server1 OPTIONS (table_name 'public.t1');

CREATE FOREIGN TABLE ft2 (
       c1 int,
       c2 int
) SERVER server2 OPTIONS (table_name 'public.t2');

CREATE FOREIGN TABLE ft3 (
       c1 int,
       c2 int
) SERVER server1 OPTIONS (table_name 'public.t3');

CREATE FOREIGN TABLE ft4 (
       c1 int,
       c2 int
) SERVER server2 OPTIONS (table_name 'public.t4');

ANALYZE;

EXPLAIN (COSTS OFF, VERBOSE ON)
SELECT ft1.c1, ft1.c2, ft2.c2, ft3.c2, ft4.c2, t5.c2, t6.c2 FROM ft1, t5, ft2, t6, ft3, ft4 WHERE ft1.c1 = ft2.c1 AND ft1.c1 = ft3.c1 AND ft1.c1 = ft4.c1 AND ft1.c1 = t5.c1 AND ft1.c1 = t6.c1 ORDER BY ft1.c1 LIMIT 10;

EXPLAIN (COSTS OFF, VERBOSE ON)
SELECT ft1.c1, ft1.c2, ft2.c2, ft3.c2, ft4.c2, t5.c2, t6.c2
       FROM ft1, t5, ft2, t6, (SELECT c1, min(c2) AS c2 FROM ft3 GROUP BY c1) ft3,
                              (SELECT c1, min(c2) AS c2 FROM ft4 GROUP BY c1) ft4
       WHERE ft1.c1 = ft2.c1 AND ft1.c1 = ft3.c1 AND ft1.c1 = ft4.c1 AND ft1.c1 = t5.c1 AND ft1.c1 = t6.c1
       ORDER BY ft1.c1 LIMIT 10;

DROP TABLE t1, t2, t3, t4, t5, t6;
DROP FOREIGN TABLE ft1, ft2, ft3, ft4;
DROP USER MAPPING FOR CURRENT_USER SERVER server2;
DROP USER MAPPING FOR CURRENT_USER SERVER server1;
DROP SERVER server2 CASCADE;
DROP SERVER server1 CASCADE;
