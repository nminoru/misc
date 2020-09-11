xquery version "1.0-ml";
import module namespace tde = "http://marklogic.com/xdmp/tde"
at "/MarkLogic/tde.xqy";
let $expenses :=
<template xmlns="http://marklogic.com/xdmp/tde">
<context>/Expenses</context>
<rows>
<row>
<schema-name>main</schema-name>
<view-name>expenses</view-name>
<columns>
<column>
<name>EmployeeID</name>
<scalar-type>int</scalar-type>
<val>EmployeeID</val>
</column>
<column>
<name>Date</name>
<scalar-type>date</scalar-type>
<val>Date</val>
</column>
<column>
<name>Category</name>
<scalar-type>string</scalar-type>
<val>Purchase/Category</val>
</column>
<column>
<name>Vendor</name>
<scalar-type>string</scalar-type>
<val>Purchase/Vendor</val>
</column>
<column>
<name>Amount</name>
<scalar-type>decimal</scalar-type>
<val>Amount</val>
</column>
</columns>
</row>
</rows>
</template>
return tde:template-insert("/expenses.xml", $expenses)
