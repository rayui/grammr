.export _str_syntax_error_end_of_command_expected
_str_syntax_error_end_of_command_expected: .asciiz "Were you finished talking? If you want to say something more, try 'then'..."

.export _str_syntax_error_command_expected
_str_syntax_error_command_expected: .asciiz "I expected something starting with go or look or use..."

.export _str_syntax_error_word_expected
_str_syntax_error_word_expected: .asciiz "Maybe you wanted a particular item, or exit?"

.export _str_syntax_error_direction_expected
_str_syntax_error_direction_expected: .asciiz "I don't know how to go to %s. Try one of the exits..."

.export _str_syntax_error_item_expected
_str_syntax_error_item_expected: .asciiz "Hm, I don't recognise %s. Did you want an item in particular?"

.export _str_syntax_error_preposition_expected
_str_syntax_error_preposition_expected: .asciiz "Did you want to put that thing on, or in another thing, or...?"

.export _str_syntax_error_unrecognised_token
_str_syntax_error_unrecognised_token: .asciiz "I'm not a computer! I don't know numbers :("

.export _str_syntax_error_item_unavailable
_str_syntax_error_item_unavailable: .asciiz "I can't see a %s around here. Maybe it's somewhere else?"

.export _str_syntax_error_no_such_action
_str_syntax_error_no_such_action: .asciiz "Nah, you can't %s that."

.export _str_terminal_system_error
_str_terminal_system_error: .asciiz "TERMINAL FAILURE\r\n\r\n"

.export _str_system_error_template
_str_system_error_template: .asciiz "UNSUPPORTED ERROR: %d VAL: %s"

.export _str_command
_str_command:  .asciiz "$"
