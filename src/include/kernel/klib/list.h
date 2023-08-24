#ifndef __list_dot_H
#define __list_dot_H

struct list_hdr {
	struct list_hdr *prev, *next;
};
typedef struct list_hdr list_t;
typedef list_t list_head_t;

#define for_each_in_list(head, i) \
	for((i) = (head); ((i) = (i)->next) != (head); )

#define list_empty(head) \
	((head).next == &(head))

#define init_list(head) \
	head.next = &(head);\
	head.prev = &(head)

#define append_to_list(head, node) \
	(node).prev = (head).prev; \
	(node).next = &(head); \
	(node).prev->next = &(node); \
	(node).next->prev = &(node);

#define remove_from_list(node) \
	(node).prev->next = (node).next; \
	(node).next->prev = (node).prev; \
	(node).next = &(node); \
	(node).prev = &(node)

#endif
