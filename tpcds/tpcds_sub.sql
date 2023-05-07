BEGIN;

DROP TABLE IF EXISTS store_sales2 CASCADE;
DROP TABLE IF EXISTS store_returns2 CASCADE;
DROP TABLE IF EXISTS web_sales2 CASCADE;
DROP TABLE IF EXISTS web_returns2 CASCADE;
DROP TABLE IF EXISTS catalog_sales2 CASCADE;
DROP TABLE IF EXISTS catalog_returns2 CASCADE;

CREATE TABLE store_sales2
(
    ss_sold_date_sk           integer                       ,
    ss_sold_time_sk           integer                       ,
    ss_item_sk                integer               NOT NULL,
    ss_customer_sk            integer                       ,
    ss_cdemo_sk               integer                       ,
    ss_hdemo_sk               integer                       ,
    ss_addr_sk                integer                       ,
    ss_store_sk               integer                       ,
    ss_promo_sk               integer                       ,
    ss_ticket_number          integer               NOT NULL,
    ss_quantity               integer                       ,
    ss_wholesale_cost         decimal(7,2)                  ,
    ss_list_price             decimal(7,2)                  ,
    ss_sales_price            decimal(7,2)                  ,
    ss_ext_discount_amt       decimal(7,2)                  ,
    ss_ext_sales_price        decimal(7,2)                  ,
    ss_ext_wholesale_cost     decimal(7,2)                  ,
    ss_ext_list_price         decimal(7,2)                  ,
    ss_ext_tax                decimal(7,2)                  ,
    ss_coupon_amt             decimal(7,2)                  ,
    ss_net_paid               decimal(7,2)                  ,
    ss_net_paid_inc_tax       decimal(7,2)                  ,
    ss_net_profit             decimal(7,2)                  ,
    ss_diff_date_sk           integer                       ,        
    PRIMARY KEY (ss_item_sk, ss_ticket_number)
);

CREATE TABLE store_returns2
(
    sr_returned_date_sk       integer                       ,
    sr_return_time_sk         integer                       ,
    sr_item_sk                integer               not null,
    sr_customer_sk            integer                       ,
    sr_cdemo_sk               integer                       ,
    sr_hdemo_sk               integer                       ,
    sr_addr_sk                integer                       ,
    sr_store_sk               integer                       ,
    sr_reason_sk              integer                       ,
    sr_ticket_number          integer               not null,
    sr_return_quantity        integer                       ,
    sr_return_amt             decimal(7,2)                  ,
    sr_return_tax             decimal(7,2)                  ,
    sr_return_amt_inc_tax     decimal(7,2)                  ,
    sr_fee                    decimal(7,2)                  ,
    sr_return_ship_cost       decimal(7,2)                  ,
    sr_refunded_cash          decimal(7,2)                  ,
    sr_reversed_charge        decimal(7,2)                  ,
    sr_store_credit           decimal(7,2)                  ,
    sr_net_loss               decimal(7,2)                  ,
    sr_dummy                  text                          ,            
    primary key (sr_item_sk, sr_ticket_number)
);

CREATE TABLE web_sales2
(
    ws_sold_date_sk           integer                       ,
    ws_sold_time_sk           integer                       ,
    ws_ship_date_sk           integer                       ,
    ws_item_sk                integer               NOT NULL,
    ws_bill_customer_sk       integer                       ,
    ws_bill_cdemo_sk          integer                       ,
    ws_bill_hdemo_sk          integer                       ,
    ws_bill_addr_sk           integer                       ,
    ws_ship_customer_sk       integer                       ,
    ws_ship_cdemo_sk          integer                       ,
    ws_ship_hdemo_sk          integer                       ,
    ws_ship_addr_sk           integer                       ,
    ws_web_page_sk            integer                       ,
    ws_web_site_sk            integer                       ,
    ws_ship_mode_sk           integer                       ,
    ws_warehouse_sk           integer                       ,
    ws_promo_sk               integer                       ,
    ws_order_number           integer               NOT NULL,
    ws_quantity               integer                       ,
    ws_wholesale_cost         decimal(7,2)                  ,
    ws_list_price             decimal(7,2)                  ,
    ws_sales_price            decimal(7,2)                  ,
    ws_ext_discount_amt       decimal(7,2)                  ,
    ws_ext_sales_price        decimal(7,2)                  ,
    ws_ext_wholesale_cost     decimal(7,2)                  ,
    ws_ext_list_price         decimal(7,2)                  ,
    ws_ext_tax                decimal(7,2)                  ,
    ws_coupon_amt             decimal(7,2)                  ,
    ws_ext_ship_cost          decimal(7,2)                  ,
    ws_net_paid               decimal(7,2)                  ,
    ws_net_paid_inc_tax       decimal(7,2)                  ,
    ws_net_paid_inc_ship      decimal(7,2)                  ,
    ws_net_paid_inc_ship_tax  decimal(7,2)                  ,
    ws_net_profit             decimal(7,2)                  ,
    ws_diff_date_sk           integer                       ,
    PRIMARY KEY (ws_item_sk, ws_order_number)
);

