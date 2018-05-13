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
                // TODO physically create dir
                size_t root_path_len = (size_t)strlen(root->root->name);
                char * full_path = malloc((root_path_len + path_len+2)*sizeof(char));
                copy_data_no_end_char(full_path, root->root->name, root_path_len+1);
                full_path[root_path_len] = '/';
                copy_data(&full_path[root_path_len+1], path, path_len+1);
                int success = mkdir(full_path, S_IRWXU | S_IRGRP | S_IROTH);
                free(full_path);
                if (!current_dir || success != 0){
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
                        // TODO physically create dir
                        size_t root_path_len = (size_t)strlen(root->root->name);
                        char * full_path = malloc((root_path_len + path_len+2)*sizeof(char));
                        copy_data_no_end_char(full_path, root->root->name, root_path_len+1);
                        full_path[root_path_len] = '/';
                        copy_data(&full_path[root_path_len+1], path, path_len+1);
                        int success = mkdir(full_path, S_IRWXU | S_IRGRP | S_IROTH);
                        free(full_path);
                        printf("created: %s\n", current_dir->name);
                        if (!current_dir || success != 0){
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
