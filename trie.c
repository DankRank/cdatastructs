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
	if (!n)
		return NULL;
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
int trie_set(struct trie_node **root, const char *key, const void *value, void **oldvalue)
{
	int rv = 0;
	if (oldvalue)
		*oldvalue = NULL;
	struct trie_node *n = *root;
	if (!n) { // create root node
		n = malloc(trie_node_size(1));
		if (!n)
			return -1;
		memset(n, 0, trie_node_size(0));
		n->cap = 1;
		*root = n;
	}
	while (*key) { // walk the trie to find the leaf
		int ent = ent_find(n, (unsigned char)*key);
		if (!trie_is_found(ent, n, *key)) { // create missing node
			if (!value) // ...unless we're deleting
				return 0;
			int cap = key[1] ? 1 : 0; // if it's a leaf, use capacity of 0
			struct trie_node *node = malloc(trie_node_size(cap));
			if (!node) {
				rv = -1;
				goto cleanup;
			}
			memset(node, 0, trie_node_size(0));
			node->key = *key;
			node->cap = cap;
			if (n->len == n->cap) { // reallocate the node
				n->cap = n->cap*2 + 1; // 0 1 3 7 15 31 63 127 255
				// we have to update parent's pointer, but ent_find needs that pointer to be valid.
				// so we have to locate it *before* reallocing.
				struct trie_node **pent = n->parent ? &n->parent->data[ent_find(n->parent, n->key)] : root;
				struct trie_node *nn = realloc(n, trie_node_size(n->cap));
				if (!nn) {
					free(node);
					rv = -1;
					goto cleanup;
				}
				if (n != nn) { // update pointers
					n = nn;
					*pent = n;
					for (int i = 0; i < n->len; i++)
						n->data[i]->parent = n;
				}
			}
			// insert the new node
			node->parent = n;
			memmove(&n->data[ent+1], &n->data[ent], trie_data_size(n->len++ - ent));
			n->data[ent] = node;
			n = node;
		} else {
			n = n->data[ent];
		}
		key++;
	}
	if (oldvalue)
		*oldvalue = n->value;
	n->value = (void *)value; // discard const
	if (!value) {
		while (!n->value && !n->len) { // free no longer used nodes
cleanup:
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
	return rv;
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
	int ent = 0; // go to the first child node
	for (;;) {
		if (!trie_is_end(ent, f->n)) {
			if (f->len == f->cap) {
				f->cap *= 2;
				void *nf = realloc(f, sizeof(struct trie_find)+f->cap);
				if (!nf) {
					*pf = NULL;
					free(f);
					return -1;
				}
				*pf = f = nf;
			}
			f->n = f->n->data[ent];
			f->key[f->len++] = f->n->key;
			if (f->n->value) { // found a value
				f->key[f->len] = '\0';
				return 1;
			}
			ent = 0; // no value, go to the first child
		} else { // no more children, go to parent's next child
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
	struct trie_find *f;
	if (*pf) {
		f = *pf;
	} else {
		f = *pf = malloc(sizeof(struct trie_find)+16);
		if (!f)
			return -1;
		f->cap = 16;
	}
	f->len = 0;
	f->n = *root;
	if (f->n->value) {
		f->key[f->len] = '\0';
		return 1;
	}
	return trie_findnext(pf);
}
