#include "vfs.h"
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
check_folder(const char* path)
{
  struct stat s;

  return (stat(path, &s) == 0 && S_ISDIR(s.st_mode) && (s.st_mode & S_IRUSR) &&
          (s.st_mode & S_IWUSR) && (s.st_mode & S_IXUSR));
}

static void
run_test(enum vfs_type t)
{
  size_t        s;
  struct vfile *f, *f2;
  struct vfs*   vfs =
    vfs_open(t, "/Users/Susy/Desktop/tmp"); // 1 if folder exists with RWX permissions or
                              // creation is successfull

  assert(vfs);
  // may be 0 or 1: mkdir does not creates by default multiple folders,
  // but either the folders may already exist
  // or your implementation can create multiple folders.
  //assert(vfs_mkdir(vfs, "l1/l2/l3") == 0);
  // OR
  //assert(vfs_mkdir(vfs, "l1/l2/l3") == 1);

  assert(vfs_mkdir(vfs, "l1"));
  assert(vfs_mkdir(vfs, "l1/l2"));
  assert(vfs_mkdir(vfs, "l1/l2/l3"));

  if (t == VFS_DISK)
    assert(check_folder("/Users/Susy/Desktop/tmp/l1/l2/l3")); // absolute path include ROOT

  f = vfile_open(vfs, "/l1/l2/first.txt");
  vfile_write(f, txt1, strlen(txt1));
  vfile_close(f);
  f = vfile_open(vfs, "/l1/l2/first.txt");
  vfile_write(f, "HELLO", 5);
  vfile_append(f, "BYE", 3);
  vfile_close(f);
  f = vfile_open(vfs, "/l1/l2/first.txt");
  f2 = vfile_open(vfs, "/l1/l2/l3/second.txt");
  s = vfile_read(f, txt2, 1024);
  vfile_write(f2, "HELLO", 5);
  vfile_append(f2, "BYE", 3);
  vfile_close(f);
  vfile_close(f2);
  assert(strncmp(txt2, expected, s) == 0);
  f = vfile_open(vfs, "/l1/l2/l3/second.txt");
  f2 = vfile_open(vfs, "/tmp/l4/third.txt");
  assert((s = vfile_read(f, txt2, 1024)) == 8);
  assert(strncmp(txt2, "HELLOBYE", s) == 0);
  vfile_write(f, txt1, strlen(txt1));
  assert(f2 == NULL);
  f2 = vfile_open(vfs, "third.txt");
  assert(f2);
  vfile_write(f2, "HELLO", 5);
  vfile_write(f2, "HELLO", 5);
  vfile_write(f2, "HELLO", 5);
  vfile_close(f2);
  f2 = vfile_open(vfs, "third.txt");
  assert(f2);
  vfile_write(f2, "HELLO", 5);
  vfile_write(f2, "HELLO", 5);
  vfile_write(f2, "HELLO", 5);
  vfile_close(f2);
  f2 = vfile_open(vfs, "third.txt");
  assert(f2);
  s = vfile_read(f2, txt2, 1024);
  assert(s == 15);
  assert(strncmp(txt2, "HELLOHELLOHELLO", s) == 0);
  vfile_close(f2);
  vfile_close(f);
  vfs_close(vfs);
}

int
main()
{
  // removes the folder created by the FILE virtual file system
    /* comment cause not implemented yet
  system("rm -rf /tmp/test");
  run_test(VFS_DISK);
     */
  run_test(VFS_DISK);

  return EXIT_SUCCESS;
}
