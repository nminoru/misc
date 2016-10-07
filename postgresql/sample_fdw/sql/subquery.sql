CREATE EXTENSION IF NOT EXISTS sample_fdw;

CREATE SERVER loopback FOREIGN DATA WRAPPER sample_fdw;
CREATE USER MAPPING FOR CURRENT_USER SERVER loopback;

CREATE TABLE t1 (c1 int, c2 int);
CREATE TABLE t2 (c1 int, c2 int);
CREATE TABLE t3 (c1 int, c2 int);

INSERT INTO t1 (c1, c2) SELECT i, i % 2 FROM GENERATE_SERIES(1, 10) AS i;
INSERT INTO t2 (c1, c2) SELECT i, i % 3 FROM GENERATE_SERIES(1, 20) AS i;
INSERT INTO t3 (c1, c2) SELECT i, i % 4 FROM GENERATE_SERIES(1, 30) AS i;

CREATE FOREIGN TABLE ft1 (
       c1 int,
       c2 int
) SERVER loopback OPTIONS (table_name 'public.t1');

CREATE FOREIGN TABLE ft2 (
       c1 int,
       c2 int
) SERVER loopback OPTIONS (table_name 'public.t2');

ANALYZE;

-- Uncorrelated foreign table subquery from other one
EXPLAIN (COSTS OFF, VERBOSE ON) SELECT * FROM ft1, t3 WHERE ft1.c1 = t3.c1 AND ft1.c2 <= (SELECT min(c2) FROM ft2);

-- Uncorrelated foreign table subquery from the same one
EXPLAIN (COSTS OFF, VERBOSE ON) SELECT * FROM ft1, t3 WHERE ft1.c1 = t3.c1 AND ft1.c2 <= (SELECT min(c2) FROM ft1);

-- Uncorrelated foreign table subquery from normal table
EXPLAIN (COSTS OFF, VERBOSE ON) SELECT * FROM ft1, t3 WHERE ft1.c1 = t3.c1 AND t3.c2 <= (SELECT min(c2) FROM ft2);

-- Correlated foreign table subquery from other one
EXPLAIN (COSTS OFF, VERBOSE ON) SELECT * FROM ft1, t3 WHERE ft1.c1 = t3.c1 AND t3.c2 <= (SELECT min(c2) FROM ft2 WHERE ft1.c1 = ft2.c1);

-- Correlated foreign table subquery from the same one
EXPLAIN (COSTS OFF, VERBOSE ON) SELECT * FROM ft1, t3 WHERE ft1.c1 = t3.c1 AND t3.c2 <= (SELECT min(c2) FROM ft1 F1 WHERE ft1.c1 = F1.c1);

-- Correlated foreign table subquery from the same one using LATERAL 
EXPLAIN (COSTS OFF, VERBOSE ON) SELECT * FROM ft1, t3, LATERAL (SELECT min(c2) AS c2_min FROM ft1 F1 WHERE ft1.c1 = F1.c1) ft2 WHERE ft1.c1 = t3.c1 AND t3.c2 <= ft2.c2_min;

DROP TABLE t1, t2, t3;
DROP FOREIGN TABLE ft1, ft2;
DROP USER MAPPING FOR CURRENT_USER SERVER loopback;
DROP SERVER loopback CASCADE;
