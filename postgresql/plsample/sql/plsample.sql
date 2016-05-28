CREATE EXTENSION IF NOT EXISTS plsample;

CREATE TABLE testtable (c01 int);

INSERT INTO testtable SELECT i FROM generate_series(1, 10) AS i;

DO LANGUAGE plsample $$ SELECT * FROM testtable LIMIT 3 $$;

-- success
CREATE FUNCTION testfunc1(limit_rows int) RETURNS testtable AS $$ SELECT * FROM testtable LIMIT limit_rows $$ LANGUAGE plsample;

SELECT testfunc1(3);

-- error
CREATE FUNCTION testfunc2(limit_rows int) RETURNS testtable AS $$ SELECTT * FROM testtable LIMIT limit_rows $$ LANGUAGE plsample;

DROP FUNCTION testfunc1(int);
DROP TABLE testtable;