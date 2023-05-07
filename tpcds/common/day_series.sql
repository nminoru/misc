CREATE TABLE day_series (day date)

WITH RECURSIVE sales_day(day) AS (
    SELECT CAST(DATE '1998-01-01' AS date)
    UNION ALL
    SELECT CAST(DATEADD(day, 1, sales_day.day) AS date) FROM sales_day WHERE sales_day.day < DATE '2002-09-01'
)
INSERT INTO day_series (day) SELECT sales_day FROM sales;


