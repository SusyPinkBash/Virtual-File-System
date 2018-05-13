#ifndef VFS_H_INCLUDED
#define VFS_H_INCLUDED
#include <stdlib.h>

struct vfs;
struct vfile;

enum vfs_type
{
    VFS_MEMORY,
    VFS_DISK
};


/* Initializes the virtual file system of type t starting from
 root_folder and returns the points to the root */
struct vfs* vfs_open(enum vfs_type t, const char* root_folder);

// Creates the specified directory specified by the absolute path and
// returns 1 in case of success or 0 otherwise
int vfs_mkdir(struct vfs* root, const char* path);

// Closes the virtual file system and frees all associated memory.
// In-memory files and folders should be destroyed.  On-disk files and
// folders should be kept.
void vfs_close(struct vfs* root);

// Opens the file 'file_name' in the root virtual file system
// and returns the pointer to the virtual file or NULL otherwise
struct vfile* vfile_open(struct vfs* root, const char* file_name);

// Writes data_len bytes of data from the beginning of virtual file f,
// returns 1 in case of success, or 0 otherwise
int vfile_write(struct vfile* f, const char* data, size_t data_len);

// Writes data_len bytes of data from the end of virtual file f,
// returns 1 in case of success, or 0 otherwise
int vfile_append(struct vfile* f, const char* data, size_t data_len);

// Read up to data_len bytes from virtual file f and puts into data,
// returning the number of bytes actually read.  A successive read to
// the same virtual_file should continue from where previous one left.
size_t vfile_read(struct vfile* f, char* data, size_t data_len);

// Closes the virtual file and frees all associated memory.  The
// content of in-memory should be kept until the virtual file system
// is closed.  On-disk files content are always kept.
void vfile_close(struct vfile* f);

#endif // VFS_H_INCLUDED
