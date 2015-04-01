DROP TABLE IF EXISTS test;

SELECT setseed(0);

CREATE TABLE test (class int2, course1 int4, course2 int4);

INSERT INTO test (class, course1, course2) SELECT (i % 10) + 1, random() * 50 + random() * 50, random() * 50 + random() * 50 FROM generate_series(1, 1000) AS i;

SELECT class, count(*), avg(course1) AS course1_avg, min(course1) AS course1_min, max(course1) AS course1_min, avg(course2) AS course2_avg, min(course2) AS course2_min, max(course2) AS course2_min FROM test GROUP BY class ORDER BY class;

SELECT class, mode() WITHIN GROUP (ORDER BY course1) AS course1_median, mode() WITHIN GROUP (ORDER BY course2) AS course2_median FROM test GROUP BY class ORDER BY class;

SELECT class, rank(80) WITHIN GROUP (ORDER BY course1 DESC) AS course1, rank(80) WITHIN GROUP (ORDER BY course2 DESC) AS course2 FROM test GROUP BY class ORDER BY class;

DROP TABLE test;
