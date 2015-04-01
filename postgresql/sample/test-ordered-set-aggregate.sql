DROP TABLE IF EXISTS agg_test;

SELECT setseed(0);

CREATE TABLE agg_test (aggkey int4, i int4, t text);

INSERT INTO agg_test (aggkey, i, t) SELECT i % 10, random() * 100, random() FROM generate_series(1, 1000) AS i;

SELECT mode() WITHIN GROUP (ORDER BY i) AS median, percentile_cont(0.5) WITHIN GROUP (ORDER BY i) AS median2, percentile_disc(0.5) WITHIN GROUP (ORDER BY i) AS median2 FROM agg_test;

DROP TABLE agg_test;
