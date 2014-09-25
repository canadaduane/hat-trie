#ifndef TEXT_H
#define TEXT_H

#include <stdbool.h>
#include "pstdint.h"
#include "hat-trie.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NGRAM_BUFFER_SIZE 4096

size_t text_clean(char* text);
void add_ngrams(hattrie_t* trie, int upto_n, char* text, size_t text_len, uint8_t incr_existing_keys_only);
void add_ngrams_with_suffix(hattrie_t* trie, int upto_n, char* text, size_t text_len, char* suffix, uint8_t incr_existing_keys_only);

#ifdef __cplusplus
}
#endif

#endif
