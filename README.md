tryreadallfiles
====================

Tests that files can be fully read. Traverses directories recursively. Skips "$RECYCLE.BIN" and entries that are not either normal files or directories.

Useful to detect if a filesystem has silently become broken.

Usage:
```
readallfiles fileOrDirectory [-v]
-v  verbose
```
Examples:
```
readallfiles f:\
readallfiles f:\files
readallfiles /home/user1
```
