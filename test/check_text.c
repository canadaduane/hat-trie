#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../src/text.h"

hattrie_t* trie;

void setup()
{
  trie = hattrie_create();
}

void teardown()
{
  hattrie_free(trie);
}

void test_text_clean()
{
  size_t len;

  fprintf(stderr, "cleaning text ... ");

  char text1[] = "  And be-\nhold Jarom\nsaid, \"Great!\n\" ";
  char text1_goal[] = "and behold jarom said great.";

  len = text_clean(text1);
  text1[len] = '\0';
  if (strcmp(text1, text1_goal) != 0) {
    fprintf(stderr, "[error] strings do not match: \"%s\" <=> \"%s\"\n", text1, text1_goal);
  }

  char text2[] = "A word. For you .  and you  ..";
  char text2_goal[] = "a word.for you.and you.";

  len = text_clean(text2);
  text2[len] = '\0';
  if (strcmp(text2, text2_goal) != 0) {
    fprintf(stderr, "[error] strings do not match: \"%s\" <=> \"%s\"\n", text2, text2_goal);
  }

  fprintf(stderr, "done\n");
}

void test_add_ngrams()
{
  value_t* value;

  fprintf(stderr, "adding ngrams ... ");


  char text0[] = "";

  add_ngrams(trie, 3, text0, strlen(text0), 0);

  if (hattrie_size(trie) > 0)
    fprintf(stderr, "[error] trie should be empty after adding nothing\n");


  char text1[] = "and what and that and what you say";

  add_ngrams(trie, 3, text1, strlen(text1), 0);

  value = hattrie_tryget(trie, "and", strlen("and"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'and' is NULL\n");
  else if (*value != 3)
    fprintf(stderr, "[error] count for 'and' is %u, expected 3\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "and what", strlen("and what"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'and what' is NULL\n");
  else if (*value != 2)
    fprintf(stderr, "[error] count for 'and what' is %u, expected 2\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "and that and", strlen("and that and"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'and that and' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'and that and' is %u, expected 1\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "what you say", strlen("what you say"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'what you say' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'what you say' is %u, expected 1\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "say", strlen("say"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'say' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'say' is %u, expected 1\n", (unsigned int)*value);


  char text2[] = "hickory dickory";

  add_ngrams(trie, 3, text2, strlen(text2), 0);

  value = hattrie_tryget(trie, "hickory", strlen("hickory"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'hickory' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'hickory' is %u, expected 1\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "dickory", strlen("dickory"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'dickory' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'dickory' is %u, expected 1\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "hickory dickory", strlen("hickory dickory"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'hickory dickory' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'hickory dickory' is %u, expected 1\n", (unsigned int)*value);


  char text3[] = "millenium falcon";

  add_ngrams(trie, 1, text3, strlen(text3), 0);

  value = hattrie_tryget(trie, "millenium", strlen("millenium"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'millenium' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'millenium' is %u, expected 1\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "falcon", strlen("falcon"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'falcon' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'falcon' is %u, expected 1\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "millenium falcon", strlen("millenium falcon"));
  if (value != NULL)
    fprintf(stderr, "[error] count for 'millenium' is not NULL but should be\n");

  fprintf(stderr, "done\n");
}


void test_add_ngrams_with_suffix()
{
  value_t* value;
  char suffix[] = "-suffix";

  fprintf(stderr, "adding ngrams with suffix ... ");


  char text1[] = "and what and that and what you say";

  add_ngrams_with_suffix(trie, 3, text1, strlen(text1), suffix, 0);


  value = hattrie_tryget(trie, "and", strlen("and"));
  if (value != NULL)
    fprintf(stderr, "[error] count for 'and' is not NULL but should be\n");

  value = hattrie_tryget(trie, "and-suffix", strlen("and-suffix"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'and-suffix' is NULL\n");
  else if (*value != 3)
    fprintf(stderr, "[error] count for 'and-suffix' is %u, expected 3\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "what you say-suffix", strlen("what you say-suffix"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'what you say-suffix' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'what you say-suffix' is %u, expected 1\n", (unsigned int)*value);

  fprintf(stderr, "done\n");
}

void test_add_ngrams_incr_existing_keys_only()
{
  value_t* value;

  fprintf(stderr, "adding ngrams but incrementing only existing keys ... ");

  char text1[] = "one two three four";

  add_ngrams(trie, 2, text1, strlen(text1), 0);

  value = hattrie_tryget(trie, "one two", strlen("one two"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'one two' is NULL\n");
  else if (*value != 1)
    fprintf(stderr, "[error] count for 'one two' is %u, expected 1\n", (unsigned int)*value);


  char text2[] = "one two millenium falcon";

  // this time, 'one' and 'two' and 'one two' should increment, while
  // 'two millenium', 'millenium falcon', and 'millenium' and 'falcon' are ignored
  add_ngrams(trie, 2, text2, strlen(text2), 1);

  value = hattrie_tryget(trie, "one", strlen("one"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'one' is NULL\n");
  else if (*value != 2)
    fprintf(stderr, "[error] count for 'one' is %u, expected 2\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "two", strlen("two"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'two' is NULL\n");
  else if (*value != 2)
    fprintf(stderr, "[error] count for 'two' is %u, expected 2\n", (unsigned int)*value);

  value = hattrie_tryget(trie, "one two", strlen("one two"));
  if (value == NULL)
    fprintf(stderr, "[error] count for 'one two' is NULL\n");
  else if (*value != 2)
    fprintf(stderr, "[error] count for 'one two' is %u, expected 2\n", (unsigned int)*value);

 value = hattrie_tryget(trie, "two millenium", strlen("two millenium"));
  if (value != NULL)
    fprintf(stderr, "[error] count for 'two millenium' is not NULL but should be\n");

 value = hattrie_tryget(trie, "millenium falcon", strlen("millenium falcon"));
  if (value != NULL)
    fprintf(stderr, "[error] count for 'millenium falcon' is not NULL but should be\n");

 value = hattrie_tryget(trie, "millenium", strlen("millenium"));
  if (value != NULL)
    fprintf(stderr, "[error] count for 'millenium' is not NULL but should be\n");

 value = hattrie_tryget(trie, "falcon", strlen("falcon"));
  if (value != NULL)
    fprintf(stderr, "[error] count for 'falcon' is not NULL but should be\n");

  fprintf(stderr, "done\n");
}

void test_add_ngrams_with_suffix_incr_existing_keys_only()
{
  value_t* value;

  fprintf(stderr, "adding ngrams with suffixes but incrementing only existing keys ... ");

  char text1[] = "one two three four";
  add_ngrams_with_suffix(trie, 2, text1, strlen(text1), "-book1", 0);

  char text2[] = "one two millenium falcon";
  add_ngrams_with_suffix(trie, 2, text2, strlen(text2), "-book2", 1);


 value = hattrie_tryget(trie, "falcon", strlen("falcon"));
  if (value != NULL)
    fprintf(stderr, "[error] count for 'falcon' is not NULL but should be\n");

  fprintf(stderr, "done\n");
}

int main()
{
  setup();
  test_text_clean();
  teardown();

  setup();
  test_add_ngrams();
  teardown();

  setup();
  test_add_ngrams_with_suffix();
  teardown();

  setup();
  test_add_ngrams_incr_existing_keys_only();
  teardown();

  setup();
  test_add_ngrams_with_suffix_incr_existing_keys_only();
  teardown();

  return 0;
}
