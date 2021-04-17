/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_DIRENT_H
#define GOOD_UTIL_DIRENT_H

#include "general.h"
#include "tree.h"

/**
 * 目录扫描。
 * 
 * 扫描的结果会自动生成一个颗“树”。
 * 
 * @param depth 遍历深度。0 只遍历当前目录，>= 1 遍历多级目录。
 * @param onefs 0 不辨别文件系统是否相同，!0 只在同一个文件系统中遍历。
 * 
 * @note 如果目录和文件较多，则需要较多的内存。
 * 
 * @see good_tree_scan()
 * @see good_tree_fprintf()
 * @see good_tree_vfprintf()
 * 
*/
void good_dirscan(good_tree_t *father,size_t depth, int onefs);

#endif //GOOD_UTIL_DIRENT_H