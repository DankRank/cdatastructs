#include "trie.h"
#include <stdlib.h>
#include <string.h>
#define trie_is_end(e, n) ((e) >= (n)->len)
#define trie_is_found(e, n, k) (!trie_is_end((e), (n)) && (n)->data[(e)]->key == (k))
#define trie_node_size(cap) (sizeof(struct trie_node)+sizeof(struct trie_node*)*((cap)-1))
#define trie_data_size(len) (sizeof(struct trie_node*)*(len))
// finds the entry for a child node O(log s)
// note that there are three cases to handle:
// - past the end pointer (key doesn't exist, larger than any existing key)
// - ent with different key (key doesn't exist, it should be inserted before this one)
// - ent with matching key (key exists)
static int ent_find(struct trie_node *n, int key)
{
	int lo = 0;
	int hi = n->len;
	while (lo != hi) {
		int mid = (lo+hi)/2;
		if (n->data[mid]->key == key)
			return mid;
		if (n->data[mid]->key > key)
			hi = mid;
		else
			lo = mid+1;
	}
	return lo;
}
// get value from trie O(n log s)
void *trie_get(struct trie_node **root, const char *key)
{
	struct trie_node *n = *root;
	while (*key) {
		int ent = ent_find(n, (unsigned char)*key);
		if (!trie_is_found(ent, n, *key))
			return NULL;
		n = n->data[ent];
		key++;
	}
	return n->value;
}
// set value in trie O(ns)
void *trie_set(struct trie_node **root, const char *key, void *value)
{
	struct trie_node *n = *root;
	if (!n) {
		n = malloc(trie_node_size(1));
		if (!n)
			abort();
		memset(n, 0, trie_node_size(0));
		n->cap = 1;
		*root = n;
	}
	while (*key) {
		int ent = ent_find(n, (unsigned char)*key);
		if (!trie_is_found(ent, n, *key)) {
			if (!value)
				return NULL;
			int cap = key[1] ? 1 : 0;
			struct trie_node *node = malloc(trie_node_size(cap));
			if (!node)
				abort();
			memset(node, 0, trie_node_size(0));
			node->key = *key;
			node->cap = cap;
			if (n->len == n->cap) {
				n->cap = n->cap*2 + 1; // 0 1 3 7 15 31 63 127 255
				struct trie_node **pent = n->parent ? &n->parent->data[ent_find(n->parent, n->key)] : root;
				struct trie_node *nn = realloc(n, trie_node_size(n->cap));
				if (!nn)
					abort();
				if (n != nn) {
					n = nn;
					*pent = n;
					for (int i = 0; i < n->len; i++)
						n->data[i]->parent = n;
				}
			}
			node->parent = n;
			memmove(&n->data[ent+1], &n->data[ent], trie_data_size(n->len++ - ent));
			n->data[ent] = node;
			n = node;
		} else {
			n = n->data[ent];
		}
		key++;
	}
	void *oldvalue = n->value;
	n->value = value;
	if (!value) {
		while (!n->value && !n->len) {
			if (n->parent) {
				struct trie_node *p = n->parent;
				int ent = ent_find(p, n->key);
				memmove(&p->data[ent], &p->data[ent+1], trie_data_size(--p->len - ent));
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
	int ent = 0;
	for (;;) {
		if (!trie_is_end(ent, f->n)) {
			if (f->len == f->cap) {
				f->cap *= 2;
				void *nf = realloc(f, sizeof(struct trie_find)+f->cap);
				if (!nf)
					abort();
				*pf = f = nf;
			}
			f->n = f->n->data[ent];
			f->key[f->len++] = f->n->key;
			if (f->n->value) {
				f->key[f->len] = '\0';
				return 1;
			}
			ent = 0;
		} else {
			f->n = f->n->parent;
			if (!f->n) {
				*pf = NULL;
				free(f);
				return 0;
			}
			f->len--;
			ent = ent_find(f->n, (unsigned char)f->key[f->len] + 1);
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
