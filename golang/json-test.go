package main

import (
    "fmt"
    "log"
    "encoding/json"    
)

type Book struct {
	GUID             string    `json:"guid"`
	FileName         string    `json:"fileName"`
}

type BookAux struct {
	Title            string    `json:"title"`
}

type BookShelf struct {
	Count int `json:"count"`
	Books []Book `json:"books"`
    Aux   *BookAux `json:"aux"`
    Time  int64
}

func main() {
    var bookShelf BookShelf

    bytes := []byte(`{"count":100,"books":[{"guid":"58c879c5-3183-4033-9789-bddbd58805b0","fileName":"foo.txt"},{"guid":"a59237f6-c084-43d8-8f13-72c4892b1048","fileName":"bar.txt"}], "aux":{"title":"TITLE"}}`)
    
    if err := json.Unmarshal(bytes, &bookShelf); err != nil {
        log.Fatal(err)
        return
    }

    fmt.Println(bookShelf.Count)

    for i, book := range bookShelf.Books {
        fmt.Printf("[%d]\n", i);
        fmt.Println("\t" + book.GUID);
        fmt.Println("\t" + book.FileName);
    }

    if bookShelf.Aux != nil {
        fmt.Println(bookShelf.Aux.Title)
    }
    
    fmt.Println(bookShelf.Time)
}

