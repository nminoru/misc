package main

import (
    "fmt"
    "time"
)

func main() {
    t, _ := time.Parse(time.RFC3339, "2019-10-20T01:32:57Z")
    fmt.Printf("time %d\n", t.UnixNano() / 1000000)

    t, _  = time.Parse(time.RFC3339, "2019-10-20T01:32:57.333Z")
    fmt.Printf("time %d\n", t.UnixNano() / 1000000)    
}
