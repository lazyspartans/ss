/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003-2004
 * Copyright (c) 2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 *     Sean Dague <sdague@users.sf.net>
 *     Rusty Lynch
 *     David Judkovics <djudkovi@us.ibm.com>
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     Renier Morales <renierm@users.sf.net>
 */

#include <string.h>
#include <SaHpi.h>
#include <openhpi.h>
#include <sahpimacros.h>
#include <oh_utils.h>

/*********************************************************************
 *
 * Begin SAHPI B.1.1 Functions. For full documentation please see
 * the specification
 *
 ********************************************************************/

SaHpiVersionT SAHPI_API saHpiVersionGet ()
{
        return SAHPI_INTERFACE_VERSION;
}

SaErrorT SAHPI_API saHpiSessionOpen(
                SAHPI_IN SaHpiDomainIdT DomainId,
                SAHPI_OUT SaHpiSessionIdT *SessionId,
                SAHPI_IN void *SecurityParams)
{
        SaHpiSessionIdT sid;
        SaHpiDomainIdT did;

        if(oh_initialized() != SA_OK) {
                oh_initialize();
        }

        /* Security Params required to be NULL by the spec at this point */
        if (SecurityParams != NULL) {
                dbg("SecurityParams must be NULL");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (DomainId == SAHPI_UNSPECIFIED_DOMAIN_ID)
                did = OH_FIRST_DOMAIN;
        else
                did = DomainId;

        sid = oh_create_session(did);
        if(!sid) {
                dbg("Domain %d does not exist or unable to create session!", did);
                return SA_ERR_HPI_INVALID_DOMAIN;
        }

        *SessionId = sid;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiSessionClose(SAHPI_IN SaHpiSessionIdT SessionId)
{
        OH_CHECK_INIT_STATE(SessionId);        

        return oh_destroy_session(SessionId);
}

SaErrorT SAHPI_API saHpiDiscover(SAHPI_IN SaHpiSessionIdT SessionId)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;        
        GSList *i = NULL;
        int rv = -1;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        /* FIXME: This should not look on the handlers directly, let's encapsulate later */
        g_slist_for_each(i, global_handler_list) {
                struct oh_handler *h = i->data;
                if (!(h->abi->discover_resources(h->hnd)))
                        rv = 0;
        }

        if (rv) {
                dbg("Error attempting to discover resource");
                return SA_ERR_HPI_UNKNOWN;
        }

        /* note, after this we have the domain lock */
        OH_GET_DOMAIN(did, d);        

        /* FIXME: This rpt access must be buried deeper later */
        rv = get_events(&(d->rpt));

        /* give back the lock */
        oh_release_domain(d);

        if (rv < 0) {
                dbg("Error attempting to process resources");
                return SA_ERR_HPI_UNKNOWN;
        }

        return SA_OK;
}


/*********************************************************************
 *
 *  Domain Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiDomainInfoGet (
        SAHPI_IN  SaHpiSessionIdT      SessionId,
        SAHPI_OUT SaHpiDomainInfoT     *DomainInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaErrorT rv;

        if (!DomainInfo) return SA_ERR_HPI_INVALID_PARAMS;
        
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        OH_GET_DOMAIN(did, d); /* Lock domain */

        rv = get_events(&(d->rpt)); /* FIXME: Is this necessary ? */
        if (rv<0) {
                dbg("Error attempting to process events");
                return SA_ERR_HPI_UNKNOWN;
        }
        
        *DomainInfo = d->info;
        
        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

SaErrorT SAHPI_API saHpiDrtEntryGet (
        SAHPI_IN  SaHpiSessionIdT     SessionId,
        SAHPI_IN  SaHpiEntryIdT       EntryId,
        SAHPI_OUT SaHpiEntryIdT       *NextEntryId,
        SAHPI_OUT SaHpiDrtEntryT      *DrtEntry)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiDomainTagSet (
        SAHPI_IN  SaHpiSessionIdT      SessionId,
        SAHPI_IN  SaHpiTextBufferT     *DomainTag)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!DomainTag) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        OH_GET_DOMAIN(did, d); /* Lock domain */

        d->info.DomainTag = *DomainTag;
        
        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

/*********************************************************************
 *
 *  Resource Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiRptEntryGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiEntryIdT EntryId,
                SAHPI_OUT SaHpiEntryIdT *NextEntryId,
                SAHPI_OUT SaHpiRptEntryT *RptEntry)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;        
        SaHpiRptEntryT *req_entry;
        SaHpiRptEntryT *next_entry;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        
        /* Test pointer parameters for invalid pointers */
        if ((NextEntryId == NULL) || (RptEntry == NULL))
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* note, after this we have the domain lock */
        OH_GET_DOMAIN(did, d);

        if (EntryId == SAHPI_FIRST_ENTRY) {
                req_entry = oh_get_resource_next(&(d->rpt), SAHPI_FIRST_ENTRY);
        } else {
                req_entry = oh_get_resource_by_id(&(d->rpt), EntryId);
        }

        /* if the entry was NULL, clearly have an issue */
        if (req_entry == NULL) {
                dbg("Invalid EntryId %d in Domain %d", EntryId, d->id);
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_CMD;
        }

        memcpy(RptEntry, req_entry, sizeof(*RptEntry));
        
        next_entry = oh_get_resource_next(&(d->rpt), req_entry->EntryId);
        
        if(next_entry != NULL) {
                *NextEntryId = next_entry->EntryId;
        } else {
                *NextEntryId = SAHPI_LAST_ENTRY;
        }

        oh_release_domain(d);
        
        return SA_OK;
}