CREATE TABLE web_returns2
(
    wr_returned_date_sk       integer                       ,
    wr_returned_time_sk       integer                       ,
    wr_item_sk                integer               not null,
    wr_refunded_customer_sk   integer                       ,
    wr_refunded_cdemo_sk      integer                       ,
    wr_refunded_hdemo_sk      integer                       ,
    wr_refunded_addr_sk       integer                       ,
    wr_returning_customer_sk  integer                       ,
    wr_returning_cdemo_sk     integer                       ,
    wr_returning_hdemo_sk     integer                       ,
    wr_returning_addr_sk      integer                       ,
    wr_web_page_sk            integer                       ,
    wr_reason_sk              integer                       ,
    wr_order_number           integer               not null,
    wr_return_quantity        integer                       ,
    wr_return_amt             decimal(7,2)                  ,
    wr_return_tax             decimal(7,2)                  ,
    wr_return_amt_inc_tax     decimal(7,2)                  ,
    wr_fee                    decimal(7,2)                  ,
    wr_return_ship_cost       decimal(7,2)                  ,
    wr_refunded_cash          decimal(7,2)                  ,
    wr_reversed_charge        decimal(7,2)                  ,
    wr_account_credit         decimal(7,2)                  ,
    wr_net_loss               decimal(7,2)                  ,
    wr_dummy                  text                          ,
    primary key (wr_item_sk, wr_order_number)
);

CREATE TABLE catalog_sales2
(
    cs_sold_date_sk           integer                       ,
    cs_sold_time_sk           integer                       ,
    cs_ship_date_sk           integer                       ,
    cs_bill_customer_sk       integer                       ,
    cs_bill_cdemo_sk          integer                       ,
    cs_bill_hdemo_sk          integer                       ,
    cs_bill_addr_sk           integer                       ,
    cs_ship_customer_sk       integer                       ,
    cs_ship_cdemo_sk          integer                       ,
    cs_ship_hdemo_sk          integer                       ,
    cs_ship_addr_sk           integer                       ,
    cs_call_center_sk         integer                       ,
    cs_catalog_page_sk        integer                       ,
    cs_ship_mode_sk           integer                       ,
    cs_warehouse_sk           integer                       ,
    cs_item_sk                integer               NOT NULL,
    cs_promo_sk               integer                       ,
    cs_order_number           integer               NOT NULL,
    cs_quantity               integer                       ,
    cs_wholesale_cost         decimal(7,2)                  ,
    cs_list_price             decimal(7,2)                  ,
    cs_sales_price            decimal(7,2)                  ,
    cs_ext_discount_amt       decimal(7,2)                  ,
    cs_ext_sales_price        decimal(7,2)                  ,
    cs_ext_wholesale_cost     decimal(7,2)                  ,
    cs_ext_list_price         decimal(7,2)                  ,
    cs_ext_tax                decimal(7,2)                  ,
    cs_coupon_amt             decimal(7,2)                  ,
    cs_ext_ship_cost          decimal(7,2)                  ,
    cs_net_paid               decimal(7,2)                  ,
    cs_net_paid_inc_tax       decimal(7,2)                  ,
    cs_net_paid_inc_ship      decimal(7,2)                  ,
    cs_net_paid_inc_ship_tax  decimal(7,2)                  ,
    cs_net_profit             decimal(7,2)                  ,
    cs_diff_date_sk           integer                       ,    
    PRIMARY KEY (cs_item_sk, cs_order_number)
);

