#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct kvpair {
	const char *key;
	const char *value;
};
int kvpair_compar(const void *a, const void *b)
{
	return strcmp(((struct kvpair *)a)->key, ((struct kvpair *)b)->key);
}
void kvpair_free(void *a)
{
	free(a);
}
void print_cb(void *a, void *ctx)
{
	(void)ctx;
	struct kvpair *kv = a;
	printf("%s -> %s\n", kv->key, kv->value);
}
void oom()
{
	fprintf(stderr, "out of memory\n");
	abort();
}
int main(int argc, char **argv)
{
	struct rbtree *rb = rb_new(kvpair_compar, kvpair_free);
	if (!rb)
		oom();
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--get")) {
			if (i+1 >= argc)
				return 1;
			struct kvpair key;
			key.key = argv[i+1];
			struct kvpair *kv = rb_search(rb, &key);
			printf("get %s: %s\n", argv[i+1], kv ? (kv->value ? kv->value : "(null)") : "(not found)");
			i += 1;
		} else if (!strcmp(argv[i], "--set")) {
			if (i+2 >= argc)
				return 1;
			struct kvpair *kv = malloc(sizeof(struct kvpair));
			if (!kv)
				oom();
			kv->key = argv[i+1];
			kv->value = argv[i+2];
			if (rb_insert(rb, kv) == -1)
				oom();
			printf("set %s: %s\n", argv[i+1], argv[i+2]);
			i += 2;
		} else if (!strcmp(argv[i], "--dump")) {
			rb_walk(rb, print_cb, NULL);
		} else if (!strcmp(argv[i], "--new")) {
			rb_free(rb);
			rb = rb_new(kvpair_compar, kvpair_free);
			if (!rb)
				oom();
		} else {
			fprintf(stderr, "unknown command: %s\n", argv[i]);
			return 1;
		}
	}
	rb_free(rb);
}
