#ifndef __DNS_H__
#define __DNS_H__

#include <stdint.h>

#define E_FATAL 0
#define E_WARN	1

#define DEFAULT_PORT	53

#define DNS_MAX_PACKET_SIZE		512

#ifdef __BIG_ENDIAN__
	#define HTONS(a) (a)
#else
	#define HTONS(a) (((a & 0x00ff)<<8) | ((a & 0xff00)>>8))
#endif

#define DNS_FLAGS_QR			(1<<15)
#define DNS_FLAGS_OPCODE_STD	0
#define DNS_FLAGS_OPCODE_INV	(1<<11)
#define DNS_FLAGS_OPCODE_STS	(2<<11)
#define DNS_FLAGS_AA			(1<<10)
#define DNS_FLAGS_TC			(1<<9)
#define DNS_FLAGS_RD			(1<<8)
#define DNS_FLAGS_RA			(1<<7)
#define DNS_FLAGS_RCODE_OK		0
#define DNS_FLAGS_RCODE_FMT		1
#define DNS_FLAGS_RCODE_FAIL	2
#define DNS_FLAGS_RCODE_NAME	3
#define DNS_FLAGS_RCODE_TODO	4
#define DNS_FLAGS_RCODE_REFUSE	5

#define DNS_TYPE_A				HTONS (1) // a host address
#define DNS_TYPE_NS				HTONS (2) // an authoritative name server
#define DNS_TYPE_MD				HTONS (3) // a mail destination (Obsolete - use MX)
#define DNS_TYPE_MF				HTONS (4) // a mail forwarder (Obsolete - use MX)
#define DNS_TYPE_CNAME			HTONS (5) // the canonical name for an alias
#define DNS_TYPE_SOA			HTONS (6) // marks the start of a zone of authority
#define DNS_TYPE_MB				HTONS (7) // a mailbox domain name (EXPERIMENTAL)
#define DNS_TYPE_MG				HTONS (8) // a mail group member (EXPERIMENTAL)
#define DNS_TYPE_MR				HTONS (9) // a mail rename domain name (EXPERIMENTAL)
#define DNS_TYPE_NULL			HTONS (10) // a null RR (EXPERIMENTAL)
#define DNS_TYPE_WKS			HTONS (11) // a well known service description
#define DNS_TYPE_PTR			HTONS (12) // a domain name pointer
#define DNS_TYPE_HINFO			HTONS (13) // host information
#define DNS_TYPE_MINFO			HTONS (14) // mailbox or mail list information
#define DNS_TYPE_MX				HTONS (15) // mail exchange
#define DNS_TYPE_TXT			HTONS (16) // text strings

#define DNS_QTYPE_AXFR			252 // A request for a transfer of an entire zone
#define DNS_QTYPE_MAILB			253 // A request for mailbox-related records (MB, MG or MR)
#define DNS_QTYPE_MAILA			254 // A request for mail agent RRs (Obsolete - see MX)
#define DNS_QTYPE_ALL			255 // A request for all records

#define DNS_CLASS_IN			HTONS (1) // the Internet
#define DNS_CLASS_CS			HTONS (2) // the CSNET class 
#define DNS_CLASS_CH			HTONS (3) // the CHAOS class
#define DNS_CLASS_HS			HTONS (4) // Hesiod [Dyer 87]

#define DNS_QCLASS_ALL			255 // any class

#define RR_BODY_MIN_SIZE (3*sizeof (uint16_t) + sizeof (uint32_t))

struct dns_header_s {
	uint16_t id;
	uint16_t flags;
	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;
} __attribute__((packed));
typedef struct dns_header_s dns_header;

struct dns_string_s {
	uint8_t len;
};
typedef struct dns_string_s dns_string;
#define dns_string_buffer(x) ((char *)(x + 1))

struct dns_question_section_body_s {
	uint16_t qtype;
	uint16_t qclass;
};
struct dns_question_section_s {
	dns_string *qname;
	struct dns_question_section_body_s *body;
};
typedef struct dns_question_section_s dns_question_section;

struct dns_resource_record_body_s {
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
	char *rdata;
};
typedef struct dns_resource_record_body_s dns_resource_record_body;
struct dns_resource_record_s {
	dns_string *name;
	dns_resource_record_body *body;
};
typedef struct dns_resource_record_s dns_resource_record;


struct dns_answer_s {
	dns_header header;
	dns_question_section question;
	dns_resource_record **answers;
	dns_resource_record **authorities;
	dns_resource_record **aditionals;
};
typedef struct dns_answer_s dns_answer;

#define min(a, b) ((a<b)?a:b)
#endif
