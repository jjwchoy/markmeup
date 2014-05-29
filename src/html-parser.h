#ifndef MMU_HTML_PARSER_H_
#define MMU_HTML_PARSER_H_

#include "builder.h"

typedef struct MMUHtmlParser {
    MMUBuilder builder[1];
} MMUHtmlParser;

void MMUHtmlParserInit(MMUHtmlParser* parser, const MMUCallbacks* callbacks,
        const MMUOptions* options);
void MMUHtmlParserDestroy(MMUHtmlParser* parser);
void MMUHtmlParserParse(MMUHtmlParser* parser, const char* html, size_t len);
    
#endif