CREATE TABLE catalog_returns2
(
    cr_returned_date_sk       integer                       ,
    cr_returned_time_sk       integer                       ,
    cr_item_sk                integer               not null,
    cr_refunded_customer_sk   integer                       ,
    cr_refunded_cdemo_sk      integer                       ,
    cr_refunded_hdemo_sk      integer                       ,
    cr_refunded_addr_sk       integer                       ,
    cr_returning_customer_sk  integer                       ,
    cr_returning_cdemo_sk     integer                       ,
    cr_returning_hdemo_sk     integer                       ,
    cr_returning_addr_sk      integer                       ,
    cr_call_center_sk         integer                       ,
    cr_catalog_page_sk        integer                       ,
    cr_ship_mode_sk           integer                       ,
    cr_warehouse_sk           integer                       ,
    cr_reason_sk              integer                       ,
    cr_order_number           integer               not null,
    cr_return_quantity        integer                       ,
    cr_return_amount          decimal(7,2)                  ,
    cr_return_tax             decimal(7,2)                  ,
    cr_return_amt_inc_tax     decimal(7,2)                  ,
    cr_fee                    decimal(7,2)                  ,
    cr_return_ship_cost       decimal(7,2)                  ,
    cr_refunded_cash          decimal(7,2)                  ,
    cr_reversed_charge        decimal(7,2)                  ,
    cr_store_credit           decimal(7,2)                  ,
    cr_net_loss               decimal(7,2)                  ,
    cr_dummy                  text                          ,    
    primary key (cr_item_sk, cr_order_number)
);

-- CREATE TABLE generator_parameter
-- (
--     diff_data_sk              integer,
--     diff_number               integer
-- );


--
--
--
INSERT INTO store_sales2
SELECT
    ss_sold_date_sk,
    ss_sold_time_sk,
    ss_item_sk,
    ss_customer_sk,
    ss_cdemo_sk,
    ss_hdemo_sk,
    ss_addr_sk,
    ss_store_sk,
    ss_promo_sk,
    ss_ticket_number,
    ss_quantity,
    ss_wholesale_cost,
    ss_list_price,
    ss_sales_price,
    ss_ext_discount_amt,
    ss_ext_sales_price,
    ss_ext_wholesale_cost,
    ss_ext_list_price,
    ss_ext_tax,
    ss_coupon_amt,
    ss_net_paid,
    ss_net_paid_inc_tax,
    ss_net_profit,
    ss_diff_date_sk
FROM
(
    SELECT
        CASE WHEN ss_sold_date_sk IS NOT NULL THEN 2453006 ELSE NULL END AS ss_sold_date_sk,
        ss_sold_time_sk,
        ss_item_sk,
        ss_customer_sk,
        ss_cdemo_sk,
        ss_hdemo_sk,
        ss_addr_sk,
        ss_store_sk,
        ss_promo_sk,
        ss_ticket_number,
        ss_quantity,
        ss_wholesale_cost,
        ss_list_price,
        ss_sales_price,
        ss_ext_discount_amt,
        ss_ext_sales_price,
        ss_ext_wholesale_cost,
        ss_ext_list_price,
        ss_ext_tax,
        ss_coupon_amt,
        ss_net_paid,
        ss_net_paid_inc_tax,
        ss_net_profit,
        CASE WHEN store_sales.ss_sold_date_sk IS NOT NULL THEN store_sales.ss_sold_date_sk - 2453006 ELSE NULL END AS ss_diff_date_sk,
        row_number() over() AS rowid
    FROM store_sales
) S
WHERE
   rowid % 10 = 0
;   
   
    

INSERT INTO web_sales2
SELECT
    ws_sold_date_sk,
    ws_sold_time_sk,
    ws_ship_date_sk,
    ws_item_sk,
    ws_bill_customer_sk,
    ws_bill_cdemo_sk,
    ws_bill_hdemo_sk,
    ws_bill_addr_sk,
    ws_ship_customer_sk,
    ws_ship_cdemo_sk,
    ws_ship_hdemo_sk,
    ws_ship_addr_sk,
    ws_web_page_sk,
    ws_web_site_sk,
    ws_ship_mode_sk,
    ws_warehouse_sk,
    ws_promo_sk,
    ws_order_number,
    ws_quantity,
    ws_wholesale_cost,
    ws_list_price,
    ws_sales_price,
    ws_ext_discount_amt,
    ws_ext_sales_price,
    ws_ext_wholesale_cost,
    ws_ext_list_price,
    ws_ext_tax,
    ws_coupon_amt,
    ws_ext_ship_cost,
    ws_net_paid,
    ws_net_paid_inc_tax,
    ws_net_paid_inc_ship,
    ws_net_paid_inc_ship_tax,
    ws_net_profit,
    ws_diff_date_sk
