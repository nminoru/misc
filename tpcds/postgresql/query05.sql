CREATE OR REPLACE FUNCTION query05(sales_date date)
RETURNS TABLE (channel text, id text, sales decimal(7,2), returns decimal(7,2), profit decimal(7,2))
AS $$

WITH ssr AS
    (SELECT s_store_id,
             sum(sales_price) AS sales,
             sum(profit) AS profit,
             sum(return_amt) AS returns,
             sum(net_loss) AS profit_loss
     FROM
         (SELECT ss_store_sk AS store_sk,
             ss_sold_date_sk  AS date_sk,
             ss_ext_sales_price AS sales_price,
             ss_net_profit AS profit,
             CAST(0 AS decimal(7,2)) AS return_amt,
             CAST(0 AS decimal(7,2)) AS net_loss
    FROM store_sales
    UNION ALL
    SELECT sr_store_sk AS store_sk,
           sr_returned_date_sk AS date_sk,
           CAST(0 AS decimal(7,2)) AS sales_price,
           CAST(0 AS decimal(7,2)) AS profit,
           sr_return_amt AS return_amt,
           sr_net_loss AS net_loss
    from store_returns
   ) salesreturns,
     date_dim,
     store
 WHERE date_sk = d_date_sk
       AND d_date BETWEEN sales_date and (sales_date +  interval '14 day')
       AND store_sk = s_store_sk
 GROUP BY s_store_id),
 csr AS
 (SELECT cp_catalog_page_id,
        sum(sales_price) AS sales,
        sum(profit) AS profit,
        sum(return_amt) AS returns,
        sum(net_loss) AS profit_loss
 FROM
  (SELECT cs_catalog_page_sk AS page_sk,
            cs_sold_date_sk  AS date_sk,
            cs_ext_sales_price AS sales_price,
            cs_net_profit AS profit,
            CAST(0 AS decimal(7,2)) AS return_amt,
            CAST(0 AS decimal(7,2)) AS net_loss
    FROM catalog_sales
    UNION ALL
    SELECT cr_catalog_page_sk AS page_sk,
           cr_returned_date_sk AS date_sk,
           CAST(0 AS decimal(7,2)) AS sales_price,
           CAST(0 AS decimal(7,2)) AS profit,
           cr_return_amount AS return_amt,
           cr_net_loss AS net_loss
    FROM catalog_returns
   ) salesreturns,
     date_dim,
     catalog_page
 WHERE date_sk = d_date_sk
       AND d_date BETWEEN sales_date and (sales_date +  interval '14 day')
       and page_sk = cp_catalog_page_sk
 group by cp_catalog_page_id)
 ,
 wsr as
 (select web_site_id,
        sum(sales_price) AS sales,
        sum(profit) AS profit,
        sum(return_amt) AS returns,
        sum(net_loss) AS profit_loss
 from
  ( select  ws_web_site_sk AS wsr_web_site_sk,
            ws_sold_date_sk  AS date_sk,
            ws_ext_sales_price AS sales_price,
            ws_net_profit AS profit,
            CAST(0 AS decimal(7,2)) AS return_amt,
            CAST(0 AS decimal(7,2)) AS net_loss
    from web_sales
    union all
    select ws_web_site_sk AS wsr_web_site_sk,
           wr_returned_date_sk AS date_sk,
           CAST(0 AS decimal(7,2)) AS sales_price,
           CAST(0 AS decimal(7,2)) AS profit,
           wr_return_amt AS return_amt,
           wr_net_loss AS net_loss
    from web_returns left outer join web_sales on
         ( wr_item_sk = ws_item_sk
           and wr_order_number = ws_order_number)
   ) salesreturns,
     date_dim,
     web_site
 where date_sk = d_date_sk
       and d_date BETWEEN sales_date and (sales_date +  interval '14 day')
       and wsr_web_site_sk = web_site_sk
 group by web_site_id)

select channel
        , id
        , sum(sales) AS sales
        , sum(returns) AS returns
        , sum(profit) AS profit
 from 
 (select 'store channel' AS channel
        , 'store' || s_store_id AS id
        , sales
        , returns
        , (profit - profit_loss) AS profit
 from   ssr
 union all
 select 'catalog channel' AS channel
        , 'catalog_page' || cp_catalog_page_id AS id
        , sales
        , returns
        , (profit - profit_loss) AS profit
 from  csr
 union all
 select 'web channel' AS channel
        , 'web_site' || web_site_id AS id
        , sales
        , returns
        , (profit - profit_loss) AS profit
 from   wsr
 ) x
 group by rollup (channel, id)
 order by channel
         ,id
limit 100

$$ LANGUAGE SQL;

--
-- データマート作成
--
DROP TABLE IF EXISTS datamart05 CASCADE;
CREATE TABLE datamart05 AS
SELECT day, channel, id, sales, returns, profit
  FROM (SELECT day FROM generated_days WHERE day BETWEEN DATE '1998-01-01' AND DATE '2002-08-31') AS T1,
       LATERAL (SELECT * FROM query05(CAST(T1.day AS date))) AS T2;

