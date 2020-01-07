package main

import (
    "os"
    "time"
    "log"
)

func main() {
    filename := os.Args[1]

    now := time.Now()

    if err := os.Chtimes(filename, now, now) ; err != nil {
        log.Fatal(err)
        return
    }
}
