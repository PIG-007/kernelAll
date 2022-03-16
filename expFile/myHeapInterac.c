//add_addr = 0xffffffffc00001e3
//free_addr = 0xffffffffc00000d4
//0x10_random = 0xfaa7b77d417c521

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <assert.h>
#include <linux/tty.h>
#include <stdio.h>
#define __USE_GNU
#include <sched.h>

void menu();
int openDev(char* pos);
void add_note(int fd);
void del_note(int fd);
void read_note(int fd);
void edit_note(int fd);


struct strcItNote
{
    int idx;
    int len;
    char* data;
};
struct strcItNote itNote;



int main(int argc, char *argv[])
{
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stdin, 0, 2, 0);

    //绑定到一个cpu上,但是绑定到该CPU上kmalloc分配的size就不能超过0xe80了,原因未知
    //作用是多线程分配的时候方便从一个CPU中分配，防止分配到其他CPU保存的内存中
    // unsigned char cpu_mask = 0x0;
    // sched_setaffinity(0, 1, &cpu_mask);


    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET(3,&cpu_mask);

    sched_setaffinity(0, sizeof(cpu_mask), &cpu_mask);

    //open Dev
    char* pos = "/dev/stack";
    int fd = openDev(pos);


    char buf[4];

    while (1) {
        menu();
        read(0, buf, 4);
        switch(atoi(buf))
        {
            case 1:
                add_note(fd);
                break;
            case 2:
                del_note(fd);
                break;
            case 3:
                read_note(fd);
                break;
            case 4:
                edit_note(fd);
                break;
            case 6:
                exit(0);
                break;
            default:
                printf("%s\n","Invalid choice!");
                break;
        }
    }
    return 0;
}


void menu() {
    puts("----------------------");
    puts("  MY  Kernel  NOTE    ");
    puts("----------------------");
    puts(" 1. Add note   ");
    puts(" 2. Delete note       ");
    puts(" 3. Print note        ");
    puts(" 4. Edit note         ");
    puts(" 5. Exit              ");
    puts("--------Author:PIG-007");
    printf("Your choice :");
};

int openDev(char* pos){
    int fd;
    printf("[+] Open %s...\n",pos);
    if ((fd = open(pos, O_RDWR)) < 0) {
        printf("    Can't open device file: %s\n",pos);
        exit(1);
    }
    return fd;
}

void add_note(int fd)
{
    char buf[8];
    char* data;
    int size;
    int idx;
    char* chunk;
    printf("Note size :");
    read(0, buf, 8);
    size = atoi(buf);
    printf("Index :");
    read(0, buf, 8);
    idx = atoi(buf);
    itNote.len = size;
    itNote.idx = idx;
    data = (char *)malloc(size);
    itNote.data = data;
    ioctl(fd,1,&itNote);
}

void del_note(int fd)
{
    char buf[8];
    int idx;
    printf("Index :");
    read(0, buf, 8);
    idx = atoi(buf);
    itNote.idx = idx;
    ioctl(fd,888,&itNote);
}

void edit_note(int fd)
{
    char buf[8];
    int idx;
    char* data;
    size_t size;
    printf("Index :");
    read(0, buf, 8);
    idx = atoi(buf);
    printf("Size :");
    read(0, buf, 8);
    size = atoi(buf);
    printf("Content :");
    data = (char *)malloc(size);
    read(0, data, size);
    itNote.len = size;
    itNote.data = data;
    ioctl(fd,3,&itNote);
}

void read_note(int fd)
{
    char buf[8];
    int idx;
    char* data;
    size_t size;
    printf("Index :");
    read(0, buf, 8);
    idx = atoi(buf);
    itNote.idx = idx;
    printf("Size :");
    read(0, buf, 8);
    size = atoi(buf);
    itNote.len = size;
    itNote.data = data;
    ioctl(fd,4,&itNote);
    printf("Content:%s\n",itNote.data);
}




