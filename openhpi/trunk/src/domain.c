/* BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

static struct list_head dlist;
static SaHpiDomainIdT dcounter = SAHPI_DEFAULT_DOMAIN_ID;


int init_domain(void) 
{
	list_init(&dlist);
	return 0;
}

int uninit_domain(void) 
{
	return 0;
}

struct oh_domain *domain_get(SaHpiDomainIdT did)
{
	struct list_head *i;
	list_for_each(i, &dlist) {
		struct oh_domain *d;
		d = list_container(i, struct oh_domain, node);
		if (d->did == did) return d;
	}

	return NULL;
}

int domain_add(struct oh_domain **domain)
{
	struct oh_domain  *d;
	
	d = malloc(sizeof(*d));
	if (!d) {
		dbg("Cannot get memory!");
		return -1;
	}
	memset(d, 0, sizeof(*d));
	
	d->did = dcounter++;

	list_add(&d->node, &dlist);
	list_init(&d->res_list);
	list_init(&d->session_list);

	d->res_counter = 0;

	*domain = d;
	return 0;
}

int domain_del(struct oh_domain *domain)
{
	/* FIXME cleaup resources in domain */
	list_del(&domain->node);
	free(domain);
	return 0;
}

int domain_process_event(struct oh_domain *d, struct oh_event *e)
{
	switch (e->type) {
	case OH_ET_ENTITY:
		if (e->entity.category!=SAHPI_EC_PRESENCE) {
			dbg("Unexcepted event category");
			return -1;
		}
		
		if (e->entity.state == SAHPI_ES_ABSENT) {
			absent_entity(d, &e->oid);
		} else if (e->entity.state == SAHPI_ES_PRESENT) {
			present_entity(d, &e->oid);
		} else {
			dbg("Unexcepted event state");
			return -1;
		}
		break;

	/* FIXME: there are controls/sensors/event logs etc 
	 * need to be processed
	 */
	default:
		break;
	}

	return 0;
}
