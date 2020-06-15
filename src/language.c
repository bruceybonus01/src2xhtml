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

#include "language.h"

#define LANG_LINE_BUF_LEN 256

int infer_language(char* lang, char* filename) {
	FILE* fd;
	unsigned int ptr, string_len;
	char* language_name = NULL, *token;
	char buffer[LANG_LINE_BUF_LEN];

	fd = fopen("langmap.txt", "r");
	if(!fd) { 
		fprintf(stderr, "[WARN] infer_language: Could not open langmap.txt for language inference. Reason: ");
		perror("");
		return 1;
	}

	/* Search backwards for the first '.' */
	ptr = (unsigned int)strlen(filename) - 1;
	while(ptr > 0 && filename[ptr--] != '.');

	/* Iterate over each language */
	while(fgets(buffer, LANG_LINE_BUF_LEN, fd)) {
		
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

		/* Get the name of the language */
		token = strtok(buffer, " \t");
		if (!token) { continue; }
		language_name = token;

		/* Iterate over each extension in the file, checking if we have a match */
		token = strtok(NULL, " \t");
		while(token) {
			if(strcmp(token, filename + ptr + 1) == 0) {
				fclose(fd);
				strncpy(lang, language_name, LANGUAGE_LEN);
				return 0;
			}
			token = strtok(NULL, " \t");
		}
		
	}
	
	/* No matching extensions were found */
	fprintf(stderr, "[WARN] infer_language: No matching languages were for the extension %s in langmap.txt\n", filename + ptr + 1);
	return 1;
}

