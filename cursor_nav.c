/* Cursor navigation functions. */
#include "editor_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* move */
int editor_move(TextEditor *e, const char *d) {
    if (!strcmp(d,"left")) { if (e->cursor_column) --e->cursor_column; else if (e->cursor->prev) { e->cursor=e->cursor->prev; --e->cursor_line; e->cursor_column=strlen(e->cursor->text); } }
    else if (!strcmp(d,"right")) { if (e->cursor->text[e->cursor_column]) ++e->cursor_column; else if (e->cursor->next) { e->cursor=e->cursor->next; ++e->cursor_line; e->cursor_column=0; } }
    else if (!strcmp(d,"up") && e->cursor->prev) { e->cursor=e->cursor->prev; --e->cursor_line; if (e->cursor_column>strlen(e->cursor->text)) e->cursor_column=strlen(e->cursor->text); }
    else if (!strcmp(d,"down") && e->cursor->next) { e->cursor=e->cursor->next; ++e->cursor_line; if (e->cursor_column>strlen(e->cursor->text)) e->cursor_column=strlen(e->cursor->text); }
    else if (!strcmp(d,"home")) e->cursor_column=0;
    else if (!strcmp(d,"end")) e->cursor_column=strlen(e->cursor->text);
    else if (!strcmp(d,"top")) { e->cursor=e->head; e->cursor_line=e->cursor_column=0; }
    else if (!strcmp(d,"bottom")) { e->cursor=e->tail; e->cursor_line=0; for(Line*l=e->head;l!=e->tail;l=l->next)++e->cursor_line; e->cursor_column=strlen(e->cursor->text); }
    else return 0;
    return 1;
}
