#include "text_editor.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Print the command list. */
static void help(void){
    puts("Commands: insert TEXT | delete N | line-delete | left/right/up/down/home/end/top/bottom\n"
         "          open FILE | save [FILE] | close | print | find TEXT\n"
         "          replace OLD | NEW | copy N | cut N | paste | undo | redo | help | quit");
}
static char *trim(char *s){while(isspace((unsigned char)*s))++s;size_t n=strlen(s);while(n&&isspace((unsigned char)s[n-1]))s[--n]='\0';return s;}
static int parse_count(const char*s,size_t*out){char*end;unsigned long n=strtoul(s,&end,10);if(end==s||*end)return 0;*out=(size_t)n;return 1;}
static char *unquote(char*s){s=trim(s);size_t n=strlen(s);if(n>=2&&((s[0]=='"'&&s[n-1]=='"')||(s[0]=='\''&&s[n-1]=='\''))){s[n-1]='\0';++s;}return s;}

/* Read commands and call the matching editor function. */
int main(void){
    TextEditor e;if(!editor_init(&e)){fputs("Unable to initialize editor (out of memory).\n",stderr);return 1;}
    puts("Text Editor — type 'help' for commands.");
    char input[8192];
    for(;;){printf("editor%s> ",e.modified?"*":"");fflush(stdout);if(!fgets(input,sizeof(input),stdin))break;
        input[strcspn(input,"\r\n")]='\0';char *cmd=trim(input);char *arg=cmd;while(*arg&&!isspace((unsigned char)*arg))++arg;if(*arg)*arg++='\0';arg=trim(arg);
        if(!strcmp(cmd,"quit")||!strcmp(cmd,"exit")){if(e.modified)puts("Unsaved changes will be discarded.");break;}
        else if(!strcmp(cmd,"help"))help();
        else if(!strcmp(cmd,"print"))editor_display(&e);
        else if(!strcmp(cmd,"insert")){size_t n=strlen(arg);char *text=(char*)malloc(n+1);if(!text){puts("Insert failed (memory allocation error).");continue;}size_t j=0;for(size_t i=0;i<n;++i){if(arg[i]=='\\'&&arg[i+1]=='n'){text[j++]='\n';++i;}else text[j++]=arg[i];}text[j]='\0';if(editor_insert(&e,text))puts("Inserted.");else puts("Insert failed (memory allocation error).");free(text);}
        else if(!strcmp(cmd,"delete")){size_t n;if(!parse_count(arg,&n))puts("Usage: delete N");else if(editor_delete(&e,n))puts("Deleted.");else puts("Delete failed.");}
        else if(!strcmp(cmd,"line-delete")){if(editor_delete_line(&e))puts("Line deleted.");else puts("Delete failed.");}
        else if(!strcmp(cmd,"left")||!strcmp(cmd,"right")||!strcmp(cmd,"up")||!strcmp(cmd,"down")||!strcmp(cmd,"home")||!strcmp(cmd,"end")||!strcmp(cmd,"top")||!strcmp(cmd,"bottom")){editor_move(&e,cmd);}
        else if(!strcmp(cmd,"open")){if(!*arg)puts("Usage: open FILE");else if(editor_open(&e,arg))printf("Opened %s\n",arg);else puts("Open failed.");}
        else if(!strcmp(cmd,"save")){const char*p=*arg?arg:e.filename;if(!p)puts("Usage: save FILE (no file name is associated yet)");else if(editor_save(&e,p))printf("Saved %s\n",p);else puts("Save failed.");}
        else if(!strcmp(cmd,"close")){editor_destroy(&e);if(!editor_init(&e)){fputs("Unable to initialize editor.\n",stderr);return 1;}puts("Document closed.");}
        else if(!strcmp(cmd,"undo")){puts(editor_undo(&e)?"Undone.":"Nothing to undo.");}
        else if(!strcmp(cmd,"redo")){puts(editor_redo(&e)?"Redone.":"Nothing to redo.");}
        else if(!strcmp(cmd,"copy")||!strcmp(cmd,"cut")){size_t n;if(!parse_count(arg,&n))puts("Usage: copy N / cut N");else if(editor_copy(&e,n,!strcmp(cmd,"cut")))puts("Copied to clipboard.");else puts("Clipboard operation failed.");}
        else if(!strcmp(cmd,"paste")){puts(editor_paste(&e)?"Pasted.":"Clipboard is empty or paste failed.");}
        else if(!strcmp(cmd,"find")){size_t n=editor_find(&e,unquote(arg));printf("%zu match(es).\n",n);}
        else if(!strcmp(cmd,"replace")){char*sep=strchr(arg,'|');if(!sep)puts("Usage: replace OLD | NEW");else{*sep='\0';size_t n=editor_replace(&e,unquote(arg),unquote(sep+1));printf("Replaced %zu occurrence(s).\n",n);}}
        else if(!*cmd)continue;else printf("Unknown command: %s (type help).\n",cmd);
    }
    editor_destroy(&e);return 0;
}
