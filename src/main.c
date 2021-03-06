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

#include "theme.h"
#include "language.h"
#include "xhtml.h"

int main(int argc, char** argv) {	
	struct Theme* theme;
	char language[LANGUAGE_LEN];

	if(argc < 3) {
		printf("Usage: %s <input_file> <output_file> [theme_file]\n", argv[0]);
		return 1;
	}

	/* Get the language from the file extension, and load the corresponding theme information */
	infer_language(language, argv[1]);
	
	theme = new_theme();
	load_theme(theme, argv[3], language);

	/* Parse the source file, and write the xhtml */
	write_xhtml(argv[1], argv[2], theme);

	/* Free the theme */
	free_theme(theme);
	return 0;
}