SaErrorT SAHPI_API saHpiRptEntryGetByResourceId(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiRptEntryT *RptEntry)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;        
        SaHpiRptEntryT *req_entry;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        
        /* Test pointer parameters for invalid pointers */
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID ||
            RptEntry == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_GET_DOMAIN(did, d);
                       
        req_entry = oh_get_resource_by_id(&(d->rpt), ResourceId);

        /*
         * is this case really supposed to be an error?  I thought
         * there was a valid return for "not found in domain"
         */

        if (req_entry == NULL) {
                dbg("No such Resource Id %d in Domain %d", ResourceId, d->id);
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_CMD;
        }

        memcpy(RptEntry, req_entry, sizeof(*RptEntry));

        oh_release_domain(d);
        
        return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceSeveritySet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSeverityT Severity)
{
        /* this requires a new abi call to push down to the plugin */
        int (*set_res_sev)(void *hnd, SaHpiResourceIdT id,
                             SaHpiSeverityT sev);

        SaHpiDomainIdT did;
        struct oh_handler *h = NULL;
        struct oh_domain *d = NULL;

        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                dbg("Invalid resource id, SAHPI_UNSPECIFIED_RESOURCE_ID passed.");
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!oh_lookup_severity(Severity)) {
                dbg("Invalid severity %d passed.", Severity);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        OH_GET_DOMAIN(did, d);

        OH_HANDLER_GET(d, ResourceId, h);

        set_res_sev = h->abi->set_resource_severity;

        if (!set_res_sev) {
                oh_release_domain(d);                
                return SA_ERR_HPI_INVALID_CMD;
        }

        if (set_res_sev(h->hnd, ResourceId, Severity) < 0) {                
                dbg("Setting severity failed for ResourceId %d in Domain %d",
                    ResourceId, d->id);
                oh_release_domain(d);
                return SA_ERR_HPI_UNKNOWN;
        }

        /* to get rpt entry into infrastructure */
        get_events(&(d->rpt));
        oh_release_domain(d);
        
        return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceTagSet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiTextBufferT *ResourceTag)
{
        SaErrorT rv;
        SaErrorT (*set_res_tag)(void *hnd, SaHpiResourceIdT id,
                                SaHpiTextBufferT *ResourceTag);

        SaHpiDomainIdT did;
        struct oh_handler *h = NULL;
        struct oh_domain *d = NULL;
                
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        OH_GET_DOMAIN(did, d);

        OH_HANDLER_GET(d, ResourceId, h);

        set_res_tag = h->abi->set_resource_tag;

        if (!set_res_tag) {
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_res_tag(h->hnd, ResourceId, ResourceTag);

        if (!rv) {
                SaHpiRptEntryT *rptentry;

                rptentry = oh_get_resource_by_id(&(d->rpt), ResourceId);
                if (!rptentry) {
                        oh_release_domain(d);
                        return SA_ERR_HPI_NOT_PRESENT;
                }

                rptentry->ResourceTag = *ResourceTag;
        } else
                dbg("Tag set failed for Resource %d in Domain %d",
                    ResourceId, d->id);

        oh_release_domain(d);
        
        return rv;
}

SaErrorT SAHPI_API saHpiResourceIdGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_OUT SaHpiResourceIdT *ResourceId)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiEntityPathT ep;
        SaHpiRptEntryT *rptentry;
        char *on_entitypath = getenv("OPENHPI_ON_EP");

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        if (!on_entitypath) {
                return SA_ERR_HPI_UNKNOWN;
        }
                
        string2entitypath(on_entitypath, &ep);

        OH_GET_DOMAIN(did, d);
        rptentry = oh_get_resource_by_ep(&(d->rpt), &ep);
        if (!rptentry) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        *ResourceId = rptentry->ResourceId;
        oh_release_domain(d);

        return SA_OK;
}

/*********************************************************************
 *
 *  Event Log Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiEventLogInfoGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiEventLogInfoT *Info)
{
        SaErrorT rv;
        SaErrorT (*get_func) (void *, SaHpiResourceIdT, SaHpiEventLogInfoT *);
        SaHpiRptEntryT *res;
        struct oh_handler *h = NULL;
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;

        /* Test pointer parameters for invalid pointers */
        if (Info == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(SAHPI_UNSPECIFIED_DOMAIN_ID);
                rv = oh_el_info(d->del, Info);

                return rv;
        }

        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have EL", ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_func = h->abi->get_el_info;

        if (!get_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, Info);
        if (rv != SA_OK) {
                dbg("EL info get failed");
        }

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogEntryGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiEventLogEntryIdT EntryId,
                SAHPI_OUT SaHpiEventLogEntryIdT *PrevEntryId,
                SAHPI_OUT SaHpiEventLogEntryIdT *NextEntryId,
                SAHPI_OUT SaHpiEventLogEntryT *EventLogEntry,
                SAHPI_INOUT SaHpiRdrT *Rdr,
                SAHPI_INOUT SaHpiRptEntryT *RptEntry)
{
        SaErrorT rv;
        SaErrorT (*get_el_entry)(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT current,
                                  SaHpiEventLogEntryIdT *prev, SaHpiEventLogEntryIdT *next, SaHpiEventLogEntryT *entry);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiEventLogEntryT *elentry;
        SaErrorT retc;
        SaHpiDomainIdT did;

        /* Test pointer parameters for invalid pointers */
        if (EventLogEntry == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }



        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(SAHPI_UNSPECIFIED_DOMAIN_ID);
                retc = oh_el_get(d->del, EntryId, PrevEntryId, NextEntryId,
                                  &elentry);
                if (retc != SA_OK) {
                        return retc;
                }

                memcpy(EventLogEntry, elentry, sizeof(SaHpiEventLogEntryT));
                return SA_OK;
        }


        OH_GET_DOMAIN(did, d);

        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have EL", ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_el_entry = h->abi->get_el_entry;

        if (!get_el_entry) {
                dbg("This api is not supported");
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_el_entry(h->hnd, ResourceId, EntryId, PrevEntryId,
                           NextEntryId, EventLogEntry);

        if(rv != SA_OK) {
                dbg("EL entry get failed");
        }

        if (RptEntry) *RptEntry = *res;
        if (Rdr) {
                SaHpiRdrT *tmprdr = NULL;
                SaHpiUint8T num;
                switch (EventLogEntry->Event.EventType) {
                        case SAHPI_ET_SENSOR:
                                num = EventLogEntry->Event.EventDataUnion.SensorEvent.SensorNum;
                                tmprdr = oh_get_rdr_by_type(&(d->rpt),ResourceId,SAHPI_SENSOR_RDR,num);
                                if (tmprdr)
                                         memcpy(Rdr,tmprdr,sizeof(SaHpiRdrT));
                                else dbg("saHpiEventLogEntryGet: Could not find rdr.");
                                break;
                        case SAHPI_ET_WATCHDOG:
                                num = EventLogEntry->Event.EventDataUnion.WatchdogEvent.WatchdogNum;
                                tmprdr = oh_get_rdr_by_type(&(d->rpt),ResourceId,SAHPI_WATCHDOG_RDR,num);
                                if (tmprdr)
                                         memcpy(Rdr,tmprdr,sizeof(SaHpiRdrT));
                                else dbg("saHpiEventLogEntryGet: Could not find rdr.");
                                break;
                        default:
                                ;
                }
        }


        return rv;
}

