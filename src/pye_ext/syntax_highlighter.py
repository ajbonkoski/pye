from pye import *
#def printf(s): print s
#debug = printf

from pygments import highlight
from pygments.lexers import PythonLexer
from pygments.lexers import CLexer
from pygments.formatters import TerminalFormatter
from pygments.formatter import Formatter
from pygments.formatters import TerminalFormatter
from pygments.style import Style
from pygments.token import *

def init():
    # global lexer, formatter
    # lexer = PythonLexer()
    # #lexer = CLexer()
    # formatter = PyeFormatter(style=PyePythonStyle)
    #formatter = PyeFormatter(style=PyeCStyle)
    pass

# def test():
#     ret = fmt_handler('from pygments import highlight\n def data(): pass')
#     debug("Results of fmt_handler:\n{}".format(ret))

def create_fmt_handler(type):
    lexer = PythonLexer()
    formatter = PyeFormatter(style=PyePythonStyle)

    def fmt_handler(data):
        try:
            highlight(data, lexer, formatter)
            ret = formatter.get_formatted(data)
        except Exception as e:
            debug("exception in syntax_highlighter: {}".format(e))
            return None

        #debug("Results of fmt_handler:\n{}".format(ret))
        return ret

    return fmt_handler

def PyeConvertColors(s):
    if s.startswith("fg:"):
        t = s[3:]
        d = t.split('-')
        is_bright = False
        if len(d) == 2:
            is_bright = (d[0] == 'bright')
            t = d[1]
        i = color.from_string(t)
        if is_bright:
            i += 8
        return '#' + ('0'*6 + str(i))[-6:]
    elif s.startswith("bg:"):
        return 'bg:#' + ('0'*6 + str(color.from_string(s[3:])))[-6:]
    else:
        return s

def PyeStyle(s):
    return ' '.join(map(PyeConvertColors, s.split(' ')))

class PyePythonStyle(Style):
    styles = {
        Comment:              PyeStyle("fg:red"),
        Keyword:              PyeStyle("fg:cyan bold"),
        Name.Function:        PyeStyle("fg:blue bold"),
        Name.Class:           PyeStyle("fg:green"),
        Name.Builtin:         PyeStyle("fg:blue bold"),
        Name.Builtin.Pseudo:  PyeStyle("fg:cyan bold"),
        String:               PyeStyle("fg:green")
        }

class PyeCStyle(Style):
    styles = {
        #Comment.Preproc: "#000004 bold",
        Keyword:         "#000006 bold",
        #Name:            "#000004 bold",
        String:          "#000010"
        }

class ExtractColorException(Exception): pass
def extract_color(c):
    if len(c) != 6: raise ExtractColorException('color length is too short')
    try:
        i = int(c)
    except NumberFormatException as e: raise ExtractColorException(e.value)
    if i < 0 or i > 15: raise ExtractColorException('integer out of range')

    i_adj = (i+1);
    return (i_adj%8)-1, i_adj/8 == 1

class PyeFormatter(Formatter):

    def __init__(self, **options):
        Formatter.__init__(self, **options)
        self.token_map = {}
        self.styles = []

        for token, style in self.style:

            try:
                s = {}
                if style['color']:
                    c, b = extract_color(style['color'])
                    s['fg_color'] = c
                    s['fg_bright'] = b
                if style['bold']:
                    s['bold'] = bool(style['bold'])
                if style['underline']:
                    s['underline'] = bool(style['underline'])
                if style['bgcolor']:
                    c, b = bool(style['bgcolor'])
                    s['bg_color'] = c
                    s['bg_bright'] = b

                i = self.add_style(s) if len(s) != 0 else -1
                self.token_map[token] = i

            except Exception as ex:
                ## use default formatting
                debug("error: style formatted wrong for {}: {}. Skipping...".format(token, ex))
                self.token_map[token] = -1

    def add_style(self, s):

        if s not in self.styles:
            i = len(self.styles)
            self.styles.append(s)
            return i

        else:
            return self.styles.index(s)

    def __str__(self):
        s = ''
        for k,v in self.token_map.items():
            s += "{}: {}\n".format(k, self.styles[v] if v != -1 else '{}')

        s += "Number of distinct styles: {}".format(len(self.styles))
        return s

    # def format(self, tokensource, outfile):
    #     for ttype, value in tokensource:
    #         outfile.write(value)

    def format(self, tokensource, outfile):

        index = 0
        self.regions = []
        for ttype, value in tokensource:
            s = self.token_map[ttype]
            #debug("index='{}', ttype='{}', value='{}', s='{}'".format(index, ttype, value, s))
            if s != -1:
                self.regions.append({'start_index': index,
                                     'length': len(value),
                                     'style_id': s,
                                     })#'value': value})

            index += len(value)

        return None

    def get_formatted(self, data):
        return { 'styles':  self.styles,
                 'regions': self.regions,
                 'data':    data }

init()


if __name__ == '__main__': test()
