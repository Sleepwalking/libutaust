/*
libutaust
===

Copyright (c) 2018, Kanru Hua
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "utaust.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NOT_A_NOTENUM (-128)

static char* utaust_strdup(const char* src) {
  char* ret = malloc(strlen(src) + 1);
  strcpy(ret, src);
  return ret;
}

typedef struct str_node {
  struct str_node* next;
  struct str_node* prev;
  char* str;
} str_node;

typedef struct {
  int size;
  str_node* first;
  str_node* last;
} str_list;

static str_node* create_str_node(const char* src) {
  str_node* ret = malloc(sizeof(str_node));
  ret -> str = utaust_strdup(src);
  ret -> next = NULL;
  ret -> prev = NULL;
  return ret;
}

static void delete_str_node(str_node* dst) {
  if(dst == NULL) return;
  free(dst -> str);
  free(dst);
}

static str_node* copy_str_node(str_node* src) {
  return create_str_node(src -> str);
}

static str_list* create_str_list() {
  str_list* ret = malloc(sizeof(str_list));
  ret -> size = 0;
  ret -> first = NULL;
  ret -> last = NULL;
  return ret;
}

static void str_list_remove_last(str_list* dst) {
  if(dst -> last != NULL) {
    dst -> size --;
    str_node* prev = dst -> last -> prev;
    delete_str_node(dst -> last);
    dst -> last = prev;
    if(prev != NULL)
      prev -> next = NULL;
    else
      dst -> first = NULL;
  }
}

static void str_list_append(str_list* dst, str_node* src) {
  if(dst -> last != NULL)
    dst -> last -> next = src;
  else
    dst -> first = src;
  src -> prev = dst -> last;
  dst -> last = src;
  dst -> size ++;
}

static void str_list_appendstr(str_list* dst, char* src) {
  str_list_append(dst, create_str_node(src));
}

static str_list* str_list_from_lines(const char* src) {
  int totalsize = strlen(src);
  char* copy = utaust_strdup(src);
  str_list* ret = create_str_list();
  int line_begin = 0;
  for(int i = 0; i < totalsize; i ++) {
    if(copy[i] == '\r') {
      copy[i] = 0;
    } else
    if(copy[i] == '\n') {
      copy[i] = 0;
      str_list_appendstr(ret, copy + line_begin);
      line_begin = i + 1;
    }
  }
  if(line_begin != totalsize)
    str_list_appendstr(ret, copy + line_begin);
  free(copy);
  return ret;
}

static void delete_str_list(str_list* dst) {
  if(dst == NULL) return;
  while(dst -> last != NULL)
    str_list_remove_last(dst);
  free(dst);
}

static void str_list_join(str_list* dst, str_list* src) {
  str_node* curr = src -> first;
  while(curr != NULL) {
    str_list_append(dst, copy_str_node(curr));
    curr = curr -> next;
  }
}

static char* str_list_concatenate(str_list* src, const char* delim) {
  int len = 0;
  int ndelim = strlen(delim);
  str_node* curr = src -> first;
  while(curr != NULL) {
    len += strlen(curr -> str);
    curr = curr -> next;
    if(curr != NULL) len += ndelim;
  }
  char* ret = malloc(len + 1);
  len = 0;
  curr = src -> first;
  while(curr != NULL) {
    strcpy(ret + len, curr -> str);
    len += strlen(curr -> str);
    curr = curr -> next;
    if(curr != NULL) {
      strcpy(ret + len, delim);
      len += ndelim;
    }
  }
  return ret;
}

utaust_note* utaust_create_note() {
  utaust_note* ret = malloc(sizeof(utaust_note));
  ret -> next = NULL;
  ret -> prev = NULL;
  ret -> id = 0;
  ret -> length = 0;
  ret -> lyric = NULL;
  ret -> note_num = NOT_A_NOTENUM;
  ret -> pre_utt = 0;
  ret -> intensity = 100;
  ret -> modulation = 0;
  ret -> flags = NULL;
  ret -> pb_type = 0;
  ret -> pb_start = 0;
  ret -> pitchbend = NULL;
  ret -> pbs = NULL;
  ret -> pbw = NULL;
  ret -> pby = NULL;
  ret -> vbr = NULL;
  ret -> envelope = NULL;
  ret -> tempo = 0;
  return ret;
}

void utaust_delete_note(utaust_note* dst) {
  if(dst == NULL) return;
  free(dst -> lyric);
  free(dst -> flags);
  free(dst -> pitchbend);
  free(dst -> pbs);
  free(dst -> pbw);
  free(dst -> pby);
  free(dst -> vbr);
  free(dst -> envelope);
  free(dst);
}

utaust_ust* utaust_create_ust() {
  utaust_ust* ret = malloc(sizeof(utaust_ust));
  const char* version_str = "UST Version1.2";
  ret -> size = 0;
  ret -> header.version = utaust_strdup(version_str);
  ret -> header.tempo = 0;
  ret -> header.project_name = NULL;
  ret -> header.voice_dir = NULL;
  ret -> header.out_file = NULL;
  ret -> header.cache_dir = NULL;
  ret -> header.tool1 = NULL;
  ret -> header.tool2 = NULL;
  ret -> header.mode2 = 0;
  ret -> first = NULL;
  ret -> last = NULL;
  return ret;
}

static void utaust_ust_remove_last(utaust_ust* dst) {
  if(dst -> last != NULL) {
    dst -> size --;
    utaust_note* prev = dst -> last -> prev;
    utaust_delete_note(dst -> last);
    dst -> last = prev;
    if(prev != NULL)
      prev -> next = NULL;
    else
      dst -> first = NULL;
  }
}

void utaust_ust_append(utaust_ust* dst, utaust_note* src) {
  if(dst -> last != NULL)
    dst -> last -> next = src;
  else
    dst -> first = src;
  src -> prev = dst -> last;
  src -> id = dst -> size;
  dst -> last = src;
  dst -> size ++;
}

void utaust_delete_ust(utaust_ust* dst) {
  if(dst == NULL) return;
  free(dst -> header.version);
  free(dst -> header.project_name);
  free(dst -> header.voice_dir);
  free(dst -> header.out_file);
  free(dst -> header.cache_dir);
  free(dst -> header.tool1);
  free(dst -> header.tool2);
  while(dst -> last != NULL)
    utaust_ust_remove_last(dst);
  free(dst);
}

static str_list* utaust_note_print(utaust_note* src) {
  str_list* ret = create_str_list();
  char buffer[256];
  char* longbuffer = NULL;

# define append_longbuffer(attr, label) \
  longbuffer = malloc(strlen(src -> attr) + 20); \
  sprintf(longbuffer, label "=%s", src -> attr); \
  str_list_appendstr(ret, longbuffer); \
  free(longbuffer)

  sprintf(buffer, "[#%04d]", src -> id);
  str_list_appendstr(ret, buffer);

  if(src -> tempo != 0) {
    sprintf(buffer, "Tempo=%f", src -> tempo);
    str_list_appendstr(ret, buffer);
  }

  sprintf(buffer, "Length=%f", src -> length);
  str_list_appendstr(ret, buffer);

  if(src -> lyric != NULL) {
    append_longbuffer(lyric, "Lyric");
  }

  if(src -> note_num != NOT_A_NOTENUM) {
    sprintf(buffer, "NoteNum=%d", src -> note_num);
    str_list_appendstr(ret, buffer);
  }

  if(src -> pre_utt != 0) {
    sprintf(buffer, "PreUtterance=%f", src -> pre_utt);
    str_list_appendstr(ret, buffer);
  }

  if(src -> intensity != 100) {
    sprintf(buffer, "Intensity=%f", src -> intensity);
    str_list_appendstr(ret, buffer);
  }

  if(src -> modulation != 0) {
    sprintf(buffer, "Modulation=%f", src -> modulation);
    str_list_appendstr(ret, buffer);
  }

  if(src -> flags != NULL) {
    append_longbuffer(flags, "Flags");
  }

  if(src -> pb_type != 0) {
    sprintf(buffer, "PBType=%d", src -> pb_type);
    str_list_appendstr(ret, buffer);
  }

  if(src -> pb_start != 0) {
    sprintf(buffer, "PBStart=%f", src -> pb_start);
    str_list_appendstr(ret, buffer);
  }

  if(src -> pitchbend != NULL) {
    append_longbuffer(pitchbend, "PitchBend");
  }

  if(src -> pbs != NULL) {
    append_longbuffer(pbs, "PBS");
  }

  if(src -> pbw != NULL) {
    append_longbuffer(pbw, "PBW");
  }

  if(src -> pby != NULL) {
    append_longbuffer(pby, "PBY");
  }

  if(src -> vbr != NULL) {
    append_longbuffer(vbr, "VBR");
  }

  if(src -> envelope != NULL) {
    append_longbuffer(envelope, "Envelope");
  }
  return ret;
}

static str_list* utaust_header_print(utaust_header* src) {
  str_list* ret = create_str_list();
  char buffer[256];
  char* longbuffer = NULL;

  if(src -> version != NULL) {
    str_list_appendstr(ret, "[#VERSION]");
    str_list_appendstr(ret, src -> version);
  }

  str_list_appendstr(ret, "[#SETTING]");
  sprintf(buffer, "Tempo=%f", src -> tempo);
  str_list_appendstr(ret, buffer);
  
  if(src -> project_name != NULL) {
    append_longbuffer(project_name, "ProjectName");
  }

  if(src -> voice_dir != NULL) {
    append_longbuffer(voice_dir, "VoiceDir");
  }

  if(src -> out_file != NULL) {
    append_longbuffer(out_file, "OutFile");
  }

  if(src -> cache_dir != NULL) {
    append_longbuffer(cache_dir, "CacheDir");
  }

  if(src -> tool1 != NULL) {
    append_longbuffer(tool1, "Tool1");
  }

  if(src -> tool2 != NULL) {
    append_longbuffer(tool2, "Tool2");
  }

  sprintf(buffer, "Mode2=%s", src -> mode2 ? "True" : "False");
  str_list_appendstr(ret, buffer);

  return ret;
}

char* utaust_print(utaust_ust* src) {
  str_list* lines = create_str_list();
  str_list* part = utaust_header_print(& src -> header);
  str_list_join(lines, part);
  delete_str_list(part);
  utaust_note* curr = src -> first;
  while(curr != NULL) {
    part = utaust_note_print(curr);
    str_list_join(lines, part);
    delete_str_list(part);
    curr = curr -> next;
  }
  str_list_appendstr(lines, "[#TRACKEND]");
  char* ret = str_list_concatenate(lines, "\r\n");
  delete_str_list(lines);
  return ret;
}

#define PARSER_STATE_DEFAULT    0
#define PARSER_STATE_VERSION    1
#define PARSER_STATE_SETTING    2
#define PARSER_STATE_NOTE       3

static int check_id(const char* line) {
  int n = strlen(line);
  if(n < 3) return 0;
  if(line[0] != '[' || line[1] != '#' || line[n - 1] != ']')
    return 0;
  for(int i = 2; i < n - 1; i ++)
    if(line[i] < '0' || line[i] > '9') return 0;
  return 1;
}

static int get_id(const char* line) {
  char* copy = utaust_strdup(line);
  int n = strlen(line);
  copy[n - 1] = 0;
  int id = atoi(copy + 2);
  free(copy);
  return id;
}

static int check_attr(const char* line, const char* expected) {
  int n1 = strlen(line);
  int n2 = strlen(expected);
  if(! strncmp(line, expected, n2)) {
    if(n2 + 1 <= n1)
      return n2 + 1;
  }
  return 0;
}

static void utaust_parse_version(utaust_ust* dst, const char* str) {
  free(dst -> header.version);
  dst -> header.version = utaust_strdup(str);
}

static void utaust_parse_setting(utaust_ust* dst, const char* str) {
  int idx = 0;
  if((idx = check_attr(str, "Tempo")) != 0)
    dst -> header.tempo = atof(str + idx);
  else
  if((idx = check_attr(str, "ProjectName")) != 0) {
    free(dst -> header.project_name);
    dst -> header.project_name = strdup(str + idx);
  } else
  if((idx = check_attr(str, "VoiceDir")) != 0) {
    free(dst -> header.voice_dir);
    dst -> header.voice_dir = strdup(str + idx);
  } else
  if((idx = check_attr(str, "OutFile")) != 0) {
    free(dst -> header.out_file);
    dst -> header.out_file = strdup(str + idx);
  } else
  if((idx = check_attr(str, "CacheDir")) != 0) {
    free(dst -> header.cache_dir);
    dst -> header.cache_dir = strdup(str + idx);
  } else
  if((idx = check_attr(str, "Tool1")) != 0) {
    free(dst -> header.tool1);
    dst -> header.tool1 = strdup(str + idx);
  } else
  if((idx = check_attr(str, "Tool2")) != 0) {
    free(dst -> header.tool2);
    dst -> header.tool2 = strdup(str + idx);
  } else
  if((idx = check_attr(str, "Mode2")) != 0)
    dst -> header.mode2 = ! strcmp(str + idx, "True");
  else {
    fprintf(stderr, "Warning: unidentified attribute '%s'\n", str);
  }
}

static void utaust_parse_note(utaust_note* dst, const char* str) {
  int idx = 0;
  if((idx = check_attr(str, "Length")) != 0)
    dst -> length = atof(str + idx);
  else
  if((idx = check_attr(str, "Lyric")) != 0) {
    free(dst -> lyric);
    dst -> lyric = strdup(str + idx);
  } else
  if((idx = check_attr(str, "NoteNum")) != 0)
    dst -> note_num = atoi(str + idx);
  else
  if((idx = check_attr(str, "PreUtterance")) != 0)
    dst -> pre_utt = atof(str + idx);
  else
  if((idx = check_attr(str, "Intensity")) != 0)
    dst -> intensity = atof(str + idx);
  else
  if((idx = check_attr(str, "Modulation")) != 0)
    dst -> modulation = atof(str + idx);
  else
  if((idx = check_attr(str, "Flags")) != 0) {
    free(dst -> flags);
    dst -> flags = strdup(str + idx);
  } else
  if((idx = check_attr(str, "PBType")) != 0)
    dst -> pb_type = atoi(str + idx);
  else
  if((idx = check_attr(str, "PBStart")) != 0)
    dst -> pb_start = atof(str + idx);
  else
  if((idx = check_attr(str, "PitchBend")) != 0) {
    free(dst -> pitchbend);
    dst -> pitchbend = strdup(str + idx);
  } else
  if((idx = check_attr(str, "PBS")) != 0) {
    free(dst -> pbs);
    dst -> pbs = strdup(str + idx);
  } else
  if((idx = check_attr(str, "PBW")) != 0) {
    free(dst -> pbw);
    dst -> pbw = strdup(str + idx);
  } else
  if((idx = check_attr(str, "PBY")) != 0) {
    free(dst -> pby);
    dst -> pby = strdup(str + idx);
  } else
  if((idx = check_attr(str, "VBR")) != 0) {
    free(dst -> vbr);
    dst -> vbr = strdup(str + idx);
  } else
  if((idx = check_attr(str, "Envelope")) != 0) {
    free(dst -> envelope);
    dst -> envelope = strdup(str + idx);
  } else
  if((idx = check_attr(str, "Tempo")) != 0)
    dst -> tempo = atof(str + idx);
  else {
    fprintf(stderr, "Warning: unidentified attribute '%s'\n", str);
  }
}

utaust_ust* utaust_parse(const char* src) {
  str_list* lines = str_list_from_lines(src);
  utaust_ust* ret = utaust_create_ust();
  str_node* curr = lines -> first;
  utaust_note* top = NULL;
  int state = PARSER_STATE_DEFAULT;
  while(curr != NULL) {
    if(! strcmp(curr -> str, "[#VERSION]")) {
      state = PARSER_STATE_VERSION;
    } else
    if(! strcmp(curr -> str, "[#SETTING]")) {
      state = PARSER_STATE_SETTING;
    } else
    if(! strcmp(curr -> str, "[#TRACKEND]")) {
      break;
    } else
    if(check_id(curr -> str)) {
      state = PARSER_STATE_NOTE;
      top = utaust_create_note();
      utaust_ust_append(ret, top);
      top -> id = get_id(curr -> str);
    } else
    if(state == PARSER_STATE_VERSION) {
      utaust_parse_version(ret, curr -> str);
    } else
    if(state == PARSER_STATE_SETTING) {
      utaust_parse_setting(ret, curr -> str);
    } else
    if(state == PARSER_STATE_NOTE) {
      utaust_parse_note(top, curr -> str);
    } else {
      fprintf(stderr, "Warning: unidentified line '%s'\n", curr -> str);
    }
    curr = curr -> next;
  }
  delete_str_list(lines);
  return ret;
}

utaust_iterator* utaust_create_iterator(utaust_ust* src) {
  utaust_iterator* ret = malloc(sizeof(utaust_iterator));
  ret -> curr_note = src -> first;
  ret -> curr_tempo = src -> header.tempo;
  if(ret -> curr_note != NULL && ret -> curr_note -> tempo != 0)
    ret -> curr_tempo = ret -> curr_note -> tempo;
  ret -> curr_ticks = 0;
  ret -> curr_time = 0;
  ret -> curr_duration = 60.0 / 480.0 / ret -> curr_tempo *
    ret -> curr_note -> length;
  ret -> correction = 0;
  ret -> iter_direction = 0;
  ret -> default_tempo = src -> header.tempo;
  return ret;
}

int utaust_iterator_next(utaust_iterator* dst) {
  if(dst -> curr_note == NULL || dst -> curr_note -> next == NULL)
    return 0;
  if(dst -> iter_direction == -1) dst -> correction *= -1;
  dst -> iter_direction = 1;
  volatile double duration = 60.0 / 480.0 / dst -> curr_tempo *
    dst -> curr_note -> length;
  volatile double diff = duration - dst -> correction;
  volatile double sum = dst -> curr_time + diff;
  dst -> correction = (sum - dst -> curr_time) - diff;
  dst -> curr_ticks += dst -> curr_note -> length;
  dst -> curr_time = sum;
  dst -> curr_note = dst -> curr_note -> next;
  if(dst -> curr_note != NULL && dst -> curr_note -> tempo != 0)
    dst -> curr_tempo = dst -> curr_note -> tempo;
  dst -> curr_duration = 60.0 / 480.0 / dst -> curr_tempo *
    dst -> curr_note -> length;
  return 1;
}

int utaust_iterator_prev(utaust_iterator* dst) {
  if(dst -> curr_note == NULL || dst -> curr_note -> prev == NULL)
    return 0;
  if(dst -> curr_note != NULL && dst -> curr_note -> tempo != 0) {
    double prev_tempo = dst -> default_tempo;
    utaust_note* curr = dst -> curr_note -> prev;
    while(curr != NULL) {
      if(curr -> tempo != 0) {
        prev_tempo = curr -> tempo;
        break;
      }
      curr = curr -> prev;
    }
    dst -> curr_tempo = prev_tempo;
  }
  dst -> curr_note = dst -> curr_note -> prev;
  dst -> curr_ticks -= dst -> curr_note -> length;

  if(dst -> iter_direction == 1) dst -> correction *= -1;
  dst -> iter_direction = -1;
  volatile double duration = 60.0 / 480.0 / dst -> curr_tempo *
    dst -> curr_note -> length;
  volatile double diff = duration - dst -> correction;
  volatile double sub = dst -> curr_time - diff;
  dst -> correction = (dst -> curr_time - sub) - diff;
  dst -> curr_time = sub;
  dst -> curr_duration = duration;
  return 1;
}

void utaust_delete_iterator(utaust_iterator* dst) {
  if(dst == NULL) return;
  free(dst);
}
