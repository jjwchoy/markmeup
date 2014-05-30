markmeup
========

markmeup is a simple markup parser to help creating annotated strings such as Cocoa's `NSAttributedString` and Android's `Spannable`.

It is written in ANSI C.

It supports the following markup elements:

- Bold
- Italic
- Underline
- Link
- Paragraph
- LineBreak
- List
- Heading

Parsers
-------

The only currently supported input language is html. markmeup can parse a subset of html using libxml2. All unexpected elements/attributes
are ignored, so this can be used to display user-provided input (providing it doesn't cause any problems for libxml2!), but you should probably
tidy it anyway.

### Recognised Elements/Attributes

- `<b>` `<strong>` -> Bold
- `<i>` `<em>` -> Italic
- `<u>` -> Underline
- `<a href="url">` -> Link
- `<p>` -> Paragraph
- `<br>` -> LineBreak
- `<ol>` `<ul>` `<li>` -> List
- `<h1>` - `<h6>` -> Heading

All other content is ignored (although children of unrecognised nodes are still checked).

Dependencies
------------

- `libxml2` - required to parse html

License
-------

The MIT License (MIT)

Copyright (c) [2014] [Jason Choy]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
