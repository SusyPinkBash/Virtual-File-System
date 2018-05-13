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
#include "stdio.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

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

char * get_full_path(const char * root_name, const char * path) {
    size_t root_path_len = (size_t)strlen(root_name);
    size_t path_len = (size_t)strlen(path);
    char * full_path = malloc((root_path_len + path_len+2)*sizeof(char));
    copy_data_no_end_char(full_path, root_name, root_path_len+1);
    full_path[root_path_len] = '/';
    copy_data(&full_path[root_path_len+1], path, path_len+1);
    return full_path;
}

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
/* creates a vfile struct with its vtable  */
struct vfile * new_vfile_disk_struct(char * name, size_t len, int fd, char * full_path) {
    /* vtable for the MEMORY_VFS vfs */
    static const struct VFILE_vtable vtable = {
        .vfile_write = disk_vfile_write,
        .vfile_append = disk_vfile_append,
        .vfile_read = disk_vfile_read,
        .vfile_close = disk_vfile_close,
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
        this->fd = fd;
        this->full_path = full_path;
    }
    
    return this;
};

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
        if (success != 0 && opendir(root_folder) == NULL) {
            free(this);
            return NULL;
        }
//        if (success != 0) {
//            free(this);
//            return NULL;
//        }
    return this;
}

int disk_vfs_mkdir(struct vfs* root, const char* path) {
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
                // physically create dir
                char * full_path = get_full_path(root->root->name, path);
                int success = mkdir(full_path,S_IRWXU | S_IRGRP | S_IROTH);
                if (success != 0 && opendir(full_path) == NULL) {
                    free(dir);
                    return 0;
                }
                free(full_path);
                if (!current_dir){
                    printf("error\n");
                    free(dir);
                    return 0;
                }
                printf("created: %s\n", current_dir->name);
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
                        // physically create dir
                        char * full_path = get_full_path(root->root->name, path);
                        int success = mkdir(full_path,S_IRWXU | S_IRGRP | S_IROTH);
                        if (success != 0 && opendir(full_path) == NULL) {
                            free(dir);
                            return 0;
                        }
                        free(full_path);
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

void disk_vfs_close(struct vfs* root) {
    memory_vfs_close(root);
}

struct vfile* disk_vfile_open(struct vfs* root, const char* file_name) {
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
                        // current_file->cursor = 0;
                        // TODO open existing file
                        int fd = open(current_file->full_path, O_RDWR);
                        if (fd < 0)
                            return NULL;
                        current_file->fd = fd;
                        
                    return current_file;
                    }
                    else if (current_file->next == NULL) {
                        caffe = 0;
                    }
                    else
                        current_file = current_file->next;
                }
                // create file and append to current_file
                // TODO Physically create file
                char * full_path = get_full_path(root->root->name, file_name);
//                int file = creat(full_path, S_IRWXU | S_IRGRP | S_IROTH);
                int file = open(full_path, O_RDWR | O_CREAT , S_IRWXU | S_IRGRP | S_IROTH);
                if (file < 0) {
                    free(name);
                    free(full_path);
                    return NULL;
                }
                struct vfile * brother_file = new_vfile_disk_struct(name, len, file, full_path);
                current_file->next = brother_file;
                free(name);
                printf("created %s at line %d\n", brother_file->name, __LINE__);
                return brother_file;
                
            }
            else {
                // no files, create one and add it to dir
                
                // TODO physically create file
                char * full_path = get_full_path(root->root->name, file_name);
//                int file = creat(full_path, S_IRWXU | S_IRGRP | S_IROTH);
                int file = open(full_path, O_RDWR | O_CREAT , S_IRWXU | S_IRGRP | S_IROTH);
                if (file < 0) {
                    free(name);
                    free(full_path);
                    return NULL;
                }
                struct vfile * child_file = new_vfile_disk_struct(name, len, file, full_path);
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

int disk_vfile_write(struct vfile* f, const char* data, size_t data_len) {
    if ((!f) || (f->open == 0) || (!f->fd))
        return 0;
    ssize_t success;
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
    success = write(f->fd, data, data_len);
    if (!f->data || success < 0) {
        return 0;
    }
    
    return 1;
}

int disk_vfile_append(struct vfile* f, const char* data, size_t data_len) {
    if ((!f) || (f->open == 0) || (!f->fd))
        return 0;
    ssize_t success;
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
    if (f->cursor != f->length) {
        close(f->fd);
        int fd = open(f->full_path, O_APPEND | O_RDWR);
        if (fd < 0)
            return 0;
        f->fd = fd;
    }
    f->cursor = f->length;
    
    success = write(f->fd, data, data_len+1);
    if (!f->data || success < 0) {
        return 0;
    }
    return 1;
}

size_t disk_vfile_read(struct vfile* f, char* data, size_t data_len) {
    if ((!f) || (!f->fd))
        return 0;
    // TODO not sure what I have to do;
    //    (IFEXPRESSION) ? (THENEXPR) : (ELSEEXPR);
    ssize_t success;
    size_t to_read;
//    (f->length > f->cursor + data_len) ? (to_read = data_len) : (to_read = f->length);
    (f->length > f->cursor + data_len) ? (to_read = data_len) : (to_read = f->length);
    
//    copy_data_no_end_char(data, &f->data[f->cursor], to_read+1);
    success = read(f->fd, data, to_read+1);
    if (success < 0)
        return 0;
//    f->cursor = f->cursor + to_read;
    
    return to_read;
}

void disk_vfile_close(struct vfile* f) {
    close(f->fd);
    memory_vfile_close(f);
}
