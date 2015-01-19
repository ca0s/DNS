#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <arpa/inet.h>

#include "misc.h"
#include "dns.h"

void error (int mode, char *msg)
{
	fprintf (stderr, "%s\n", msg);
	if (mode == E_FATAL)
		exit (-1);
}

int find_creds (int *id, int *group)
{
	struct passwd *info = getpwnam ("nobody");
	if (!info)
		return -1;
	*id = info->pw_uid;
	*group = info->pw_gid;
	return 0;
}

char *str2ip (char *str)
{
	char *res = malloc (INET_ADDRSTRLEN * sizeof (char));
	if (res)
		inet_pton (AF_INET, str, res);
	return res;
}