SaErrorT SAHPI_API saHpiEventLogEntryAdd (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiEventT *EvtEntry)
{
        SaErrorT rv;
        SaErrorT (*add_el_entry)(void *hnd, SaHpiResourceIdT id,
                                  const SaHpiEventT *Event);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiDomainIdT did;

        if (EvtEntry == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }


        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(SAHPI_UNSPECIFIED_DOMAIN_ID);
                rv = oh_el_add(d->del, EvtEntry);
                return rv;
        }

        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have EL", ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        add_el_entry = h->abi->add_el_entry;

        if (!add_el_entry) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = add_el_entry(h->hnd, ResourceId, EvtEntry);
        if(rv != SA_OK) {
                dbg("EL add entry failed");
        }


        /* to get REL entry into infrastructure */
        rv = get_events(&(d->rpt));
        if(rv != SA_OK) {
                dbg("Event loop failed");
        }

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogClear (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaErrorT rv;
        SaErrorT (*clear_el)(void *hnd, SaHpiResourceIdT id);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiDomainIdT did;


        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(SAHPI_UNSPECIFIED_DOMAIN_ID);
                rv = oh_el_clear(d->del);
                return rv;
        }

        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have EL", ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        clear_el = h->abi->clear_el;
        if (!clear_el) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = clear_el(h->hnd, ResourceId);
        if(rv != SA_OK) {
                dbg("EL delete entry failed");
        }


        return rv;
}

SaErrorT SAHPI_API saHpiEventLogTimeGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiTimeT *Time)
{
        SaHpiEventLogInfoT info;
        SaErrorT rv;

        /* Test pointer parameters for invalid pointers */
        if (Time == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rv = saHpiEventLogInfoGet(SessionId, ResourceId, &info);

        if(rv < 0) {
                return rv;
        }

        *Time = info.CurrentTime;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiEventLogTimeSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiTimeT Time)
{
        SaErrorT rv;
        SaErrorT (*set_el_time)(void *hnd, SaHpiResourceIdT id, SaHpiTimeT time);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiDomainIdT did;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(SAHPI_UNSPECIFIED_DOMAIN_ID);
                rv = oh_el_timeset(d->del, Time);
                return rv;
        }

        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have EL", ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        set_el_time = h->abi->set_el_time;

        if (!set_el_time) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_el_time(h->hnd, ResourceId, Time);
        if(rv != SA_OK) {
                dbg("Set EL time failed");
        }


        return rv;
}

SaErrorT SAHPI_API saHpiEventLogStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiBoolT *Enable)
{
        SaHpiEventLogInfoT info;
        SaErrorT rv;

        /* Test pointer parameters for invalid pointers */
        if (Enable == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rv = saHpiEventLogInfoGet(SessionId, ResourceId, &info);

        if(rv < 0) {
                return rv;
        }

        *Enable = info.Enabled;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiEventLogStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiBoolT Enable)
{
        struct oh_domain *d;
        SaHpiDomainIdT did;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(SAHPI_UNSPECIFIED_DOMAIN_ID);
                d->del->enabled = Enable;

                return SA_OK;
        }


        /* this request is not valid on an REL */
        return SA_ERR_HPI_INVALID_REQUEST;
}

SaErrorT SAHPI_API saHpiEventLogOverflowReset (
        SAHPI_IN  SaHpiSessionIdT     SessionId,
        SAHPI_IN  SaHpiResourceIdT    ResourceId)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/*********************************************************************
 *
 *  Event Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiSubscribe (
        SAHPI_IN SaHpiSessionIdT SessionId)
{
        SaHpiDomainIdT did;
        SaHpiBoolT session_state;
        SaErrorT error;
        
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        error = oh_get_session_state(SessionId, &session_state);
        if (error) return error;
        
        if (session_state != OH_UNSUBSCRIBED) {
                dbg("Cannot subscribe if session is not unsubscribed.");
                return SA_ERR_HPI_DUPLICATE;
        }

        error = oh_set_session_state(SessionId, SAHPI_TRUE);

        return error;
}

SaErrorT SAHPI_API saHpiUnsubscribe (
                SAHPI_IN SaHpiSessionIdT SessionId)
{
        SaHpiDomainIdT did;
        SaHpiBoolT session_state;
        SaErrorT error;
        struct oh_event *event = NULL;
        
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        error = oh_get_session_state(SessionId, &session_state);
        if (error) return error;
        
        if (session_state != OH_SUBSCRIBED) {
                dbg("Cannot unsubscribe if session is not subscribed.");
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        error = oh_set_session_state(SessionId, SAHPI_FALSE);
        if (error) return error;

        /* Flush session's event queue */
        error = oh_dequeue_session_event(SessionId,
                                         SAHPI_TIMEOUT_IMMEDIATE,
                                         event);
        if (error) return error;
        
        while (event != NULL && !error) {
                error = oh_dequeue_session_event(SessionId,
                                                 SAHPI_TIMEOUT_IMMEDIATE,
                                                 event);
        }        
                
        return error;
}

SaErrorT SAHPI_API saHpiEventGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiTimeoutT Timeout,
                SAHPI_OUT SaHpiEventT *Event,
                SAHPI_INOUT SaHpiRdrT *Rdr,
                SAHPI_INOUT SaHpiRptEntryT *RptEntry,
                SAHPI_INOUT SaHpiEvtQueueStatusT *EventQueueStatus
        )
{

        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;        
        SaHpiBoolT session_state;
        struct oh_event e;
        SaErrorT error;        
        
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        if (Timeout < SAHPI_TIMEOUT_BLOCK || !Event) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        error = oh_get_session_state(SessionId, &session_state);
        if (error) return error;

        if (session_state != OH_SUBSCRIBED) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_GET_DOMAIN(did, d);
        
        if (get_events(&(d->rpt)) < 0) {
                return SA_ERR_HPI_UNKNOWN;
        }

        error = oh_dequeue_session_event(SessionId, Timeout, &e);
        if (error) return error;

        /* Return event, resource and rdr */
        /* FIXME!!: dequeuing returns an oh_event but need something
         * like oh_session_event to populate RptEntry and Rdr.
         */
        /*
        memcpy(Event, &e.event, sizeof(*Event));


        if (RptEntry)
             memcpy(RptEntry, &e.rpt_entry, sizeof(*RptEntry));

        if (Rdr)
             memcpy(Rdr, &e.rdr, sizeof(*Rdr));
        */


        return SA_OK;                
}

