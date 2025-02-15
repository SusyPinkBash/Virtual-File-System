//
//  VFS_MEMORY.h
//  Virtual File System
//
//  Created by Susanna Ardigo' on 07.05.18.
//  Copyright © 2018 Susanna Ardigo'. All rights reserved.
//

#ifndef VFS_MEMORY_h
#define VFS_MEMORY_h

#include <stdio.h>

#ifndef VFS_H_INCLUDED
#include "vfs.h"
#endif /* included vfs */

#include "VFS_MEMORY.h"

struct VFS_MEMORY;

struct vfs* memory_vfs_open(enum vfs_type type, const char* root_folder);
int memory_vfs_mkdir(struct vfs* root, const char* path);
void memory_vfs_close(struct vfs* root);
struct vfile* memory_vfile_open(struct vfs* root, const char* file_name);
int memory_vfile_write(struct vfile* f, const char* data, size_t data_len);
int memory_vfile_append(struct vfile* f, const char* data, size_t data_len);
size_t memory_vfile_read(struct vfile* f, char* data, size_t data_len);
void memory_vfile_close(struct vfile* f);

// other functions declarations
int check_words(const char * dir, char * to_check, size_t len);
void * copy_data_no_end_char(char * to, const char * from, size_t len);
void * copy_data(char * to, const char * from, size_t len);
size_t get_length_without_slashes(const char *path);
struct directory * new_directory(const char* folder, size_t len);
struct directory * make_directory_child(struct directory * parent, char * dir_name, size_t len);
struct directory * make_directory_brother(struct directory * before, char * dir_name, size_t len);
void destroy_file_list (struct vfile * this);
void destroy_vfile(struct vfile * file);



#endif /* VFS_MEMORY_h */
