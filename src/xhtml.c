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

#include <stdio.h>
#include <string.h>

#include "xhtml.h"

int write_xhtml_declaration(FILE* fd) {
	/* Check that the file is still open */
	if(!fd) {
		fprintf(stderr, "[ERROR] write_xhtml_declaration: File no longer open for writing\n");
		return 1;
	}

	fputs("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n", fd);
	fputs("<html xmlns=\"http://www.w3.org/1999/xhtml\">\n", fd);
	return 0;
}

int write_css(FILE* fd, struct Theme* theme) {
	struct Token* token;
	struct Block* block;
	
	/* Check that the file is still open */
	if(!fd) {
		fprintf(stderr, "[ERROR] write_css: File no longer open for writing\n");
		return 1;
	}

	/* Write the theme style information */
	fputs("<style type=\"text/css\">\n", fd);
	fputs("body {font-family:monospace;", fd);
	fprintf(fd, "color:%s;", theme->foreground);
	fprintf(fd, "background-color:%s;", theme->background);
	fprintf(fd, "font-size:%s;}\n", theme->font_size);
	fputs("p {margin: 5px}\n", fd);

	/* Iterate over each block in the theme, and write its CSS */
	block = theme->blocks;
	while(block) {
		fprintf(fd, ".s2x%d {", block->css_class);
		fprintf(fd, "color:%s;", block->foreground);

		if(block->bold)       { fputs("font-weight:bold;", fd); }
		if(block->underlined) { fprintf(fd, "border-bottom:1px solid %s;", block->foreground); }
		if(block->italic)     { fputs("font-style:italic;", fd); }

		fputs("}\n", fd);
		block = block->next;
	}

	/* Iterate over each token in the theme, and write its CSS */
	token = theme->tokens;
	while(token) {
		fprintf(fd, ".s2x%d {", token->css_class);
		fprintf(fd, "color:%s;", token->foreground);

		if(token->bold)       { fputs("font-weight:bold;", fd); }
		if(token->underlined) { fprintf(fd, "border-bottom:1px solid %s;", token->foreground); }
		if(token->italic)     { fputs("font-style:italic;", fd); }

		fputs("}\n", fd);
		token = token->next;
	}

	fputs("</style>\n", fd);
	return 0;
}

int write_xhtml_head(FILE* fd, char* filename, struct Theme* theme) {
	/* Check that the file is still open */
	if(!fd) {
		fprintf(stderr, "[ERROR] write_xhtml_head: File no longer open for writing\n");
		return 1;
	}

	/* Write the <head> section */
	fputs("<head>\n", fd);
	fprintf(fd, "<title>%s</title>\n", filename);
	fputs("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n", fd);
	if(write_css(fd, theme)) { return 1; }
	fputs("</head>\n", fd);

	return 0;
}

