#include "html-parser.h"

#include <stdio.h>

static void onAppendText(const char* text, size_t len, const MMUContext* context) {
    printf("APPEND: [%s] [%zu] [%d] [%d]\n", text, len, (int)context->textStyle, (int)context->headingLevel);
}

static void onMarkLink(const char* text, size_t start, size_t end) {
    printf("MARK LINK: [%s] (%zu, %zu)\n", text, start, end);
}

static void onMarkParagraph(size_t start, size_t end) {
    printf("MARK PARAGRAPH: (%zu, %zu)\n", start, end);
}

static void onFinish() {
    printf("FINISH\n");
}

int main() {
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
    mmuParseHtml("<p>Paragraph 1 <a href=\"hello\" data-abc=\"goodbye\"><b>hello</b> <i>world</i></a></p><p>Paragraph 2<br>More content</p>", &callbacks, &options);
}
