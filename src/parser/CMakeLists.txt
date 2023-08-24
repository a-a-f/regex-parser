set(parser_sources
    scanner.cpp
    api.cpp
    regex_parser.cpp
    term_parser.cpp
    atom_op_parser.cpp
    atom_parser.cpp
    repetition_parser.cpp
    group_parser.cpp
    char_class_common.cpp
    shorthand_char_class_parser.cpp
    char_class_parser.cpp
    non_printable_char_parser.cpp
)

add_library(parser_lib ${parser_sources})

perform_static_checks(parser_lib)
target_code_coverage(parser_lib)
