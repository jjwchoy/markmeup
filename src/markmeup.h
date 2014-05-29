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
    void (*appendText)(const char* text, size_t len, const MMUContext* context);
    void (*markLink)(const char* href, size_t start, size_t end);
    void (*markParagraph)(size_t start, size_t end);
    void (*finish)();
} MMUCallbacks;

typedef struct MMUOptions {
    const char* lineSeparator;
    const char* paragraphSeparator;
} MMUOptions;

void mmuParseHtml(const char* html, size_t len, const MMUCallbacks* callbacks,
        const MMUOptions* options);

#endif
