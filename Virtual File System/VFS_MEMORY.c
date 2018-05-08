//
//  VFS_MEMORY.c
//  Virtual File System
//
//  Created by Susanna Ardigo' on 07.05.18.
//  Copyright © 2018 Susanna Ardigo'. All rights reserved.
//

#include "VFS_MEMORY.h"
#include "VFS_impl.h"
#include <string.h>

//struct MEMORY_VFS {
//    struct vfs vfs;
//};

//struct vfs {
//    const struct VFS_vtable * vtable;
//    const struct node * root;
//};
//
//struct node {
//    const char * name;
//    struct node * parent;
//    struct node * next;
//    struct node * child;
//};


// ########## HELPER FUNCTIONS ##########

/* creates a vfs struct with its vtable and a null root node  */
struct vfs * new_vfs_struct() {
    /* vtable for the MEMORY_VFS vfs */
    static const struct VFS_vtable vtable = {
        .vfs_open = memory_vfs_open,
        .vfs_mkdir = memory_vfs_mkdir,
        .vfs_close = memory_vfs_close,
        .vfile_open =  memory_vfile_open,
        .vfile_write = memory_vfile_write,
        .vfile_append = memory_vfile_append,
        .vfile_read = memory_vfile_read,
        .vfile_close = memory_vfile_close,
    };
    
    /* allocats memory for the MEMORY_VFS struct */
    struct vfs * this = malloc(sizeof(struct vfs));
    if (this) {
        this->vtable = &vtable;
        this->root = NULL;
    }

    
    return this;
};

/* allocates memory for a new node with nulls */
struct node * new_node(const char* root_folder) {
    struct node * this = malloc(sizeof(struct node));
    if (this) {
        this->name = root_folder;
        this->next = NULL;
        this->child = NULL;
        this->parent = NULL;
    }
    return this;
}

/* releases the memory of the given node */
void destroy_node(struct node * this) {
    if (this) {
        free(this);
        
    }
}

/* recursive functions that releases all the memory given a root vfs pointer */
void memory_close_node (struct node * this) {
    if (this->child == NULL) {
        if (this->next == NULL) {
            destroy_node(this);
            return;
        }
        memory_close_node(this->next);
    }
    memory_close_node(this->child);
    
}


// ######### MEMORY GIVEN FUNCTIONS TO IMPLEMENT ##########

struct vfs* memory_vfs_open(enum vfs_type type, const char* root_folder) {
    struct vfs * this = new_vfs_struct();
    if (!this) {
        printf("error\n");
        return NULL;
    }
    this->root = new_node(root_folder);
//    printf("%s\n", this->vfs.root->name);
    return this;
    
}

void memory_vfs_close(struct vfs* root) {
    memory_close_node(root->root);
    if (root)
        free(root);
}

int memory_vfs_mkdir(struct vfs* root, const char* path) {
    int path_len = (int)strlen(path);
    int start = 0;
    printf("%s\n", path);
    for (int c =0; c <= path_len; ++c) {
        if ((path[c] == '/')|| (c==path_len)) {
            size_t len = c-start;
            char * dir = malloc(len*sizeof(char));
//            memcpy(dir, &path[start], len);
            strncpy(dir, &path[start], len);
//            printf("%lu\n", strlen(dir));
            printf("%s\n", dir);
            start = c+1;
        }
    }
//    printf("%lu\n", strlen(path));

    // root->root
    // 1 success 0 failure
    return 0;
}



struct vfile* memory_vfile_open(struct vfs* root, const char* file_name) {
    // TODO
    return NULL;
}

int memory_vfile_write(struct vfile* f, const char* data, size_t data_len) {
    // TODO
    return -1;
}

int memory_vfile_append(struct vfile* f, const char* data, size_t data_len) {
    // TODO
    return -1;
}

size_t memory_vfile_read(struct vfile* f, const char* data, size_t data_len) {
    // TODO
    return -1;
}

void memory_vfile_close(struct vfile* f) {
    // TODO
}
