package main

import (
	"fmt"
	"regexp"
)

var pattern = regexp.MustCompile("^(\\S+) (\\S+) (\\S+) (/.*)")

func main() {
	var guid1 string
	var guid2 string
	var id int
	var path string
	var err error

	// sscanf
	_, err = fmt.Sscanf("7991d316-0fe4-4457-87bb-f2c9b6c7d012 f2bbf484-f787-4e59-a216-4cda59ba2863 1 /dir/path", "%s %s %d %s",
		&guid1, &guid2, &id, &path)

	fmt.Println(err, " [", guid1, " : ", guid2, " : ", id, " : ", path+"]")

	// sscanf: %s は空白で区切られた文字列を読み込めない
	_, err = fmt.Sscanf("7991d316-0fe4-4457-87bb-f2c9b6c7d012 f2bbf484-f787-4e59-a216-4cda59ba2863 1 /dir /path /hoge", "%s %s %d %s",
		&guid1, &guid2, &id, &path)

	fmt.Println(err, " [", guid1, " : ", guid2, " : ", id, " : ", path+"]")

	// regexp
	results := pattern.FindStringSubmatch("7991d316-0fe4-4457-87bb-f2c9b6c7d012 f2bbf484-f787-4e59-a216-4cda59ba2863 -1 /dir /path /hoge")

	fmt.Println(len(results))
	fmt.Println(" [" + results[1] + "] : [" + results[2] + "] : [" + results[3] + "] : [" + results[4] + "]")
}
