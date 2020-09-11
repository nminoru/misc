xquery version "1.0-ml";
import module namespace tde = "http://marklogic.com/xdmp/tde"
at "/MarkLogic/tde.xqy";
let $vendors :=
<template xmlns="http://marklogic.com/xdmp/tde">
<context>ApprovedVendor</context>
<rows>
<row>
<schema-name>main</schema-name>
<view-name>approvedvendor</view-name>
<columns>
<column>
<name>Vendor</name>
<scalar-type>string</scalar-type>
<val>Name</val>
</column>
<column>
<name>City</name>
<scalar-type>string</scalar-type>
<val>Address/City</val>
</column>
</columns>
</row>
</rows>
</template>
return tde:template-insert("/vendors.xml", $vendors);