/* Shared helpers used by the editor modules. */
#ifndef EDITOR_INTERNAL_H
#define EDITOR_INTERNAL_H
#include "text_editor.h"
Line *new_line(const char *s);
char *duplicate(const char *s);
void clear_lines(TextEditor *e);
void stack_clear(SnapshotStack *s);
int stack_push(SnapshotStack *s, Snapshot item);
char *flatten(const TextEditor *e);
int restore(TextEditor *e, const Snapshot *s);
int save_snapshot(TextEditor *e);
#endif
