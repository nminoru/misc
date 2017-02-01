#include <stdio.h>
#include <stdint.h>

#include <bson.h>
#include <mongoc.h>

static void parse_json(bson_iter_t *iter, int level);

int
main(int argc, char **argv)
{
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    int64_t count;
    bson_t reply;
    bson_t *query;
    mongoc_cursor_t *cursor;
    const bson_t *doc;

    mongoc_init ();

    client = mongoc_client_new ("mongodb://localhost:27017/");
    collection = mongoc_client_get_collection (client, "mydb", "mycoll");

    /* Count the collection */
    count = mongoc_collection_count (collection, MONGOC_QUERY_NONE, NULL, 0, 0, NULL, &error);
    if (count < 0) {
        fprintf (stderr, "%s\n", error.message);
    } else {
        printf ("count: %" PRId64 "\n", count);
    }

    if (mongoc_collection_stats (collection, NULL, &reply, &error))
    {
        bson_iter_t iter;

        if (bson_iter_init (&iter, &reply)) {
            while (bson_iter_next (&iter)) {
                if (strcmp(bson_iter_key (&iter), "size") == 0) {
                    printf("size: %ld\n", bson_iter_as_int64 (&iter));
                }
                else if (strcmp(bson_iter_key (&iter), "count") == 0) {
                    printf("count: %ld\n", bson_iter_as_int64 (&iter));
                }
            }
        }
    } 

    /* Scan all documents in the collection */
    query = bson_new ();
    cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);
    
    while (mongoc_cursor_next (cursor, &doc)) {
        char *str;
        bson_iter_t iter;

        str = bson_as_json (doc, NULL);
        printf ("%s\n", str);
        bson_free (str);

        if (bson_iter_init (&iter, doc)) {
            parse_json(&iter, 0);
        }
    }

    bson_destroy (query);

    mongoc_cursor_destroy (cursor);
    mongoc_collection_destroy (collection);
    mongoc_client_destroy (client);
    mongoc_cleanup ();

    return count < 0 ? -1 : 0;
}

static void
parse_json(bson_iter_t *iter, int level)
{
    uint32_t length = 0;;

    while (bson_iter_next (iter)) {
        printf ("%*s\"%s\":", 4 * (level + 1), " ", bson_iter_key (iter));

        switch (bson_iter_type (iter))
        {
        case BSON_TYPE_DOUBLE:
            printf("%f", bson_iter_double (iter));
            break;
        case BSON_TYPE_UTF8:
            printf("\"%s\"", bson_iter_utf8 (iter, &length));
            break;
        case BSON_TYPE_DOCUMENT:
            break;
        case BSON_TYPE_ARRAY:
            break;
        case BSON_TYPE_BINARY:
            break;
        case BSON_TYPE_OID:
            break;
        case BSON_TYPE_BOOL:
            printf("%s", bson_iter_bool (iter) ? "true" : "false");
            break;
        case BSON_TYPE_DATE_TIME:
            printf("%" PRId64, bson_iter_date_time (iter));
            break;
        case BSON_TYPE_NULL:
            printf("null");
            break;
        case BSON_TYPE_REGEX:
            break;
        case BSON_TYPE_CODE:
            break;
        case BSON_TYPE_CODEWSCOPE:
            break;
        case BSON_TYPE_INT32:
            printf("%d", bson_iter_int32 (iter));
            break;
        case BSON_TYPE_TIMESTAMP:
            break;
        case BSON_TYPE_INT64:
            printf("%" PRId64, bson_iter_int64 (iter));
            break;
        case BSON_TYPE_MAXKEY:
            printf("max");
            break;
        case BSON_TYPE_MINKEY:
            printf("min");
            break;
        default:
            break;
        }
            
        printf ("\n");

        if (bson_iter_type (iter) == BSON_TYPE_DOCUMENT) {
            bson_iter_t child;

            if (bson_iter_recurse(iter, &child)) {
                parse_json(&child, level + 1);
            }
        }
    }
}
