#include "arena.h"

int main(void) {
    init_global_arena(1024);
    char* text = galloc(5);
    strncpy(text, "Haat", 5);
    gprint_arena();
    int* kaas = galloc(sizeof(*kaas));
    *kaas = 25;
    gfree(text);
    text = galloc(6);
    text[0] = 'h';
    text[1] = 'a';
    text[2] = 't';
    text[3] = 'e';
    text[4] = 'r';
    text[5] = '\0';
    printf("%s\n", text);
    gprint_arena();
    int* haat = galloc(sizeof(*haat));
    *haat = 9;
    galloc(1);
    gprint_arena();
    gdelete();
    return 0;
}
