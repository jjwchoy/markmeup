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
        const MMUOptions* options) {
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

    builder->flushedLen = 0;
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

void MMUBuilderPushUnderlined(MMUBuilder* builder) {
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
    size_t freeSpace = builder->bufferCapacity - builder->bufferLen;
    while ((builder->bufferCapacity - builder->bufferLen) < size) {
        builder->buffer = realloc(builder->buffer, builder->bufferCapacity * 2);
    }

    memcpy(builder->buffer + builder->bufferLen, text, size);
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

    builder->callbacks->markLink(builder->currentHref, builder->linkStart, builder->flushedLen);
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
            MMUBuilderCurrentOffset(builder));

    builder->inParagraph = 0;
    builder->paragraphStart = 0;
}

void MMUBuilderFinish(MMUBuilder* builder) {
    MMUBuilderFlush(builder);
    builder->callbacks->finish();
}

void MMUBuilderStartBlock(MMUBuilder* builder) {
    if (builder->inParagraph) {
        MMUBuilderEndParagraph(builder);
    }
}

size_t MMUBuilderCurrentOffset(MMUBuilder* builder) {
    return builder->flushedLen + builder->bufferLen;
}

void MMUBuilderFlush(MMUBuilder* builder) {
    const MMUContext* context = MMUContextStackTop(builder->contextStack);
    builder->callbacks->appendText(builder->buffer, builder->bufferLen, context);
    builder->flushedLen += builder->bufferLen;
    builder->bufferLen = 0;
}
