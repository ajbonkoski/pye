from pye import *
from pygments import highlight
from pygments.lexers import PythonLexer
from pygments.formatters import TerminalFormatter
from pygments.formatters import Formatter

def init():
    global lexer, formatter
    lexer = PythonLexer()
    formatter = TerminalFormatter()

def fmt_handler(data):
    debug("syntax_highlighter: inside formatter")
    res = highlight(data, lexer, formatter).rstrip()
    res_ascii = res.encode('ascii', 'replace')
    debug("res='{}' type='{}'".format(res_ascii, type(res_ascii)))
    return res_ascii


from pygments.formatter import Formatter

class NullFormatter(Formatter):
    def format(self, tokensource, outfile):
        for ttype, value in tokensource:
            outfile.write(value)