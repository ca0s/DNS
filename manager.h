#ifndef __DNS_MANAGER_H__
#define __DNS_MANAGER_H__

#include "dns.h"
#include "label.h"

void manage_query (char *, dns_answer *, int);
unsigned int build_answer (dns_answer *, char *);
uint16_t append_label (char *, char *, uint16_t *);
void set_answer_flag (dns_answer *, uint16_t);
void add_record_to_answer (dns_answer *, dns_resource_record *);
uint8_t inline is_offset (uint8_t);
int copy_dns_string (char *, dns_string *, char *);
int copy_rr_body (char *, dns_resource_record_body *, char *);
void *end_of_dns_string (dns_string *);
void fix_endianness (dns_answer *);
void free_answer (dns_answer *);

#endif
