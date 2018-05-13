//
//  VFS_DISK.c
//  Virtual File System
//
//  Created by Susanna Ardigo' on 07.05.18.
//  Copyright © 2018 Susanna Ardigo'. All rights reserved.
//


#include "VFS_DISK.h"
#include "VFS_MEMORY.h"
#include <string.h>
#include <sys/stat.h>

/*
 struct vfs {
 const struct VFS_vtable * vtable;
 struct directory * root;
 enum vfs_type type;
 };
 
 struct vfile {
 const struct VFILE_vtable * vtable;
 const char * name;
 char * data;
 size_t length;
 size_t cursor;
 int open;
 struct vfile * next;
 enum vfs_type type;
 };
 
 
 struct directory {
 const char * name;
 struct directory * next;
 struct directory * child;
 struct vfile * vfile;
 };
 */

// ########## HELPER FUNCTIONS ##########

// ##### VFS #####

/* creates a vfs struct with its vtable and a null root node  */
struct vfs * new_vfs_disk_struct(enum vfs_type type) {
    /* vtable for the MEMORY_VFS vfs */
    static const struct VFS_vtable vtable = {
        .vfs_open = disk_vfs_open,
        .vfs_mkdir = disk_vfs_mkdir,
        .vfs_close = disk_vfs_close,
        .vfile_open =  disk_vfile_open,
    };
    
    /* allocats memory for the MEMORY_VFS struct */
    struct vfs * this = malloc(sizeof(struct vfs));
    if (this) {
        this->vtable = &vtable;
        this->root = NULL;
        this->type = type;
    }
    
    return this;
};

// ##### VFILE #####

// ######### MEMORY GIVEN FUNCTIONS TO IMPLEMENT ##########


/* Initializes the virtual file system of type t starting from
 root_folder and returns the points to the root */
struct vfs* disk_vfs_open(enum vfs_type type, const char* root_folder) {
    struct vfs * this = new_vfs_disk_struct(type);
    if (!this) {
        printf("error\n");
        return NULL;
    }
    this->root = new_directory(root_folder, (strlen(root_folder)+1));
    
//    Function: int mkdir (const char *filename, mode_t mode)
    int success = mkdir(root_folder, S_IRWXU | S_IRGRP | S_IROTH);
    if (success != 0)
        return NULL;
    
    return this;
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
