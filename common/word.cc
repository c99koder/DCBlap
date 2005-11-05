/* word.cc - wordification library
 * Copyright (c) 1999-2005 Sam Steele
 *
 * This file is part of DCBlap.
 *
 * DCBlap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * DCBlap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct c99_word {
  char *text;
  struct c99_word *next;
};

struct c99_word *word_list=NULL;
int wrdcnt=0;

int get_word_count() { return wrdcnt; }

void destroy_word_list() {
  //free the words from the linked list
  struct c99_word *current=word_list;
  struct c99_word *temp=NULL;
  while(current!=NULL) {
    temp=current;
    current=current->next;
    free(temp->text);
    free(temp);
  }
  word_list=NULL;
	wrdcnt=0;
}

char *get_word(int num) {
  //get a word from the list by number
  struct c99_word *current=word_list;
  int x;
  for(x=0;x<num;x++) {
    if(current==NULL) { return NULL; }
    current=current->next;
  }
  if(current==NULL) return NULL;
  return current->text;
}

void replace_word(int num, char *text) {
  //replace a word in the list by number
  struct c99_word *current=word_list;
  int x;
  for(x=0;x<num;x++) {
    if(!current) { return; }
    current=current->next;
  }
  strcpy(current->text,text);
}

void delete_word(int num) {
  //delete a word from the list by number
  struct c99_word *current=word_list,*last=NULL;
  int x;
  for(x=0;x<num;x++) {
    if(!current) { return; }
    last=current;
    current=current->next;
  }
  last->next=current->next;
  free(current->text);
  free(current);
	wrdcnt--;
}

void add_word(char *string) {
  //add a word to the end of the list
  struct c99_word *current=word_list;
  struct c99_word *temp=NULL;
  while(current!=NULL) {
    temp=current;
    current=current->next;
  }
  current=(struct c99_word *)malloc(sizeof(struct c99_word));
  if(temp) { temp->next=current; }
  current->next=NULL;
  //allocate room for the string dynamically
  current->text=(char *)malloc(strlen(string)+1);
  strcpy(current->text,string);
  if(!word_list) { word_list=current; }
	wrdcnt++;
}

void wordify(char *string, char seperator) {
  //turn a string into a linked list of words
  unsigned int x;
  int y=0;
  char *buf;
  destroy_word_list();
  buf=(char *)malloc(strlen(string)+1); //be able to store the whole string if we have to
  for(x=0;x<strlen(string);x++) {
    if(string[x]==seperator) {
      buf[y]='\0';
      add_word(buf);
      y=0;
      continue;
    } else {
      buf[y]=string[x];
      y++;
    }
  }
  //add the last word
  buf[y]='\0';
  add_word(buf);
  free(buf);
}

