#ifndef TRIE_H_
#define TRIE_H_
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
// all complexity is in terms of n = strlen(key) and s = size of the character set
struct trie_node {
	unsigned char key;
	unsigned char len;
	unsigned char cap;
	void *value;
	struct trie_node *parent;
	struct trie_node *data[1];
};
// get value from trie O(n log s)
void *trie_get(struct trie_node **root, const char *key);
// set value in trie O(ns)
void *trie_set(struct trie_node **root, const char *key, void *value);
// free a trie
void trie_free(struct trie_node **root);
struct trie_find {
	struct trie_node *n;
	size_t len;
	size_t cap;
	char key[1];
};
int trie_findfirst(struct trie_find **pf, struct trie_node **root);
int trie_findnext(struct trie_find **pf);
#ifdef __cplusplus
}
#endif
#endif