int write_xhtml_body(FILE* out, FILE* in, struct Theme* theme) {
	char buffer[TOKEN_LEN];            /* Using int allows us to store EOF */
	BOOL empty_line = TRUE, match; /* can_highlight will be based on the last removed char */
	struct Block* cur_block = NULL, *block;
	struct Token* token;
	unsigned int index = 0;
	char last_char = '\0';

	/* Check that the file is still open */
	if(!out) {
		fprintf(stderr, "[ERROR] write_xhtml_body: File no longer open for writing\n");
		return 1;
	}

	fread(buffer, TOKEN_LEN, 1, in);
	fputs("<body>\n<p>", out);

	while((buffer[0] & 0xff) != 0xff) {

		switch(buffer[0]) {

			/* Handle newlines */
			case '\n':
				if(cur_block)                          { fputs("</span>", out); }
				if(empty_line)                         { fputs("&nbsp;", out); }
				empty_line = TRUE;
				fputs("</p>\n<p>", out);
				if(cur_block && cur_block->multiline)  { fprintf(out, "<span class=\"s2x%d\">", cur_block->css_class); }
			      	if(cur_block && !cur_block->multiline) { cur_block = NULL; }
				
				/* Cycle the buffer */
				shuffle_and_read(in, buffer, 1);
				continue;
		
			/* Handle everything else */
			default:
				if(last_char == '\\') {

					/* escape the escape :( */
					if(last_char == '\\' && buffer[0] == '\\') {
						write_escaped(out, buffer, 1);
						last_char = '\0';
						shuffle_and_read(in, buffer, 1);
						continue;
					}

					write_escaped(out, buffer, 1);
					last_char = buffer[0];
					shuffle_and_read(in, buffer, 1);
					continue;
				}

				match = FALSE;

				/* Prioritise closing blocks, otherwise, we lose control of our spans */
				if(cur_block) {
					match = TRUE;
					for(index = 0; index < strlen(cur_block->end_delim); index++) {
						if(cur_block->end_delim[index] != buffer[index]) {
							match = FALSE;
							break;
						}
					}
					if(match) {
						write_escaped(out, cur_block->end_delim, index);
						last_char = cur_block->end_delim[index - 1];
						fputs("</span>", out);
						cur_block = NULL;
						empty_line = FALSE;
						shuffle_and_read(in, buffer, index);
						continue;
					}
				}

				/* Check for matching blocks */
				match = FALSE;
				block = theme->blocks;
				while(block) {

					match = TRUE;
					/* Check for opening delimiter */
					for(index = 0; index < strlen(block->start_delim); index++) {
						if(block->start_delim[index] != buffer[index]) {
							match = FALSE;
							break;
						}
					}
					if(match) { 
						/* If we are not in a block, we can become the block */
						if(!cur_block) {
							cur_block = block;
							fprintf(out, "<span class=\"s2x%d\">", block->css_class);
							last_char = block->start_delim[index - 1];
							write_escaped(out, block->start_delim, index);
						} else {
							last_char = block->start_delim[index - 1];
							write_escaped(out, buffer, index);
						}
						
						empty_line = FALSE;
						shuffle_and_read(in, buffer, index);
						break;
					}

					if(match) { break; }
					block = block->next;
				}

				if(match) { continue; }

				/* Check for matching tokens */
				token = theme->tokens;
				while(token) {
					match = TRUE;
					for(index = 0; index < strlen(token->text); index++) {
						if(token->text[index] != buffer[index]) {
							match = FALSE;
							break;
						}
					
					}
					if(match) { break; }
					
					token = token->next;
				}

				if(match) {
					/* If we're not in a comment or string, we can syntax highlight */
					if(!cur_block) {
						fprintf(out, "<span class=\"s2x%d\">", token->css_class);
						fputs(token->text, out);
						fputs("</span>", out);
						last_char = token->text[index - 1];
					} else {
						last_char = token->text[index - 1];
						write_escaped(out, buffer, index);
					}

					empty_line = FALSE;
					shuffle_and_read(in, buffer, index);
				}

			if(match) { continue; }
			last_char = buffer[0];
			write_escaped(out, buffer, 1);
			empty_line = FALSE;
			shuffle_and_read(in, buffer, 1);	
		}
	}

	if(cur_block) { fputs("</span>", out); }
	fputs("</p>\n</body>\n", out);
	return 0;
}

int write_xhtml_end(FILE* fd) {
	/* Check that the file is still open */
	if(!fd) {
		fprintf(stderr, "[ERROR] write_xhtml_end: File no longer open for writing\n");
		return 1;
	}

	fputs("</html>\n",fd);
	return 0;
}

int write_xhtml(char* src_file, char* out_file, struct Theme* theme) {
	FILE* src_fd, *out_fd;

	/* Check that the source file is open for reading */
	if(!(src_fd = fopen(src_file, "r"))) {
		fprintf(stderr, "[ERROR] write_xhtml: Cannot open source file %s. Reason: ", src_file);
		perror("");
		return 1;
	}

	/* Check that the output file is open for writing */
	if(!(out_fd = fopen(out_file, "w+"))) {
		fprintf(stderr, "[ERROR] write_xhtml: Cannot open output file %s. Reason: ", out_file);
		perror("");
		fclose(src_fd);
		return 1;
	}

	/* Break out as soon as we hit an error (I think this cheeky lil method should work */
	if(write_xhtml_declaration(out_fd) || 
	   write_xhtml_head(out_fd, src_file, theme) ||
	   write_xhtml_body(out_fd, src_fd, theme)   ||
	   write_xhtml_end(out_fd))
	;

	fclose(src_fd);
	fclose(out_fd);
	return 0;
}

int write_escaped(FILE* fd, char* string, unsigned int count) {
	unsigned int index = 0;
	if(!fd) {
		fprintf(stderr, "[ERROR] write_escaped: File no longer open for writing\n");
		return 1;
	}

	while(index < count) {
		
		switch(string[index]) {
		case  ' ':
			fputs("&nbsp;", fd);
		     	break;
		case '\t':
			fputs("&nbsp;&nbsp;&nbsp;&nbsp;", fd);
			break;
		case '\r':
			break;
		case '>':
			fputs("&gt;", fd);
			break;
		case '<':
			fputs("&lt;", fd);
			break;
		case '"':
			fputs("&quot;", fd);
			break;
		case '\'':
			fputs("&apos;", fd);
			break;
		case '&':
			fputs("&amp;", fd);
			break;
		default:
			fputc(string[index], fd);

		}
		index++;
	}
	return 0;
}

int shuffle_and_read(FILE* fd, char* buffer, unsigned int count) {
	unsigned int cnt;
	int c;
	memcpy(buffer, buffer + count, TOKEN_LEN - count);
	for(cnt = 0; cnt < count; cnt++) {
		c = fgetc(fd);
		buffer[TOKEN_LEN - count + cnt] = (c == EOF ? (char)0xff : (char)c);
	}
	return 0;
}

