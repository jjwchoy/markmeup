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

#include "builder.h"

#include <assert.h>
#include <stdlib.h>

static void MMUContextInit(MMUContext* context);
static void MMUContextClone(MMUContext* out,
        const MMUContext* in);

static void MMUContextStackInit(MMUContextStack* stack);
static MMUContext* MMUContextStackPush(
        MMUContextStack* stack);
static void MMUContextStackPop(MMUContextStack* stack);

static void MMUBuilderStartBlock(MMUBuilder* builder);
static size_t MMUBuilderCurrentOffset(MMUBuilder* builder);
static void MMUBuilderFlush(MMUBuilder* builder);

void MMUContextInit(MMUContext* context) {
    memset(context, 0, sizeof(MMUContext));
}

void MMUContextClone(MMUContext* out,
        const MMUContext* in) {
    MMUContextInit(out);
    out->textStyle = in->textStyle;
    out->headingLevel = in->headingLevel;
}

void MMUContextStackInit(MMUContextStack* stack) {
    stack->level = 0;
    MMUContextInit(stack->stack);
}

MMUContext* MMUContextStackPush(MMUContextStack* stack) {
    ++stack->level;

    assert(stack->level < MMUContextStackSize);

    MMUContextClone(stack->stack + stack->level,
            stack->stack + stack->level - 1);

    return stack->stack + stack->level;
}

void MMUContextStackPop(MMUContextStack* stack) {
    assert(stack->level);
    --stack->level;
}

MMUContext* MMUContextStackTop(MMUContextStack* stack) {
    return stack->stack + stack->level;
}

int MMUContextStackCurrentEqualsPrev(MMUContextStack* stack) {
    assert(stack->level);
    return memcmp(stack->stack + stack->level,
            stack->stack + stack->level - 1, sizeof(MMUContext)) == 0;
}

void MMUBuilderInit(MMUBuilder* builder, const MMUCallbacks* callbacks,
        const MMUOptions* options, void* callbackContext) {
    static const size_t INITIAL_BUFFER_SIZE = 4096;

    MMUContextStackInit(builder->contextStack);
    builder->callbacks = callbacks;
    builder->options = options;

    builder->buffer = malloc(INITIAL_BUFFER_SIZE);
    builder->bufferLen = 0;
    builder->bufferCapacity = INITIAL_BUFFER_SIZE;

    builder->currentHref = NULL;
    builder->linkStart = 0;

    builder->inParagraph = 0;
    builder->paragraphStart = 0;

    builder->listDepth = 0;

    builder->flushedLen = 0;
    builder->callbackContext = callbackContext;
}

void MMUBuilderDestroy(MMUBuilder* builder) {
    free(builder->buffer);
}

void MMUBuilderPushBold(MMUBuilder* builder) {
    if (!(MMUContextStackTop(builder->contextStack)->textStyle & MMU_TEXT_STYLE_BOLD)) {
        MMUBuilderFlush(builder);
    }

    MMUContextStackPush(builder->contextStack)->textStyle |= MMU_TEXT_STYLE_BOLD;
}

void MMUBuilderPushItalic(MMUBuilder* builder) {
    if (!(MMUContextStackTop(builder->contextStack)->textStyle & MMU_TEXT_STYLE_ITALIC)) {
        MMUBuilderFlush(builder);
    }

    MMUContextStackPush(builder->contextStack)->textStyle |= MMU_TEXT_STYLE_ITALIC;
}

void MMUBuilderPushUnderline(MMUBuilder* builder) {
    if (!(MMUContextStackTop(builder->contextStack)->textStyle & MMU_TEXT_STYLE_UNDERLINED)) {
        MMUBuilderFlush(builder);
    }

    MMUContextStackPush(builder->contextStack)->textStyle |= MMU_TEXT_STYLE_UNDERLINED;
}

void MMUBuilderPushHeading(MMUBuilder* builder, int level) {
    if (MMUContextStackTop(builder->contextStack)->headingLevel != level) {
        MMUBuilderFlush(builder);
    }

    MMUContextStackPush(builder->contextStack)->headingLevel = level;
}

