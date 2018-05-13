//
//  VFS_impl.c
//  Virtual File System
//
//  Created by Susanna Ardigo' on 07.05.18.
//  Copyright © 2018 Susanna Ardigo'. All rights reserved.
//

#include "vfs.h"
#include "VFS_impl.h"
#include "VFS_DISK.h"



struct vfs* vfs_open(enum vfs_type t, const char* root_folder) {
    if (t == VFS_MEMORY) {
        return memory_vfs_open(t, root_folder);
    }
    else if (t == VFS_DISK){
        return disk_vfs_open(t, root_folder);
    }
    return NULL;
}

int vfs_mkdir (struct vfs* root, const char* path) {
    return root->vtable->vfs_mkdir(root, path);
}

void vfs_close(struct vfs* root) {
    root->vtable->vfs_close(root);
};

struct vfile* vfile_open (struct vfs* root, const char* file_name) {
    return root->vtable->vfile_open(root, file_name);
}

int vfile_write (struct vfile* f, const char* data, size_t data_len) {
    return f->vtable->vfile_write(f, data, data_len);
}

int vfile_append (struct vfile* f, const char* data, size_t data_len) {
    return f->vtable->vfile_append(f, data, data_len);
}

size_t vfile_read (struct vfile* f, char* data, size_t data_len) {
    return f->vtable->vfile_read(f, data, data_len);
}

void vfile_close (struct vfile* f) {
    f->vtable->vfile_close(f);
}
