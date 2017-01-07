.export _str_syntax_error_end_of_command_expected
_str_syntax_error_end_of_command_expected: .asciiz "Were you finished talking? If you want to say something more, try 'then'..."

.export _str_syntax_error_command_expected
_str_syntax_error_command_expected: .asciiz "I expected something starting with go or look or use..."

.export _str_syntax_error_item_expected
_str_syntax_error_item_expected: .asciiz "I don't know what %s is."

.export _str_syntax_error_preposition_expected
_str_syntax_error_preposition_expected: .asciiz "Did you want to put that thing on, or in another thing, or...?"

.export _str_syntax_error_unrecognised_token
_str_syntax_error_unrecognised_token: .asciiz "I'm not a computer! I don't know numbers :("

.export _str_syntax_error_no_such_action
_str_syntax_error_no_such_action: .asciiz "I just can't see how to do that."

.export _str_system_error_no_memory
_str_system_error_no_memory: .asciiz "Out of memory at %s! Instruction too long; aborting."

.export _str_parse_error_item_not_found
_str_parse_error_item_not_found: .asciiz "I can't see a %s around here."

.export _str_parse_error_item_not_in_inventory
_str_parse_error_item_not_in_inventory: .asciiz "I'm not carrying that."

.export _str_parse_error_item_in_inventory
_str_parse_error_item_in_inventory: .asciiz "You've already got that with you."

.export _str_parse_error_quit
_str_parse_error_quit: .asciiz "Quitting..."

.export _str_terminal_system_error
_str_terminal_system_error: .asciiz "TERMINAL FAILURE\r\n\r\n"

.export _str_system_error_template
_str_system_error_template: .asciiz "UNSUPPORTED ERROR: %d VAL: %s"

.export _str_command
_str_command:  .asciiz "$"
