//
//  VFS_DISK.c
//  Virtual File System
//
//  Created by Susanna Ardigo' on 07.05.18.
//  Copyright © 2018 Susanna Ardigo'. All rights reserved.
//


#include "VFS_DISK.h"
#include "VFS_MEMORY.h"


struct VFS_DISK {
    struct vfs vfs;
    
};



struct vfs* disk_vfs_open(const char* root_folder) {
    return NULL;
}

int disk_vfs_mkdir(struct vfs* root, const char* path) {
    return -1;
}

void disk_vfs_close(struct vfs* root) {
}

struct vfile* disk_vfile_open(struct vfs* root, const char* file_name) {
    return NULL;
}

int disk_vfile_write(struct vfile* f, const char* data, size_t data_len) {
    return -1;
}

int disk_vfile_append(struct vfile* f, const char* data, size_t data_len) {
    return -1;
}

size_t disk_vfile_read(struct vfile* f, char* data, size_t data_len) {
    return -1;
}

void disk_vfile_close(struct vfile* f) {
}
