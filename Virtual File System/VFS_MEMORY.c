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


// ##### VFS #####


/* creates a vfs struct with its vtable and a null root node  */
struct vfs * new_vfs_struct() {
    /* vtable for the MEMORY_VFS vfs */
    static const struct VFS_vtable vtable = {
        .vfs_open = memory_vfs_open,
        .vfs_mkdir = memory_vfs_mkdir,
        .vfs_close = memory_vfs_close,
        .vfile_open =  memory_vfile_open,
    };
    
    /* allocats memory for the MEMORY_VFS struct */
    struct vfs * this = malloc(sizeof(struct vfs));
    if (this) {
        this->vtable = &vtable;
        this->root = NULL;
        this->type = VFS_MEMORY;
    }

    
    return this;
};

/* allocates memory for a new node with nulls */
struct directory * new_directory(const char* folder, size_t len) {
    struct directory * this = malloc(sizeof(struct directory));
    if (this) {
        char * name = malloc(len*sizeof(char));
        strncpy(name, folder, len);
        this->name = name;
        this->next = NULL;
        this->child = NULL;
        this->vfile = NULL;
    }
    return this;
}

/* releases the memory of the given node */
void destroy_dir(struct directory * this) {
    printf("freeing node: %s\n", this->name);
    if (this) {
        free((void *)this->name);
        free(this);
    }
}



/* recursive functions that releases all the memory given a root vfs pointer */
void memory_close_directory (struct directory * this) {
    if (this != NULL) {
        if (this->next) {
            memory_close_directory(this->next);
            this->next = NULL;
        }
        if (this->child) {
            memory_close_directory(this->child);
            this->child = NULL;
        }
        // TODO FILE
        if (this->vfile) {
            destroy_file_list(this->vfile);
            this->vfile = NULL;
        }
        destroy_dir(this);

    }
}

/* it creates a new directory given the parent node and the dir name */
struct directory * make_directory_child(struct directory * parent, char * dir_name, size_t len) {
    struct directory * child = new_directory(dir_name, len);
    parent->child = child;
    return child;
}
struct directory * make_directory_brother(struct directory * before, char * dir_name, size_t len) {
    struct directory * brother = new_directory(dir_name, len);
    before->next = brother;
    return brother;
}


// ##### VFILE #####
/* creates a vfile struct with its vtable  */
struct vfile * new_vfile_struct(char * name, size_t len) {
    /* vtable for the MEMORY_VFS vfs */
    static const struct VFILE_vtable vtable = {
        .vfile_write = memory_vfile_write,
        .vfile_append = memory_vfile_append,
        .vfile_read = memory_vfile_read,
        .vfile_close = memory_vfile_close,
    };
    
    /* allocats memory for the MEMORY_VFS struct */
    struct vfile * this = malloc(sizeof(struct vfile));

    if (this) {
        this->vtable = &vtable;
        char * title = malloc(len*sizeof(char));
        strncpy(title, name, len);
        this->name = title;
        this->data = NULL;
        this->next = NULL;
        this->type = VFS_MEMORY;
        
    }
    
    return this;
};


/* releases the memory of the given file */
void destroy_vfile(struct vfile * file) {
    if (file) {
        free((void *)file->name);
        free((void *)file->data);
        free(file);
    }
}


/* recursive functions that releases all the memory given a vfile pointer */
void destroy_file_list (struct vfile * this) {
    if (this->next != NULL) {
        destroy_file_list(this->next);
        this->next = NULL;
    }
    
    destroy_vfile(this);
}



// ######### MEMORY GIVEN FUNCTIONS TO IMPLEMENT ##########

struct vfs* memory_vfs_open(enum vfs_type type, const char* root_folder) {
    struct vfs * this = new_vfs_struct();
    if (!this) {
        printf("error\n");
        return NULL;
    }
    this->root = new_directory(root_folder, strlen(root_folder));
    
    return this;
    
}

void memory_vfs_close(struct vfs* root) {
    memory_close_directory(root->root);
    if (root) {
        free(root);
    }
}

