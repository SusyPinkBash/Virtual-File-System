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

/* checks if the input is equal to a string */
int check_words(const char * dir, char * to_check, size_t len) {
    for (int i = 0; i<len; ++i) {
        if (dir[i] != to_check[i])
            return 0;
    }
    return 1;
}

/* copies a char data inside another char data with the given length without an end character*/
void * copy_data_no_end_char(char * to, const char * from, size_t len) {
    for (int i = 0; i < len-1; ++i) {
        to[i] = from[i];
    }
    return to;
}

/* copies a char data inside another char data with the given length and adds at the end the end character */
void * copy_data(char * to, const char * from, size_t len) {
    copy_data_no_end_char(to, from, len);
    to[len-1] = '\0';
    return to;
}

size_t get_length_without_slashes(const char *path) {
    size_t i =  (int) strlen(path)-1;
    while (path[i] == '/')
    {
        --i;
    }
    return i+1;
}


// ##### VFS #####

/* creates a vfs struct with its vtable and a null root node  */
struct vfs * new_vfs_memory_struct(enum vfs_type type) {
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
        copy_data(name, folder, len);
        this->name = name;
        this->next = NULL;
        this->child = NULL;
        this->vfile = NULL;
    }
    return this;
}

/* releases the memory of the given node */
void destroy_dir(struct directory * this) {
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
struct vfile * new_vfile_memory_struct(char * name, size_t len) {
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
        copy_data(title, name, len);
        this->name = title;
        this->data = NULL;
        this->next = NULL;
        this->open = 1;
        this->length = 0;
        this->cursor = 0;
        this->type = VFS_MEMORY;
        this->fd = -1;
        this->full_path = NULL;
    }
    
    return this;
};


