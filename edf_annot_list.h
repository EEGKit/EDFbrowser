/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010 - 2021 Teunis van Beelen
*
* Email: teuniz@protonmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/


#ifndef EDFPLUS_ANNOT_LIST_H
#define EDFPLUS_ANNOT_LIST_H


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

#define MAX_ANNOTATION_LEN_II   512


#ifdef __cplusplus
extern "C" {
#endif

struct annotationblock{
        void *edfhdr;
        long long onset;
        char duration[16];
        long long long_duration;
        char description[MAX_ANNOTATION_LEN_II + 1];
        int modified;
        int x_pos;
        int selected;
        int jump;
        int hided;
        int hided_in_list;
        unsigned int ident;
        int selected_in_dock;
       };

struct annotation_list{
        struct annotationblock *items;
        int sz;
        int used_sz;
        int mem_sz;
        int *idx;
};


int edfplus_annotation_add_item(struct annotation_list *, struct annotationblock);
int edfplus_annotation_size(struct annotation_list *);
int edfplus_annotation_cnt(struct annotation_list *, struct annotationblock *);
void edfplus_annotation_empty_list(struct annotation_list *);
void edfplus_annotation_remove_item(struct annotation_list *, int);
struct annotationblock * edfplus_annotation_get_item(struct annotation_list *, int);
struct annotationblock * edfplus_annotation_get_item_visible_only(struct annotation_list *, int);
struct annotationblock * edfplus_annotation_get_item_visible_only_cached(struct annotation_list *, int, int *, int *);
int edfplus_annotation_get_index(struct annotation_list *, struct annotationblock *);
void edfplus_annotation_sort(struct annotation_list *, void (*)(void));
struct annotation_list * edfplus_annotation_create_list_copy(struct annotation_list *);
int edfplus_annotation_remove_duplicates(struct annotation_list *);
int edfplus_annotation_delete_multiple(struct annotation_list *, const char *);
int edfplus_annotation_rename_multiple(struct annotation_list *, const char *, const char *);
int edfplus_annotation_get_max_annotation_strlen(struct annotation_list *);
void edfplus_annotation_copy_list(struct annotation_list *, struct annotation_list *);
long long edfplus_annotation_get_long_from_number(const char *);
int edfplus_annotation_get_index_at(struct annotation_list *, long long, int);
void edfplus_annotation_cancel_all_selected_in_dock(struct annotation_list *);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif




