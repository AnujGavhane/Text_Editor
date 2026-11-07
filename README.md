# Text_Editor

A small command-line text editor written in C. It lets you edit a document one character at a time, move a cursor through its lines, search and replace text, and save the result to a file.

The project is organized into modules so readers can follow the code by feature. It demonstrates a doubly linked list for storing lines, dynamic arrays for undo and redo history, file input/output, and manual memory management.

## Requirements

- A C compiler that supports the C99 standard, such as GCC or Clang.
- A terminal or command prompt.
- GNU Make is optional; the project can also be compiled directly with the compiler.

## Build

Run the command from the folder containing the source files.

### GCC or Clang

```sh
gcc -std=c99 -Wall -Wextra -pedantic main.c text_editor.c text_ops.c cursor_nav.c file_ops.c display_search.c undo_redo.c -o text_editor
```

On Windows, the resulting program may be named `text_editor.exe` depending on your compiler:

```powershell
gcc -std=c99 -Wall -Wextra -pedantic main.c text_editor.c text_ops.c cursor_nav.c file_ops.c display_search.c undo_redo.c -o text_editor.exe
```

Compiler options used above:

- `-std=c99` selects the C99 language standard.
- `-Wall -Wextra -pedantic` enables useful warnings.
- `-o` chooses the name of the executable.

### Make

If GNU Make is installed, run:

```sh
make
```

To remove the executable created by Make:

```sh
make clean
```

## Run

On Linux or macOS:

```sh
./text_editor
```

On Windows, from PowerShell:

```powershell
./text_editor.exe
```

The program displays the `editor>` prompt. Enter one command per line. Type `help` at any time to show the short command list, or `quit` to exit.

## Commands

| Command | What it does | Example |
| --- | --- | --- |
| `insert TEXT` | Inserts the given text at the cursor. Use `\n` to start another line. | `insert First line\nSecond line` |
| `delete N` | Deletes up to `N` characters to the right of the cursor on the current line. | `delete 3` |
| `line-delete` | Deletes the current line. An empty line remains if it was the only line. | `line-delete` |
| `left`, `right` | Moves one character. At a line edge, movement continues to the adjacent line when possible. | `left` |
| `up`, `down` | Moves to the adjacent line and keeps the column when possible. If that line is shorter, the cursor moves to its end. | `down` |
| `home`, `end` | Moves to the start or end of the current line. | `home` |
| `top`, `bottom` | Moves to the start of the first line or end of the last line. | `top` |
| `copy N` | Copies up to `N` characters to the right of the cursor on the current line. | `copy 5` |
| `cut N` | Copies and removes up to `N` characters to the right of the cursor on the current line. | `cut 5` |
| `paste` | Inserts the clipboard contents at the cursor. | `paste` |
| `find TEXT` | Prints the line and column of every match. | `find hello` |
| `replace OLD \| NEW` | Replaces all occurrences of `OLD` with `NEW`. The vertical bar separates the two strings. | `replace cat \| dog` |
| `open FILE` | Loads a file, replacing the current document. | `open notes.txt` |
| `save [FILE]` | Saves to `FILE`; if omitted, saves to the file currently open or last saved. | `save notes.txt` |
| `print` | Displays all document lines and the cursor location. | `print` |
| `undo`, `redo` | Reverses or reapplies the most recent text change. | `undo` |
| `close` | Closes the current document and starts an empty one. | `close` |
| `help` | Displays a compact command summary. | `help` |
| `quit` | Exits the editor. | `quit` |

Columns shown by the editor start at **0**. For example, column 0 is before the first character. Line numbers shown to the reader start at **1**.

## Walkthrough

The following session creates a two-line document, makes an edit, tries undo and redo, searches, replaces text, and saves the document:

```text
editor> insert Hello World\nThis is line two.
Inserted.
editor> print
     1 | Hello World
>    2 | This is line two.
Cursor: line 2, column 17 [modified]
editor*> left
editor*> insert !
Inserted.
editor*> undo
Undone.
editor*> redo
Redone.
editor*> find World
Found at line 1, column 6
1 match(es).
editor*> replace World | Editor
Replaced 1 occurrence(s).
editor*> print
>    1 | Hello Editor
     2 | This is line two!.
Cursor: line 1, column 0 [modified]
editor*> save notes.txt
Saved notes.txt
editor> quit
```

The `>` marker identifies the current line. An asterisk in the prompt, as in `editor*>`, indicates the document has unsaved changes.

## How the editor works

### Document storage

Each line is a node in a doubly linked list. A node points to the previous and next lines, and stores its text in a dynamically allocated character array. The editor keeps pointers to the first line, last line, and current cursor line.

### Cursor

The cursor is stored as a line pointer plus a zero-based column number. Horizontal movement changes the column or crosses to a neighboring line. Vertical movement tries to preserve the column, but stops at the end of a shorter line.

### Undo and redo

Before an edit, the editor copies the document text and cursor location into a snapshot. Snapshots are stored in dynamically resized arrays. Undo restores the previous snapshot; redo restores an undone state. Making a new edit clears the redo history.

### File handling

Files are read and written as text. When a file is opened, Windows CRLF and old-style CR line endings are converted to LF line endings internally. Saving writes LF line endings.

## Source files

| File | Responsibility |
| --- | --- |
| `main.c` | Reads commands, parses arguments, and calls editor functions. |
| `text_editor.c` | Initializes and frees the editor, linked lines, and shared snapshot helpers. |
| `text_ops.c` | Inserts text, deletes characters or lines, and handles copy, cut, and paste. |
| `cursor_nav.c` | Implements cursor movement. |
| `file_ops.c` | Opens and saves documents. |
| `display_search.c` | Displays document contents and implements find and replace. |
| `undo_redo.c` | Implements undo and redo using document snapshots. |
| `text_editor.h` | Defines shared structures and declares public editor functions. |
| `editor_internal.h` | Declares helper functions shared by implementation files. |
| `Makefile` | Provides `make` and `make clean` build targets. |
| `notes.txt` | Sample document created and saved with the editor. Run `open notes.txt` to load it. |

## Current behavior and limitations

- Commands are entered one line at a time. `insert` interprets the two characters `\n` as a line break; it does not accept multiline terminal input directly.
- `delete`, `copy`, and `cut` work on the current line only. They do not join or span multiple lines.
- Search and replace are case-sensitive. Replace processes all matches in the document and moves the cursor to the beginning afterward.
- The clipboard exists only while the editor is running.
- `open` replaces the current document. Save important changes before opening another file or using `close`.
- `close` starts a new empty document. `quit` displays a reminder if changes are unsaved, then exits.
- Text is stored as C strings, so this learning project is intended for ordinary text files and does not support embedded NUL bytes or full Unicode cursor positioning.