SaErrorT SAHPI_API saHpiEventAdd (
        SAHPI_IN SaHpiSessionIdT      SessionId,
        SAHPI_IN SaHpiEventT          *EvtEntry)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/*********************************************************************
 *
 *  DAT Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiAlarmGetNext (
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiSeverityT             Severity,
    SAHPI_IN SaHpiBoolT                 UnacknowledgedOnly,
    SAHPI_INOUT SaHpiAlarmT             *Alarm)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAlarmGet(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiAlarmIdT              AlarmId,
    SAHPI_OUT SaHpiAlarmT               *Alarm)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAlarmAcknowledge(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiAlarmIdT              AlarmId,
    SAHPI_IN SaHpiSeverityT             Severity)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAlarmAdd(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_INOUT SaHpiAlarmT             *Alarm)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAlarmDelete(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiAlarmIdT              AlarmId,
    SAHPI_IN SaHpiSeverityT             Severity)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/*********************************************************************
 *
 *  RDR Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiRdrGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiEntryIdT EntryId,
                SAHPI_OUT SaHpiEntryIdT *NextEntryId,
                SAHPI_OUT SaHpiRdrT *Rdr)
{
        struct oh_domain *d;
        SaHpiDomainIdT did;
        RPTable *rpt = NULL;
        SaHpiRptEntryT *res = NULL;
        SaHpiRdrT *rdr_cur;
        SaHpiRdrT *rdr_next;

        if(oh_initialized() != SA_OK) {
                dbg("Session %d not initalized", SessionId);
                return SA_ERR_HPI_INVALID_SESSION;
        }
        
        if(!(did = oh_get_session_domain(SessionId))) {
                dbg("No domain for session id %d",SessionId);
                return SA_ERR_HPI_INVALID_SESSION;
        }

        /* Test pointer parameters for invalid pointers */
        if ((Rdr == NULL)||(NextEntryId == NULL))
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* note, after this we have the domain lock */
        if(!(d = oh_get_domain(did))) {
                dbg("Domain %d doesn't exist", did);
                return SA_ERR_HPI_INVALID_DOMAIN;
        }
        rpt = &(d->rpt);
        
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_RDR)) {
                dbg("No RDRs for Resource %d",ResourceId);
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        if(EntryId == SAHPI_FIRST_ENTRY) {
                rdr_cur = oh_get_rdr_next(rpt, ResourceId, SAHPI_FIRST_ENTRY);
        } else {
                rdr_cur = oh_get_rdr_by_id(rpt, ResourceId, EntryId);
        }

        if (rdr_cur == NULL) {
                dbg("Requested RDR, Resource[%d]->RDR[%d], is not present",
                    ResourceId, EntryId);
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(Rdr, rdr_cur, sizeof(*Rdr));

        rdr_next = oh_get_rdr_next(rpt, ResourceId, rdr_cur->RecordId);
        if(rdr_next == NULL) {
                *NextEntryId = SAHPI_LAST_ENTRY;
        } else {
                *NextEntryId = rdr_next->RecordId;
        }

        oh_release_domain(d);

        return SA_OK;
}

SaErrorT SAHPI_API saHpiRdrGetByInstrumentId (
        SAHPI_IN  SaHpiSessionIdT        SessionId,
        SAHPI_IN  SaHpiResourceIdT       ResourceId,
        SAHPI_IN  SaHpiRdrTypeT          RdrType,
        SAHPI_IN  SaHpiInstrumentIdT     InstrumentId,
        SAHPI_OUT SaHpiRdrT              *Rdr)
{
        SaHpiRptEntryT *res = NULL;
        SaHpiRdrT *rdr_cur;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        /* Test pointer parameters for invalid pointers */
        if (Rdr == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }


        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);

        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_RDR)) {
                dbg("No RDRs for Resource %d",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr_cur = oh_get_rdr_by_type(&(d->rpt), ResourceId, RdrType, InstrumentId);

        if (rdr_cur == NULL) {
                dbg("Requested RDR, Resource[%d]->RDR[%d,%d], is not present",
                    ResourceId, RdrType, InstrumentId);
                return SA_ERR_HPI_NOT_PRESENT;
        }


        return SA_OK;
        // return SA_ERR_HPI_UNSUPPORTED_API;
}

/*********************************************************************
 *
 *  Sensor Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiSensorReadingGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_INOUT SaHpiSensorReadingT *Reading,
                SAHPI_INOUT SaHpiEventStateT *EventState)
{
        SaErrorT rv;
        SaErrorT (*get_func) (void *, SaHpiResourceIdT, SaHpiSensorNumT, SaHpiSensorReadingT *);

        struct oh_handler *h;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiSensorRecT *sensor;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                dbg("No Sensor %d found for ResourceId %d", SensorNum, ResourceId);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        sensor = &(rdr->RdrTypeUnion.SensorRec);

        OH_HANDLER_GET(d, ResourceId, h);

        get_func = h->abi->get_sensor_data;

        if (!get_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, SensorNum, Reading);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorThresholdsSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_OUT SaHpiSensorThresholdsT *SensorThresholds)
{
        SaErrorT rv;
        SaErrorT (*set_func) (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                         const SaHpiSensorThresholdsT *);
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiSensorRecT *sensor;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                dbg("No Sensor %d found for ResourceId %d", SensorNum, ResourceId);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        sensor = &(rdr->RdrTypeUnion.SensorRec);

        OH_HANDLER_GET(d, ResourceId, h);

        set_func = h->abi->set_sensor_thresholds;

        if (!set_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_func(h->hnd, ResourceId, SensorNum, SensorThresholds);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorThresholdsGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_IN SaHpiSensorThresholdsT *SensorThresholds)
{
        SaErrorT rv;
        SaErrorT (*get_func) (void *, SaHpiResourceIdT, SaHpiSensorNumT, SaHpiSensorThresholdsT *);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_func = h->abi->get_sensor_thresholds;

        if (!get_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, SensorNum, SensorThresholds);


        return rv;
}

/* Function: SaHpiSensorTypeGet */
/* Core SAF_HPI function */
/* Not mapped to plugin */
/* Data in RDR */
SaErrorT SAHPI_API saHpiSensorTypeGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_OUT SaHpiSensorTypeT *Type,
                SAHPI_OUT SaHpiEventCategoryT *Category)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                dbg("No Sensor num %d found for Resource %d", SensorNum, ResourceId);
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        if (!memcpy(Type, &(rdr->RdrTypeUnion.SensorRec.Type),
                    sizeof(SaHpiSensorTypeT))) {
                return SA_ERR_HPI_ERROR;
        }

        if (!memcpy(Category, &(rdr->RdrTypeUnion.SensorRec.Category),
                    sizeof(SaHpiEventCategoryT))) {
                return SA_ERR_HPI_ERROR;
        }


        return SA_OK;
}

