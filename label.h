#ifndef __DNS_LABEL_H__
#define __DNS_LABEL_H__

#include "dns.h"

struct dns_label_s {
	char *label;
	struct dns_label_s *up;
	struct dns_label_s *down;
};
typedef struct dns_label_s dns_label;

dns_label *name_from_dns_string (dns_string *);
dns_label *label_from_dns_string (dns_string *);
void print_name (dns_label *);
dns_label *dns_label_from_string (char *);
dns_label *dns_label_head (dns_label *);
dns_label *new_dns_label ();
void free_dns_name (dns_label *);

#endif