FROM
(
    SELECT
        CASE WHEN ws_sold_date_sk IS NOT NULL THEN 2453006 ELSE NULL END AS ws_sold_date_sk,
        ws_sold_time_sk,
        CASE WHEN ws_ship_date_sk IS NOT NULL THEN 2453006 + ws_ship_date_sk - ws_sold_date_sk ELSE NULL END AS ws_ship_date_sk,
        ws_item_sk,
        ws_bill_customer_sk,
        ws_bill_cdemo_sk,
        ws_bill_hdemo_sk,
        ws_bill_addr_sk,
        ws_ship_customer_sk,
        ws_ship_cdemo_sk,
        ws_ship_hdemo_sk,
        ws_ship_addr_sk,
        ws_web_page_sk,
        ws_web_site_sk,
        ws_ship_mode_sk,
        ws_warehouse_sk,
        ws_promo_sk,
        ws_order_number,
        ws_quantity,
        ws_wholesale_cost,
        ws_list_price,
        ws_sales_price,
        ws_ext_discount_amt,
        ws_ext_sales_price,
        ws_ext_wholesale_cost,
        ws_ext_list_price,
        ws_ext_tax,
        ws_coupon_amt,
        ws_ext_ship_cost,
        ws_net_paid,
        ws_net_paid_inc_tax,
        ws_net_paid_inc_ship,
        ws_net_paid_inc_ship_tax,
        ws_net_profit,
        CASE WHEN web_sales.ws_sold_date_sk IS NOT NULL THEN web_sales.ws_sold_date_sk - 2453006 ELSE NULL END AS ws_diff_date_sk,
        row_number() over() AS rowid
    FROM web_sales
    WHERE NOT (web_sales.ws_sold_date_sk IS NULL AND web_sales.ws_ship_date_sk IS NULL)
) S
WHERE
   rowid % 10 = 0
;       


INSERT INTO catalog_sales2
SELECT
    cs_sold_date_sk2,
    cs_sold_time_sk,
    cs_ship_date_sk2,
    cs_bill_customer_sk,
    cs_bill_cdemo_sk,
    cs_bill_hdemo_sk,
    cs_bill_addr_sk,
    cs_ship_customer_sk,
    cs_ship_cdemo_sk,
    cs_ship_hdemo_sk,
    cs_ship_addr_sk,
    cs_call_center_sk,
    cs_catalog_page_sk,
    cs_ship_mode_sk,
    cs_warehouse_sk,
    cs_item_sk,
    cs_promo_sk,
    cs_order_number,
    cs_quantity,
    cs_wholesale_cost,
    cs_list_price,
    cs_sales_price,
    cs_ext_discount_amt,
    cs_ext_sales_price,
    cs_ext_wholesale_cost,
    cs_ext_list_price,
    cs_ext_tax,
    cs_coupon_amt,
    cs_ext_ship_cost,
    cs_net_paid,
    cs_net_paid_inc_tax,
    cs_net_paid_inc_ship,
    cs_net_paid_inc_ship_tax,
    cs_net_profit,
    cs_diff_date_sk
FROM
(    
    SELECT
        CASE WHEN cs_sold_date_sk IS NOT NULL THEN 2453006 ELSE NULL END AS cs_sold_date_sk2,
        cs_sold_time_sk,
        CASE WHEN cs_ship_date_sk IS NOT NULL THEN 2453006 + cs_ship_date_sk - cs_sold_date_sk ELSE NULL END AS cs_ship_date_sk2,
        cs_bill_customer_sk,
        cs_bill_cdemo_sk,
        cs_bill_hdemo_sk,
        cs_bill_addr_sk,
        cs_ship_customer_sk,
        cs_ship_cdemo_sk,
        cs_ship_hdemo_sk,
        cs_ship_addr_sk,
        cs_call_center_sk,
        cs_catalog_page_sk,
        cs_ship_mode_sk,
        cs_warehouse_sk,
        cs_item_sk,
        cs_promo_sk,
        cs_order_number,
        cs_quantity,
        cs_wholesale_cost,
        cs_list_price,
        cs_sales_price,
        cs_ext_discount_amt,
        cs_ext_sales_price,
        cs_ext_wholesale_cost,
        cs_ext_list_price,
        cs_ext_tax,
        cs_coupon_amt,
        cs_ext_ship_cost,
        cs_net_paid,
        cs_net_paid_inc_tax,
        cs_net_paid_inc_ship,
        cs_net_paid_inc_ship_tax,
        cs_net_profit,
        CASE WHEN cs_sold_date_sk IS NOT NULL THEN cs_sold_date_sk - 2453006 ELSE NULL END AS cs_diff_date_sk,
        row_number() over() AS rowid
    FROM catalog_sales
    WHERE NOT (catalog_sales.cs_sold_date_sk IS NULL AND catalog_sales.cs_ship_date_sk IS NULL)
) S
WHERE
   rowid % 10 = 0
