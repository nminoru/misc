BEGIN;

DROP TABLE IF EXISTS store_sales4     CASCADE;
DROP TABLE IF EXISTS store_returns4   CASCADE;
DROP TABLE IF EXISTS web_sales4       CASCADE;
DROP TABLE IF EXISTS web_returns4     CASCADE;
DROP TABLE IF EXISTS catalog_sales4   CASCADE;
DROP TABLE IF EXISTS catalog_returns4 CASCADE;

SELECT * INTO store_sales4 FROM store_sales3 WHERE ss_sold_time_sk BETWEEN {start_time} AND {end_time};
CREATE INDEX ON store_sales4 (ss_item_sk, ss_ticket_number);
SELECT
    sr_returned_date_sk,
    sr_return_time_sk,
    sr_item_sk,
    sr_customer_sk,
    sr_cdemo_sk,
    sr_hdemo_sk,
    sr_addr_sk,
    sr_store_sk,
    sr_reason_sk,
    sr_ticket_number,
    sr_return_quantity,
    sr_return_amt,
    sr_return_tax,
    sr_return_amt_inc_tax,
    sr_fee,
    sr_return_ship_cost,
    sr_refunded_cash,
    sr_reversed_charge,
    sr_store_credit,
    sr_net_loss,
    sr_dummy
INTO store_returns4 FROM store_returns3 LEFT OUTER JOIN store_sales4 ON (ss_item_sk = sr_item_sk AND ss_ticket_number = sr_ticket_number)
WHERE store_sales4.ss_item_sk IS NOT NULL AND store_sales4.ss_ticket_number IS NOT NULL;

SELECT * INTO catalog_sales4 FROM catalog_sales3 WHERE cs_sold_time_sk BETWEEN {start_time} AND {end_time};
CREATE INDEX ON catalog_sales4 (cs_item_sk, cs_order_number);
SELECT
    cr_returned_date_sk,
    cr_returned_time_sk,
    cr_item_sk,
    cr_refunded_customer_sk,
    cr_refunded_cdemo_sk,
    cr_refunded_hdemo_sk,
    cr_refunded_addr_sk,
    cr_returning_customer_sk,
    cr_returning_cdemo_sk,
    cr_returning_hdemo_sk,
    cr_returning_addr_sk,
    cr_call_center_sk,
    cr_catalog_page_sk,
    cr_ship_mode_sk,
    cr_warehouse_sk,
    cr_reason_sk,
    cr_order_number,
    cr_return_quantity,
    cr_return_amount,
    cr_return_tax,
    cr_return_amt_inc_tax,
    cr_fee,
    cr_return_ship_cost,
    cr_refunded_cash,
    cr_reversed_charge,
    cr_store_credit,
    cr_net_loss,
    cr_dummy
INTO catalog_returns4 FROM catalog_returns3 LEFT OUTER JOIN catalog_sales4 ON (cs_item_sk = cr_item_sk AND cs_order_number = cr_order_number)
WHERE catalog_sales4.cs_item_sk IS NOT NULL AND catalog_sales4.cs_order_number IS NOT NULL;

SELECT * INTO web_sales4 FROM web_sales3 WHERE ws_sold_time_sk BETWEEN {start_time} AND {end_time};
CREATE INDEX ON web_sales4 (ws_item_sk, ws_order_number);
SELECT
    wr_returned_date_sk,
    wr_returned_time_sk,
    wr_item_sk,
    wr_refunded_customer_sk,
    wr_refunded_cdemo_sk,
    wr_refunded_hdemo_sk,
    wr_refunded_addr_sk,
    wr_returning_customer_sk,
    wr_returning_cdemo_sk,
    wr_returning_hdemo_sk,
    wr_returning_addr_sk,
    wr_web_page_sk,
    wr_reason_sk,
    wr_order_number,
    wr_return_quantity,
    wr_return_amt,
    wr_return_tax,
    wr_return_amt_inc_tax,
    wr_fee,
    wr_return_ship_cost,
    wr_refunded_cash,
    wr_reversed_charge,
    wr_account_credit,
    wr_net_loss,
    wr_dummy
INTO web_returns4 FROM web_returns3 LEFT OUTER JOIN web_sales4 ON (ws_item_sk = wr_item_sk AND ws_order_number = wr_order_number)
WHERE web_sales4.ws_item_sk IS NOT NULL AND web_sales4.ws_order_number IS NOT NULL;

END;
