#ifndef TRIE_H_
#define TRIE_H_
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
// all complexity is in terms of n = strlen(key) and s = size of the character set
struct trie_node {
	char key;
	unsigned char len;
	unsigned char cap;
	struct trie_node **data;
	void *value;
	struct trie_node *parent;
};
// get value from trie O(n log s)
void *trie_get(struct trie_node *n, const char *key);
// set value in trie O(ns)
void *trie_set(struct trie_node *n, const char *key, void *value);
// create a new trie
struct trie_node *trie_new(void);
// free a trie
void trie_free(struct trie_node *n);
struct trie_find {
	struct trie_node *n;
	size_t len;
	size_t cap;
	char key[1];
};
int trie_findfirst(struct trie_find **pf, struct trie_node *root);
int trie_findnext(struct trie_find **pf);
#ifdef __cplusplus
}
#endif
#endif
