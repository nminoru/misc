CREATE EXTENSION IF NOT EXISTS plsample;

CREATE TABLE testtable (c1 int);

INSERT INTO testtable SELECT i FROM generate_series(1, 10) AS i;

DO LANGUAGE plsample $$ SELECT * FROM testtable ORDER BY c1 LIMIT 3 $$;

-- success
CREATE FUNCTION testfunc() RETURNS SETOF testtable AS $$ SELECT * FROM testtable ORDER BY c1 LIMIT 3 $$ LANGUAGE plsample;

SELECT testfunc();

DROP FUNCTION testfunc();
DROP TABLE testtable;
