# Configurations & Constants definitions

"""___________________________________GENERAL___________________________________"""
CHARACTER_ENCODING = 'iso-8859-1'
MINIMAL_EXACT_MATCH_LENGTH = 0
RESULTS_FORM = 'char'

"""_____________________________EXACT MATCH EXTRACTOR_____________________________"""
meta_characters = ['.', '^', '$', '*', '+', '?', '{', '}', '[', ']', '(', ')', '|', '\\', '/', 'r', 'n', 't']

unwanted_pattens = [
        r'\/.{0,3}$',                                   # Removes pcre flags after '/' closure (/ims)
        r'(\\)([AbBdDfsStvwWzZ])([\+\*])?(\?)?',        # all regex of type \c or \c+ where c is a unique metacharacter.
        # r'(\\)([AbBdDfrnsStvwWzZ])([\+\*])?(\?)?'     # INACTIVE: also removes \r\n
        r'(\/)(\^)?',                                   # Removes pcre opening flag ('\' of '^')
        r'(\.)([\+\*])?(\?)?',                          # all regex of type '.*?' or '.+?' where ? might appear or not.
        r'\(\?:',                                       # all regex of type '(?:' indicating unwanted match groups.
        r'\(\?!',                                       # all regex of type '(?!' indicating negative lookahead.
        # r'(\(.*)\|(.*\))',              # INACTIVE:   # all regex of type a | b where a and b could be any symbols.
        r'\(',                            # OPTIONAL:   # Removing capturing groups '(' ')'
        r'\)',                            # OPTIONAL:   # -"-
        r'\|',                            # OPTIONAL:   # Removing Special char '|' for OR operation in regex
        r'\[.*?\]{\d+,?\d+?}',                          # all regex of type [abc]{2,50} where 2,50 are integers.
        r'\[.*?\]([\+\*])?(\?)?',                       # all regex of type [abc] where in [] could be any symbols.
        r'.{(\d+),(\d+)}',                              # all regex of type .{2,50} where . is any symbol and 2,50 ints.
        r'.{(\d+),}',                                   # all regex of type .{2,} where . is any symbol and 2 is int.
        r'.{\d+}',                                      # all regex of type .{2} where . is any symbol and 2 is integer.
        r'\(.*?\)([\+\*])?(\?)?',                       # all regex of type (something)*? where */+/? might appear.
        r'\\x..\?',                       # OPTIONAL:   # all regex of type \x..? where '.' could be any symbol.
        r'[^\\]\?',                                     # all regex of type c? where c is any symbol that isn't '\'.
    ]