;

INSERT INTO store_returns2
SELECT
    S.sr_returned_date_sk + ss_diff_date_sk,
    S.sr_return_time_sk,
    S.sr_item_sk,
    S.sr_customer_sk,
    S.sr_cdemo_sk,
    S.sr_hdemo_sk,
    S.sr_addr_sk,
    S.sr_store_sk,
    S.sr_reason_sk,
    S.sr_ticket_number,
    S.sr_return_quantity,
    S.sr_return_amt,
    S.sr_return_tax,
    S.sr_return_amt_inc_tax,
    S.sr_fee,
    S.sr_return_ship_cost,
    S.sr_refunded_cash,
    S.sr_reversed_charge,
    S.sr_store_credit,
    S.sr_net_loss,
    null
FROM store_returns S LEFT OUTER JOIN store_sales2 ON (ss_item_sk = S.sr_item_sk AND ss_ticket_number = S.sr_ticket_number)
WHERE store_sales2.ss_item_sk IS NOT NULL AND store_sales2.ss_ticket_number IS NOT NULL;

INSERT INTO catalog_returns2
SELECT
    C.cr_returned_date_sk + cs_diff_date_sk,
    C.cr_returned_time_sk,
    C.cr_item_sk,
    C.cr_refunded_customer_sk,
    C.cr_refunded_cdemo_sk,
    C.cr_refunded_hdemo_sk,
    C.cr_refunded_addr_sk,
    C.cr_returning_customer_sk,
    C.cr_returning_cdemo_sk,
    C.cr_returning_hdemo_sk,
    C.cr_returning_addr_sk,
    C.cr_call_center_sk,
    C.cr_catalog_page_sk,
    C.cr_ship_mode_sk,
    C.cr_warehouse_sk,
    C.cr_reason_sk,
    C.cr_order_number,
    C.cr_return_quantity,
    C.cr_return_amount,
    C.cr_return_tax,
    C.cr_return_amt_inc_tax,
    C.cr_fee,
    C.cr_return_ship_cost,
    C.cr_refunded_cash,
    C.cr_reversed_charge,
    C.cr_store_credit,
    C.cr_net_loss,
    null
FROM catalog_returns C LEFT OUTER JOIN catalog_sales2 ON (cs_item_sk = C.cr_item_sk AND cs_order_number = C.cr_order_number)
WHERE catalog_sales2.cs_item_sk IS NOT NULL AND catalog_sales2.cs_order_number IS NOT NULL;

INSERT INTO web_returns2
SELECT
    W.wr_returned_date_sk + ws_diff_date_sk,
    W.wr_returned_time_sk,
    W.wr_item_sk,
    W.wr_refunded_customer_sk,
    W.wr_refunded_cdemo_sk,
    W.wr_refunded_hdemo_sk,
    W.wr_refunded_addr_sk,
    W.wr_returning_customer_sk,
    W.wr_returning_cdemo_sk,
    W.wr_returning_hdemo_sk,
    W.wr_returning_addr_sk,
    W.wr_web_page_sk,
    W.wr_reason_sk,
    W.wr_order_number,
    W.wr_return_quantity,
    W.wr_return_amt,
    W.wr_return_tax,
    W.wr_return_amt_inc_tax,
    W.wr_fee,
    W.wr_return_ship_cost,
    W.wr_refunded_cash,
    W.wr_reversed_charge,
    W.wr_account_credit,
    W.wr_net_loss,
    null
FROM web_returns W LEFT OUTER JOIN web_sales2 ON (ws_item_sk = W.wr_item_sk AND ws_order_number = W.wr_order_number)
WHERE web_sales2.ws_item_sk IS NOT NULL AND web_sales2.ws_order_number IS NOT NULL;

END;
