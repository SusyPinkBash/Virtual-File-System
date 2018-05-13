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

static char txt2[1024];

static const int   NFILES = 100;
static const char* root = "/Users/Susy/Desktop/temp";
static const int   NFOLDERS = 10;
static const char* folders[] = { "//f1",       "f1/f2/",     "f1/f2/f3//",
                                 "another",    "f",          "f//o//",
                                 "f/o//l/",    "f/o//l//d/", "/f/o/l/d/e//",
                                 "f/o/l/d/e/r" };

static int
check_folder(const char* path)
{
  struct stat s;
  char        fpath[1024];

  sprintf(fpath, "%s/%s", root, path);
  return (stat(fpath, &s) == 0 && S_ISDIR(s.st_mode) && (s.st_mode & S_IRUSR) &&
          (s.st_mode & S_IWUSR) && (s.st_mode & S_IXUSR));
}

static void
write_multiple_files(struct vfs* vfs, const char* path)
{
  int           i = 0;
  char          fname[1024] = { 0 };
  struct vfile* f;

  for (i = 0; i < NFILES; ++i) {
    sprintf(fname, "%s/file-%6.6d.txt", path, i);
    f = vfile_open(vfs, fname);
    assert(f);
    assert(vfile_write(f, txt1, strlen(txt1)));
    vfile_close(f);
  }
}

static int
read_multiple_files(struct vfs* vfs, const char* path)
{
  int           i = 0;
  char          fname[1024] = { 0 };
  size_t        s;
  struct vfile* f;
  int           ret = 0;

  for (i = 0; i < NFILES; ++i) {
    sprintf(fname, "%s/file-%6.6d.txt", path, i);
    f = vfile_open(vfs, fname);
    assert(f);
    s = vfile_read(f, txt2, 1024);

    ret += ((s == strlen(txt1) && strncmp(txt1, txt2, s) == 0));
    vfile_close(f);
  }
  return ret;
}

// tests FILE virtual file system and asserts if the actual folders exist
static void
create_folders(struct vfs* vfs)
{
  int i;

  for (i = 0; i < NFOLDERS; ++i) {
    assert(vfs_mkdir(vfs, folders[i]));
  }
}

static void
run_test(enum vfs_type t, int read_only)
{
  int         i, ret;
  struct vfs* vfs = vfs_open(t, root);
  const char* path;

  assert(vfs);
  create_folders(vfs);

  for (i = 0; i <= NFOLDERS; ++i) {
    path = i == NFOLDERS ? "" : folders[i];

    if (t == VFS_DISK)
      assert(check_folder(path));

    if (!read_only)
      write_multiple_files(vfs, path);

    ret = read_multiple_files(vfs, path);
    assert((t == VFS_DISK || !read_only) ? (ret == NFILES) : (ret == 0));
  }
  vfs_close(vfs);
}

int
main()
{
  // removes the folder created by the FILE virtual file system
    /* comment cause not implemented yet */
  system("rm -rf /Users/Susy/Desktop/temp");
  run_test(VFS_DISK, 0);
  run_test(VFS_MEMORY, 0);
  run_test(VFS_DISK, 1);
  run_test(VFS_MEMORY, 1);

  return EXIT_SUCCESS;
}
