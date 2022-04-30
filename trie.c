#include "trie.h"
#include <stdlib.h>
#include <string.h>
#define trie_is_end(e, n) ((e) >= (n)->data + (n)->len)
#define trie_is_found(e, n, k) (!trie_is_end((e), (n)) && (*(e))->key == (k))
// finds the entry for a child node O(log s)
// note that there are four cases to handle:
// - past the end pointer (key doesn't exist, larger than any existing key)
// - ent with different key (key doesn't exist, it should be inserted before this one)
// - ent with matching key (key exists)
static struct trie_node **trie_ent_find(struct trie_node *n, char key)
{
	int lo = 0;
	int hi = n->len;
	while (lo != hi) {
		int mid = (lo+hi)/2;
		if (n->data[mid]->key == key)
			return &n->data[mid];
		if (n->data[mid]->key > key)
			hi = mid;
		else
			lo = mid+1;
	}
	return &n->data[lo];
}
// adds a new entry O(s)
// precondition: !trie_found(ent, n, node->key)
static void trie_ent_add(struct trie_node **ent, struct trie_node *n, struct trie_node *node, struct trie_node **root)
{
	if (n->len == n->cap) {
		int idx = ent - n->data;
		n->cap = n->cap*2 + 1; // 0 1 3 7 15 31 63 127 255
		struct trie_node **pent = n->parent ? trie_ent_find(n->parent, n->key) : root;
		struct trie_node *nn = realloc(n, sizeof(struct trie_node)+sizeof(struct trie_node *)*(n->cap-1));
		if (!nn)
			abort();
		if (n != nn) {
			n = nn;
			*pent = n;
			for (int i = 0; i < n->len; i++)
				n->data[i]->parent = n;
			node->parent = n;
		}
		ent = n->data + idx;
	}
	memmove(ent+1, ent, sizeof(struct trie_node*)*(n->data + n->len++ - ent));
	*ent = node;
}
// removes an entry O(s)
void trie_ent_del(struct trie_node *n, char key)
{
	struct trie_node **ent = trie_ent_find(n, key);
	if (!trie_is_found(ent, n, key))
		return;
	memmove(ent, ent+1, sizeof(struct trie_node*)*(n->data + --n->len - ent));
}
// get value from trie O(n log s)
void *trie_get(struct trie_node **root, const char *key)
{
	struct trie_node *n = *root;
	while (*key) {
		struct trie_node **ent = trie_ent_find(n, *key);
		if (!trie_is_found(ent, n, *key))
			return NULL;
		n = *ent;
		key++;
	}
	return n->value;
}
// set value in trie O(ns)
void *trie_set(struct trie_node **root, const char *key, void *value)
{
	struct trie_node *n = *root;
	if (!n) {
		n = malloc(sizeof(struct trie_node));
		if (!n)
			abort();
		memset(n, 0, sizeof(struct trie_node)-sizeof(struct trie_node *));
		n->cap = 1;
		*root = n;
	}
	while (*key) {
		struct trie_node **ent = trie_ent_find(n, *key);
		if (!trie_is_found(ent, n, *key)) {
			if (!value)
				return NULL;
			int cap = key[1] ? 1 : 0;
			struct trie_node *node = malloc(sizeof(struct trie_node)+sizeof(struct trie_node *)*(cap-1));
			if (!node)
				abort();
			memset(node, 0, sizeof(struct trie_node)-sizeof(struct trie_node *));
			node->key = *key;
			node->parent = n;
			node->cap = cap;
			trie_ent_add(ent, n, node, root);
			n = node;
		} else {
			n = *ent;
		}
		key++;
	}
	void *oldvalue = n->value;
	n->value = value;
	if (!value) {
		while (!n->value && !n->len) {
			if (n->parent) {
				struct trie_node *p = n->parent;
				trie_ent_del(p, n->key);
				free(n);
				n = p;
			} else {
				free(n);
				*root = NULL;
				break;
			}
		}
	}
	return oldvalue;
}
// free a trie
void trie_free(struct trie_node **root) {
	struct trie_node *n = *root;
	while (n) {
		if (n->len) {
			n = n->data[--n->len];
		} else {
			struct trie_node *p = n->parent;
			free(n);
			n = p;
		}
	}
}
int trie_findnext(struct trie_find **pf) {
	struct trie_find *f = *pf;
	struct trie_node **ent = f->n->data;
	for (;;) {
		if (!trie_is_end(ent, f->n)) {
			if (f->len == f->cap) {
				f->cap *= 2;
				void *nf = realloc(f, sizeof(struct trie_find)+f->cap);
				if (!nf)
					abort();
				*pf = f = nf;
			}
			f->n = *ent;
			f->key[f->len++] = f->n->key;
			if (f->n->value) {
				f->key[f->len] = '\0';
				return 1;
			}
			ent = f->n->data;
		} else {
			f->n = f->n->parent;
			if (!f->n) {
				*pf = NULL;
				free(f);
				return 0;
			}
			f->len--;
			ent = f->key[f->len] != '\xff' ? trie_ent_find(f->n, f->key[f->len]+1) : NULL;
		}
	}
}
int trie_findfirst(struct trie_find **pf, struct trie_node **root) {
	if (!*root)
		return 0;
	struct trie_find *f = *pf = malloc(sizeof(struct trie_find)+16);
	if (!f)
		return 0;
	f->n = *root;
	f->len = 0;
	f->cap = 16;
	if (f->n->value) {
		f->key[f->len] = '\0';
		return 1;
	}
	return trie_findnext(pf);
}
