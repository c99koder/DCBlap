/* word.h - wordification library
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

//Free the word list
void destroy_word_list();

//Get a word by position
char *get_word(int num);

//Replace a word
void replace_word(int num, char *text);

//Delete a word
void delete_word(int num);

//Append a word
void add_word(char *string);

//Split a sentence into a linked-list of words
void wordify(char *string, char seperator);

//Return the number of words in the list
int get_word_count();




