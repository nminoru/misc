name := "dbt3query1"

version := "1.0"

scalaVersion := "2.11.7"

libraryDependencies += "org.apache.spark" %% "spark-core" % "1.5.0" % "provided"
libraryDependencies += "org.apache.spark" %% "spark-sql" % "1.5.0" % "provided"
libraryDependencies += "org.postgresql" % "postgresql" % "9.4-1203.jdbc41" from "https://jdbc.postgresql.org/download/postgresql-9.4-1203.jdbc4.jar"
