from pye import *
from pygments import highlight
from pygments.lexers import PythonLexer
from pygments.formatters import TerminalFormatter
from pygments.formatter import Formatter
from pygments.formatters import TerminalFormatter

def init():
    global lexer, formatter
    lexer = PythonLexer()
    #formatter = TerminalFormatter()
    formatter = PyeFormatter()

def fmt_handler(data):
    try:
        res = highlight(data, lexer, formatter).rstrip()
    except Exception: return None

    # res_ascii = res.encode('ascii', 'replace')
    #debug("res='{}' type='{}'".format(res_ascii, type(res_ascii)))
    # return res_ascii

    return { 'data':    data,
             'styles':  [{'fg_color': 1}],
             'regions': [{'start_index': 0,
                          'length': len(data),
                          'style_id': 0 }
                         ]
             }


class PyeFormatter(Formatter):

    def __init__(self, **options):
        Formatter.__init__(self, **options)
        self.styles = {}

        i = 1
        for token, style in self.style:
            debug("=== i={}, token={}, style={} ===".format(i, token, style))

            start = end = ''

            if style['color']:
                debug("color={}".format(style['color']))
            if style['bold']:
                debug("bold={}".format(style['bold']))
            if style['underline']:
                debug("underline={}".format(style['underline']))
            if style['bgcolor']:
                debug("bgcolor={}".format(style['bgcolor']))

            i+=1
            self.styles[token] = (start, end)

    # def format(self, tokensource, outfile):
    #     for ttype, value in tokensource:
    #         outfile.write(value)

    def format(self, tokensource, outfile):
        debug("INSIDE format()")

        for ttype, value in tokensource:
            debug("ttype={}, value={}".format(ttype, value))

            # if the token type doesn't exist in the stylemap
            # we try it with the parent of the token type
            # eg: parent of Token.Literal.String.Double is
            # Token.Literal.String
            while ttype not in self.styles:
                ttype = ttype.parent
            if ttype == lasttype:
                # the current token type is the same of the last
                # iteration. cache it
                lastval += value
            else:
                # not the same token as last iteration, but we
                # have some data in the buffer. wrap it with the
                # defined style and write it to the output file
                if lastval:
                    stylebegin, styleend = self.styles[lasttype]
                    outfile.write(stylebegin + lastval + styleend)
                # set lastval/lasttype to current values
                lastval = value
                lasttype = ttype

        # if something is left in the buffer, write it to the
        # output file, then close the opened <pre> tag
        if lastval:
            stylebegin, styleend = self.styles[lasttype]
            outfile.write(stylebegin + lastval + styleend)
        outfile.write('</pre>\n')

init()
