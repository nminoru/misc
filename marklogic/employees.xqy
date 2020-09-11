xquery version "1.0-ml";

import module namespace tde = "http://marklogic.com/xdmp/tde"
at "/MarkLogic/tde.xqy";
let $employees :=
<template xmlns="http://marklogic.com/xdmp/tde">
<context>/Employee</context>
<rows>
<row>
<schema-name>main</schema-name>
<view-name>employees</view-name>
<columns>
<column>
<name>EmployeeID</name>
<scalar-type>int</scalar-type>
<val>ID</val>
</column>
<column>
<name>FirstName</name>
<scalar-type>string</scalar-type>
<val>FirstName</val>
</column>
<column>
<name>LastName</name>
<scalar-type>string</scalar-type>
<val>LastName</val>
</column>
<column>
<name>Position</name>
<scalar-type>string</scalar-type>
<val>Position</val>
</column>
</columns>
</row>
</rows>
</template>
return tde:template-insert("/employees.xml", $employees)
