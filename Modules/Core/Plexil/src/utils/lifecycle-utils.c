/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lifecycle-utils.h"

#include <stddef.h> /* for NULL */
#include <stdlib.h> /* malloc(), free() */
#include <string.h> /* memset() */

/* #define LIFECYCLE_DEBUG 1 */

#ifdef LIFECYCLE_DEBUG
#include <stdio.h> /* puts(), printf() */
#endif


/*
 * An opstack is a stack, implemented as a linked list of buckets.
 * Buckets in the list are never empty.
 * insert_idx should only be 0 when list is empty or during allocation of a bucket. 
 * New buckets are allocated by opstack_push when the head has no slots free.
 */

/* Each bucket should fit in a CPU cache line */
#define PLEXIL_OPSTACK_BUCKET_SIZE 7

struct plexil_opstack_bucket {
  struct plexil_opstack_bucket *prev;
  lc_operator ops[PLEXIL_OPSTACK_BUCKET_SIZE];
};

struct plexil_opstack {
  struct plexil_opstack_bucket *head; /* pointer to most recent bucket */
  size_t insert_idx;           /* index of first open slot in bucket; range 1 - PLEXIL_OPSTACK_BUCKET_SIZE */
#ifdef LIFECYCLE_DEBUG
  size_t n_buckets;           /* total # of buckets */
#endif
};

/* Allocate and initialize a bucket */
static struct plexil_opstack_bucket *new_plexil_opstack_bucket()
{
  struct plexil_opstack_bucket *result = 
    (struct plexil_opstack_bucket *) malloc(sizeof(struct plexil_opstack_bucket));
  memset((void *) result, 0, sizeof(struct plexil_opstack_bucket));
  return result;
}

static void plexil_opstack_push(struct plexil_opstack *list, lc_operator op)
{
  struct plexil_opstack_bucket *head;
  /* Ensure there is space for the new entry */
  if ((head = list->head) == NULL) {
    head = list->head = new_plexil_opstack_bucket();
    list->insert_idx = 0;
#ifdef LIFECYCLE_DEBUG
    list->n_buckets = 1;
#endif
  }
  else if (list->insert_idx == PLEXIL_OPSTACK_BUCKET_SIZE) {
    /* Bucket full, allocate new bucket */
    struct plexil_opstack_bucket *old_head = list->head;
    head = new_plexil_opstack_bucket();
    head->prev = old_head;
    list->head = head;
    list->insert_idx = 0;
#ifdef LIFECYCLE_DEBUG
    list->n_buckets++;
#endif
  }

  /* Insert */
  head->ops[list->insert_idx++] = op;
}

static void plexil_opstack_run(struct plexil_opstack *list)
{
#ifdef LIFECYCLE_DEBUG
  if (!list->head)
    puts("plexil_opstack_run, empty list\n");
  else 
    printf("plexil_opstack_run, list has %u entries\n",
           list->n_buckets * PLEXIL_OPSTACK_BUCKET_SIZE + list->insert_idx - 1);
#endif
  struct plexil_opstack_bucket *head = list->head;
  size_t i = list->insert_idx;
  while (head != NULL) {
#ifdef LIFECYCLE_DEBUG
    if (i == 0) {
      puts("plexil_opstack_run: ERROR: insert_idx is 0 at top of loop\n");
    }
#endif
    while (i > 0) {
      lc_operator op = head->ops[--i];
      (*op)();
    }
    /* Free this bucket and go to previous */
    list->head = head->prev;
    free(head);
    head = list->head;
    i = PLEXIL_OPSTACK_BUCKET_SIZE;
  }
  /* At end */
  list->head = NULL;
  list->insert_idx = 0;
#ifdef LIFECYCLE_DEBUG
  list->n_buckets = 0;
  puts("plexil_opstack_run finished\n");
#endif
}

static struct plexil_opstack s_finalizers = {NULL,
                                      0
#ifdef LIFECYCLE_DEBUG
                                      , 0
#endif                             
};

void plexilAddFinalizer(lc_operator op)
{
  plexil_opstack_push(&s_finalizers, op);
}

void plexilRunFinalizers()
{
  plexil_opstack_run(&s_finalizers);
}
