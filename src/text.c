#include "text.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/** Transforms text such as the following:
 *
 *   And behold, I said, "This is no good!"
 *   What shall ye say unto these people, there-
 *   fore?
 *
 * Into a cleaned up single line of text, like the following:
 *
 *   and behold i said this is no good.what shall ye say unto these people therefore.
 *
 * Spaces indicate word boundaries, while periods indicate sentence boundaries.
 */
size_t text_clean(char* text, size_t len)
{
  if (len == 0) return 0;

  char* read;
  char* write = text;
  uint8_t join_lines = false,
          just_added_space = true,   // prevent prefix spaces
          just_added_period = false;
  for (read=text; read<text+len; read++) {
    char c = *read;
    if (c >= 'A' && c <= 'Z') {
      // Change upper case to lowercase
      c += 32;
    } else if (c == '\n') {
      // Change newlines to spaces (i.e. both count as whitespace)
      c = ' ';
    } else if (c == '?' || c == '!') {
      // Change exclamation, question marks to periods (i.e. sentence boundaries)
      c = '.';
    }

    if (c == '-') {
      join_lines = true;
    } else if (join_lines && c == ' ') {
      // ignore whitespace after a dash (i.e. including newlines, which is the
      // most common case because words that are broken by syllables are dashed)
    } else if (c == '.' && !just_added_period) {
      // erase space before period
      if (just_added_space) write--;
      *write++ = '.';
      just_added_period = true;
      just_added_space = false;
      join_lines = false;
    } else if (c == ' ' && !just_added_space && !just_added_period) {
      *write++ = ' ';
      just_added_space = true;
      just_added_period = false;
    } else if (c >= 'a' && c <= 'z') {
      *write++ = c;
      just_added_space = false;
      just_added_period = false;
      join_lines = false;
    }
  }
  // erase space at end of text
  if (just_added_space) write--;

  // Return the new length of the string
  return (size_t)(write - text);
}

void add_ngrams(hattrie_t* trie, int upto_n, char* text, size_t text_len, uint8_t incr_existing_keys_only)
{
  char blank_suffix[] = "\0";
  add_ngrams_with_suffix(trie, upto_n, text, text_len, blank_suffix, 0, incr_existing_keys_only);
}

static inline void incr_value(
  hattrie_t* trie,
  char* buffer,
  char* buffer_pre,
  char* key,
  size_t len,
  size_t suffix_len,
  uint8_t incr_existing_keys_only)
{
  value_t* value = NULL;

  assert(buffer_pre - len >= buffer);
  
  if (incr_existing_keys_only) {
    value = hattrie_tryget(trie, key, len);
    if (value) {
      memcpy(buffer_pre - len, key, len);
      value = hattrie_get(trie, buffer_pre - len, len + suffix_len);
      (*value)++;
    }
  } else {
    memcpy(buffer_pre - len, key, len);
    value = hattrie_get(trie, buffer_pre - len, len + suffix_len);
    (*value)++;
  }

}

void add_ngrams_with_suffix(hattrie_t* trie, int upto_n, char* text, size_t text_len, char* suffix, size_t suffix_len, uint8_t incr_existing_keys_only)
{
  char* head = text;
  char* tail = text;
  char* next_head = text;
  char* next_tail = text;
  int word_count = 0;

  if (text_len == 0) return;

  char buffer[NGRAM_BUFFER_SIZE];
  size_t buffer_offset = NGRAM_BUFFER_SIZE - suffix_len - 1;
  char* buffer_pre = buffer + buffer_offset;
  memcpy(buffer_pre, suffix, suffix_len);

  do {
    if (*tail == ' ' || *tail == '.' || tail >= head+text_len) {
      word_count++;
      if (word_count == 1 || upto_n == 1) {
        next_head = next_tail = tail + 1;
      } else if (word_count == 2) {
        next_tail = tail;
      }
      if (word_count <= upto_n) {
        incr_value(trie, buffer, buffer_pre,
          head, tail - head, suffix_len,
          incr_existing_keys_only);
      }
      if (word_count == upto_n) {
        head = next_head;
        tail = next_tail;
        word_count = 0;
      } else {
        tail++;
      }
    } else {
      tail++;
    }
  } while(*tail);

  // add the last ngram of size upto_n
  incr_value(trie, buffer, buffer_pre, 
    head, tail - head, suffix_len,
    incr_existing_keys_only);

  // add the 1..(upto_n-1) sized ngrams at the tail
  if (upto_n > 1) {
    while(head < text+text_len) {
      if(*head == ' ' || *head == '.') {
        incr_value(trie, buffer, buffer_pre,
          head + 1, tail - head - 1, suffix_len,
          incr_existing_keys_only);
      }
      head++;
    }
  }
}
