WITH ss AS (
    SELECT
        day_series.day AS day, -- このカラムを足している
        s_store_sk,
        sum(ss_ext_sales_price) AS sales,
        sum(ss_net_profit) AS profit
    FROM
        store_sales,
        date_dim,
        store,
        day_series -- このテーブルを足している
    WHERE
        ss_sold_date_sk = d_date_sk
        -- and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
        AND d_date BETWEEN day_series.day AND DATEADD(day, +30, day_series.day)
        AND ss_store_sk = s_store_sk
    GROUP BY
        day_series.day, -- このカラムを足している
        s_store_sk
),

sr AS (
    SELECT
        day_series.day AS day, -- このカラムを足している
        s_store_sk,
        sum(sr_return_amt) AS returns,
        sum(sr_net_loss) AS profit_loss
    FROM
        store_returns,
        date_dim,
        store,
        day_series -- このテーブルを足している
    WHERE
        sr_returned_date_sk = d_date_sk
        -- and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
        AND d_date BETWEEN day_series.day AND DATEADD(day, +30, day_series.day)
        AND sr_store_sk = s_store_sk
    GROUP BY
        day_series.day, -- このカラムを足している
        s_store_sk
),

 cs AS (
     SELECT
        day_series.day AS day, -- このカラムを足している
        cs_call_center_sk,
        sum(cs_ext_sales_price) AS sales,
        sum(cs_net_profit) AS profit
    FROM
        catalog_sales,
        date_dim,
        day_series -- このテーブルを足している
    WHERE
        cs_sold_date_sk = d_date_sk
        -- and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
        AND d_date BETWEEN day_series.day AND DATEADD(day, +30, day_series.day)
    GROUP BY
        day_series.day, -- このカラムを足している
        cs_call_center_sk
 ),

 cr AS (
    SELECT
        day_series.day AS day, -- このカラムを足している
        cr_call_center_sk,
        sum(cr_return_amount) AS returns,
        sum(cr_net_loss) AS profit_loss
    FROM
        catalog_returns,
        date_dim,
        day_series -- このテーブルを足している
    WHERE
        cr_returned_date_sk = d_date_sk
        -- and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
        AND d_date BETWEEN day_series.day AND DATEADD(day, +30, day_series.day)
    GROUP BY
        day_series.day, -- このカラムを足している
        cr_call_center_sk
 ),

 ws AS (
     SELECT
        day_series.day AS day, -- このカラムを足している
        wp_web_page_sk,
        sum(ws_ext_sales_price) AS sales,
        sum(ws_net_profit) AS profit
    FROM
        web_sales,
        date_dim,
        web_page,
        day_series -- このテーブルを足している
    WHERE
        ws_sold_date_sk = d_date_sk
        -- and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
        AND d_date BETWEEN day_series.day AND DATEADD(day, +30, day_series.day)
        AND ws_web_page_sk = wp_web_page_sk
    GROUP BY
        day_series.day, -- このカラムを足している
        wp_web_page_sk
),

 wr AS (
    SELECT
        wp_web_page_sk,
        sum(wr_return_amt) AS returns,
        sum(wr_net_loss) AS profit_loss
    FROM
        web_returns,
        date_dim,
        web_page,
        day_series -- このテーブルを足している
    WHERE
        wr_returned_date_sk = d_date_sk
        -- AND d_date BETWEEN sales_date AND (sales_date +  interval '30 day')
        AND d_date BETWEEN day_series.day AND DATEADD(day, +30, day_series.day)
        AND wr_web_page_sk = wp_web_page_sk
    GROUP BY
        day_series.day, -- このカラムを足している
        wp_web_page_sk
)

SELECT
    day, -- この列を足している
    channel,
    id,
    sum(sales) AS sales,
    sum(returns) AS returns,
    sum(profit) AS profit
FROM
    (
        SELECT
            ss.day, -- このカラムを足している
            'store channel' AS channel,
            ss.s_store_sk AS id,
            sales,
            coalesce(returns, 0) AS returns,
            (profit - coalesce(profit_loss,0)) AS profit
        FROM
            ss LEFT JOIN sr ON ss.s_store_sk = sr.s_store_sk
        UNION ALL
        SELECT
            cs.day, -- このカラムを足している
            'catalog channel' AS channel,
            cs_call_center_sk AS id,
            sales,
            returns,
            (profit - profit_loss) AS profit
        FROM
            cs,
            cr
        UNION ALL
        SELECT
            ws.day, -- このカラムを足している
            'web channel' AS channel,
            ws.wp_web_page_sk AS id,
            sales,
            coalesce(returns, 0) AS returns,
            (profit - coalesce(profit_loss,0)) AS profit
        FROM
            ws LEFT JOIN wr ON ws.wp_web_page_sk = wr.wp_web_page_sk
    ) x
GROUP BY
    ROLLUP (
        day, -- このカラムを足している
        channel,
        id
    )
ORDER BY
    day, -- このカラムを足している
    channel,
    id;
