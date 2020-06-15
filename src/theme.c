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

#include <string.h>
#include <stdlib.h>

#include "theme.h"
#include "bool.h"

#define LINE_BUF_LEN 256

struct Theme* new_theme() {
	struct Theme* theme = (struct Theme*) malloc(sizeof(struct Theme));
	memset(theme, '\0', sizeof(struct Theme));
	return theme;
}

struct Token* new_token() {
	struct Token* token = (struct Token*) malloc(sizeof(struct Token));
	memset(token, '\0', sizeof(struct Token));
	return token;
}

struct Block* new_block() {
	struct Block* block = (struct Block*) malloc(sizeof(struct Block));
	memset(block, '\0', sizeof(struct Block));
	return block;
}

void free_theme(struct Theme* theme) {
	struct Token* token, *next_token;
	struct Block* block, *next_block;

	token = theme->tokens;
	block = theme->blocks;

	while(token) {
		next_token = token->next;
		free(token);
		token = next_token;
	}

	while(block) {
		next_block = block->next;
		free(block);
		block = next_block;
	}

	free(theme);
}

int load_theme(struct Theme* theme, char* theme_file, char* language) {
	char buffer[LINE_BUF_LEN];
	char* token_str;
	unsigned int string_len;
	FILE* fd;
	BOOL active = FALSE;
	struct Token *token = NULL, *prev_token = NULL;
	struct Block *block = NULL, *prev_block = NULL;
	
	/* Set default theme values */
	strncpy(theme->foreground, "#FFFFFF", 8);
	strncpy(theme->background, "#000000", 8);
	strncpy(theme->font_size,  "14pt"   , 5);
	theme->css_class = 0;

	/* Return early if no theme has been provided */
	if(!theme_file) {
		fprintf(stderr, "[WARN] load_theme: No theme file provided, using default\n");
		return 0;
	}

	/* Return early if we couldn't read the file */
	fd = fopen(theme_file, "r");
	if(!fd) {
		fprintf(stderr, "[WARN] load_theme: Cannot open %s, falling back to default theme. Reason: ", theme_file);
		perror("");
		return 1;
	}

	/* Iterate over each line in the theme file */
	while(fgets(buffer, LINE_BUF_LEN, fd)) {

		/* Remove newline */
		string_len = (unsigned int)strlen(buffer);
		if(string_len && buffer[string_len - 1] == '\n') { 
			buffer[string_len - 1] = '\0';
		}

		/* Remove DOS carriage return */
		string_len = (unsigned int)strlen(buffer);
		if(string_len && buffer[string_len - 1] == '\r') {
			buffer[string_len - 1] = '\0';
		}

		/* Check for empty line */
		if(!(token_str = strtok(buffer, " \t"))) {
			continue;
		}

		/* Start of a language block */
		if(strncmp(token_str, "LANGUAGE", 8) == 0) {
			if((token_str = strtok(NULL, " \t"))) {
				active = strncmp(token_str, language, strlen(language)) == 0;
			} else {
				fprintf(stderr, "[ERROR] load_theme: No value for LANGUAGE given in %s\n", theme_file);
				fclose(fd);
				return 1;
			}
		}

		/* End of a language block */
		else if(active && strncmp(token_str, "END", 3) == 0) {
			return 0;
		}

		/* Background color */
		else if(strncmp(token_str, "BACKGROUND", 10) == 0) {
			if((token_str = strtok(NULL, " \t"))) {
				strncpy(theme->background, token_str, OPTION_LEN);
			} else {
				fprintf(stderr, "[WARN] load_theme: No value for BACKGROUND given in %s\n", theme_file);
			}
		}

		/* Foreground color */
		else if(strncmp(token_str, "FOREGROUND", 10) == 0) {
			if((token_str = strtok(NULL, " \t"))) {
				strncpy(theme->foreground, token_str, OPTION_LEN);
			} else {
				fprintf(stderr, "[WARN] load_theme: No value for FOREGROUND given in %s\n", theme_file);
			}
		}

		/* Font size */
		else if(strncmp(token_str, "FONTSIZE", 8) == 0) {
			if((token_str = strtok(NULL, " \t"))) {
				strncpy(theme->font_size, token_str, OPTION_LEN);
			} else {
				fprintf(stderr, "[WARN] load_theme: No value for FONTSIZE given in %s\n", theme_file);
			}
		}

		/* A token */
		else if(strncmp(token_str, "TOKEN", 5) == 0) {
			token = new_token();
			token->css_class = (theme->css_class)++;

			/* Check the token has associated text */
			if(!(token_str = strtok(NULL, " \t"))) {
				fprintf(stderr, "[WARN] load_theme: No value for TOKEN given in %s\n", theme_file);
				free(token);
				continue;
			}
			strncpy(token->text, token_str, strlen(token_str));

			/* Check the token has an associated color */
			if(!(token_str = strtok(NULL, " \t"))) {
				fprintf(stderr, "[WARN] load_theme: No color for TOKEN given in %s\n", theme_file);
				free(token);
				continue;
			}
			strncpy(token->foreground, token_str, strlen(token_str));

			/* Iterate over the optional attributes */
			token_str = strtok(NULL, " \t");
			while(token_str) {
				if(strncmp(token_str, "BOLD", 4) == 0)        { token->bold = TRUE; }
				if(strncmp(token_str, "ITALIC", 6) == 0)      { token->italic = TRUE; }
				if(strncmp(token_str, "UNDERLINED", 10) == 0) { token->underlined = TRUE; }
				token_str = strtok(NULL, " \t");
			}
			
			/* Add the token to the theme */
			if(!theme->tokens) { theme->tokens = token; } 
			else               { prev_token->next = token; }
			prev_token = token;
		}

		/* A block (e.g. comment, string literal */
		else if(strncmp(token_str, "BLOCK", 5) == 0 || strncmp(token_str, "COMMENT", 7) == 0) {
			block = new_block();
			block->css_class = (theme->css_class)++;
			
			/* Only blocks are multiline, comments are not */
			if(strncmp(token_str, "BLOCK", 5) == 0) { block->multiline = TRUE; }

			/* Check the block has a start delimiter */
			if(!(token_str = strtok(NULL, " \t"))) {
				fprintf(stderr, "[WARN] load_theme: No start delimiter for BLOCK given in %s\n", theme_file);
				free(block);
				continue;
			}
			strncpy(block->start_delim, token_str, strlen(token_str));

			/* Check the block has a end delimiter */
			if(!(token_str = strtok(NULL, " \t"))) {
				fprintf(stderr, "[WARN] load_theme: No end delimiter for BLOCK given in %s\n", theme_file);
				free(block);
				continue;
			}
			strncpy(block->end_delim, token_str, strlen(token_str));

			/* Check the block has a color */
			if(!(token_str = strtok(NULL, " \n"))) {
				fprintf(stderr, "[WARN] load_theme: No color for BLOCK given in %s\n", theme_file);
				free(block);
				continue;
			}
			strncpy(block->foreground, token_str, strlen(token_str));

			/* Iterate over the optional attributes */
			token_str = strtok(NULL, " \t");
			while(token_str) {
				if(strncmp(token_str, "BOLD", 4) == 0)        { block->bold = TRUE; }
				if(strncmp(token_str, "ITALIC", 6) == 0)      { block->bold = TRUE; }
				if(strncmp(token_str, "UNDERLINED", 10) == 0) { block->underlined = TRUE; }
				token_str = strtok(NULL, " \t");
			}

			/* Add the token to the theme */
			if(!theme->blocks) { theme->blocks = block; }
			else               { prev_block->next = block; }
			prev_block = block;
		}
	}

	fclose(fd);

	/* The language was not found in the theme file */
	if(active) { fprintf(stderr, "[WARN] load_theme: No END statement found in file %s for language %s\n", theme_file, language); }
	else       { fprintf(stderr, "[WARN] load_theme: No theme found in file %s for language %s\n", theme_file, language); }
	return 1;	
}

