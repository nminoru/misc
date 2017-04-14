import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf
import org.apache.spark.sql.SQLContext

object dbt3query1 {
       def main(args: Array[String]) {
       val conf = new SparkConf().setAppName("DBT3 Query1")
       val sc = new SparkContext(conf)
       val sqlContext = new SQLContext(sc)
       import sqlContext.implicits._
       val lineitemRDD = sqlContext.jdbc("jdbc:postgresql://10.25.140.82:5432/dbt3", "public.lineitem", "l_orderkey", 1, 60000000, 16)
       lineitemRDD.registerTempTable("lineitem")
       var res = sqlContext.sql("SELECT l_returnflag, l_linestatus, sum(l_quantity), sum(l_extendedprice), sum(l_extendedprice * (1 - l_discount)), sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)), avg(l_quantity), avg(l_extendedprice), avg(l_discount), count(*) FROM lineitem WHERE l_shipdate <= '1998-09-12' GROUP BY l_returnflag, l_linestatus ORDER BY l_returnflag, l_linestatus")
       val start = System.currentTimeMillis
       res.show()
       println((System.currentTimeMillis - start) + "msec")
  }
}