#include <pthread.h>
#include <src/util/color.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://github.com/angrave/SystemProgramming/wiki/Synchronization%2C-Part-1%3A-Mutex-Locks
typedef struct MutexList {
  char *project_name;
  bool is_locked;
  pthread_mutex_t *lock;

  struct MutexList *next;
} MutexList;

MutexList *list = NULL;

MutexList *mutexlist_new() { return calloc(1, sizeof(MutexList)); }

bool add_project_mutex(char *project_name) {
  MutexList *curr = list;
  MutexList *prev = NULL;

  while (curr != NULL) {
    if (strcmp(curr->project_name, project_name) == 0) {
      printf(BRED "[Mutex Error]" RESET
                  " Can't add a project that's already been added\n");
      return false;
    }

    prev = curr;
    curr = curr->next;
  }

  // Create MutexList node
  MutexList *node = mutexlist_new();

  pthread_mutex_t *lock = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(lock, NULL);
  node->lock = lock;

  node->is_locked = false;
  node->project_name = strdup(project_name);

  // Add to MutexList
  printf(BWHT "[Mutex]" RESET " Adding project '%s' to mutex list\n",
         project_name);

  if (prev == NULL) {
    list = node;
    return true;
  }

  prev->next = node;
  return true;
}

bool remove_project_mutex(char *project_name) {
  MutexList *curr = list;
  MutexList *prev = NULL;

  while (curr != NULL) {
    if (strcmp(curr->project_name, project_name) == 0) {
      printf(BWHT "[Mutex]" RESET " Removing project '%s' from mutex list\n",
             project_name);

      if (curr->is_locked) {
        printf(BWHT "[Mutex]" RESET " Unlock project '%s' before removing\n",
               project_name);
        return false;
      }

      // TODO(Sam): Free and unlock removed item
      if (prev == NULL) {
        list = list->next;
        return true;
      }

      prev->next = curr->next;
      return true;
    }

    prev = curr;
    curr = curr->next;
  }

  printf(BRED "[Mutex Error]" RESET
              " Can't remove a project that doesn't exist\n");
  return false;
}

bool lock_project(char *project_name) {
  MutexList *curr = list;

  while (curr != NULL) {
    if (strcmp(curr->project_name, project_name) == 0) {
      if (curr->is_locked) {
        printf(BRED "[Mutex Error]" RESET
                    " Project '%s' is locked, waiting for it to unlock\n",
               project_name);
      }

      pthread_mutex_lock(curr->lock);
      curr->is_locked = true;

      printf(BWHT "[Mutex]" RESET " Locking project '%s'\n", project_name);

      return true;
    }

    curr = curr->next;
  }

  return false;
}

bool unlock_project(char *project_name) {
  MutexList *curr = list;

  while (curr != NULL) {
    if (strcmp(curr->project_name, project_name) == 0) {
      if (!curr->is_locked) {
        printf(BRED "[Mutex Error]" RESET
                    " Can't unlock a project that is already unlocked\n");
        return false;
      }

      pthread_mutex_unlock(curr->lock);
      curr->is_locked = false;

      printf(BWHT "[Mutex]" RESET " Unlocking project '%s'\n", project_name);

      return true;
    }

    curr = curr->next;
  }

  return false;
}