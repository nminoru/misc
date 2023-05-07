SELECT
    day_series.day AS day, -- このカラムを足している
    w_state,
    i_item_id,
    sum(CASE WHEN (CAST(d_date AS date) < CAST (day_series.day AS date))
             THEN cs_sales_price - COALESCE(cr_refunded_cash,0) ELSE 0 END) AS sales_before,
    sum(CASE WHEN (CAST(d_date AS date) >= CAST (day_series.day AS date))
             THEN cs_sales_price - COALESCE(cr_refunded_cash,0) ELSE 0 END) AS sales_after
FROM
    catalog_sales LEFT OUTER JOIN catalog_returns
    ON (cs_order_number = cr_order_number AND cs_item_sk = cr_item_sk),
    warehouse,
    item,
    date_dim,
    day_series -- このテーブルを足している
WHERE
     i_current_price BETWEEN 0.99 AND 1.49
     AND i_item_sk          = cs_item_sk
     AND cs_warehouse_sk    = w_warehouse_sk
     AND cs_sold_date_sk    = d_date_sk
     AND d_date BETWEEN dateadd(day, -30, day_series.day) AND dateadd(day, +30, day_series.day)
GROUP BY
     day, -- このカラムを足している
     w_state,
     i_item_id
ORDER BY
     day, -- このカラムを足している
     w_state,
     i_item_id;
