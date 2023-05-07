SELECT
    *
FROM
   (
       SELECT
           day_series.day AS day, -- このカラムを足している
           w_warehouse_name,
           i_item_id,
           -- sum(CASE WHEN (cast(d_date as date) < cast ('[SALES_DATE]' as date))
           sum(CASE WHEN (CAST(d_date as date) < CAST(day_series.day AS date))
    	        THEN inv_quantity_on_hand
                    ELSE 0 END) AS inv_before,
           sum(CASE WHEN (CAST(d_date as date) >= CAST(day_series.day AS date))
                    THEN inv_quantity_on_hand
                    ELSE 0 END) AS inv_after
        FROM
            inventory,
            warehouse,
            item,
            date_dim,
            day_series -- このテーブルを足している
        WHERE
            i_current_price BETWEEN 0.99 AND 1.49
            AND i_item_sk        = inv_item_sk
            AND inv_warehouse_sk = w_warehouse_sk
            AND inv_date_sk      = d_date_sk
            -- AND d_date between (cast ('[SALES_DATE]' as date) - 30 days)
            --                and (cast ('[SALES_DATE]' as date) + 30 days)
            AND d_date BETWEEN dateadd(day, -30, day_series.day) AND dateadd(day, +30, day_series.day)
        GROUP BY
            day, -- このカラムを足している
            w_warehouse_name,
            i_item_id
    ) x
WHERE
    (CASE WHEN inv_before > 0
             THEN inv_after / inv_before
             ELSE null
             END) BETWEEN 2.0/3.0 AND 3.0/2.0
ORDER BY
    day, -- このカラムを足している
    w_warehouse_name,
    i_item_id;
