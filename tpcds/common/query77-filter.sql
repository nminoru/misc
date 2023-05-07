SELECT
    day, channel, sales, returns, profit
FROM
    (
        SELECT
            day, channel, sales, returns, profit,
            row_number() over (partition by day order by channel, id) as row_id
        FROM
            datamart77
    ) w
WHERE
    w.row_id <= 100;
