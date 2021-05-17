reserved_words = [
    0,
    'begin', 'end', 'integer',
    'if', 'then', 'else',
    'function', 'read', 'write',
    10, 11, '=',
    '<>', '<=', '<',
    '>=', '>', '-',
    '*', ':=', '(',
    ')', ';',
    'EOLN', 'EOF'
]
blank = [' ', '\t', '\n']
symbols = []
constants = []


def is_letter(x):
    if ord('a') <= ord(x) <= ord('z') or ord('A') <= ord(x) <= ord('Z'):
        return True
    else:
        return False


def is_digit(x):
    if ord('0') <= ord(x) <= ord('9'):
        return True
    else:
        return False


def is_num(x):
    for c in x:
        if not is_digit(c):
            return False
    return True


def reserve(x):
    if x in reserved_words:
        return reserved_words.index(x)
    else:
        return 0


# find the index of the token
def lexical_analyse(tok):
    if tok in reserved_words:
        return tok, reserved_words.index(tok)
    elif is_num(tok):
        return tok, 11
    else:
        return tok, 10


def print_result(res):
    if res == 1:
        print('Error 1')
    elif res == 2:
        print('Error 2')
    elif res == 3:
        print('Error 3')
    else:
        print(res[0], res[1])


filename = 'source'
source = filename + '.pas'
dyd = filename + '.dyd'
err = filename + '.err'

state = 0
token = ''


with open(source) as f:
    for line in f:
        for c in line:
            if c in blank:
                if state != 0:
                    # accept
                    result = lexical_analyse(token)
                    print_result(result)
                    token = ''
                    state = 0
            elif is_letter(c):
                if state in [0, 1]:
                    state = 1
                    token += c
                else:
                    result = lexical_analyse(token)
                    print_result(result)
                    token = c
                    state = 1
            elif is_digit(c):
                if state in [0, 3]:
                    token += c
                    state = 3
                elif state == 1:
                    token += c
                else:
                    result = lexical_analyse(token)
                    print_result(result)
                    token = c
                    state = 3
            elif c in ['=', '-', '*', '(', ')', ';']:
                result = lexical_analyse(token)
                print_result(result)
                result = lexical_analyse(c)
                print_result(result)
                token = ''
                state = 0
            elif c == '<':
                if state == 0:
                    token += c
                    state = 10
                else:
                    result = lexical_analyse(token)
                    print_result(result)
                    token = c
                    state = 10
            else:
                result = lexical_analyse(token)
                print_result(result)
