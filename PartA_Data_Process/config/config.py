import logging
# Configurations & Constants definitions

"""___________________________________GENERAL______________________________________"""
CHARACTER_ENCODING = 'iso-8859-1'
MINIMAL_EXACT_MATCH_LENGTH = 2
RESULTS_FORM = 'char'

"""_____________________________EXACT MATCH EXTRACTOR______________________________"""
meta_characters = ['.', '^', '$', '*', '+', '?', '{', '}', '[', ']', '(', ')', '|', '\\', '/', 'r', 'n', 't']

bracket_patterns = [
        r'(\\x..){(\d+)}',
        r'(\\x..){(\d+),\d*?}',
        r'(\\?\w){(\d+)}',
        r'(\\?\w){(\d+),\d*?}'
]

unwanted_pattens = [
        r'\/.{0,4}$',                                   # Removes pcre flags after '/' closure (/ims)
        r'\(\?.*?\)',                                   # Remove unwanted / ambiguous capturing groups
        r'(\\)([AbBdDfsStvwWzZ])([\+\*])?(\?)?({\d*?,?\d*?})?',
        r'(\\)([AbBdDfsStvwWzZ])([\+\*])?(\?)?',        # all regex of type \c or \c+ where c is a unique metacharacter.
        # r'(\\)([AbBdDfrnsStvwWzZ])([\+\*])?(\?)?'     # INACTIVE: also removes \r\n
        r'(\/)(\^)?',                                   # Removes pcre opening flag ('\' of '^')
        r'(\.)([\+\*])?(\?)?',                          # all regex of type '.*?' or '.+?' where ? might appear or not.
        # r'\(\?:',                                       # all regex of type '(?:' indicating unwanted match groups.
        # r'\(\?!',                                       # all regex of type '(?!' indicating negative lookahead.
        # r'(\(.*)\|(.*\))',              # INACTIVE:   # all regex of type a | b where a and b could be any symbols.
        # r'\(.*?\)([\+\*\?])(\?)?',         # OPTIONAL:   # all regex of type (something)*? where */+/? might appear.
        r'\|',                            # OPTIONAL:   # Removing Special char '|' for OR operation in regex
        r'\[.*?\]([\+\*])?(\?)?',                       # all regex of type [abc] where in [] could be any symbols.
        r'\[.*?\]({\d*,?\d*?})?',                          # all regex of type [abc]{2,50} where 2,50 are integers.
        r'\\x..{\d*,?\d*?}',                            # all regex of type (\x..){ , }
        r'.{\d*,?\d*?}',                                # all regex of type .{ , } where . is a symbol.
        r'\\x..\?',                       # OPTIONAL:   # all regex of type (\x..)? where ? makes the () optional.
        r'[^\\]\?',                                     # all regex of type c? where c is any symbol that isn't '\'.
        r'\(',                            # OPTIONAL:   # Removing capturing groups '(' ')'
        r'\)',                            # OPTIONAL:   # -"-
    ]
