# Syntax Operators

TODO: Document more...

# System Procedures

## Type-checking functions

`string?`, `symbol?`, `number?`, `pair?`, `closure?`, `macro?`, `procedure?` and `boolean?` return true for their appropriate types or false otherwise.

## Display functions

`display` writes a single object to the console (it may be abbreviated if it's a complex object)

## String functions

`strlen` returns the length of a string.

`strcat` accepts two strings and returns a string containing the contents of the first string followed by the contents of the second string.

## Number functions

`+`, `-`, `*` and `/` correspond to their equivalent mathematical functions.

## Persistence functions

`save-state` and `resume-state` save and restore program state to a file (named by the first argument).

Upon saving program state, the symbol `saved` is returned upon success. If the saved state is later restored, the call to `resume-state` will effectively replace the running program with the program in the given state file, and will be as though the original call to `save-state` returns the symbol `restored` to it's caller.

(Unix programmers may find the persistence functions comparable to `fork` and `exec` calls, except that forked processes are saved to disk instead of being immediately invoked.)

TODO: Document more...
