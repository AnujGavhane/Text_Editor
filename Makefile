CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2
TARGET = text_editor
SOURCES = main.c text_editor.c text_ops.c cursor_nav.c file_ops.c display_search.c undo_redo.c

all: $(TARGET)

$(TARGET): $(SOURCES) text_editor.h editor_internal.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	$(RM) $(TARGET) $(TARGET).exe

.PHONY: all clean
