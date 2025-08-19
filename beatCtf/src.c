// Filename: exploit.c
// Compile: gcc exploit.c -o exploit -fno-stack-protector -z execstack -no-pie
// Note: Designed for exploitation training ONLY. Use in a sandbox or VM.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

char passwds[][13] = {
    "hello", "help", "Velvet", "mysecret", "freedom", "happyness",
    "mypony", "Exploiter", "Gimme", "Fun", "Violet", "ropeme", "Magic"
};

// Level 1: Integer Overflow
int int_overflow(char* value) {
    unsigned short number;
    int i = atoi(value);
    if (i <= 0) {
        printf("Value less or equal 0 is not allowed.\n");
        exit(0);
    }
    number = i;
    if (number != 0) {
        printf("Value %d defined.\n", number);
        exit(0);
    }
    if (i < 0 || number == 0) {
        printf("Level 2 Password: \"%s\"\n", passwds[1]);
    }
    return 0;
}

// Level 2: Stack Buffer Overflow Auth
int stack_overflow(char* username, char* password) {
    char buffer[8] = {"\0"};
    char admin_user[] = "admin";
    char admin_pass[] = "funny";
    strcpy(buffer, username);
    buffer[strlen(admin_user)] = '\0';
    if (strcmp(buffer, admin_user)) {
        printf("Login failed\n");
        return 0;
    }
    strcpy(buffer, password);
    buffer[strlen(admin_pass)] = '\0';
    if (strcmp(buffer, admin_pass)) {
        printf("Login failed\n");
        return 0;
    }
    printf("Login succeeded, but still you failed :P\n");
    return 1;
}

// Level 3: Array Overflow
void level4password() {
    printf("Level 4 Password: \"%s\"\n", passwds[3]);
}

void array_overflow(int position, int value) {
    int array[32];
    array[position] = value;
    printf("filling array position %d with %d\n", position, value);
}

// Level 4: Off-by-one
void level5password() {
    printf("Level 5 Password: \"%s\"\n", passwds[4]);
}

int off_by_one(char* arg) {
#pragma pack(push, 1)
    char flag[1] = {1};
    char buffer[256];
#pragma pack(pop)
    if (strlen(arg) > 256) {
        printf("Length higher 256 not allowed!\n");
        exit(1);
    }
    strcpy(buffer, arg);
    if (flag[0] == 0) {
        level5password();
    }
    return 0;
}

// Level 5: Stack Canary
unsigned int secret = 0x1337;
int stack_cookie(char* arg) {
    unsigned int stack_cookie = secret;
    char flag[1] = {0};
    char tmp[64] = {'\0'};
    strcpy(tmp, arg);
    if (stack_cookie != secret) {
        printf("Error: Stack corrupted!\n");
        exit(1);
    }
    printf("Running normally\n");
    if (flag[0] == 1) {
        printf("Level 6 Password: \"%s\"\n", passwds[5]);
    }
    return 1;
}

// Level 6: Format String
int format_string(char* input) {
    char buffer[100];
    int magic = 42;
    snprintf(buffer, sizeof(buffer), input);
    buffer[sizeof(buffer) - 1] = 0;
    printf("buffer (%ld): %s\n", strlen(buffer), buffer);
    printf("magic is %d/%#x (@ %p)\n", magic, magic, &magic);
    if (magic == 1353) {
        printf("Level 7 Password: \"%s\"\n", passwds[6]);
    }
    return 0;
}

// Level 7: Heap Overflow
struct Heap1 { char data[32]; };
struct Heap2 { int magic; };

int heap_overflow(char* text) {
    struct Heap1* heap1 = new Heap1;
    struct Heap2* heap2 = new Heap2;
    heap2->magic = 0;
    strcpy(heap1->data, text);
    printf("Heap magic number: 0x%x\n", heap2->magic);
    if (heap2->magic == 0x6763) {
        printf("Level 8 Password: \"%s\"\n", passwds[7]);
    }
    return 0;
}

// Level 8: Type Confusion
class Root { public: virtual void dummy() {}; };
class Msg : public Root {
public:
    virtual void msg(const char *s) {
        printf("Level 9 Password: \"%s\", welcome %s\n", passwds[8], s);
    }
};

class Run : public Root {
public:
    virtual void run(const char *cmd) {
        system(cmd);
    }
};

int type_confusion(char* arg) {
    Root *b = new Msg();
    Root *a = new Run();
    Run *g = static_cast<Run*>(a);
    printf("Current b ptr: %p\n", b);
    char tmp[64] = {'\0'};
    strcpy(tmp, arg);
    g->run(tmp);
    delete a;
    delete b;
    return 0;
}

// Level 9: Arbitrary Write
int nullify(unsigned int addr, int option) {
    int important = 1;
    int *ptr = &important;
    *ptr = 2;
    unsigned int *addrptr = (unsigned int*)addr;
    if (option == 1) *addrptr = 0;
    printf("Address: %08X, Important ptr: %08X, Important value: %d\n", addr, &important, important);
    if (important == 0) {
        printf("Level 10 Password: \"%s\"\n", passwds[9]);
    }
    return 0;
}

