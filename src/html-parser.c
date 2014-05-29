#include "html-parser.h"

void MMUHtmlParserInit(MMUHtmlParser* parser, const MMUCallbacks* callbacks,
        const MMUOptions* options) {
    MMUBuilderInit(parser->builder, callbacks, options);
}

void MMUHtmlParserDestroy(MMUHtmlParser* parser) {
    MMUBuilderDestroy(parser->builder);
}

void MMUHtmlParserParse(MMUHtmlParser* parser, const char* html, size_t len) {
    // TODO
    int rc;
    htmlParserCtxtPtr htmlParserContext = htmlCreateMemoryParserCtxt(html, (int)len);
    rc = htmlParseDocument(htmlParserContext);
}

void mmuParseHtml(const char* html, size_t len, const MMUCallbacks* callbacks,
        const MMUOptions* options) {
    MMUHtmlParser parser[1];
    MMUHtmlParserInit(parser, callbacks, options);
    MMUHtmlParserParse(parser, html, len);
    MMUHtmlParserDestroy(parser);
}
