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

#ifndef MMU_CONTEXT_H_
#define MMU_CONTEXT_H_

#include "markmeup.h"

#include <string.h>

enum MMUDefaults {
    MMUContextStackSize = 128
};

typedef struct MMUContextStack {
    MMUContext stack[MMUContextStackSize];
    unsigned int level;
} MMUContextStack;

typedef struct MMUListState {
    unsigned char index;
} MMUListState;

typedef struct MMUBuilder {
    MMUContextStack contextStack[1];
    const MMUCallbacks* callbacks;
    const MMUOptions* options;

    char* buffer;
    size_t bufferLen;
    size_t bufferCapacity;

    const char* currentHref;
    size_t linkStart;

    char inParagraph;
    size_t paragraphStart;

    MMUListState listStack[MMUContextStackSize];
    int listDepth;

    size_t flushedLen;
    void* callbackContext;
} MMUBuilder;

void MMUBuilderInit(MMUBuilder* builder, const MMUCallbacks* callbacks,
        const MMUOptions* options, void* callbackContext);
void MMUBuilderDestroy(MMUBuilder* builder);

void MMUBuilderPushBold(MMUBuilder* builder);
void MMUBuilderPushItalic(MMUBuilder* builder);
void MMUBuilderPushUnderline(MMUBuilder* builder);
void MMUBuilderPushHeading(MMUBuilder* builder, int level);

void MMUBuilderPop(MMUBuilder* builder);

void MMUBuilderAppendText(MMUBuilder* builder, const char* text, size_t size);
void MMUBuilderAppendLineSeparator(MMUBuilder* builder);

void MMUBuilderStartLink(MMUBuilder* builder, const char* href);
void MMUBuilderEndLink(MMUBuilder* builder);

void MMUBuilderStartParagraph(MMUBuilder* builder);
void MMUBuilderEndParagraph(MMUBuilder* builder);

void MMUBuilderStartList(MMUBuilder* builder, int ordered);
void MMUBuilderEndList(MMUBuilder* builder);

void MMUBuilderStartListItem(MMUBuilder* builder);
void MMUBuilderEndListItem(MMUBuilder* builder);

void MMUBuilderFinish(MMUBuilder* builder);

#endif
