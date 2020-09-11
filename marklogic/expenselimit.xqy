xquery version "1.0-ml";
import module namespace tde = "http://marklogic.com/xdmp/tde"
at "/MarkLogic/tde.xqy";
let $limits :=
<template xmlns="http://marklogic.com/xdmp/tde">
<context>ExpenseLimit</context>
<rows>
<row>
<schema-name>main</schema-name>
<view-name>expenselimit</view-name>
<columns>
<column>
<name>Category</name>
<scalar-type>string</scalar-type>
<val>Category</val>
</column>
<column>
<name>Limit</name>
<scalar-type>decimal</scalar-type>
<val>Limit</val>
</column>
</columns>
</row>
</rows>
</template>
return tde:template-insert("/limits.xml", $limits)
