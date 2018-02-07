import scala.collection.Seq
import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Row
import org.apache.spark.sql.types._

object parquet_append_test {

    case class Person(name: String, age: Long)

    def main(args: Array[String]) {
        val spark = SparkSession
		    .builder()
			.appName("parquet-append-test")
			.getOrCreate()

		import spark.implicits._

		val caseClassDS = Seq(Person("Andy", 32)).toDS()
		caseClassDS.show()

		caseClassDS.write.format("parquet").mode("overwrite").save("person.parquet");

		caseClassDS.write.format("parquet").mode("append").save("person.parquet");

		spark.stop()
    }
}