SaErrorT SAHPI_API saHpiSensorEventEnableGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_OUT SaHpiBoolT *SensorEventsEnabled)
{
        SaErrorT rv;
        SaErrorT (*get_sensor_event_enables)(void *hnd, SaHpiResourceIdT,
                                             SaHpiSensorNumT,
                                             SaHpiBoolT *enables);

        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_sensor_event_enables = h->abi->get_sensor_event_enables;

        if (!get_sensor_event_enables) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_sensor_event_enables(h->hnd, ResourceId, SensorNum, SensorEventsEnabled);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEventEnableSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_IN SaHpiBoolT SensorEventsEnabled)
{
        SaErrorT rv;
        SaErrorT (*set_sensor_event_enables)(void *hnd, SaHpiResourceIdT,
                                             SaHpiSensorNumT,
                                             const SaHpiBoolT enables);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        set_sensor_event_enables = h->abi->set_sensor_event_enables;

        if (!set_sensor_event_enables) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_sensor_event_enables(h->hnd, ResourceId, SensorNum, SensorEventsEnabled);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEventMasksGet (
        SAHPI_IN  SaHpiSessionIdT         SessionId,
        SAHPI_IN  SaHpiResourceIdT        ResourceId,
        SAHPI_IN  SaHpiSensorNumT         SensorNum,
        SAHPI_INOUT SaHpiEventStateT      *AssertEventMask,
        SAHPI_INOUT SaHpiEventStateT      *DeassertEventMask)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSensorEventMasksSet (
        SAHPI_IN  SaHpiSessionIdT                 SessionId,
        SAHPI_IN  SaHpiResourceIdT                ResourceId,
        SAHPI_IN  SaHpiSensorNumT                 SensorNum,
        SAHPI_IN  SaHpiSensorEventMaskActionT     Action,
        SAHPI_IN  SaHpiEventStateT                AssertEventMask,
        SAHPI_IN  SaHpiEventStateT                DeassertEventMask)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/* End Sensor functions */

/*********************************************************************
 *
 *  Control Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiControlTypeGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiCtrlNumT CtrlNum,
                SAHPI_OUT SaHpiCtrlTypeT *Type)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                dbg("Resource %d doesn't have .ResourceCapabilities flag:"
                    " SAHPI_CAPABILITY_CONTROL set ",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_CTRL_RDR, CtrlNum);
        if (!rdr) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        if (!memcpy(Type, &(rdr->RdrTypeUnion.CtrlRec.Type),
                    sizeof(SaHpiCtrlTypeT))) {
                return SA_ERR_HPI_ERROR;
        }


        return SA_OK;
}

SaErrorT SAHPI_API saHpiControlGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiCtrlNumT CtrlNum,
                SAHPI_OUT SaHpiCtrlModeT *CtrlMode,
                SAHPI_INOUT SaHpiCtrlStateT *CtrlState)
{
        SaErrorT rv;
        SaErrorT (*get_func)(void *, SaHpiResourceIdT, SaHpiCtrlNumT,
                             SaHpiCtrlModeT *, SaHpiCtrlStateT *);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                dbg("Resource %d doesn't have controls",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_func = h->abi->get_control_state;
        if (!get_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, CtrlNum, CtrlMode, CtrlState);

        return rv;
}

SaErrorT SAHPI_API saHpiControlSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiCtrlNumT CtrlNum,
                SAHPI_IN SaHpiCtrlModeT CtrlMode,
                SAHPI_IN SaHpiCtrlStateT *CtrlState)
{
        SaErrorT rv;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiCtrlNumT, SaHpiCtrlModeT, SaHpiCtrlStateT *);

        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                dbg("Resource %d doesn't have controls",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        set_func = h->abi->set_control_state;
        if (!set_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_func(h->hnd, ResourceId, CtrlNum, CtrlMode, CtrlState);

        return rv;
}


/*********************************************************************
 *
 *  Inventory Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiIdrInfoGet(
        SAHPI_IN SaHpiSessionIdT         SessionId,
        SAHPI_IN SaHpiResourceIdT        ResourceId,
        SAHPI_IN SaHpiIdrIdT             IdrId,
        SAHPI_OUT SaHpiIdrInfoT          *IdrInfo)
{

        SaHpiRptEntryT *res;
	SaErrorT rv = SA_OK;	/* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrInfoT *);
	
	
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

	/* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                rv =  SA_ERR_HPI_CAPABILITY;
        }
	
	if ((IdrInfo == NULL) && (rv == SA_OK)) {
                dbg("NULL IdrInfo");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}

        OH_HANDLER_GET(d, ResourceId, h);
        set_func = h->abi->get_idr_info;
        if ((!set_func) && (rv == SA_OK)) {
                dbg("Plugin does not have this function in jump table.");
		/* SA_ERR_HPI_UNSUPPORTED_API is used for non-conformance HPI implementation */
                rv = SA_ERR_HPI_INVALID_CMD;
        }


	/* Access Inventory Info from plugin */
	if (rv == SA_OK) { 
	        dbg("Access IdrInfo from plugin.");
        	rv = set_func(h->hnd, ResourceId, IdrId, IdrInfo);
	}
	
	/* Free mutext then return */
	return rv;
}

SaErrorT SAHPI_API saHpiIdrAreaHeaderGet(
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_IN SaHpiIdrAreaTypeT        AreaType,
        SAHPI_IN SaHpiEntryIdT            AreaId,
        SAHPI_OUT SaHpiEntryIdT           *NextAreaId,
        SAHPI_OUT SaHpiIdrAreaHeaderT     *Header)
{

        SaHpiRptEntryT *res;
	SaErrorT rv = SA_OK;	/* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrAreaTypeT,
			      SaHpiEntryIdT, SaHpiEntryIdT *,  SaHpiIdrAreaHeaderT *);
	
	
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

	/* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                rv =  SA_ERR_HPI_CAPABILITY;
        }
	
	
	if ( ((AreaType < SAHPI_IDR_AREATYPE_INTERNAL_USE) ||
	     ((AreaType > SAHPI_IDR_AREATYPE_PRODUCT_INFO) &&
	     (AreaType != SAHPI_IDR_AREATYPE_UNSPECIFIED)  &&
	     (AreaType != SAHPI_IDR_AREATYPE_OEM)) || 
	     (AreaId == SAHPI_LAST_ENTRY)||
	     (NextAreaId == NULL) ||
	     (Header == NULL)) && (rv == SA_OK))   {
                dbg("Invalid Parameters");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}

        OH_HANDLER_GET(d, ResourceId, h);
        set_func = h->abi->get_idr_area_header;
        if ((!set_func) && (rv == SA_OK)) {
                dbg("Plugin does not have this function in jump table.");
		/* SA_ERR_HPI_UNSUPPORTED_API is used for non-conformance HPI implementation */
                rv = SA_ERR_HPI_INVALID_CMD;
        }


	/* Access Inventory Info from plugin */
	if (rv == SA_OK) { 
	        dbg("Access IdrAreaHeader from plugin.");
        	rv = set_func(h->hnd, ResourceId, IdrId,AreaType, AreaId, NextAreaId, Header);
	}
	
	/* Free mutext then return */
	return rv;
}

