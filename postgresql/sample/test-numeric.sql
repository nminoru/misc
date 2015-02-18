CREATE TABLE numeric_test (nu_a NUMERIC(10,3), nu_b NUMERIC(10,3));

INSERT INTO numeric_test VALUES (5, 5);
INSERT INTO numeric_test VALUES (50, 50);
INSERT INTO numeric_test VALUES (500, 500);
INSERT INTO numeric_test VALUES (5000, 50000);
INSERT INTO numeric_test VALUES (50000, 500000);
INSERT INTO numeric_test VALUES (500000, 5000000);
INSERT INTO numeric_test VALUES (5000000, 50000000);
INSERT INTO numeric_test VALUES (50000000, 500000000);

CREATE TABLE int_test (in_a smallint, in_b smallint);

INSERT INTO int_test VALUES (32767, 32767);
