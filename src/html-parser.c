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

#include "html-parser.h"

#include <libxml/HTMLparser.h>
#include <libxml/tree.h>

void MMUHtmlParserInit(MMUHtmlParser* parser, const MMUCallbacks* callbacks,
        const MMUOptions* options, void* callbackContext) {
    MMUBuilderInit(parser->builder, callbacks, options, callbackContext);
}

void MMUHtmlParserDestroy(MMUHtmlParser* parser) {
    MMUBuilderDestroy(parser->builder);
}

static void processNode(xmlNodePtr node, MMUBuilder* builder);

static void processLink(xmlElementPtr linkNode, MMUBuilder* builder) {
    xmlAttributePtr attr = linkNode->attributes;
    while (attr) {
        if (strcmp("href", (const char*)attr->name) == 0) {
            if (attr->children && attr->children->type == XML_TEXT_NODE) {
                MMUBuilderStartLink(builder, (const char*)attr->children->content);
                return;
            }
        }
        if (attr->next && attr->next->type == XML_ATTRIBUTE_NODE) {
            attr = (xmlAttributePtr)attr->next;
        } else {
            attr = NULL;
        }
    }
    MMUBuilderStartLink(builder, "");
}

static void processElementStart(xmlElementPtr elementNode, MMUBuilder* builder) {
    const char* tagName = (const char*)elementNode->name;
    if (strcmp("a", tagName) == 0) {
        processLink(elementNode, builder);
    } else if (strcmp("b", tagName) == 0) {
        MMUBuilderPushBold(builder);
    } else if (strcmp("br", tagName) == 0) {
        MMUBuilderAppendLineSeparator(builder);
    } else if (strcmp("em", tagName) == 0) {
        MMUBuilderPushItalic(builder);
    } else if (strcmp("h1", tagName) == 0) {
        MMUBuilderPushHeading(builder, 1);
    } else if (strcmp("h2", tagName) == 0) {
        MMUBuilderPushHeading(builder, 2);
    } else if (strcmp("h3", tagName) == 0) {
        MMUBuilderPushHeading(builder, 3);
    } else if (strcmp("h4", tagName) == 0) {
        MMUBuilderPushHeading(builder, 4);
    } else if (strcmp("h5", tagName) == 0) {
        MMUBuilderPushHeading(builder, 5);
    } else if (strcmp("h6", tagName) == 0) {
        MMUBuilderPushHeading(builder, 6);
    } else if (strcmp("i", tagName) == 0) {
        MMUBuilderPushItalic(builder);
    } else if (strcmp("li", tagName) == 0) {
        // TODO
    } else if (strcmp("ol", tagName) == 0) {
        // TODO
    } else if (strcmp("p", tagName) == 0) {
        MMUBuilderStartParagraph(builder);
    } else if (strcmp("strong", tagName) == 0) {
        MMUBuilderPushBold(builder);
    } else if (strcmp("u", tagName) == 0) {
        MMUBuilderPushUnderline(builder);
    } else if (strcmp("ul", tagName) == 0) {
        // TODO
    }
}

static void processElementEnd(xmlElementPtr elementNode, MMUBuilder* builder) {
    const char* tagName = (const char*)elementNode->name;
    if (strcmp("a", tagName) == 0) {
        MMUBuilderEndLink(builder);
    } else if (strcmp("b", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("br", tagName) == 0) {
        // Do nothing
    } else if (strcmp("em", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("h1", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("h2", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("h3", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("h4", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("h5", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("h6", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("i", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("li", tagName) == 0) {
        // TODO
    } else if (strcmp("ol", tagName) == 0) {
        // TODO
    } else if (strcmp("p", tagName) == 0) {
        MMUBuilderEndParagraph(builder);
    } else if (strcmp("strong", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("u", tagName) == 0) {
        MMUBuilderPop(builder);
    } else if (strcmp("ul", tagName) == 0) {
        // TODO
    }
}

static void processElementNode(xmlElementPtr elementNode, MMUBuilder* builder) {
    processElementStart(elementNode, builder);

    if (elementNode->children) {
        processNode(elementNode->children, builder);
    }

    processElementEnd(elementNode, builder);
}



static void processTextNode(xmlNodePtr textNode, MMUBuilder* builder) {
    const char* content = (const char*)textNode->content;
    MMUBuilderAppendText(builder, content, strlen(content));
}

void processNode(xmlNodePtr node, MMUBuilder* builder) {
    switch (node->type) {
        case XML_ELEMENT_NODE:
            processElementNode((xmlElementPtr)node, builder);
            break;
        case XML_TEXT_NODE:
            processTextNode(node, builder);
            break;
        default:
            if (node->children) {
                processNode(node->children, builder);
            }
            break;
    }

    if (node->next) {
        processNode(node->next, builder);
    }
}

void MMUHtmlParserParse(MMUHtmlParser* parser, const char* html) {
    // TODO
    int rc;
    htmlDocPtr doc;

    doc = htmlReadDoc((const unsigned char*)html, NULL, "UTF-8", XML_PARSE_NOERROR | XML_PARSE_NOWARNING);

    if (doc && doc->type == XML_HTML_DOCUMENT_NODE) {
        processNode((xmlNodePtr)doc, parser->builder);
        xmlFreeDoc(doc);
    }

    MMUBuilderFinish(parser->builder);
}

void mmuParseHtml(const char* html, const MMUCallbacks* callbacks,
        const MMUOptions* options, void* callbackContext) {
    MMUHtmlParser parser[1];
    MMUHtmlParserInit(parser, callbacks, options, callbackContext);
    MMUHtmlParserParse(parser, html);
    MMUHtmlParserDestroy(parser);
}
