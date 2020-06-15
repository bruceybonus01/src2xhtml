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

#ifndef XHTML_H
#define XHTML_H

#include "theme.h"

/* Writes the DOCTYPE and xmlns sections */
int write_xhtml_declaration(FILE* fd);

/* Writes the CSS for the loaded theme */
int write_css(FILE* fd, struct Theme* theme);

/* Writes the html <head> section, including the <style> section */
int write_xhtml_head(FILE* fd, char* filename, struct Theme* theme);

/* Parses the source code, and writes it to the xhtml document */
int write_xhtml_body(FILE* out, FILE* in, struct Theme* theme);

/* Writes the final </body> and </html> tags */
int write_xhtml_end(FILE* fd);

/* Parses and writes the source code to an xhtml document */
int write_xhtml(char* src_file, char* out_file, struct Theme* theme);

/* Escapes a string and writes them to file */
int write_escaped(FILE* fd, char* string, unsigned int count);

/* Shuffles the buffer, and reads new characters */
int shuffle_and_read(FILE* fd, char* buffer, unsigned int count);

#endif

