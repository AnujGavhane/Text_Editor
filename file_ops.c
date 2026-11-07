#include "text_editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int editor_open(TextEditor *e, const char *path) {
    FILE *f=fopen(path,"rb"); if(!f){perror(path);return 0;}
    if(fseek(f,0,SEEK_END)!=0){fclose(f);return 0;} long size=ftell(f); if(size<0||fseek(f,0,SEEK_SET)!=0){fclose(f);return 0;}
    char *data=(char*)malloc((size_t)size+1); if(!data){fclose(f);return 0;}
    size_t got=fread(data,1,(size_t)size,f); if(ferror(f)){free(data);fclose(f);return 0;} fclose(f);data[got]='\0';
    /* Text files are treated as UTF-8/byte strings; normalize CRLF and CR to LF. */
    size_t w=0; for(size_t r=0;r<got;++r){if(data[r]=='\r'){if(r+1<got&&data[r+1]=='\n')++r;data[w++]='\n';}else data[w++]=data[r];}data[w]='\0';
    TextEditor fresh; if(!editor_init(&fresh)){free(data);return 0;}
    if(!editor_insert(&fresh,data)){editor_destroy(&fresh);free(data);return 0;}
    free(data); free(fresh.undo.items[0].text); free(fresh.undo.items); memset(&fresh.undo,0,sizeof(fresh.undo));
    fresh.modified=0; fresh.filename=(char*)malloc(strlen(path)+1); if(!fresh.filename){editor_destroy(&fresh);return 0;}strcpy(fresh.filename,path);
    editor_destroy(e); *e=fresh; return 1;
}

int editor_save(TextEditor *e, const char *path) {
    FILE *f=fopen(path,"wb"); if(!f){perror(path);return 0;}
    for(Line*l=e->head;l;l=l->next){if(fputs(l->text,f)==EOF){fclose(f);return 0;}if(l->next&&fputc('\n',f)==EOF){fclose(f);return 0;}}
    if(fclose(f)!=0)return 0;
    char *name=(char*)malloc(strlen(path)+1);if(!name)return 0;strcpy(name,path);free(e->filename);e->filename=name;e->modified=0;return 1;
}
