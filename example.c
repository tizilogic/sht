#include "sht.h"
#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN 32
#define min_val(a, b) ((a) < (b)) ? (a) : (b)

typedef struct my_type {
	int id;
	char name[MAX_NAME_LEN];
} my_type_t;

int main(void) {
	// hashtable (int, int)
	{
		printf("Example of hashtable (int, int)\n");
		sht_t *sht = sht_init(sizeof(int), 1, 42);
		int k = 0;
		int v = 10;
		sht_set(sht, &k, sizeof(int), &v);
		k = 1;
		v = 42;
		sht_set(sht, &k, sizeof(int), &v);
		k = 0;
		v = 43;
		sht_set(sht, &k, sizeof(int), &v);
		k = 1;
		printf("Number of items in the hashtable: %u\n", sht_size(sht));
		sht_del(sht, &k, sizeof(int));
		printf("Number of items in the hashtable (after delete): %u\n",
		       sht_size(sht));

		int *item;
		sht_it_t *it;
		sht_foreach(sht, it, item) {
			printf("Got item with value %d\n", *item);
		}
		sht_iter_destroy(it);

		sht_destroy(sht);
		printf("Done\n\n");
	}

	// hashtable (int, my_type_t)
	{
		printf("Example of hashtable (int, my_type_t)\n");
		sht_t *sht = sht_init(sizeof(my_type_t), 5, 42);
		my_type_t item;
		item.id = 123;
		memset(item.name, 0, MAX_NAME_LEN);
		strncpy(item.name, "Alice",
		        min_val(strlen("Alice") + 1, MAX_NAME_LEN - 1));
		sht_set(sht, &item.id, sizeof(int), &item);
		item.id = 987;
		memset(item.name, 0, MAX_NAME_LEN);
		strncpy(item.name, "Bob", min_val(strlen("Bob") + 1, MAX_NAME_LEN - 1));
		sht_set(sht, &item.id, sizeof(int), &item);
		printf("Number of items in the hashtable: %u\n", sht_size(sht));

		my_type_t *v;
		sht_it_t *it;
		sht_foreach(sht, it, v) {
			printf("Got item with id %d and name '%s'\n", v->id, v->name);
		}
		sht_iter_destroy(it);

		sht_destroy(sht);
		printf("Done\n\n");
	}

	return 0;
}
