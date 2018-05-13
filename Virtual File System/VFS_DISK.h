//
//  VFS_DISK.h
//  Virtual File System
//
//  Created by Susanna Ardigo' on 07.05.18.
//  Copyright © 2018 Susanna Ardigo'. All rights reserved.
//

#ifndef VFS_DISK_h
#define VFS_DISK_h

#include <stdio.h>

#ifndef VFS_MEMORY_h
#include "VFS_MEMORY.h"
#endif /* included vfs */

#include "VFS_impl.h"

struct VFS_DISK;


struct vfs* disk_vfs_open(enum vfs_type type, const char* root_folder);
int disk_vfs_mkdir(struct vfs* root, const char* path);
void disk_vfs_close(struct vfs* root);
struct vfile* disk_vfile_open(struct vfs* root, const char* file_name);
int disk_vfile_write(struct vfile* f, const char* data, size_t data_len);
int disk_vfile_append(struct vfile* f, const char* data, size_t data_len);
size_t disk_vfile_read(struct vfile* f, char* data, size_t data_len);
void disk_vfile_close(struct vfile* f);



#endif /* VFS_DISK_h */
