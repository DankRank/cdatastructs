CFLAGS=-g -Wall -Wextra
.PHONY: all clean
all: test_trie
clean:
	$(RM) test_trie
test_trie: test_trie.c trie.c trie.h
	$(CC) $(CFLAGS) $(LDFLAGS) test_trie.c trie.c $(LDLIB) -o $@
