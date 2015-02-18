DROP TABLE IF EXISTS agg_test;

SELECT setseed(0);

CREATE TABLE agg_test (aggkey int4, i int4, t text);

INSERT INTO agg_test (aggkey, i, t) SELECT i % 10, random() * 100, random() FROM generate_series(1, 1000) AS i;

SELECT percentile_disc(0.5) WITH GROUP (ORDER BY aggkey) AS median FROM agg_test;

DROP TABLE agg_test;
