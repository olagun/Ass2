#include <pthread.h>
#include <src/util/color.h>
#include <stdbool.h>
#include <string.h>

// https://github.com/angrave/SystemProgramming/wiki/Synchronization%2C-Part-1%3A-Mutex-Locks

#ifndef MUTEX_LIST_H
#define MUTEX_LIST_H

typedef struct MutexList {
  char *project_name;
  bool is_locked;
  pthread_mutex_t *lock;

  struct MutexList *next;
} MutexList;

MutexList *mutexlist_new();

bool *add_project_mutex(char *project_name);
bool *remove_project_mutex(char *project_name);

bool lock_project(char *project_name);
bool unlock_project(char *project_name);

#endif
