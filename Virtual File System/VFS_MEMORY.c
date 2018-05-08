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

/* checks if the input is equal to a string */
int check_input(char * input, char * check) {
    int len = 6;
    if (!strcmp(check, "draw") || !strcmp(check, "rook") || !strcmp(check, "none"))
        len = 4;
    if (!strcmp(check, "queen"))
        len = 5;
    for (int i=0; i<len; ++i) {
        if (input[i] != check[i])
            return 0;
    }
    return 1;
}


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
//        this->parent = NULL;
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

/* it creates a new directory given the parent node and the dir name */
struct node * make_directory_child(struct node * parent, char * dir_name) {
    struct node * child = new_node(dir_name);
    parent->child = child;
    return child;
}
struct node * make_directory_brother(struct node * brother, char * dir_name) {
    struct node * child = new_node(dir_name);
    brother->next = child;
    return child;
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
    struct node * current_dir = root->root;
    int path_len = (int)strlen(path);
    int start = 0;
//    printf("path: %s\n", path);
    for (int c =0; c <= path_len; ++c) {
        if ((path[c] == '/')|| (c==path_len)) {
            size_t len = c-start;
            char * dir = malloc(len*sizeof(char));
//            memcpy(dir, &path[start], len);
            strncpy(dir, &path[start], len);
//            printf("dir: %s\n", dir);
            
            
            if (current_dir->child == NULL) {
                current_dir = make_directory_child(current_dir, dir);
                if (!current_dir)
                    return 0;
//                printf("from if: %s\n", current_dir->name);
            }
            else {
                struct node * child_dir = current_dir->child;
                int caffe = 1;
                while (caffe) {
                    if (!strcmp(child_dir->name, dir)) {
                        // already exists
                        current_dir = child_dir;
//                        printf("exists already: %s\n", current_dir->name);
                        caffe = 0;
                    }
                    else if (child_dir->next == NULL) {
                        current_dir = make_directory_brother(child_dir, dir);
                        if (!current_dir)
                            return 0;
//                        printf("from else if: %s\n", current_dir->name);
                        caffe = 0;
                    }
                    else
                        child_dir = child_dir->next;
                }
            }
            start = c+1;
        }
    }


    return 1;
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
