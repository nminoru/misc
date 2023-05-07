CREATE OR REPLACE FUNCTION query77(sales_date date)
RETURNS TABLE (channel text, id integer, sales decimal(7,2), returns decimal(7,2), profit decimal(7,2))
AS $$
 
 with ss as
 (select s_store_sk,
         sum(ss_ext_sales_price) as sales,
         sum(ss_net_profit) as profit
 from store_sales,
      date_dim,
      store
 where ss_sold_date_sk = d_date_sk
       and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
       and ss_store_sk = s_store_sk
 group by s_store_sk)
 ,
 sr as
 (select s_store_sk,
         sum(sr_return_amt) as returns,
         sum(sr_net_loss) as profit_loss
 from store_returns,
      date_dim,
      store
 where sr_returned_date_sk = d_date_sk
       and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
       and sr_store_sk = s_store_sk
 group by s_store_sk), 
 cs as
 (select cs_call_center_sk,
        sum(cs_ext_sales_price) as sales,
        sum(cs_net_profit) as profit
 from catalog_sales,
      date_dim
 where cs_sold_date_sk = d_date_sk
       and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
 group by cs_call_center_sk 
 ), 
 cr as
 (select cr_call_center_sk,
         sum(cr_return_amount) as returns,
         sum(cr_net_loss) as profit_loss
 from catalog_returns,
      date_dim
 where cr_returned_date_sk = d_date_sk
       and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
 group by cr_call_center_sk
 ), 
 ws as
 ( select wp_web_page_sk,
        sum(ws_ext_sales_price) as sales,
        sum(ws_net_profit) as profit
 from web_sales,
      date_dim,
      web_page
 where ws_sold_date_sk = d_date_sk
       and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
       and ws_web_page_sk = wp_web_page_sk
 group by wp_web_page_sk), 
 wr as
 (select wp_web_page_sk,
        sum(wr_return_amt) as returns,
        sum(wr_net_loss) as profit_loss
 from web_returns,
      date_dim,
      web_page
 where wr_returned_date_sk = d_date_sk
       and d_date BETWEEN sales_date and (sales_date +  interval '30 day')
       and wr_web_page_sk = wp_web_page_sk
 group by wp_web_page_sk)
 
select channel
        , id
        , sum(sales) as sales
        , sum(returns) as returns
        , sum(profit) as profit
 from 
 (select 'store channel' as channel
        , ss.s_store_sk as id
        , sales
        , coalesce(returns, 0) as returns
        , (profit - coalesce(profit_loss,0)) as profit
 from   ss left join sr
        on  ss.s_store_sk = sr.s_store_sk
 union all
 select 'catalog channel' as channel
        , cs_call_center_sk as id
        , sales
        , returns
        , (profit - profit_loss) as profit
 from  cs
       , cr
 union all
 select 'web channel' as channel
        , ws.wp_web_page_sk as id
        , sales
        , coalesce(returns, 0) as returns
        , (profit - coalesce(profit_loss,0)) as profit
 from   ws left join wr
        on  ws.wp_web_page_sk = wr.wp_web_page_sk
 ) x
 group by rollup (channel, id)
 order by channel
         ,id
limit 100

$$ LANGUAGE SQL;

--
-- データマート作成
--
DROP TABLE IF EXISTS datamart77 CASCADE;
CREATE TABLE datamart77 AS
SELECT day, channel, id, sales, returns, profit
  FROM (SELECT day FROM generated_days WHERE day BETWEEN DATE '1998-01-01' AND DATE '2002-08-31') AS T1,
       LATERAL (SELECT * FROM query77(CAST(T1.day AS date))) AS T2;
