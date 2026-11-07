/* Undo and redo functions. */
#include "editor_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* undo */
int editor_undo(TextEditor *e) {
    if (!e->undo.size) return 0;
    Snapshot current={flatten(e),e->cursor_line,e->cursor_column}; if(!current.text||!stack_push(&e->redo,current)){free(current.text);return 0;}
    Snapshot s=e->undo.items[--e->undo.size]; int ok=restore(e,&s); free(s.text); return ok;
}

/* redo */
int editor_redo(TextEditor *e) {
    if (!e->redo.size) return 0;
    Snapshot current={flatten(e),e->cursor_line,e->cursor_column}; if(!current.text||!stack_push(&e->undo,current)){free(current.text);return 0;}
    Snapshot s=e->redo.items[--e->redo.size]; int ok=restore(e,&s); free(s.text); return ok;
}
