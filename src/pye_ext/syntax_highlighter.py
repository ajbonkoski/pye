from pye import *
from pygments import highlight
from pygments.lexers import PythonLexer
from pygments.formatters import TerminalFormatter

lexer = PythonLexer()
formatter = TerminalFormatter()

def fmt_handler(data):
    debug("syntax_highlighter: inside formatter")
    res = highlight(data, lexer, formatter).rstrip()
    res_ascii = res.encode('ascii', 'replace')
    debug("res='{}' type='{}'".format(res_ascii, type(res_ascii)))
    return res_ascii