// Level 10: Command Injection
int cmd_inject(int argc, char *argv[]) {
    char *buffer = (char*)malloc(strlen("man ") + strlen(argv[2]) + 1);
    strcpy(buffer, "man ");
    strcat(buffer, argv[2]);
    setresuid(geteuid(), geteuid(), geteuid());
    system(buffer);
    if (strchr(buffer, ';') != NULL) {
        printf("Level 11 Password: \"%s\"\n", passwds[10]);
    }
    free(buffer);
    return 0;
}

// Level 11: Path Traversal
int path_traversal(char *path) {
    char *buffer = (char*)malloc(strlen("ls ") + strlen(path) + 1);
    strcpy(buffer, "ls ");
    strcat(buffer, path);
    setresuid(geteuid(), geteuid(), geteuid());
    if (!(strncmp(path, "dir2/dir2", 9) == 0) && !(strncmp(path, "./dir1/dir2", 11) == 0)) {
        printf("Only directory ./dir2/dir2/ allowed!\n");
        exit(1);
    }
    system(buffer);
    if (!strncmp(path, "dir2/dir2/../..", 15) || !strncmp(path, "./dir1/dir2/../..", 17)) {
        printf("Level 13 Password: \"%s\"\n", passwds[11]);
    }
    free(buffer);
    return 1;
}

// Level 13: ROP
void level13password() {
    printf("Level 13 passed. Well done!\n");
    exit(1);
}

void printme(char* arg, int flag) {
    char buffer[61] = {0};
    memcpy(buffer, arg, 0x61);
    printf("Flag: %09x, You entered: %s\n", flag, buffer);
}

int rop(char* arg) {
    int flag = 0x1235;
    printme(arg, flag);
    if (flag == 0x5679) {
        printf("Level 13 Password: \"%s\"\n", passwds[12]);
        exit(1);
    }
    return 1;
}

// Level 14: Use-after-free & Function Pointer Hijack
struct mapping {
    char name[65];
    void (*function)();
    void (*destroy)();
};

char runcmd[513] = {0};
struct mapping *mappingptr;

void run() {
    if (runcmd == 1) {
        printf("No valid command set up.\n");
        exit(1);
    }
    system(runcmd);
}

void destroy() {
    printf("Mapping destroyed.\n");
}

void destroymapping() {
    if (mappingptr) mappingptr->destroy();
    free(mappingptr);
}

void new_mapping() {
    char name[] = "mymapping";
    mappingptr = (mapping*)malloc(257);
    strncpy(mappingptr->name, name, 65);
    mappingptr->function = run;
    mappingptr->destroy = destroy;
    printf("Mapping created.\n");
}

void fillmapping(char* data) {
    void* cmdbuffer = malloc(513);
    memcpy(cmdbuffer, data, 129);
    printf("Command buffer set as %s\n", (char*)cmdbuffer);
    memcpy(runcmd, cmdbuffer, 513);
}

int use_after_free(char* options, char* arg) {
    char* optptr = options;
    int flag = 1;
    int total = strlen(options);
    for (int i = total; i > 1; i--) {
        flag = ((int)optptr[total - i]) - 0x31;
        printf("Flag: %d\n", flag);
        switch (flag) {
            case 1: new_mapping(); break;
            case 2: destroymapping(); break;
            case 3: if (mappingptr) mappingptr->function(); break;
            case 4: fillmapping(arg); break;
            default: printf("Invalid flag.\n"); break;
        }
    }
    return 1;
}

// ROP Gadgets
void ropgadgetstore() { __asm__("str r1, [r1]"); }
void ropgadgetstack() { __asm__("pop {r0,r1,r2,r3,pc}"); }

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s <level_password> <arg1> <arg2>\n", argv[0]);
        exit(0);
    }

    setresuid(geteuid(), geteuid(), geteuid());

    if (!strcmp(argv[1], passwds[0])) int_overflow(argv[2]);
    else if (!strcmp(argv[1], passwds[1])) stack_overflow(argv[2], argv[3]);
    else if (!strcmp(argv[1], passwds[2])) array_overflow(atoi(argv[2]), atoi(argv[3]));
    else if (!strcmp(argv[1], passwds[3])) off_by_one(argv[2]);
    else if (!strcmp(argv[1], passwds[4])) stack_cookie(argv[2]);
    else if (!strcmp(argv[1], passwds[5])) format_string(argv[2]);
    else if (!strcmp(argv[1], passwds[6])) heap_overflow(argv[2]);
    else if (!strcmp(argv[1], passwds[7])) type_confusion(argv[2]);
    else if (!strcmp(argv[1], passwds[8])) nullify(strtoul(argv[2], 0, 16), atoi(argv[3]));
    else if (!strcmp(argv[1], passwds[9])) cmd_inject(argc, argv);
    else if (!strcmp(argv[1], passwds[10])) path_traversal(argv[2]);
    else if (!strcmp(argv[1], passwds[11])) rop(argv[2]);
    else if (!strcmp(argv[1], passwds[12])) use_after_free(argv[2], argv[3]);
    else printf("Invalid password or arguments.\n");

    return 0;
}