SaErrorT SAHPI_API saHpiIdrAreaAdd(
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_IN SaHpiIdrAreaTypeT        AreaType,
        SAHPI_OUT SaHpiEntryIdT           *AreaId)
{
        SaHpiRptEntryT *res;
	SaErrorT rv = SA_OK;	/* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrAreaTypeT,
			        SaHpiEntryIdT *);
	
	
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

	/* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                rv =  SA_ERR_HPI_CAPABILITY;
        }
	
	
	if ( ((AreaType < SAHPI_IDR_AREATYPE_INTERNAL_USE) ||
	     ((AreaType > SAHPI_IDR_AREATYPE_PRODUCT_INFO) &&
	     (AreaType != SAHPI_IDR_AREATYPE_UNSPECIFIED)  &&
	     (AreaType != SAHPI_IDR_AREATYPE_OEM)) || 
	     (AreaId == NULL)) && (rv == SA_OK))   {
                dbg("Invalid Parameters");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}

        OH_HANDLER_GET(d, ResourceId, h);
        set_func = h->abi->add_idr_area;
        if ((!set_func) && (rv == SA_OK)) {
                dbg("Plugin does not have this function in jump table.");
		/* SA_ERR_HPI_UNSUPPORTED_API is used for non-conformance HPI implementation */
                rv = SA_ERR_HPI_INVALID_CMD;
        }


	/* Access Inventory Info from plugin */
	if (rv == SA_OK) { 
	        dbg("Access IdrAreaAdd from plugin.");
        	rv = set_func(h->hnd, ResourceId, IdrId, AreaType, AreaId);
	}
	
	/* Free mutext then return */
	return rv;

}

SaErrorT SAHPI_API saHpiIdrAreaDelete(
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiResourceIdT       ResourceId,
    SAHPI_IN SaHpiIdrIdT            IdrId,
    SAHPI_IN SaHpiEntryIdT          AreaId)
{
        SaHpiRptEntryT *res;
	SaErrorT rv = SA_OK;	/* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT );

	
	
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

	/* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                rv =  SA_ERR_HPI_CAPABILITY;
        }
	
	
	if ( (AreaId == SAHPI_LAST_ENTRY) && (rv == SA_OK))   {
                dbg("Invalid Parameters");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}

        OH_HANDLER_GET(d, ResourceId, h);
        set_func = h->abi->del_idr_area;
        if ((!set_func) && (rv == SA_OK)) {
                dbg("Plugin does not have this function in jump table.");
		/* SA_ERR_HPI_UNSUPPORTED_API is used for non-conformance HPI implementation */
                rv = SA_ERR_HPI_INVALID_CMD;
        }


	/* Access Inventory Info from plugin */
	if (rv == SA_OK) { 
	        dbg("Access IdrAreaDelete from plugin.");
        	rv = set_func(h->hnd, ResourceId, IdrId, AreaId);
	}
	
	/* Free mutext then return */
	return rv;

      
}

SaErrorT SAHPI_API saHpiIdrFieldGet(
        SAHPI_IN SaHpiSessionIdT         SessionId,
        SAHPI_IN SaHpiResourceIdT        ResourceId,
        SAHPI_IN SaHpiIdrIdT             IdrId,
        SAHPI_IN SaHpiEntryIdT           AreaId,
        SAHPI_IN SaHpiIdrFieldTypeT      FieldType,
        SAHPI_IN SaHpiEntryIdT           FieldId,
        SAHPI_OUT SaHpiEntryIdT          *NextFieldId,
        SAHPI_OUT SaHpiIdrFieldT         *Field)
{
        SaHpiRptEntryT *res;
	SaErrorT rv = SA_OK;	/* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiIdrIdT,
	                     SaHpiEntryIdT, SaHpiIdrFieldTypeT, SaHpiEntryIdT,
			     SaHpiEntryIdT *, SaHpiIdrFieldT * );
	
	
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

	/* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                rv =  SA_ERR_HPI_CAPABILITY;
        }
	
	
	if ((((FieldType > SAHPI_IDR_FIELDTYPE_CUSTOM) &&
	     (FieldType != SAHPI_IDR_FIELDTYPE_UNSPECIFIED)) ||
	     (AreaId == SAHPI_LAST_ENTRY) ||
	     (FieldId == SAHPI_LAST_ENTRY) ||
	     (NextFieldId == NULL) ||  
	     (Field == NULL)) && (rv == SA_OK))    {
                dbg("Invalid Parameters");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}

        OH_HANDLER_GET(d, ResourceId, h);
        set_func = h->abi->get_idr_field;
        if ((!set_func) && (rv == SA_OK)) {
                dbg("Plugin does not have this function in jump table.");
		/* SA_ERR_HPI_UNSUPPORTED_API is used for non-conformance HPI implementation */
                rv = SA_ERR_HPI_INVALID_CMD;
        }


	/* Access Inventory Info from plugin */
	if (rv == SA_OK) { 
	        dbg("Access saHpiIdrFieldGet from plugin.");
        	rv = set_func(h->hnd, ResourceId, IdrId, AreaId,
		               FieldType, FieldId, NextFieldId, Field);
	}
	
	/* Free mutext then return */
	return rv;

}

