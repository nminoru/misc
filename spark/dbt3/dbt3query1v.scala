import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf
import org.apache.spark.sql.SQLContext

object dbt3query1v {
       def main(args: Array[String]) {
       val conf = new SparkConf().setAppName("DBT3 Query1 Variant")
       val sc = new SparkContext(conf)
       val sqlContext = new SQLContext(sc)
       import sqlContext.implicits._
       val lineitemRDD1 = sqlContext.jdbc("jdbc:postgresql://10.25.140.82:5432/dbt3", "public.lineitem", "l_orderkey", 1, 60000000, 16)
       lineitemRDD1.registerTempTable("lineitem1")
       val lineitemRDD2 = sqlContext.jdbc("jdbc:postgresql://10.25.140.82:5433/dbt3", "public.lineitem", "l_orderkey", 1, 60000000, 16)
       lineitemRDD2.registerTempTable("lineitem2")
       var res = sqlContext.sql("SELECT l1.l_returnflag, l1.l_linestatus, sum(l1.l_extendedprice * (1 - l2.l_discount) * (1 + l2.l_tax)) AS sum_charge, count(*) AS count_order FROM lineitem1 l1, lineitem2 l2 WHERE l1.l_orderkey = l2.l_orderkey AND l1.l_linenumber = l2.l_linenumber AND l2.l_shipdate <= '1997-09-12' GROUP BY l1.l_returnflag, l1.l_linestatus ORDER BY l1.l_returnflag, l1.l_linestatus")
       val start = System.currentTimeMillis
       res.show()
       println((System.currentTimeMillis - start) + "msec")
  }
}