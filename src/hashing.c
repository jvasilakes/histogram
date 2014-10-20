// See gist.github.com/tonious/1377667
// For a linked-list hash-table example.

struct Entry {
    char * key;
    int value;
    struct Entry *next;
}


struct Hashtable {
    int size;
    struct Entry ** table;
}
