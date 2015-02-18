DROP TABLE IF EXISTS agg_test;

CREATE TABLE agg_test (aggkey integer, value integer, cond integer);

INSERT INTO agg_test VALUES (1, 10, 1);
INSERT INTO agg_test VALUES (1, 10, 1);
INSERT INTO agg_test VALUES (1, 20, 1);
INSERT INTO agg_test VALUES (2, 10, 1);
INSERT INTO agg_test VALUES (2, 20, 2);
INSERT INTO agg_test VALUES (2, 30, 3);
INSERT INTO agg_test VALUES (3, 10, 3);

SELECT aggkey, sum(value) AS sum1, sum(DISTINCT value) AS sum2, sum(value) FILTER (WHERE cond = 1) AS sum3 FROM agg_test GROUP BY aggkey;

DROP TABLE agg_test;