void MMUBuilderPop(MMUBuilder* builder) {
    if (!MMUContextStackCurrentEqualsPrev(builder->contextStack)) {
        MMUBuilderFlush(builder);
    }
    MMUContextStackPop(builder->contextStack);
}

void MMUBuilderAppendText(MMUBuilder* builder, const char* text, size_t size) {
    while ((builder->bufferCapacity - builder->bufferLen) < (size + 1)) {
        builder->buffer = realloc(builder->buffer, builder->bufferCapacity * 2);
    }

    memcpy(builder->buffer + builder->bufferLen, text, size);
    builder->bufferLen += size;
}

void MMUBuilderAppendLineSeparator(MMUBuilder* builder) {
    const char* lineSeparator = builder->options->lineSeparator;
    MMUBuilderAppendText(builder, lineSeparator, strlen(lineSeparator));
}

void MMUBuilderStartLink(MMUBuilder* builder, const char* href) {
    assert(builder->currentHref == NULL);
    builder->currentHref = href;
    builder->linkStart = MMUBuilderCurrentOffset(builder);
}

void MMUBuilderEndLink(MMUBuilder* builder) {
    MMUBuilderFlush(builder);

    builder->callbacks->markLink(builder->currentHref, builder->linkStart, builder->flushedLen, builder->callbackContext);
    builder->currentHref = NULL;
    builder->linkStart = 0;
}

void MMUBuilderStartParagraph(MMUBuilder* builder) {
    MMUBuilderStartBlock(builder);

    builder->inParagraph = 1;
    builder->paragraphStart = MMUBuilderCurrentOffset(builder);
}

void MMUBuilderEndParagraph(MMUBuilder* builder) {
    const char* paragraphSeparator = builder->options->paragraphSeparator;
    MMUBuilderAppendText(builder, paragraphSeparator, strlen(paragraphSeparator));

    builder->callbacks->markParagraph(builder->paragraphStart,
            MMUBuilderCurrentOffset(builder), builder->callbackContext);

    builder->inParagraph = 0;
    builder->paragraphStart = 0;
}

void MMUBuilderStartList(MMUBuilder* builder, int ordered) {
    MMUBuilderStartBlock(builder);
    ++builder->listDepth;
    MMUListState* listState = builder->listStack + builder->listDepth - 1;
    listState->index = ordered ? 1 : 0;
}

void MMUBuilderEndList(MMUBuilder* builder) {
    assert(builder->listDepth);
    --builder->listDepth;
}

void MMUBuilderStartListItem(MMUBuilder* builder) {
    assert(builder->listDepth);
    MMUBuilderFlush(builder);
    MMUListState* listState = builder->listStack + builder->listDepth - 1;
    builder->callbacks->startListItem(builder->listDepth, listState->index, builder->callbackContext);
}

void MMUBuilderEndListItem(MMUBuilder* builder) {
    assert(builder->listDepth);
    builder->callbacks->endListItem(builder->callbackContext);
}

void MMUBuilderFinish(MMUBuilder* builder) {
    MMUBuilderFlush(builder);
    builder->callbacks->finish(builder->callbackContext);
}

void MMUBuilderStartBlock(MMUBuilder* builder) {
    if (builder->inParagraph) {
        MMUBuilderEndParagraph(builder);
    } else if (MMUBuilderCurrentOffset(builder) > 0) {
        MMUBuilderAppendText(builder, builder->options->paragraphSeparator, strlen(builder->options->paragraphSeparator));
    }
}

size_t MMUBuilderCurrentOffset(MMUBuilder* builder) {
    return builder->flushedLen + builder->bufferLen;
}

void MMUBuilderFlush(MMUBuilder* builder) {
    if (builder->bufferLen == 0) {
        return;
    }

    const MMUContext* context = MMUContextStackTop(builder->contextStack);
    // We always keep at least one spare slot in the buffer
    // NULL terminate the string
    builder->buffer[builder->bufferLen] = '\0';
    builder->callbacks->appendText(builder->buffer, builder->bufferLen, context, builder->callbackContext);
    builder->flushedLen += builder->bufferLen;
    builder->bufferLen = 0;
}
