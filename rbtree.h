#ifndef RBTREE_H_
#define RBTREE_H_
struct rbtree *rb_new(int (*compar)(const void *, const void *), void (*free)(void *));
void rb_free(struct rbtree *t);
void *rb_search(struct rbtree *t, const void *value);
void rb_walk(struct rbtree *t, void (*cb)(void *, void *), void *ctx);
int rb_insert(struct rbtree *t, void *value);
#endif
