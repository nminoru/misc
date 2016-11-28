#include <stdio.h>

#include <bson.h>
#include <mongoc.h>

int
main(int argc, char **argv)
{
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    int64_t count;

    mongoc_init();

    client = mongoc_client_new("mongodb://localhost:27017/");
    collection = mongoc_client_get_collection(client, "mydb", "mycoll");
    count = mongoc_collection_count(collection, MONGOC_QUERY_NONE, NULL, 0, 0, NULL, &error);

    if (count < 0) {
        fprintf (stderr, "%s\n", error.message);
    } else {
        printf ("%" PRId64 "\n", count);
    }

    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return count < 0 ? -1 : 0;
}
