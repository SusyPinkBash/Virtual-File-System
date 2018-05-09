#ifndef VFS_impl_h
#define VFS_impl_h

#include <stdio.h>
#include "VFS.h"

struct VFS_vtable {
    struct vfs* (* vfs_open) (enum vfs_type type, const char* root_folder);
    int (*vfs_mkdir) (struct vfs* root, const char* path);
    void (*vfs_close) (struct vfs* root);
    struct vfile* (*vfile_open) (struct vfs* root, const char* file_name);
    
};

struct VFILE_vtable {
    int (*vfile_write) (struct vfile* file, const char* data, size_t data_len);
    int (*vfile_append) (struct vfile* file, const char* data, size_t data_len);
    size_t (*vfile_read) (struct vfile* file, const char* data, size_t data_len);
    void (*vfile_close) (struct vfile* file);
};



struct vfs {
    const struct VFS_vtable * vtable;
    struct node * root;
    enum vfs_type type;
};

struct vfile {
    const struct VFILE_vtable * vtable;
    enum vfs_type type;
    const char * title;
    const char * data;
//    struct node * parent; 
};


struct node {
    const char * name;
//    struct node * parent;
    struct node * next;
    struct node * child;
};



#endif /* VFS_impl_h */
