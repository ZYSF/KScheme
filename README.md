# KScheme
An updated &amp; refactored version of Mini-Scheme, a public domain Scheme interpreter.

## Introduction

This is about as simple of an implementation of Scheme (a very flexible LISP-like programming language) as you'll find.

It's based on Mini-Scheme, but has been updated and refactored mostly to make it compile and work properly under modern compilers and operating systems (it should build out-of-the-box on Visual Studio 2019).

## Some notes on features & limitations

It _does_ support:

 * Basic Scheme syntax - brackets, symbols, simple strings and numbers (see notes below about strings & numbers)
 * Basic modularity - `load` command works ~but only one level deep~
 * The most basic Scheme functions - `cons`, `car`, `cdr`, `lambda`, `display`, `+`, `-`, etc. should all work more-or-less as expected
 * `call-with-current-continuation` _is_ supported, and seems to work quite well (TODO: better tests)
 * Basic garbage collection is supported (see note below)
 * Reasonable performance (compared to other simple interpreters)
 * Running in low-resource environments (the heap is allocated in separate chunks and the chunk size and maximum number of chunks are configurable in the source)
 * Basic ability to embed it in a larger C/C++ program (most functions are prefixed and most "globals" are factored into a struct passed between functions, so multiple VMs _could_ run side-by-side, but there is no fancy embedding API or easy way to deallocate a VM structure later)
 * Very basic macros (there was a function implementing better macro support in the mini-scheme sources but that functionality had a copyright notice attached, so it was removed from this distribution but it may be simple enough to make the macro support work for your use case if required)

It _doesn't_ support:

 * Advanced string or number functionality, in particular:
    * Only small integers are supported (depending on C platform, typically 32-bit) - particularly, it does not support floating-point or rational numbers
    * Strings are not garbage collected (they are effectively equivalent to symbols, i.e. a string is only to represent some constant text within a program, not to represent any kind of dynamic data)
    * There is no support for Unicode internally (strings are assumed to be a short sequence of non-zero bytes)
 * Particularly high performance
 * Running in completely standalone environments (it relies on a few standard C functions including malloc/calloc/free and some of stdio)
 * A nice API for embedding in larger C/C++ programs

Things I'm looking at maybe adding soon:

 * ~Persistence (saving and restoring program state)~ Added 8 March 2020 (`save-state` and `resume-state` functions accepting a filename, but further testing/extension is required)
 * A somewhat-proper test suite
 * Improved API for embedding (including factoring out malloc/stdio type calls into ones that can be managed by a larger program)
 * Some improved string/number/math/unicode support (ideally, this would be factored out so it can be disabled for low-resource environments, as the existing functionality might be "just enough" for many situations and in many cases the limitations can be worked around in Scheme code anyway)
 * ~Basic structure/array/object support (the main problem here is garbage collection, the current GC seems to work quite well but support for structures of arbitrary size might break some of it's assumptions and require some careful design)~
     - This is starting to be added now (buffer for byte arrays and "abstraction" for type-and-value pairs)

## Basic workarounds

If you absolutely need some specific bits of functionality and don't care how ugly or slow the hack is, the easiest ways to solve the string/number/structure limitations would be:

1. Adding functions written in Scheme to make some kind of lists-of-strings look like regular strings
2. Changing the internal number handling to use a float or double instead of integer would probably solve most practical limitations at a smaller cost to performance than implementing a full Scheme number stack (and changing the parser to handle decimal points, which shouldn't be too hard)
3. Based on primitive integer and/or floating-point support, more advanced number types (particularly rationals and arbitrary-precision integers) could be implemented by functions written in Scheme, i.e. storing the more advanced number types as lists
4. More advanced structure/array/object types could be implemented based on lists (performance would probably be poor in many cases but acceptable in others)
5. After testing more advanced types using pairs/lists, an equivalent of the normal pair type could be made to distinguish system-built lists from application-built lists if the workarounds needed to be properly hidden (e.g. to make sure `pair?` doesn't return true on a number or string)
6. _If_ performance of programs is poor using these workarounds, it will probably make sense to move some of those features into the core VM
7. Other features like precise compatibility with particular Scheme standards could probably mostly be implemented on top of a relatively-small core using a layered approach

## Why some workarounds might be better than real fixes

The core language seems to function quite well, and adding features like more advanced numbers types would probably impact performance (as well as size and disk/memory resource requirements).

So, to build a larger system that's 1) reliable, 2) efficient and 3) standards-compliant, you'd probably want to keep the "standards-compliant" parts _optional_, so they are less likely to impact reliability or efficiency in parts of the system that don't need the more advanced features.

For example, a bitmap graphics program might benefit from faster integer and floating point support whereas a scientific or mathematical program might need support for rationals and perhaps user-defined number types. Importantly, a low-level system program that supports these kinds of programs might need to run without using the floating point unit (either to avoid interrupting application performance or due to system-specific floating point limitations in some environments).

Separating high-level features (i.e. workarounds which can be written in Scheme code) from low-level features (needing changes within the VM) further allows for the possibility of supporting multiple high-level languages running on top of a shared core, not just a single Scheme standard.
