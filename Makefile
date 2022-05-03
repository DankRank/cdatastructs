CFLAGS=-g -Wall -Wextra
.PHONY: all clean
all: test_trie test_rbtree
clean:
	$(RM) test_trie test_rbtree
test_trie: test_trie.c trie.c trie.h
	$(CC) $(CFLAGS) $(LDFLAGS) test_trie.c trie.c $(LDLIBS) -o $@
test_rbtree: test_rbtree.c rbtree.c rbtree.h
	$(CC) $(CFLAGS) $(LDFLAGS) test_rbtree.c rbtree.c $(LDLIBS) -o $@
