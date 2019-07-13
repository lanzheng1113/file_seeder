/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   cptype.h
 * `cp` for cross-platform, this file define some data type from Windows.
 * Author: duocore
 *
 * Created on 2019年7月2日, 下午9:47
 */

#ifndef CPTYPE_H
#define CPTYPE_H

#ifndef DWORD
#define DWORD unsigned long
#endif // !DWORD

#ifndef MAX_PATH
#define MAX_PATH 256
#endif // !MAX_PATH

#ifndef INT64
#include <stdint.h>
#define INT64 uint64_t
#endif


#endif /* CPTYPE_H */

