/* Display, search, and replace functions. */
#include "editor_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* display */
void editor_display(const TextEditor *e) {
    size_t n=0; for(Line*l=e->head;l;l=l->next,++n) printf("%c %4zu | %s\n",l==e->cursor?'>':' ',n+1,l->text);
    printf("Cursor: line %zu, column %zu%s\n",e->cursor_line+1,e->cursor_column,e->modified?" [modified]":"");
}

/* find */
size_t editor_find(TextEditor *e,const char *needle) {
    size_t found = 0;
    size_t line_no = 0;

    if (!*needle) return 0;
    for(Line*l=e->head;l;l=l->next,++line_no) for(char*p=l->text;(p=strstr(p,needle))!=NULL;++p){printf("Found at line %zu, column %zu\n",line_no+1,(size_t)(p-l->text));++found;}
    return found;
}

/* replace */
size_t editor_replace(TextEditor *e,const char *needle,const char *replacement) {
    char *all;
    size_t a, b, count = 0;

    if (!*needle) return 0;
    all = flatten(e);
    if (!all) return 0;
    a = strlen(needle);
    b = strlen(replacement);
    for(char*p=all;(p=strstr(p,needle))!=NULL;p+=a)++count;
    if(!count){free(all);return 0;} size_t oldlen=strlen(all); size_t outlen= b>=a ? oldlen+count*(b-a) : oldlen-count*(a-b); char*out=(char*)malloc(outlen+1);if(!out){free(all);return 0;}
    char *src=all,*dst=out,*hit; while((hit=strstr(src,needle))!=NULL){size_t n=(size_t)(hit-src);memcpy(dst,src,n);dst+=n;memcpy(dst,replacement,b);dst+=b;src=hit+a;}strcpy(dst,src);
    Snapshot before={flatten(e),e->cursor_line,e->cursor_column}; if(!before.text||!stack_push(&e->undo,before)){free(before.text);free(all);free(out);return 0;} stack_clear(&e->redo);
    Snapshot target={out,0,0};restore(e,&target);free(out);free(all);e->modified=1;return count;
}
