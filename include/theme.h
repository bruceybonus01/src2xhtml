/*
 * MIT License
 *
 * Copyright (c) 2020 fputs
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef THEME_H
#define THEME_H

#include <stdio.h>

#include "bool.h"

#define TOKEN_LEN  64 /* The maximum length of a token */
#define OPTION_LEN 32 /* The maximum length of a option */

/* Individual words */
struct Token {
	int css_class;
	BOOL bold;
	BOOL italic;
	BOOL underlined;
	char text[TOKEN_LEN];
	char foreground[TOKEN_LEN];
	struct Token* next;
};

/* String literals and comments */
struct Block {
	int css_class;
	BOOL multiline;
	BOOL bold;
	BOOL italic;
	BOOL underlined;
	char start_delim[TOKEN_LEN];
	char end_delim[TOKEN_LEN];
	char foreground[OPTION_LEN];
	struct Block* next;
};

/* A theme for a specific language */
struct Theme {
	int css_class;
	char foreground[OPTION_LEN];
	char background[OPTION_LEN];
	char font_size[OPTION_LEN];
	struct Token* tokens;
	struct Block* blocks;
};

/* Allocate memory for a  new theme */
struct Theme* new_theme(void);

/* Allocate memory for a new token */
struct Token* new_token(void);

/* Allocate memory for a new block */
struct Block* new_block(void);

/* Free a theme */
void free_theme(struct Theme* theme);

/* Writes an error message to stderr */
void theme_error(char* func, char* desc, char* theme_file);

/* Load the theme for a specified language */
int load_theme(struct Theme* theme, char* theme_path, char* language);

#endif

