/* Editor setup, memory cleanup, and shared document helpers. */
#include "editor_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* duplicate */
char *duplicate(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

/* new line */
Line *new_line(const char *s) {
    Line *line = (Line *)calloc(1, sizeof(*line));
    if (!line) return NULL;
    line->text = duplicate(s);
    if (!line->text) { free(line); return NULL; }
    return line;
}

/* clear lines */
void clear_lines(TextEditor *e) {
    Line *line = e->head;
    while (line) { Line *next = line->next; free(line->text); free(line); line = next; }
    e->head = e->tail = e->cursor = NULL;
}

/* stack clear */
void stack_clear(SnapshotStack *s) {
    for (size_t i = 0; i < s->size; ++i) free(s->items[i].text);
    free(s->items); memset(s, 0, sizeof(*s));
}

/* stack push */
int stack_push(SnapshotStack *s, Snapshot item) {
    if (s->size == s->capacity) {
        size_t cap = s->capacity ? s->capacity * 2 : 8;
        Snapshot *p = (Snapshot *)realloc(s->items, cap * sizeof(*p));
        if (!p) return 0;
        s->items = p; s->capacity = cap;
    }
    s->items[s->size++] = item;
    return 1;
}

/* flatten */
char *flatten(const TextEditor *e) {
    size_t n = 1;
    for (Line *l = e->head; l; l = l->next) n += strlen(l->text) + (l->next ? 1 : 0);
    char *out = (char *)malloc(n);
    if (!out) return NULL;
    char *p = out;
    for (Line *l = e->head; l; l = l->next) {
        size_t len = strlen(l->text); memcpy(p, l->text, len); p += len;
        if (l->next) *p++ = '\n';
    }
    *p = '\0'; return out;
}

/* restore */
int restore(TextEditor *e, const Snapshot *s) {
    Line *head = NULL, *tail = NULL, *cur = NULL;
    size_t line_no = 0, col = 0;
    const char *start = s->text;
    for (const char *p = s->text;; ++p) {
        if (*p == '\n' || *p == '\0') {
            size_t len = (size_t)(p - start);
            char *part = (char *)malloc(len + 1);
            if (!part) { while (head) { Line *n = head->next; free(head->text); free(head); head = n; } return 0; }
            memcpy(part, start, len); part[len] = '\0';
            Line *l = new_line(part); free(part);
            if (!l) { while (head) { Line *n = head->next; free(head->text); free(head); head = n; } return 0; }
            l->prev = tail; if (tail) tail->next = l; else head = l; tail = l;
            if (line_no == s->line) { cur = l; col = s->column < len ? s->column : len; }
            ++line_no;
            if (!*p) break;
            start = p + 1;
        }
    }
    clear_lines(e); e->head = head; e->tail = tail;
    e->cursor = cur ? cur : head;
    e->cursor_line = cur ? s->line : 0; e->cursor_column = cur ? col : 0;
    return 1;
}

/* save snapshot */
int save_snapshot(TextEditor *e) {
    Snapshot s; s.text = flatten(e); s.line = e->cursor_line; s.column = e->cursor_column;
    if (!s.text) return 0;
    if (!stack_push(&e->undo, s)) { free(s.text); return 0; }
    stack_clear(&e->redo); e->modified = 1; return 1;
}

/* init */
int editor_init(TextEditor *e) {
    memset(e, 0, sizeof(*e));
    e->head = e->tail = e->cursor = new_line("");
    return e->head != NULL;
}

/* destroy */
void editor_destroy(TextEditor *e) {
    clear_lines(e); free(e->clipboard); free(e->filename);
    stack_clear(&e->undo); stack_clear(&e->redo); memset(e, 0, sizeof(*e));
}
