#include "trie.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
	struct trie_node *trie = NULL;
	struct trie_find *f;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--get")) {
			if (i+1 >= argc)
				return 1;
			const char *s = trie_get(&trie, argv[i+1]);
			printf("get %s: %s\n", argv[i+1], s ? s : "(null)");
			i += 1;
		} else if (!strcmp(argv[i], "--set")) {
			if (i+2 >= argc)
				return 1;
			const char *s = trie_set(&trie, argv[i+1], argv[i+2]);
			printf("set %s: %s -> %s\n", argv[i+1], s ? s : "(null)", argv[i+2]);
			i += 2;
		} else if (!strcmp(argv[i], "--del")) {
			if (i+1 >= argc)
				return 1;
			const char *s = trie_set(&trie, argv[i+1], NULL);
			printf("del %s: %s -> (null)\n", argv[i+1], s ? s : "(null)");
			i += 1;
		} else if (!strcmp(argv[i], "--dump")) {
			if (trie_findfirst(&f, &trie))
				do {
					printf("%s -> %s\n", f->key, (char *)f->n->value);
				} while(trie_findnext(&f));
		} else if (!strcmp(argv[i], "--new")) {
			trie_free(&trie);
		} else {
			fprintf(stderr, "unknown command: %s\n", argv[i]);
			return 1;
		}
	}
	trie_free(&trie);
}
