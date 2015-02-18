CREATE TABLE employee (
       ID         int,
       name       varchar(10),
       salary     real,
       start_date date,
       city       varchar(10),
       region     char(1)) WITH OIDS;

INSERT INTO employee VALUES (1,  'Jason', 40420,  '94/02/01', 'New York', 'W');
INSERT INTO employee VALUES (2,  'Robert',14420,  '95/01/02', 'Vancouver','N');
INSERT INTO employee VALUES (3,  'Celia', 24020,  '96/12/03', 'Toronto',  'W');
INSERT INTO employee VALUES (4,  'Linda', 40620,  '87/11/04', 'New York', 'N');
INSERT INTO employee VALUES (5,  'David', 80026,  '98/10/05', 'Vancouver','W');
INSERT INTO employee VALUES (6,  'James', 70060,  '99/09/06', 'Toronto',  'N');
INSERT INTO employee VALUES (7,  'Alison',90620,  '00/08/07', 'New York', 'W');
INSERT INTO employee VALUES (8,  'Chris', 26020,  '01/07/08', 'Vancouver','N');
INSERT INTO employee VALUES (9,  'Mary',  60020,  '02/06/09', 'Toronto',  'W');

SELECT oid FROM employee WHERE ID > 4;

DROP TABLE employee;
