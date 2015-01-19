#ifndef __DNS_RESOLVE_H__
#define __DNS_RESOLVE_H__

#include "dns.h"
#include "label.h"

#define FIND_TOTAL		0
#define FIND_PARTIAL	1

struct dns_node_s {
	char *label;
	int n_records;
	dns_resource_record *records;
	struct dns_node_s *up;
	int n_down;
	struct dns_node_s **down;
};
typedef struct dns_node_s dns_node;

void add_record (char *, int, int, char *);
dns_node *find_node (dns_node *, dns_label *, uint8_t, dns_label **);
dns_resource_record *resolve_name (dns_label *, dns_answer *, uint16_t *);
void add_all_nodes ();
void free_all_nodes ();
void add_node (dns_node *, dns_node *);
dns_node *create_node (char *);
dns_string *dns_string_from_char (char *);
dns_resource_record *new_rr (char *, int, int, char *);
void insert_rr_in_node (dns_node *, dns_resource_record *);

#endif
