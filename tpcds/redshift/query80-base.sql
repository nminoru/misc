WITH ssr AS (
    SELECT
        day_series.day AS day, -- このカラムを足している
        s_store_id AS store_id,
        sum(ss_ext_sales_price) AS sales,
        sum(coalesce(sr_return_amt, 0)) AS returns,
        sum(ss_net_profit - coalesce(sr_net_loss, 0)) AS profit
    FROM
        store_sales LEFT OUTER JOIN store_returns ON (ss_item_sk = sr_item_sk AND ss_ticket_number = sr_ticket_number),
        date_dim,
        store,
        item,
        promotion,
        day_series -- このテーブルを足している
    WHERE
        ss_sold_date_sk = d_date_sk
        -- AND d_date BETWEEN sales_date AND (sales_date +  interval '30 day')
        AND d_date BETWEEN day_series.day AND DATEADD(day, +30, day_series.day)
        AND ss_store_sk = s_store_sk
        AND ss_item_sk = i_item_sk
        AND i_current_price > 50
        AND ss_promo_sk = p_promo_sk
        AND p_channel_tv = 'N'
    GROUP BY
        day_series.day, -- このカラムを足している
        s_store_id
),

csr AS (
    SELECT
        day_series.day AS day, -- このカラムを足している
        cp_catalog_page_id AS catalog_page_id,
        sum(cs_ext_sales_price) AS sales,
        sum(coalesce(cr_return_amount, 0)) AS returns,
        sum(cs_net_profit - coalesce(cr_net_loss, 0)) AS profit
    FROM
        catalog_sales LEFT OUTER JOIN catalog_returns ON (cs_item_sk = cr_item_sk AND cs_order_number = cr_order_number),
        date_dim,
        catalog_page,
        item,
        promotion,
        day_series -- このテーブルを足している
    WHERE
        cs_sold_date_sk = d_date_sk
        -- AND d_date BETWEEN sales_date AND (sales_date +  interval '30 day')
        AND d_date BETWEEN day_series.day AND DATEADD(day, +30, day_series.day)
        AND cs_catalog_page_sk = cp_catalog_page_sk
        AND cs_item_sk = i_item_sk
        AND i_current_price > 50
        AND cs_promo_sk = p_promo_sk
        AND p_channel_tv = 'N'
    GROUP BY
        day_series.day, -- このカラムを足している
        cp_catalog_page_id
),

wsr AS (
    SELECT
        day_series.day AS day, -- このカラムを足している
        web_site_id,
        sum(ws_ext_sales_price) AS sales,
        sum(coalesce(wr_return_amt, 0)) AS returns,
        sum(ws_net_profit - coalesce(wr_net_loss, 0)) AS profit
    FROM
        web_sales LEFT OUTER JOIN web_returns ON (ws_item_sk = wr_item_sk AND ws_order_number = wr_order_number),
        date_dim,
        web_site,
        item,
        promotion,
        day_series -- このテーブルを足している
    WHERE
        ws_sold_date_sk = d_date_sk
        -- AND d_date BETWEEN sales_date AND (sales_date +  interval '30 day')
        AND d_date BETWEEN day_series.day AND DATEADD(day, +30, day_series.day)
        AND ws_web_site_sk = web_site_sk
        AND ws_item_sk = i_item_sk
        AND i_current_price > 50
        AND ws_promo_sk = p_promo_sk
        AND p_channel_tv = 'N'
    GROUP BY
        day_series.day, -- このカラムを足している
        web_site_id
)

SELECT
    day, -- このカラムを足している
    channel,
    id,
    sum(sales) AS sales,
    sum(returns) AS returns,
    sum(profit) AS profit
FROM
    (
        SELECT
           day, -- このカラムを足している
            'store channel' AS channel,
            'store' || store_id AS id,
            sales,
            returns,
            profit
        FROM
            ssr
        UNION ALL
        SELECT
            day, -- このカラムを足している
            'catalog channel' AS channel,
            'catalog_page' || catalog_page_id AS id,
            sales,
            returns,
            profit
        FROM
            csr
        UNION ALL
        SELECT
            day, -- このカラムを足している
            'web channel' AS channel,
            'web_site' || web_site_id AS id,
            sales,
            returns,
            profit
        FROM
            wsr
    ) x
GROUP BY
   ROLLUP (
       day, -- このカラムを足している
       channel,
       id)
ORDER BY
    day, -- このカラムを足している
    channel,
    id;

