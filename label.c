#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "label.h"
#include "dns.h"

uint8_t inline is_offset (uint8_t len)
{
	return (len & 0xC0);
}

// Crea el label con el nombre a resolver
dns_label *name_from_dns_string (dns_string *str)
{
	dns_label *label = NULL, *root = NULL;
	while (str->len) {
		if (!label) {
			label = label_from_dns_string (str);
			root = label;
		}
		else {
			label->up = label_from_dns_string (str);
			label->up->down = label;
			label = label->up;
		}
		if (is_offset (str->len))
			str += 2;
		else str += (str->len + 1);
	}
	return root;
}

// Crea un nodo para la funcion anterior
dns_label *label_from_dns_string (dns_string *string)
{
	dns_label *ret = malloc (sizeof (dns_label));
	ret->up = NULL;
	ret->down = NULL;
	if (is_offset (string->len)) {
		ret->label = malloc (5);
		strcpy (ret->label, "foo");
	} else {
		ret->label = malloc (string->len + 1);
		strncpy (ret->label, dns_string_buffer (string), string->len);
		ret->label[string->len] = 0x00;
	}
	return ret;
}

void print_name (dns_label *root)
{
	while (root) {
		printf ("%s.", root->label);
		root = root->up;
	}
	printf ("\n");
}

dns_label *dns_label_from_string (char *str)
{
	dns_label *res = new_dns_label ();
	int i = 0, j = strlen (str), k = 0;

	while (i <= j) {
		if (str[i] == '.' || !str[i]) {
			res->label = malloc (i - k + 1);
			strncpy (res->label, &str[k], i - k);
			res->label[i - k] = 0x00;
			res->up = new_dns_label ();
			res->up->down = res;
			res = res->up;
			i++;
			k = i;
		}
		else i++;
	}
	res->label = malloc (1);
	res->label[0] = 0x00;
	res->up = NULL;
	return res;
}
	
dns_label *dns_label_head (dns_label *label)
{
	while (label && label->up)
		label = label->up;
	return label;
}

dns_label *new_dns_label ()
{
	dns_label *new = malloc (sizeof (dns_label));
	new->up = NULL;
	new->down = NULL;
	new->label = NULL;
	return new;
}

void free_dns_name (dns_label *root)
{
	dns_label *tmp;
	while (root) {
		tmp = root->down;
		if (root->label)
			free(root->label);
		free (root);
		root = tmp;
	}
}
