#include "../utaust.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int main() {
  utaust_ust* score = utaust_create_ust();
  score -> header.tempo = 120;
  score -> header.project_name = malloc(100);
  strcpy(score -> header.project_name, "Test Project");
  for(int i = 0; i < 100; i ++) {
    utaust_note* note = utaust_create_note();
    note -> length = 480 * ((i % 2) + 1) + 0.5;
    note -> lyric = calloc(5, 1);
    strcpy(note -> lyric, "abcd");
    note -> note_num = 60 + (i % 5);
    if(i % 5 == 0)
      note -> tempo = 120 + i;
    utaust_ust_append(score, note);
  }
  
  char* str = utaust_print(score);
  utaust_ust* score2 = utaust_parse(str);
  assert(score2 -> header.tempo == score -> header.tempo);
  assert(! strcmp(score2 -> header.project_name,
    score -> header.project_name));
  
  utaust_iterator* cursor = utaust_create_iterator(score2);
  do {
    assert(60 + (cursor -> curr_note -> id % 5) ==
      cursor -> curr_note -> note_num);
  } while(utaust_iterator_next(cursor) != 0);
  assert(fabs(cursor -> curr_time - 54.835046) < 1e-5);

  while(utaust_iterator_prev(cursor) != 0);
  printf("Time error: %e sec\n", cursor -> curr_time);
  printf("Ticks error: %e\n", cursor -> curr_ticks);
  assert(fabs(cursor -> curr_time) < 1e-5);
  utaust_delete_iterator(cursor);

  char* str2 = utaust_print(score2);
  assert(! strcmp(str, str2));
  free(str); free(str2);
  utaust_delete_ust(score);
  utaust_delete_ust(score2);
  return 0;
}
