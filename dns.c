#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "dns.h"
#include "manager.h"
#include "misc.h"
#include "resolve.h"

extern dns_node *root_node;

void sig_exit (int s)
{
	free_all_nodes (root_node);
	printf ("Exiting...\n");
	exit (0);
}

int main (int argc, char *argv[])
{
	int server;
	int id, group;
	int port = DEFAULT_PORT;
	struct sockaddr_in addr, client;
	int len = sizeof (struct sockaddr), l;
	char buffer[DNS_MAX_PACKET_SIZE], response[DNS_MAX_PACKET_SIZE];
	int r = 0;
	
	dns_answer answer = {
		.header = {
			.flags = 0,
			.qdcount = 0,
			.ancount = 0,
			.nscount = 0,
			.arcount = 0
		},
		.answers = NULL,
		.authorities = NULL,
		.aditionals = NULL
	};

	if ((server = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
		error (E_FATAL, "error: socket\n");
		
	addr = (struct sockaddr_in) {
		.sin_port = htons (port),
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY
	};

	if (bind (server, (struct sockaddr *) &addr, sizeof (struct sockaddr)) != 0)
		error (E_FATAL, "error: bind");
		
	if (getuid () == 0) {
		if (find_creds (&id, &group))
			error (E_WARN, "warning: cannot find user/group nobody");
		else {
			if (setgid (id))
				error (E_WARN, "warning: drop root gid");
			if (setuid (id))
				error (E_WARN, "warning: drop root id");
		}
	}
	
	root_node = malloc (sizeof (dns_node));
	char *a = malloc (1);
	a[0] = 0x00;
	root_node->label = a;
	root_node->up = NULL;
	root_node->n_down = 0;
	root_node->down = NULL;
	root_node->records = NULL;
	root_node->n_records = 0;
	
	add_record ("ka0labs.net",			DNS_TYPE_A, 	DNS_CLASS_IN, "208.89.214.47");
	add_record ("www.ka0labs.net",		DNS_TYPE_CNAME, DNS_CLASS_IN, "ka0labs.net");
	add_record ("foro.ka0labs.net",		DNS_TYPE_CNAME, DNS_CLASS_IN, "ka0labs.net");
	add_record ("blog.ka0labs.net",		DNS_TYPE_CNAME, DNS_CLASS_IN, "ka0labs.net");
	add_record ("codeshare.ka0labs.net",DNS_TYPE_CNAME, DNS_CLASS_IN, "ka0labs.net");
	add_record ("biblio.ka0labs.net",	DNS_TYPE_CNAME, DNS_CLASS_IN, "ka0labs.net");
	
	signal (SIGINT, sig_exit);
	
	while (1) {
		while ((r = recvfrom (server, buffer, DNS_MAX_PACKET_SIZE, 0, (struct sockaddr *) &client, (socklen_t *) &len)) > 0) {
			manage_query (buffer, &answer, r);
			memset (response, 0, DNS_MAX_PACKET_SIZE);
			l = build_answer (&answer, response);
			sendto (server, response, l, 0, (struct sockaddr *) &client, len);
			free_answer (&answer);
			memset (buffer, 0, DNS_MAX_PACKET_SIZE);
		}
	}

	close (server);
	return 0;
}