int memory_vfs_mkdir(struct vfs* root, const char* path) {
//    printf("called with path: %s\n", path);
//    if (root->type != VFS_MEMORY)
//        return 0;
    struct directory * current_dir = root->root;
    int path_len = (int)strlen(path);
    int start = 0;
    for (int c = 0; c <= path_len; ++c) {
        if ((path[c] == '/')|| (c==path_len)) {
            size_t len = c-start;
            char * dir = malloc(len*sizeof(char));
            // memcpy(dir, &path[start], len);
            strncpy(dir, &path[start], len);
//            printf("dir: %s\n", dir);
            if (current_dir->child == NULL) {
                current_dir = make_directory_child(current_dir, dir, len);
//                printf("Line: %d\n", __LINE__);
                printf("created: %s\n", current_dir->name);
                if (!current_dir){
                    printf("error\n");
                    free(dir);
                    return 0;
                }
            }
            else {
                current_dir = current_dir->child;
                int caffe = 1;
                while (caffe) {
                    if (!strcmp(current_dir->name, dir)) {
                        // dir already exists
//                        printf("Line: %d\n", __LINE__);
                        caffe = 0;
                    }
                    else if (current_dir->next == NULL) {
                        current_dir = make_directory_brother(current_dir, dir, len);
//                         printf("Line: %d\n", __LINE__);
                        printf("created: %s\n", current_dir->name);
                        if (!current_dir){
                            printf("error\n");
                            free(dir);
                            return 0;
                        }
                        caffe = 0;
                    }
                    else
                        current_dir = current_dir->next;
                }
            }
            free (dir);
            start = c+1;
            if (current_dir->name == NULL)
                printf("problem man\n");
        }
    }
//    memory_print_vfs(root);
    return 1;
}



struct vfile* memory_vfile_open(struct vfs* root, const char* file_name) {
    printf("called with: %s\n", file_name);
    if (root->root == NULL)
        return NULL;
    struct directory * current_dir = root->root;
    int path_len = (int)strlen(file_name);
    int start = 0;
    for (int c =0; c <= path_len; ++c) {
        if (file_name[c] == '/') {
            size_t len = c-start;
            char * dir = malloc(len*sizeof(char));
            strncpy(dir, &file_name[start], len);
            if (current_dir->child == NULL) {
                free(dir);
                printf("returnin from %d\n", __LINE__);
                return NULL;
            }
            else {
                current_dir = current_dir->child;
                int caffe = 1;
                while (caffe) {
                    if (!strcmp(current_dir->name, dir)) {
//                        printf("Line: %d\n", __LINE__);
                        caffe = 0;
                    }
                    else if (current_dir->next == NULL) {
                        free(dir);
                        printf("returnin from %d\n", __LINE__);
                        return NULL;
                    }
                    else
                        current_dir = current_dir->next;
                }
            }
            
            free(dir);
            start = c +1;
//            printf("the current directorys is: %s\n", current_dir->name);
        }
        else if (c==path_len) {
            size_t len = c-start;
            char * name = malloc(len*sizeof(char));
            strncpy(name, &file_name[start], len);
            
            if (current_dir->vfile) {
                struct vfile * current_file = current_dir->vfile;
                int caffe = 1;
                while (caffe) {
                   if (!strcmp(current_file->name, name)) {
                       free(name);
                       return current_file;
                    }
                   else if (current_file->next == NULL) {
                       caffe = 0;
                   }
                    else
                        current_file = current_file->next;
                }
                // create file and append to current_file
                struct vfile * brother_file = new_vfile_struct(name, len);
                current_file->next = brother_file;
                free(name);
                printf("created %s\n", brother_file->name);
                return brother_file;
                
            }
            else {
                // no files, create one and add it to dir
                struct vfile * child_file = new_vfile_struct(name, len);
                current_dir->vfile = child_file;
                free(name);
                printf("created %s at line %d\n", child_file->name, __LINE__);
                return child_file;
            }
            
        }
    }
    
    printf("returnin from  %d\n", __LINE__);
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
