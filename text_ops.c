/* Text insertion, deletion, and clipboard functions. */
#include "editor_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* insert */
int editor_insert(TextEditor *e, const char *text) {
    if (!save_snapshot(e)) return 0;
    for (const char *p = text; *p; ++p) {
        if (*p == '\n') {
            char *right = duplicate(e->cursor->text + e->cursor_column);
            if (!right) return 0;
            e->cursor->text[e->cursor_column] = '\0';
            Line *l = new_line(right); free(right);
            if (!l) return 0;
            l->prev = e->cursor; l->next = e->cursor->next;
            if (l->next) l->next->prev = l; else e->tail = l;
            e->cursor->next = l; e->cursor = l; ++e->cursor_line; e->cursor_column = 0;
        } else {
            size_t len = strlen(e->cursor->text);
            char *buf = (char *)malloc(len + 2);
            if (!buf) return 0;
            memcpy(buf, e->cursor->text, e->cursor_column);
            buf[e->cursor_column] = *p;
            memcpy(buf + e->cursor_column + 1, e->cursor->text + e->cursor_column, len - e->cursor_column + 1);
            free(e->cursor->text); e->cursor->text = buf; ++e->cursor_column;
        }
    }
    return 1;
}

/* delete */
int editor_delete(TextEditor *e, size_t count) {
    if (!count) return 1;
    if (!save_snapshot(e)) return 0;
    while (count-- && e->cursor->text[e->cursor_column]) {
        memmove(e->cursor->text + e->cursor_column, e->cursor->text + e->cursor_column + 1,
                strlen(e->cursor->text + e->cursor_column));
    }
    return 1;
}

/* delete line */
int editor_delete_line(TextEditor *e) {
    if (!save_snapshot(e)) return 0;
    if (e->head == e->tail) { e->cursor->text[0] = '\0'; e->cursor_column = 0; return 1; }
    Line *old = e->cursor;
    if (old->next) { e->cursor = old->next; e->cursor->prev = old->prev; }
    else { e->cursor = old->prev; e->cursor->next = NULL; }
    if (old == e->head) e->head = old->next;
    if (old == e->tail) e->tail = old->prev;
    free(old->text); free(old); e->cursor_line = 0;
    for (Line *l = e->head; l != e->cursor; l = l->next) ++e->cursor_line;
    e->cursor_column = 0; return 1;
}

/* copy */
int editor_copy(TextEditor *e,size_t count,int cut) {
    size_t len=strlen(e->cursor->text); if(count>len-e->cursor_column) count=len-e->cursor_column;
    char *p=(char*)malloc(count+1); if(!p)return 0; memcpy(p,e->cursor->text+e->cursor_column,count);p[count]='\0';free(e->clipboard);e->clipboard=p;
    if(cut && count){if(!save_snapshot(e))return 0;memmove(e->cursor->text+e->cursor_column,e->cursor->text+e->cursor_column+count,len-e->cursor_column-count+1);}return 1;
}

/* paste */
int editor_paste(TextEditor *e){return e->clipboard?editor_insert(e,e->clipboard):0;}
