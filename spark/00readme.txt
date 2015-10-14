$ sbt package
$ spark-submit --master spark://10.25.140.82:7077 --class "dbt3query1" --driver-memory 64g --executor-memory 64g target/scala-2.11/dbt3query1_2.11-1.0.jar
$ spark-submit --master spark://10.25.140.82:7077 --class "dbt3query1v" --driver-memory 64g --executor-memory 64g target/scala-2.11/dbt3query1v_2.11-1.0.jar

