from pye import *
#def printf(s): print s
#debug = printf

from pygments import highlight
from pygments.lexers import PythonLexer
from pygments.lexers import CLexer
from pygments.lexers import CppLexer
from pygments.formatters import TerminalFormatter
from pygments.formatter import Formatter
from pygments.formatters import TerminalFormatter
from pygments.style import Style
from pygments.token import *


class UndefinedSyntaxType(Exception):
    def __init__(self, v): self.value = v

def create(type):

    if type == 'py':
        lexer = PythonLexer()
        formatter = PyeFormatter(style=PyePythonStyle)

    elif type == 'c' or type == 'h':
        #lexer = CLexer()
        lexer = CppLexer()
        formatter = PyeFormatter(style=PyeCStyle)

    else:
        raise UndefinedSyntaxType("Cannot create a syntax highligher for '{}'".format(type))


    def fmt_handler(data, regions):
        debug("regions: {}".format(regions))
        try:
            #formatter.set_lineno(lineno)
            formatter.set_highlight_regions(regions)
            highlight(data, lexer, formatter)
            ret = formatter.get_formatted(data)
            debug("fmt handler result: {}".format(ret))
        except Exception as e:
            debug("exception in syntax_highlighter: {}".format(e))
            return None

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
    cs = ' '.join(map(PyeConvertColors, s.split(' ')))
    #debug("style: '{}'".format(cs))
    return cs

class PyePythonStyle(Style):
    styles = {
        Comment:              PyeStyle("fg:red"),
        Keyword:              PyeStyle("fg:cyan bold"),
        Name.Function:        PyeStyle("fg:blue bold"),
        Name.Class:           PyeStyle("fg:green"),
        Name.Builtin:         PyeStyle("fg:blue bold"),
        Name.Builtin.Pseudo:  PyeStyle("fg:cyan bold"),
        String:               PyeStyle("fg:green"),
        Generic.Emph:         PyeStyle("fg:white bg:red")  ## this is used for highlighting features (really just a hack...)
        }

class PyeCStyle(Style):
    styles = {
        Comment:         "#000004 bold",
        Keyword:         PyeStyle("fg:cyan bold"),
        #Name:            "#000004 bold",
        String:          PyeStyle("fg:green")
        }

class ExtractColorException(Exception): pass
def extract_color(c):
    if len(c) != 6: raise ExtractColorException('color length is too short')
    try:
        i = int(c)
    except NumberFormatException as e: raise ExtractColorException(e.value)
    if i < 0 or i > 15: raise ExtractColorException('integer out of range')

    return i%8, i/8 == 1

class PyeFormatter(Formatter):

    def __init__(self, **options):
        Formatter.__init__(self, **options)
        self.token_map = {}
        self.styles = []

        for token, style in self.style:
            debug("formatter: {} {}".format(token, style))

            try:
                s = {}
                if style['color']:
                    c, b = extract_color(style['color'])
                    debug("fg color: c='{}' b='{}'".format(c, b))
                    s['fg_color'] = c
                    s['fg_bright'] = b
                if style['bold']:
                    s['bold'] = bool(style['bold'])
                if style['underline']:
                    s['underline'] = bool(style['underline'])
                if style['bgcolor']:
                    c, b = extract_color(style['bgcolor'])
                    debug("bg color: c='{}' b='{}'".format(c, b))
                    s['bg_color'] = c
                    s['bg_bright'] = b

                i = self.add_style(s) if len(s) != 0 else -1
                self.token_map[token] = i

                debug("token_map: {}".format(self.token_map))

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

    def set_highlight_regions(self, regions):
        self.highlight_regions = regions

    def set_lineno(self, lineno):
        self.lineno = lineno

    def highlighting_token_chop(self, index, ttype, value):
        #debug("token_chop '{}' : '{}' : '{}'".format(index, ttype, value))
        new_tokens = []
        tok_start = index
        tok_end = index + len(value)
        tok_val_left = value
        for start, length, id in self.highlight_regions:
            end = start + length
            if end < tok_start: continue
            if start >= tok_end: break

            if start > tok_start:
                new_tokens.append((ttype, tok_val_left[:start-tok_start]))
                tok_val_left = tok_val_left[start-tok_start:]
            tok_start = start

            new_tokens.append((Token.Generic.Emph, tok_val_left[:end-start]))
            tok_val_left = tok_val_left[end-start:]
            tok_start = end

            if len(tok_val_left) == 0:
                break

        if len(tok_val_left) != 0:
            new_tokens.append((ttype, tok_val_left))

        return new_tokens

    def format(self, tokensource, outfile):

        index = 0
        self.regions = []
        for ttype, value in tokensource:
            newtokens = self.highlighting_token_chop(index, ttype, value)
            #debug("newtokens: {}".format(newtokens))
            for n_ttype, n_value in newtokens:
                token_style_id = self.token_map[n_ttype]

                if token_style_id != -1:
                    self.regions.append({'start_index': index,
                                         'length': len(n_value),
                                         'style_id': token_style_id,
                                         })

                index += len(n_value)

        return None

    def get_formatted(self, data):
        return { 'styles':  self.styles,
                 'regions': self.regions,
                 'data':    data }


if __name__ == '__main__': test()
