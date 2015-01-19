#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "resolve.h"
#include "dns.h"
#include "manager.h"
#include "misc.h"
#include "label.h"

dns_node *root_node;

dns_node *find_node (dns_node *root, dns_label *name, uint8_t mode, dns_label **partial)
{
	int i;
	dns_node *res = NULL;
	
	for (i = 0; i < root->n_down; i++) {
		if (!strcasecmp (root->down[i]->label, name->label)) {
			if (!name->down) {
				res = root->down[i];
				break;
			}
			else {
				res = find_node (root->down[i], name->down, mode, partial);
				if (partial && !res) {
					*partial = name;
					res = root->down[i];
					break;
				}
			}
		}
	}
	
	return res;
}

dns_resource_record *resolve_name (dns_label *name, dns_answer *answer, uint16_t *n)
{
	dns_node *find = find_node (root_node, dns_label_head (name), FIND_TOTAL, NULL);
	if (find) {
		*n = find->n_records;
		return find->records;
	}
	*n = 0;
	return NULL;
}

void free_record (dns_resource_record *r)
{
	if (r->name)
		free (r->name);
	if (r->body) {
		if (r->body->rdata)
			free (r->body->rdata);
		free (r->body);
	}
}

void free_all_nodes (dns_node *root)
{
	int i;
	if (root) {
		if (root->label)
			free (root->label);
		if (root->down) {
			for (i = 0; i < root->n_down; i++)
				free_all_nodes (root->down[i]);
			free (root->down);
		}
		if (root->records) {
			for (i = 0; i < root->n_records; i++)
				free_record (&root->records[i]);
			free (root->records);
		}
		free (root);
	}
}

void add_record (char *name, int type, int class, char *content)
{
	dns_label *partial = NULL;
	dns_label *label = dns_label_from_string (name);
	dns_node *x = find_node (root_node, label->down, FIND_PARTIAL, &partial);
	dns_node *new = NULL;
	dns_resource_record *rr;
	
	if (!x)
		x = root_node;
	
	if (!partial)
		partial = label->down;
	else if (partial->down)
		partial = partial->down;
	
	while (partial) {
		new = create_node (partial->label);
		add_node (x, new);
		partial = partial->down;
		x = new;
	}
	
	rr = new_rr (name, type, class, content);
	insert_rr_in_node (x, rr);	
	
	free (rr);
	free_dns_name (label);
}

void add_node (dns_node *parent, dns_node *child)
{	
	if (!parent)
		return;
	
	parent->down = realloc (parent->down, (++parent->n_down + 1) * sizeof (dns_node));
	parent->down[parent->n_down - 1] = child;
	parent->down[parent->n_down] = NULL;
	child->up = parent;
	return;
}

dns_node *create_node (char *label)
{
	dns_node *new = malloc (sizeof (dns_node));
	
	new->up = NULL;
	new->down = NULL;
	new->n_down = 0;
	new->n_records = 0;
	new->records = NULL;
	
	if (label) {
		new->label = malloc (strlen (label) + 1);
		strcpy (new->label, label);
	}
	else
		new->label = NULL;
		
	return new;
}

dns_resource_record *new_rr (char *name, int type, int class, char *content)
{
	dns_resource_record *rr;
	unsigned int len = 0;
	
	rr = malloc (sizeof (dns_resource_record));
	rr->body = malloc (sizeof (dns_resource_record_body));
	
	rr->name = dns_string_from_char (name);
	rr->body->class = class;
	rr->body->type = type;
	rr->body->ttl = htons (400);

	if (content) {
		switch (type) {
		case DNS_TYPE_A:
			rr->body->rdata = str2ip (content);
			len = 4;
			break;
		case DNS_TYPE_CNAME:
			rr->body->rdata = (char *)dns_string_from_char (content);
			len = strlen (rr->body->rdata)+1;
			break;
		default:
			printf ("Not implemented\n");
		}
	}
	else
		rr->body->rdata = NULL;
		
	rr->body->rdlength = htons (len);
	
	return rr;
}

dns_string *dns_string_from_char (char *str)
{
	char *res = NULL;
	uint8_t len = 0;
	uint32_t total = 0;	
	uint32_t s_len = strlen (str), i = 0, j = 0;
	
	while (i < s_len) {
		while (i < s_len && str[i] && str[i] != '.')
			i++;
		len = i - j;
		res = realloc (res, total + len + 1);
		((dns_string *) (res + total))->len = len;
		memcpy (&res[total+1], &str[j], len);
		total += len + 1;
		j = ++i;
	}
	res = realloc (res,++total);
	res[total-1] = 0x00;
	return (dns_string *) res;
}

void insert_rr_in_node (dns_node *node, dns_resource_record *rr)
{
	if (!node || !rr)
		return;
		
	node->records = realloc (node->records, ++node->n_records * sizeof (dns_resource_record));
	memcpy (&node->records[node->n_records - 1], rr, sizeof (dns_resource_record));
}