SaErrorT SAHPI_API saHpiIdrFieldAdd(
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_INOUT SaHpiIdrFieldT        *Field)
{
        SaHpiRptEntryT *res;
	SaErrorT rv = SA_OK;	/* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiIdrIdT,  SaHpiIdrFieldT * );
	
	
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

	/* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                rv =  SA_ERR_HPI_CAPABILITY;
        }
	
	
	if ((Field == NULL) && (rv == SA_OK))   {
                dbg("Invalid Parameter: Field is NULL ");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}
	
	if ((Field->Type > SAHPI_IDR_FIELDTYPE_CUSTOM) && (rv == SA_OK)) {	
		dbg("Invalid Parameters in Field->Type");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}
		

        OH_HANDLER_GET(d, ResourceId, h);
        set_func = h->abi->add_idr_field;
        if ((!set_func) && (rv == SA_OK)) {
                dbg("Plugin does not have this function in jump table.");
		/* SA_ERR_HPI_UNSUPPORTED_API is used for non-conformance HPI implementation */
                rv = SA_ERR_HPI_INVALID_CMD;
        }


	/* Access Inventory Info from plugin */
	if (rv == SA_OK) { 
	        dbg("Access saHpiIdrFieldAdd from plugin.");
        	rv = set_func(h->hnd, ResourceId, IdrId, Field);
	}

	/* Free mutext then return */
	return rv;
}

SaErrorT SAHPI_API saHpiIdrFieldSet(
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_IN SaHpiIdrFieldT           *Field)
{
        SaHpiRptEntryT *res;
	SaErrorT rv = SA_OK;	/* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT * );
	
	
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

	/* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                rv =  SA_ERR_HPI_CAPABILITY;
        }
	
	
	if ((Field == NULL) && (rv == SA_OK))   {
                dbg("Invalid Parameter: Field is NULL ");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}
	
	if ((Field->Type > SAHPI_IDR_FIELDTYPE_CUSTOM) && (rv == SA_OK)) {	
		dbg("Invalid Parameters in Field->Type");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}
		

        OH_HANDLER_GET(d, ResourceId, h);
        set_func = h->abi->set_idr_field;
        if ((!set_func) && (rv == SA_OK)) {
                dbg("Plugin does not have this function in jump table.");
		/* SA_ERR_HPI_UNSUPPORTED_API is used for non-conformance HPI implementation */
                rv = SA_ERR_HPI_INVALID_CMD;
        }


	/* Access Inventory Info from plugin */
	if (rv == SA_OK) { 
	        dbg("Access saHpiIdrFieldSet from plugin.");
        	rv = set_func(h->hnd, ResourceId, IdrId, Field);
	}

	/* Free mutext then return */
	return rv;

}

SaErrorT SAHPI_API saHpiIdrFieldDelete(
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_IN SaHpiEntryIdT            AreaId,
        SAHPI_IN SaHpiEntryIdT            FieldId)
{
        SaHpiRptEntryT *res;
	SaErrorT rv = SA_OK;	/* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_handler *h;
        struct oh_domain *d = NULL;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT, SaHpiEntryIdT );
	
	
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

	/* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                rv =  SA_ERR_HPI_CAPABILITY;
        }
	
	
	if ( ((FieldId == SAHPI_LAST_ENTRY)|| (AreaId == SAHPI_LAST_ENTRY)) && (rv == SA_OK))   {
                dbg("Invalid Parameters");
                rv =  SA_ERR_HPI_INVALID_PARAMS;
	}
	

        OH_HANDLER_GET(d, ResourceId, h);
        set_func = h->abi->del_idr_field;
        if ((!set_func) && (rv == SA_OK)) {
                dbg("Plugin does not have this function in jump table.");
		/* SA_ERR_HPI_UNSUPPORTED_API is used for non-conformance HPI implementation */
                rv = SA_ERR_HPI_INVALID_CMD;
        }


	/* Access Inventory Info from plugin */
	if (rv == SA_OK) { 
	        dbg("Access saHpiIdrFieldDelete from plugin.");
        	rv = set_func(h->hnd, ResourceId, IdrId, AreaId, FieldId);
	}

	/* Free mutext then return */
	return rv;

}

/* End of Inventory Functions  */

/*********************************************************************
 *
 *  Watchdog Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiWatchdogTimerGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
                SAHPI_OUT SaHpiWatchdogT *Watchdog)
{
        SaErrorT rv;
        SaErrorT (*get_func)(void *, SaHpiResourceIdT, SaHpiWatchdogNumT, SaHpiWatchdogT *);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
                dbg("Resource %d doesn't have watchdog",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_func = h->abi->get_watchdog_info;
        if (!get_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, WatchdogNum, Watchdog);


        return rv;
}

SaErrorT SAHPI_API saHpiWatchdogTimerSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
                SAHPI_IN SaHpiWatchdogT *Watchdog)
{
        SaErrorT rv;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiWatchdogNumT, SaHpiWatchdogT *);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
                dbg("Resource %d doesn't have watchdog",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        set_func = h->abi->set_watchdog_info;
        if (!set_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_func(h->hnd, ResourceId, WatchdogNum, Watchdog);

        return rv;
}

SaErrorT SAHPI_API saHpiWatchdogTimerReset (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiWatchdogNumT WatchdogNum)
{
        SaErrorT rv;
        SaErrorT (*reset_func)(void *, SaHpiResourceIdT, SaHpiWatchdogNumT);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
                dbg("Resource %d doesn't have watchdog",ResourceId);
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        reset_func = h->abi->reset_watchdog;
        if (!reset_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = reset_func(h->hnd, ResourceId, WatchdogNum);

        return rv;
}

/*******************************************************************************
 *
 *  Annunciator Functions
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiAnnunciatorGetNext(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiSeverityT             Severity,
        SAHPI_IN SaHpiBoolT                 UnacknowledgedOnly,
        SAHPI_INOUT SaHpiAnnouncementT      *Announcement)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorGet(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiEntryIdT              EntryId,
        SAHPI_OUT SaHpiAnnouncementT        *Announcement)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorAcknowledge(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiEntryIdT              EntryId,
        SAHPI_IN SaHpiSeverityT             Severity)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorAdd(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_INOUT SaHpiAnnouncementT      *Announcement)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorDelete(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiEntryIdT              EntryId,
        SAHPI_IN SaHpiSeverityT             Severity)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorModeGet(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_OUT SaHpiAnnunciatorModeT     *Mode)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorModeSet(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiAnnunciatorModeT      Mode)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/*******************************************************************************
 *
 *  Hotswap Functions
 *
 ******************************************************************************/

