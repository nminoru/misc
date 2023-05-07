SELECT
    day, w_warehouse_name, i_item_id, inv_quantity_on_hand, inv_after
FROM
    (
        SELECT
            day, w_warehouse_name, i_item_id, inv_quantity_on_hand, inv_after,
            row_number() over (partition by day order by w_warehouse_name, i_item_id) as row_id
        FROM
            datamart21
    ) w
WHERE
    w.row_id <= 100;
