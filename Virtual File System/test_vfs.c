
#include "VFS.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


static const char* txt1 =
"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
"veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
"commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "
"velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat "
"cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id "
"est laborum.";
static const char* expected =
"HELLO ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
"veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
"commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "
"velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat "
"cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id "
"est laborum.BYE";

static char txt2[1024];

static int
assert_folder(const char* path)
{
    struct stat s;
    return (stat(path, &s) == 0 && S_ISDIR(s.st_mode) && (s.st_mode & S_IRUSR) &&
            (s.st_mode & S_IWUSR) && (s.st_mode & S_IXUSR));
}


int
main()
{
    struct vfile *f;
    struct vfs*   vfs = vfs_open(VFS_MEMORY, "/tmp");
    
    assert(vfs);
    assert(vfs_mkdir(vfs, "l1"));
    assert(vfs_mkdir(vfs, "l1/l2"));
    assert(vfs_mkdir(vfs, "l1/l2/l3"));

        vfs_close(vfs);
//    vfs_mkdir(vfs, "a/b/c");
//    assert(vfs_mkdir(vfs, "l1/l2"));
//    assert(vfs_mkdir(vfs, "l1/l2/l3"));
//    assert(vfs_mkdir(vfs, "l1/l2/l4"));
//    assert_folder("/tmp/l1/l2/l3");
//    f = vfile_open(vfs, "/l1/l2/first.txt");
    printf("end\n");
    // ##########################################
    /*
    struct vfile *f, *f2;
    struct vfs*   vfs = vfs_open(VFS_DISK, "/tmp");
    
    assert(vfs);
    assert(vfs_mkdir(vfs, "l1/l2/l3") == 0);
    assert(vfs_mkdir(vfs, "l1"));
    assert(vfs_mkdir(vfs, "l1/l2"));
    assert(vfs_mkdir(vfs, "l1/l2/l3"));
    assert_folder("/tmp/l1/l2/l3");
    f = vfile_open(vfs, "/l1/l2/first.txt");
    vfile_write(f, txt1, strlen(txt1));
    vfile_close(f);
    f = vfile_open(vfs, "/l1/l2/first.txt");
    vfile_write(f, "HELLO", 5);
    vfile_append(f, "BYE", 3);
    vfile_close(f);
    f = vfile_open(vfs, "/l1/l2/first.txt");
    vfile_read(f, txt2, 1024);
    vfile_close(f);
    assert(strcmp(txt2, expected) == 0);
    f = vfile_open(vfs, "/l1/l2/l3/second.txt");
    vfile_write(f, "HELLO", 5);
    vfile_append(f, "BYE", 3);
    vfile_close(f);
    f = vfile_open(vfs, "/l1/l2/l3/second.txt");
    assert(vfile_read(f, txt2, 1024) == 8);
    assert(strncmp(txt2, "HELLOBYE", 8) == 0);
    vfile_write(f, txt1, strlen(txt1));
    vfile_close(f);
    f2 = vfile_open(vfs, "/tmp/l4/third.txt");
    assert(f2 == NULL);
    vfs_close(vfs);
    
    printf("Don't forget to remove the folder '/tmp/l1' before running the test "
           "again!\n");
    return EXIT_SUCCESS;
     */
    
}