#if 0 // this is not in HPI B, will come out later
SaErrorT SAHPI_API saHpiHotSwapControlRequest (
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaHpiRptEntryT *res;
        struct oh_resource_data *rd;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        rd = oh_get_resource_data(&(d->rpt), ResourceId);
        if (!rd) {
                dbg("Cannot find resource data for Resource %d", ResourceId);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rd->controlled = 1;

        return SA_OK;
}

#endif

SaErrorT SAHPI_API saHpiHotSwapPolicyCancel (
        SAHPI_IN SaHpiSessionIdT      SessionId,
        SAHPI_IN SaHpiResourceIdT     ResourceId)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiResourceActiveSet (
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaErrorT rv;
        SaErrorT (*set_hotswap_state)(void *hnd, SaHpiResourceIdT,
                        SaHpiHsStateT state);

        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_resource_data *rd;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        rd = oh_get_resource_data(&(d->rpt), ResourceId);
        if (!rd) {
                dbg( "Can't find resource data for Resource %d", ResourceId);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (!rd->controlled) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        set_hotswap_state = h->abi->set_hotswap_state;
        if (!set_hotswap_state) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        /* this was done in the old code, so we do it here */
        rd->controlled = 0;

        rv = set_hotswap_state(h->hnd, ResourceId, SAHPI_HS_STATE_ACTIVE);


        return rv;
}

SaErrorT SAHPI_API saHpiResourceInactiveSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaErrorT rv;
        SaErrorT (*set_hotswap_state)(void *hnd, SaHpiResourceIdT rid,
                                      SaHpiHsStateT state);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_resource_data *rd;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        rd = oh_get_resource_data(&(d->rpt), ResourceId);
        if (!rd) {
                dbg( "Can't find resource data for Resource %d", ResourceId);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (!rd->controlled) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        set_hotswap_state = h->abi->set_hotswap_state;
        if (!set_hotswap_state) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rd->controlled = 0;

        rv = set_hotswap_state(h->hnd, ResourceId, SAHPI_HS_STATE_INACTIVE);

        return rv;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_OUT SaHpiTimeoutT *Timeout)
{

        SaHpiDomainIdT did;
        
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        *Timeout = get_hotswap_auto_insert_timeout();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutSet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiTimeoutT Timeout)
{
        SaHpiDomainIdT did;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        set_hotswap_auto_insert_timeout(Timeout);

        return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiTimeoutT *Timeout)
{
        SaHpiRptEntryT *res;
        struct oh_resource_data *rd;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        rd = oh_get_resource_data(&(d->rpt), ResourceId);
        if (!rd) {
                dbg("Cannot find resource data for Resource %d", ResourceId);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        *Timeout = rd->auto_extract_timeout;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutSet(
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiTimeoutT Timeout)
{
        SaHpiRptEntryT *res;
        struct oh_resource_data *rd;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        rd = oh_get_resource_data(&(d->rpt), ResourceId);
        if (!rd) {
                dbg("Cannot find resource data for Resource %d", ResourceId);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rd->auto_extract_timeout = Timeout;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiHotSwapStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiHsStateT *State)
{
        SaErrorT rv;
        SaErrorT (*get_hotswap_state)(void *hnd, SaHpiResourceIdT rid,
                                 SaHpiHsStateT *state);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_hotswap_state = h->abi->get_hotswap_state;
        if (!get_hotswap_state) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_hotswap_state(h->hnd, ResourceId, State);

        return rv;
}

SaErrorT SAHPI_API saHpiHotSwapActionRequest (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiHsActionT Action)
{
        SaErrorT rv;
        SaErrorT (*request_hotswap_action)(void *hnd, SaHpiResourceIdT rid,
                        SaHpiHsActionT act);

        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        request_hotswap_action = h->abi->request_hotswap_action;
        if (!request_hotswap_action) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = request_hotswap_action(h->hnd, ResourceId, Action);

        get_events(&(d->rpt));

        return rv;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiHsIndicatorStateT *State)
{
        SaErrorT rv;
        SaErrorT (*get_indicator_state)(void *hnd, SaHpiResourceIdT id,
                                        SaHpiHsIndicatorStateT *state);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_indicator_state = h->abi->get_indicator_state;
        if (!get_indicator_state) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_indicator_state(h->hnd, ResourceId, State);

        return rv;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiHsIndicatorStateT State)
{
        SaErrorT rv;
        SaErrorT (*set_indicator_state)(void *hnd, SaHpiResourceIdT id,
                                        SaHpiHsIndicatorStateT state);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        set_indicator_state = h->abi->set_indicator_state;
        if (!set_indicator_state) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_indicator_state(h->hnd, ResourceId, State);

        return rv;
}

/*******************************************************************************
 *
 *  Configuration Function(s)
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiParmControl (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiParmActionT Action)
{
        SaErrorT rv;
        SaErrorT (*control_parm)(void *, SaHpiResourceIdT, SaHpiParmActionT);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        control_parm = h->abi->control_parm;
        if (!control_parm) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = control_parm(h->hnd, ResourceId, Action);

        return rv;
}

/*******************************************************************************
 *
 *  Reset Functions
 *
 ******************************************************************************/


SaErrorT SAHPI_API saHpiResourceResetStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiResetActionT *ResetAction)
{
        SaErrorT rv;
        SaErrorT (*get_func)(void *, SaHpiResourceIdT, SaHpiResetActionT *);

        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_RESET)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_func = h->abi->get_reset_state;
        if (!get_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, ResetAction);

        return rv;
}

SaErrorT SAHPI_API saHpiResourceResetStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiResetActionT ResetAction)
{
        SaErrorT rv;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiResetActionT);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_RESET)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        set_func = h->abi->set_reset_state;
        if (!set_func) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_func(h->hnd, ResourceId, ResetAction);

        return rv;
}

/*******************************************************************************
 *
 *  Power Functions
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiResourcePowerStateGet (
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_OUT SaHpiPowerStateT *State)
{
        SaErrorT rv;
        SaErrorT (*get_power_state)(void *hnd, SaHpiResourceIdT id,
                               SaHpiPowerStateT *state);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        get_power_state = h->abi->get_power_state;
        if (!get_power_state) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_power_state(h->hnd, ResourceId, State);

        return rv;
}

SaErrorT SAHPI_API saHpiResourcePowerStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiPowerStateT State)
{
        SaErrorT rv;
        SaErrorT (*set_power_state)(void *hnd, SaHpiResourceIdT id,
                                    SaHpiPowerStateT state);
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d);
        OH_RESOURCE_GET(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);

        set_power_state = h->abi->set_power_state;
        if (!set_power_state) {
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_power_state(h->hnd, ResourceId, State);

        return rv;
}


