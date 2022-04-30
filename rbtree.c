#include <stdlib.h>
struct rbnode {
	struct rbnode *child[2], *parent;
	void *value;
	int color;
};
struct rbtree {
	int (*compar)(const void *, const void *);
	void (*free)(void *);
	struct rbnode *root;
};
struct rbtree *rb_new(int (*compar)(const void *, const void *), void (*free)(void *))
{
	struct rbtree *t = malloc(sizeof(struct rbtree));
	if (!t)
		return NULL;
	t->compar = compar;
	t->free = free;
	t->root = NULL;
	return t;
}
static void rb_free_node(struct rbtree *t, struct rbnode *n)
{
	if (n->child[0])
		rb_free_node(t, n->child[0]);
	struct rbnode *rchild = n->child[1];
	t->free(n->value);
	free(n);
	if (rchild)
		rb_free_node(t, rchild);
}
void rb_free(struct rbtree *t)
{
	if (t) {
		if (t->root)
			rb_free_node(t, t->root);
		free(t);
	}
}
void *rb_search(struct rbtree *t, const void *value)
{
	struct rbnode *n = t->root;
	while (n) {
		int c = t->compar(n->value, value);
		if (!c)
			return n->value;
		n = n->child[c<0];
	}
	return NULL;
}
static void rb_walk_node(struct rbnode *n, void (*cb)(void *, void *), void *ctx)
{
	if (n->child[0])
		rb_walk_node(n->child[0], cb, ctx);
	cb(n->value, ctx);
	if (n->child[1])
		rb_walk_node(n->child[1], cb, ctx);
}
void rb_walk(struct rbtree *t, void (*cb)(void *, void *), void *ctx)
{
	if (t->root)
		rb_walk_node(t->root, cb, ctx);
}
static void rb_rotate(struct rbtree *t, struct rbnode *p, int dir)
{
	struct rbnode *c = p->child[1-dir];
	if (p->parent)
		p->parent->child[p == p->parent->child[1]] = c;
	else
		t->root = c;
	if (c->child[dir])
		c->child[dir]->parent = p;
	c->parent = p->parent;
	p->parent = c;
	p->child[1-dir] = c->child[dir];
	c->child[dir] = p;
}
int rb_insert(struct rbtree *t, void *value)
{
	struct rbnode *N;
	{
		struct rbnode *P = t->root;
		int c;
		while (P) {
			c = t->compar(P->value, value);
			if (!c) {
				if (t->free)
					t->free(P->value);
				P->value = value;
				return 0;
			}
			struct rbnode *child = P->child[c<0];
			if (!child)
				break;
			P = child;
		}
		N = malloc(sizeof(struct rbnode));
		if (!N)
			return -1;
		N->child[0] = N->child[1] = 0;
		N->parent = P;
		if (P)
			P->child[c<0] = N;
		else
			t->root = N;
		N->value = value;
		N->color = 1;
	}

	for (;;) {
		struct rbnode *P = N->parent;
		if (!P || !P->color)
			return 0;

		struct rbnode *G = P->parent;
		if (!G) {
			P->color = 0;
			return 0;
		}

		int Pdir = P == G->child[1];
		struct rbnode *U = G->child[1-Pdir];
		if (!U || !U->color) {
			int Ndir = N == P->child[1];
			if (Pdir != Ndir) {
				rb_rotate(t, P, Pdir);
				N = P;
				P = N->parent;
			}
			rb_rotate(t, G, 1-Pdir);
			P->color = 0;
			G->color = 1;
			return 0;
		}

		P->color = 0;
		U->color = 0;
		G->color = 1;
		N = G;
	}
}
