SELECT
    day, w_state, i_item_id, sales_before, sales_after
FROM
    (
        SELECT
            day, w_state, i_item_id, sales_before, sales_after,
            row_number() over (partition by day order by w_state, i_item_id) as row_id
        FROM
            datamart40
    ) w
WHERE
    w.row_id <= 100;
