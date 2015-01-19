#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "dns.h"
#include "manager.h"
#include "resolve.h"
#include "label.h"

void manage_query (char *query, dns_answer *answer, int len)
{
	dns_header *header = (dns_header *) query;
	dns_question_section question;
	dns_label *root = NULL;
	dns_resource_record *res;
	
	uint16_t j, n_records, n_questions;
	
	question.qname = (dns_string *) (query + sizeof (dns_header));
	n_questions = ntohs (header->qdcount);
	
	answer->header.id = header->id;
	answer->header.ancount = ntohs (header->ancount);
	answer->header.arcount = 0;
	answer->header.qdcount = ntohs (header->qdcount);
	answer->header.nscount = ntohs (header->nscount);
	
	answer->question.qname = question.qname;
	answer->question.body = end_of_dns_string (question.qname);
	
	if (n_questions >= 1) {
		root = name_from_dns_string (question.qname);
		if(root) {
			res  = resolve_name (root, answer, &n_records);
			for (j = 0; j < n_records; j++) {
				add_record_to_answer (answer, res);
			}
			free_dns_name (dns_label_head (root));
		}
	}
	
	return;
}

unsigned int build_answer (dns_answer *answer, char *buffer)
{
	int i;
	dns_resource_record **rr;
	char *ini = buffer;
	
	answer->header.flags = 0;
	set_answer_flag (answer, DNS_FLAGS_RCODE_OK);
	set_answer_flag (answer, DNS_FLAGS_AA);
	set_answer_flag (answer, DNS_FLAGS_QR);
	
	// Al final haremos unos arreglos, no lo copiamos aun
	buffer += sizeof (dns_header);
	
	// Questions
	buffer += copy_dns_string (buffer, answer->question.qname, ini);
	memcpy (buffer, answer->question.body, sizeof (struct dns_question_section_body_s));
	buffer += sizeof (struct dns_question_section_body_s);
	
	rr = answer->answers;
	for (i = 0; ((i < answer->header.ancount) && rr && rr[i]); i++, rr++) {
		buffer += copy_dns_string (buffer, rr[i]->name, ini);
		buffer += copy_rr_body (buffer, rr[i]->body, ini);
	}
	
	rr = answer->authorities;
	for (i = 0; ((i < answer->header.nscount) && rr && rr[i]); i++, rr++) {
		buffer += copy_dns_string (buffer, rr[i]->name, ini);
		buffer += copy_rr_body (buffer, rr[i]->body, ini);		
	}
	
	rr = answer->aditionals;
	for (i = 0; ((i < answer->header.arcount) && rr && rr[i]); i++, rr++) {
		buffer += copy_dns_string (buffer, rr[i]->name, ini);
		buffer += copy_rr_body (buffer, rr[i]->body, ini);		
	}
	
	fix_endianness (answer);
	memcpy (ini, &answer->header, sizeof (dns_header));
	
	return buffer - ini;
}

void inline set_answer_flag (dns_answer *answer, uint16_t flag)
{
	answer->header.flags |= flag;
}

void add_record_to_answer (dns_answer *answer, dns_resource_record *record)
{
	answer->header.ancount++;
	answer->answers = realloc (answer->answers, answer->header.ancount * sizeof (dns_resource_record *));
	answer->answers[answer->header.ancount - 1] = record;
}

uint16_t append_label (char *string, char *dst, uint16_t *offset)
{
	dns_string *str = (dns_string *) string;
	if (str->len & 0xC0)
		return 0;
	if (str->len > 0) {
		strncat (dst, dns_string_buffer (str), str->len);
		strcat (dst, ".");
		*offset += (1 + str->len);
	}
	return str->len;
}

int copy_dns_string (char *dst, dns_string *str, char *ini)
{
	if (!str) 
		return 0;
		
	int maxlen = DNS_MAX_PACKET_SIZE - (dst - ini);

	int len = 0;
	while (str->len && !is_offset (str->len) && --maxlen) {
		memcpy (dst + len, str, str->len + 1);
		len += str->len + 1;
		str += str->len + 1;
	} 
	dst[len++] = 0x00;
	return len;
}

int copy_rr_body (char *dst, dns_resource_record_body *body, char *ini)
{
	int len = min (DNS_MAX_PACKET_SIZE - RR_BODY_MIN_SIZE - (dst - ini), ntohs (body->rdlength));
	
	memcpy (dst, body, RR_BODY_MIN_SIZE);
	dst += RR_BODY_MIN_SIZE;
	
	if (body->rdata)
		memcpy (dst, body->rdata, len);
		
	return RR_BODY_MIN_SIZE + len;
}

void *end_of_dns_string (dns_string *str)
{
	if (!str)
		return NULL;
	
	while (str->len && !is_offset (str->len))
		str += (str->len + 1);
	
	return str + 1;
}

void fix_endianness (dns_answer *answer)
{
	answer->header.ancount = htons (answer->header.ancount);
	answer->header.qdcount = htons (answer->header.qdcount);
	answer->header.nscount = htons (answer->header.nscount);
	answer->header.arcount = htons (answer->header.arcount);
	answer->header.flags = htons (answer->header.flags);
}

void free_answer (dns_answer *answer)
{
	if (answer) {
		if (answer->answers) {
			free (answer->answers);
			answer->answers = NULL;
		}
		if (answer->authorities) {
			free (answer->authorities);
			answer->authorities = NULL;
		}
		if (answer->aditionals) {
			free (answer->aditionals);
			answer->aditionals = NULL;
		}
	}
}
