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
int check_words(const char * dir, char * to_check, size_t len) {
    for (int i = 0; i<len; ++i) {
        if (dir[i] != to_check[i])
            return 0;
    }
    return 1;
}

/* copies a string inside another string with the given length */
void * copy_string(char * to, const char * from, size_t len) {
    for (int i = 0; i < len-1; ++i) {
        to[i] = from[i];
    }
    to[len-1] = '\0';
    return to;
}

/* copies a char data inside another char data with the given length */
void * copy_data(char * to, char * from, size_t len) {
    for (int i = 0; i < len-1; ++i) {
        to[i] = from[i];
    }
    to[len-1] = EOF;
    return to;
}




// ##### VFS #####

/* creates a vfs struct with its vtable and a null root node  */
struct vfs * new_vfs_struct(enum vfs_type type) {
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
        this->type = type;
    }
    
    return this;
};

/* allocates memory for a new node with nulls */
struct directory * new_directory(const char* folder, size_t len) {
    struct directory * this = malloc(sizeof(struct directory));
    if (this) {
        char * name = malloc(len*sizeof(char));
        copy_string(name, folder, len);
//        strncpy(name, folder, len);
        this->name = name;
        this->next = NULL;
        this->child = NULL;
        this->vfile = NULL;
    }
    return this;
}

/* releases the memory of the given node */
void destroy_dir(struct directory * this) {
//    printf("freeing node: %s\n", this->name);
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

/* it creates a new directory given the parent node, the dir name and length */
struct directory * make_directory_child(struct directory * parent, char * dir_name, size_t len) {
    struct directory * child = new_directory(dir_name, len);
    parent->child = child;
    return child;
}

/* it creates a new directory given the brother node, the dir name and length */
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
        copy_string(title, name, len);
        this->name = title;
        this->data = NULL;
        this->next = NULL;
        this->length = 0;
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

/*  */
struct vfs* memory_vfs_open(enum vfs_type type, const char* root_folder) {
    struct vfs * this = new_vfs_struct(type);
    if (!this) {
        printf("error\n");
        return NULL;
    }
    this->root = new_directory(root_folder, (strlen(root_folder)+1));
    
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
            size_t len = c-start+1;
            char * dir = malloc(len*sizeof(char));
            copy_string(dir, &path[start], len);
            if (current_dir->child == NULL) {
                current_dir = make_directory_child(current_dir, dir, len);
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
                    if (check_words(current_dir->name, dir,len)) {
                        caffe = 0;
                    }
                    else if (current_dir->next == NULL) {
                        current_dir = make_directory_brother(current_dir, dir, len);
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
    return 1;
}



struct vfile* memory_vfile_open(struct vfs* root, const char* file_name) {
    if (root->root == NULL)
        return NULL;
    struct directory * current_dir = root->root;
    int path_len = (int)strlen(file_name);
    int start = 0;
    for (int c =0; c <= path_len; ++c) {
        if (file_name[c] == '/') {
            if (c == 0 && current_dir->child) {
                start = 1;
                continue;
            }
            size_t len = c-start+1;
            char * dir = malloc(len*sizeof(char));
            copy_string(dir, &file_name[start], len);
            if (current_dir->child == NULL) {
                free(dir);
                return NULL;
            }
            else {
                current_dir = current_dir->child;
                int caffe = 1;
                while (caffe) {
                    if (check_words(current_dir->name, dir, len)) {
                        caffe = 0;
                    }
                    else if (current_dir->next == NULL) {
                        free(dir);
                        return NULL;
                    }
                    else
                        current_dir = current_dir->next;
                }
            }
            
            free(dir);
            start = c +1;
        }
        else if (c==path_len) {
            size_t len = c-start+1;
            char * name = malloc((1+len)*sizeof(char));
            copy_string(name, &file_name[start], len+1);
            
            if (current_dir->vfile) {
                struct vfile * current_file = current_dir->vfile;
                int caffe = 1;
                while (caffe) {
                    if (check_words(current_file->name, name, len)) {
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
                printf("created %s at line %d\n", brother_file->name, __LINE__);
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
    if (!f)
        return 0;
    if (f->data)
        free((void*)f->data);
    char * buffer = malloc((data_len+1)*sizeof(char));
    copy_string(buffer, data, data_len+1);
    f->data = buffer;
    f->length = data_len;
    if (!f->data) {
        free(buffer);
        return 0;
    }
    return 1;
}

int memory_vfile_append(struct vfile* f, const char* data, size_t data_len) {
    if (!f)
        return 0;
    if (!f->data) {
        return memory_vfile_write(f, data, data_len);
    }
    char * buffer = f->data;
    size_t len =f->length + data_len;
    buffer = realloc(buffer, len+1);
    copy_string(&buffer[f->length], data, data_len+1);
    f->data = buffer;
    f->length = len;
    
    if (!f->data) {
        free(buffer);
        return 0;
    }
    return 1;
}

size_t memory_vfile_read(struct vfile* f, char* data, size_t data_len) {
    // TODO not sure what I have to do;
    return -1;
}

void memory_vfile_close(struct vfile* f) {
}
