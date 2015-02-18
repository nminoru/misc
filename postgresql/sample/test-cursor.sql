BEGIN;

DECLARE liahona CURSOR FOR SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice, l_discount, l_tax FROM lineitem WHERE l_shipdate <= date'1998-12-01' - interval '80 days' ORDER BY l_returnflag, l_linestatus;

CLOSE liahona;

END;
