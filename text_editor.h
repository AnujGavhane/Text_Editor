#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <stddef.h>

typedef struct Line {
    char *text;
    struct Line *prev;
    struct Line *next;
} Line;

typedef struct Snapshot {
    char *text;
    size_t line;
    size_t column;
} Snapshot;

typedef struct SnapshotStack {
    Snapshot *items;
    size_t size;
    size_t capacity;
} SnapshotStack;

typedef struct TextEditor {
    Line *head;
    Line *tail;
    Line *cursor;
    size_t cursor_line;
    size_t cursor_column;
    char *clipboard;
    char *filename;
    int modified;
    SnapshotStack undo;
    SnapshotStack redo;
} TextEditor;

int editor_init(TextEditor *editor);
void editor_destroy(TextEditor *editor);
int editor_insert(TextEditor *editor, const char *text);
int editor_delete(TextEditor *editor, size_t count);
int editor_delete_line(TextEditor *editor);
int editor_move(TextEditor *editor, const char *direction);
void editor_display(const TextEditor *editor);
int editor_open(TextEditor *editor, const char *path);
int editor_save(TextEditor *editor, const char *path);
int editor_undo(TextEditor *editor);
int editor_redo(TextEditor *editor);
int editor_copy(TextEditor *editor, size_t count, int cut);
int editor_paste(TextEditor *editor);
size_t editor_find(TextEditor *editor, const char *needle);
size_t editor_replace(TextEditor *editor, const char *needle, const char *replacement);

#endif
