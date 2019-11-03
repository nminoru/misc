package main

import (
	"fmt"
)

func main() {
	var guid1 string
	var guid2 string
	var id int

	_, err := fmt.Sscanf("7991d316-0fe4-4457-87bb-f2c9b6c7d012 f2bbf484-f787-4e59-a216-4cda59ba2863 1", "%s %s %d", &guid1, &guid2, &id)

	fmt.Println(err, " ", guid1, " ", guid2, " ", id)
}
