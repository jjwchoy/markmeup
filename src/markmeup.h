// The MIT License (MIT)
// 
// Copyright (c) [2014] [Jason Choy]
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef MMU_MARKMEUP_H
#define MMU_MARKMEUP_H

#include <stdlib.h>

enum MMUTextStyle {
    MMU_TEXT_STYLE_BOLD = 1 << 0
  , MMU_TEXT_STYLE_ITALIC = 1 << 1
  , MMU_TEXT_STYLE_UNDERLINED = 1 << 2
};

typedef struct MMUContext {
    unsigned char textStyle;
    char headingLevel;
} MMUContext;

typedef struct MMUCallbacks {
    void (*appendText)(const char* text, size_t len, const MMUContext* textContext, void* callbackContext);
    void (*markLink)(const char* href, size_t start, size_t end, void* callbackContext);
    void (*markParagraph)(size_t start, size_t end, void* callbackContext);
    void (*startListItem)(int depth, unsigned int index, void* callbackContext);
    void (*endListItem)(void* callbackContext);
    void (*finish)(void* callbackContext);
} MMUCallbacks;

typedef struct MMUOptions {
    const char* lineSeparator;
    const char* paragraphSeparator;
} MMUOptions;

void mmuParseHtml(const char* html, const MMUCallbacks* callbacks,
        const MMUOptions* options, void* callbackContext);

#endif
