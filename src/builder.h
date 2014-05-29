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

    size_t flushedLen;
} MMUBuilder;

void MMUBuilderInit(MMUBuilder* builder, const MMUCallbacks* callbacks,
        const MMUOptions* options);
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

void MMUBuilderFinish(MMUBuilder* builder);

#endif