/* releases the memory of the given file */
void destroy_vfile(struct vfile * file) {
    if (file) {
        free((void *)file->name);
        free((void *)file->data);
        if (file->full_path)
            free(file->full_path);
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

/* Initializes the virtual file system of type t starting from
   root_folder and returns the points to the root */
struct vfs* memory_vfs_open(enum vfs_type type, const char* root_folder) {
    struct vfs * this = new_vfs_memory_struct(type);
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

/* mkdir that creates only the last folder and returns 0 if the previous ones do not exist */
int memory_vfs_mkdir(struct vfs* root, const char* path) {
    if (!root)
        return 0;
    struct directory * current_dir = root->root;
    size_t path_len = (size_t)strlen(path);
    int start = 0;
    printf("called dir with path: %s\n", path);
    path_len = get_length_without_slashes(path);
    for (int c = 0; c <= path_len; ++c) {
        size_t len = c-start+1;
        if (c == 0 && path[c] == '/') {
            while (path[c+1] == '/') {
                c++;
            }
            start = c+1;
            continue;
        }
        else if (c==path_len) {
            char * dir = malloc(len*sizeof(char));
            copy_data(dir, &path[start], len);
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
        }
        else if (path[c] == '/') {
            char * dir = malloc(len*sizeof(char));
            copy_data(dir, &path[start], len);
            if (current_dir->child == NULL) {
                free(dir);
                return 0;
            }
            else {
                current_dir = current_dir->child;
                int caffe = 1;
                while (caffe) {
                    if (check_words(current_dir->name, dir,len)) {
                        caffe = 0;
                    }
                    else if (current_dir->next == NULL) {
                        free(dir);
                        return 0;
                    }
                    else
                        current_dir = current_dir->next;
                }
            }
            free (dir);
            while(path[c+1] == '/' && c < path_len)
            {
                ++c;
            }
            start = c+1;
            if (current_dir->name == NULL)
                printf("problem man\n");
        }
    }
    return 1;
}



/* uncomment if mkdir should create all folders and not just the last one */
/*
int memory_vfs_mkdir(struct vfs* root, const char* path) {
    struct directory * current_dir = root->root;
    int path_len = (int)strlen(path);
    int start = 0;
    for (int c = 0; c <= path_len; ++c) {
        if ((path[c] == '/')|| (c==path_len)) {
            size_t len = c-start+1;
            char * dir = malloc(len*sizeof(char));
            copy_data(dir, &path[start], len);
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
*/

/* uncomment if mkdir should create only the last folder */
struct vfile* memory_vfile_open(struct vfs* root, const char* file_name) {
    printf("called vfile with path: %s\n", file_name);
    if (root->root == NULL)
        return NULL;
    struct directory * current_dir = root->root;
    int path_len = (int)strlen(file_name);
    for (int c =0, start = 0; c <= path_len; ++c) {
        if (c == 0 && file_name[c] == '/') {
            while (file_name[c+1] == '/') {
                c++;
            }
            start = c + 1;
            continue;
        }
        else if (file_name[c] == '/') {
            size_t len = c-start+1;
            char * dir = malloc(len*sizeof(char));
            copy_data(dir, &file_name[start], len);
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
            
//            free(dir);
            while(file_name[c+1] == '/' && c < path_len)
            {
                ++c;
            }
            start = c +1;
        }
        else if (c==path_len) {
            size_t len = c-start+1;
            char * name = malloc((1+len)*sizeof(char));
            copy_data(name, &file_name[start], len+1);
            
            if (current_dir->vfile) {
                struct vfile * current_file = current_dir->vfile;
                int caffe = 1;
                while (caffe) {
                    if (check_words(current_file->name, name, len)) {
                       free(name);
                        current_file->open = 1;
//                        current_file->cursor = 0;
                        return current_file;
                    }
                   else if (current_file->next == NULL) {
                       caffe = 0;
                   }
                    else
                        current_file = current_file->next;
                }
                // create file and append to current_file
                struct vfile * brother_file = new_vfile_memory_struct(name, len);
                current_file->next = brother_file;
                free(name);
                printf("created %s at line %d\n", brother_file->name, __LINE__);
                return brother_file;
                
            }
            else {
                // no files, create one and add it to dir
                struct vfile * child_file = new_vfile_memory_struct(name, len);
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
    if ((!f) || (f->open == 0))
        return 0;
    if (!f->data) {
        char * buffer = malloc((data_len+1)*sizeof(char));
        copy_data(buffer, data, data_len+1);
        f->data = buffer;
        f->length = data_len;
        f->cursor = data_len;
    }
    else if (f->length > f->cursor + data_len) {
        char * buffer = f->data;
        copy_data_no_end_char(&buffer[f->cursor], data, data_len+1);
        f->data = buffer;
        f->cursor = f->cursor + data_len;
    }
    else {
        char * buffer = f->data;
        buffer = realloc(buffer, f->cursor + data_len+1);
        copy_data(&buffer[f->cursor], data, data_len+1);
        f->length = f->cursor + data_len;;
        f->cursor = f->length;
        f->data = buffer;
    }
    
    if (!f->data) {
        return 0;
    }
    return 1;
}

int memory_vfile_append(struct vfile* f, const char* data, size_t data_len) {
    if ((!f) || (f->open == 0))
        return 0;
    if (!f->data) {
        char * buffer = malloc((data_len+1)*sizeof(char));
        copy_data(buffer, data, data_len+1);
        f->data = buffer;
        f->length = data_len;
    }
    else {
        char * buffer = f->data;
        size_t len =f->length + data_len;
        buffer = realloc(buffer, len+1);
        copy_data(&buffer[f->length], data, data_len+1);
        f->data = buffer;
        f->length = len;
    }
    
    f->cursor = f->length;
    
    if (!f->data) {
        return 0;
    }
    return 1;
}

size_t memory_vfile_read(struct vfile* f, char* data, size_t data_len) {
    if ((!f) || (f->open == 0) || (!f->data))
        return 0;
    // TODO not sure what I have to do;
//    (IFEXPRESSION) ? (THENEXPR) : (ELSEEXPR);
    size_t to_read;
    (f->length > f->cursor + data_len) ? (to_read = data_len) : (to_read = f->length);
    copy_data_no_end_char(data, &f->data[f->cursor], to_read+1);
    f->cursor = f->cursor + to_read;
    
    return to_read;
}

void memory_vfile_close(struct vfile* f) {
    if (f) {
        f->open = 0;
        f->cursor = 0;
    }
}
