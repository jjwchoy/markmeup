#include "html-parser.h"

#include <stdio.h>

static void onAppendText(const char* text, size_t len, const MMUContext* context, void* callbackContext) {
    printf("APPEND: [%s] [%zu] [%d] [%d] [%s]\n", text, len, (int)context->textStyle, (int)context->headingLevel, (const char*)callbackContext);
}

static void onMarkLink(const char* text, size_t start, size_t end, void* callbackContext) {
    printf("MARK LINK: [%s] (%zu, %zu) [%s]\n", text, start, end, (const char*)callbackContext);
}

static void onMarkParagraph(size_t start, size_t end, void* callbackContext) {
    printf("MARK PARAGRAPH: (%zu, %zu) [%s]\n", start, end, (const char*)callbackContext);
}

static void onFinish(void* callbackContext) {
    printf("FINISH: [%s]\n", (const char*)callbackContext);
}

int main() {
    const char* context="Callback Context";
    MMUCallbacks callbacks = {
        onAppendText,
        onMarkLink,
        onMarkParagraph,
        onFinish
    };
    MMUOptions options = {
        "<BR>",
        "</P>"
    };
    mmuParseHtml("<p>Paragraph 1 <a href=\"hello\" data-abc=\"goodbye\"><b>hello</b> <i>world</i></a></p><p>Paragraph 2<br>More content</p>", &callbacks, &options, (void*)context);
}
