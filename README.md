# KScheme
An updated & refactored version of Mini-Scheme, a public domain Scheme interpreter.

## Introduction

This is about as simple of an implementation of Scheme (a very flexible LISP-like programming language) as you'll find.

It's based on Mini-Scheme, but has been updated and refactored mostly to make it compile and work properly under modern compilers and operating systems (it should build out-of-the-box on Visual Studio 2019).

I've since added many features and cleaned up some of the existing ones. More advanced features include persistence (you can save a program to disk and continue it later or on a different machine) and examples include a basic conversation engine (see `untitled.scm`).

R5RS is used as a guide when implementing new features but full compatibility is not provided.

## NOTE

Documentation is rather lacking, I've added a lot of features since Mini-Scheme so new documentation is needed.

For now, you'll have to have a look through the C++ source and the `init.scm` file to get an idea of what standard functions are available and what they do. Most of the new features are implemented as optional extensions, so they can be disabled at build-time. Feature/version detection from Scheme code is also planned but not implemented yet.

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
 * Basic structural types (including byte buffers and a special type-and-value pairs for creating customised types based on the built-in ones)

It _doesn't_ support:

 * Particularly high performance (although this is intentional, since many optimisations limit portability and future-proofing)
 * Running in completely standalone environments (it relies on a few standard C functions including malloc/calloc/free and some of stdio)
 * A nice API for embedding in larger C/C++ programs
 * Advanced string or number functionality, in particular:
    * ~Only small integers are supported (depending on C platform, typically 32-bit) - particularly, it does not support floating-point or rational numbers~
    * Only 32-bit integers are fully-supported (but overflow detection is included so you can implement arbitrary precision types on top)
    * Basic floating point support is being added (as of March 2021), so floating point values can now be defined and stored (but functions to do maths on them haven't been added yet - not sure whether to integrate them with the integer functions or provide separate ones, so will get to this after finalising other features - but you could easily add whatever primitive functions you need manually now that floats can be passed around)
    * ~Strings are not fully garbage collected (they are effectively equivalent to symbols, i.e. a string is only to represent some constant text within a program, not to represent any kind of dynamic data)~ This is now irrelevant, strings are now allocated from heap memory rather than an internal pool (and the memory is freed).
    * NOTE: A byte buffer type has been added which is fully garbage collected, so in the future strings will probably just use a similar mechanism (and you're better off using byte buffers if you're doing a lot of manipulation on the strings anyway). In the meantime, saving and resuming a state file will also garbage collect the strings.
    * There is no support for Unicode internally (strings are assumed to be a short sequence of non-zero bytes)

Things I'm looking at maybe adding soon:

 * ~Persistence (saving and restoring program state)~ Added 8 March 2020 (`save-state` and `resume-state` functions accepting a filename, but further testing/extension is required)
 * A somewhat-proper test suite
 * Improved API for embedding (including factoring out malloc/stdio type calls into ones that can be managed by a larger program)
 * Some improved string/number/math/unicode support (ideally, this would be factored out so it can be disabled for low-resource environments, as the existing functionality might be "just enough" for many situations and in many cases the limitations can be worked around in Scheme code anyway)
 * ~Basic structure/array/object support (the main problem here is garbage collection, the current GC seems to work quite well but support for structures of arbitrary size might break some of it's assumptions and require some careful design)~
     - This is starting to be added now (buffer for byte arrays and "abstraction" for type-and-value pairs)

## Basic workarounds

If you absolutely need some specific bits of functionality and don't care how ugly or slow the hack is, the easiest ways to solve the string/number/structure limitations would be:

1. Adding functions written in Scheme to make byte buffers (or some kind of lists-of-strings) look like regular strings
2. ~Changing the internal number handling to use a float or double instead of integer would probably solve most practical limitations at a smaller cost to performance than implementing a full Scheme number stack (and changing the parser to handle decimal points, which shouldn't be too hard)~ Floating point support is now partly-implemented (you'll just need to make sure the relevant `#define` is enabled and add some primitive functions as needed for your application)
3. Based on primitive integer and/or floating-point support, more advanced number types (particularly rationals and arbitrary-precision integers) could be implemented by functions written in Scheme, i.e. storing the more advanced number types as lists (and making use of the integer overflow detection to determine when a larger type is required)
4. More advanced structure/array/object types could be implemented based on lists (performance would probably be poor in many cases but acceptable in others)
5. ~After testing more advanced types using pairs/lists, an equivalent of the normal pair type could be made to distinguish system-built lists from application-built lists if the workarounds needed to be properly hidden (e.g. to make sure `pair?` doesn't return true on a number or string)~ There is now a special "abstraction" type to hold custom type-and-value pairs built out of the standard types (it's basically like a pair, except with separate functions and type id so it can be distinguished from a regular pair)
6. _If_ performance of programs is poor using these workarounds, it will probably make sense to move some of those features into the core VM
7. Other features like precise compatibility with particular Scheme standards could probably mostly be implemented on top of a relatively-small core using a layered approach

## Why some workarounds might be better than real fixes

The core language seems to function quite well, and adding features like more advanced numbers types would probably impact performance (as well as size and disk/memory resource requirements).

So, to build a larger system that's 1) reliable, 2) efficient and 3) standards-compliant, you'd probably want to keep the "standards-compliant" parts _optional_, so they are less likely to impact reliability or efficiency in parts of the system that don't need the more advanced features.

For example, a bitmap graphics program might benefit from faster integer and floating point support whereas a scientific or mathematical program might need support for rationals and perhaps user-defined number types. Importantly, a low-level system program that supports these kinds of programs might need to run without using the floating point unit (either to avoid interrupting application performance or due to system-specific floating point limitations in some environments).

Separating high-level features (i.e. workarounds which can be written in Scheme code) from low-level features (needing changes within the VM) further allows for the possibility of supporting multiple high-level languages running on top of a shared core, not just a single Scheme standard.
