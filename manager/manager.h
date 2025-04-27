//
// Created by Adam Hassani on 27/04/2025.
//

#ifndef MANAGER_H
#define MANAGER_H
#include <pthread.h>



void init_manager();
void* routine_manager(void* arg);

#endif //MANAGER_H
