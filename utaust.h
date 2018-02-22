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

#ifndef LIBUTAUST_H
#define LIBUTAUST_H

typedef struct utaust_note {
  struct utaust_note* next;
  struct utaust_note* prev;
  int    id;
  double length;
  char*  lyric;
  int    note_num;
  double pre_utt;
  double intensity;
  double modulation;
  char*  flags;
  int    pb_type;
  double pb_start;
  char*  pitchbend;
  char*  pbs;
  char*  pbw;
  char*  pby;
  char*  vbr;
  char*  envelope;
  double tempo;
} utaust_note;

typedef struct {
  char* version;
  double tempo;
  char* project_name;
  char* voice_dir;
  char* out_file;
  char* cache_dir;
  char* tool1;
  char* tool2;
  int mode2;
} utaust_header;

typedef struct {
  int size;
  utaust_header header;
  utaust_note* first;
  utaust_note* last;
} utaust_ust;

typedef struct {
  utaust_note* curr_note;
  double default_tempo;
  double curr_ticks;
  double curr_time;
  double curr_tempo;

  double correction;
  int iter_direction;
} utaust_iterator;

utaust_note* utaust_create_note();
void utaust_delete_note(utaust_note* dst);

utaust_ust* utaust_create_ust();
void utaust_ust_append(utaust_ust* dst, utaust_note* src);
void utaust_delete_ust(utaust_ust* dst);

utaust_ust* utaust_parse(const char* src);
char* utaust_print(utaust_ust* src);

utaust_iterator* utaust_create_iterator(utaust_ust* src);
int utaust_iterator_next(utaust_iterator* dst);
int utaust_iterator_prev(utaust_iterator* dst);
void utaust_delete_iterator(utaust_iterator* dst);

#endif
