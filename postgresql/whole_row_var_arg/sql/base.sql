SET client_min_messages TO 'warning';

CREATE EXTENSION IF NOT EXISTS whole_row_var_arg;

CREATE TABLE T1 (
       C1 int,
       C2 text,
       C3 float8
);

INSERT INTO T1 (C1, C2, C3) SELECT i, i + 1, i + 2 FROM generate_series(1, 100) AS i;

SELECT S.C1, S.C2, S.C3 FROM (SELECT *, whole_row_var_arg('dummy', T1.*) FROM T1 WHERE C1 % 9 = 0) AS S ORDER BY S.C1;

SELECT S.C1, AVG(S.C3) FROM (SELECT *, whole_row_var_arg('dummy', T1.*) FROM T1 WHERE C1 % 7 = 0) AS S GROUP BY S.C1 ORDER BY S.C1;

EXPLAIN (VERBOSE ON, COSTS OFF) SELECT S.C1, AVG(S.C3) FROM (SELECT *, whole_row_var_arg('dummy', T1.*) FROM T1 WHERE C1 % 7 = 0) AS S GROUP BY S.C1 ORDER BY S.C1;

DROP TABLE T1 CASCADE;


