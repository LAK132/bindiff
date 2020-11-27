# bindiff
binary file diff utility for Unicode enabled consoles

# Build

## C

`gcc bindiff.c -o bindiff`

## Rust

`rustc bindiff.rs -o bindiff`

# Usage
Basic:

`./bindiff <path/to/file/one> <path/to/file/two>`

With less:

`./bindiff <path/to/file/one> <path/to/file/two> | less -R`

Output:

![bindiff screenshot](bindiff.png)
