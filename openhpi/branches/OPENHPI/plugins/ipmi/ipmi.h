/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
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
 *     Kevin Gao <kevin.gao@linux.intel.com>
 *     Rusty Lynch <rusty.lynch@linux.intel.com>
 */

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <OpenIPMI/ipmiif.h>
#include <OpenIPMI/ipmi_sel.h>
#include <OpenIPMI/ipmi_smi.h>
#include <OpenIPMI/ipmi_err.h>
#include <OpenIPMI/ipmi_auth.h>
#include <OpenIPMI/ipmi_lan.h>
#include <OpenIPMI/selector.h>
#include <OpenIPMI/ipmi_int.h>
#include <OpenIPMI/os_handler.h>
#include <OpenIPMI/ipmi_domain.h>

#include <SaHpi.h>
#include <openhpi.h>

/* OpenIPMI defines the selector */
extern selector_t       *ui_sel;

struct ohoi_handler {
	int SDRs_read_done;
        int SELs_read_done;
	ipmi_domain_id_t domain_id;
};

struct ohoi_resource_id {
        enum {
                OHOI_RESOURCE_ENTITY = 0,
                OHOI_RESOURCE_MC
        } type;
        union {
                ipmi_entity_id_t entity_id;
                ipmi_mcid_t      mc_id;
        } u;
};


char *entity_root;

/* implemented in ipmi_event.c */
void ohoi_setup_done(ipmi_domain_t *domain, int err, unsigned int  conn_num,
		unsigned int  port_num, int still_connected, void *user_data);

/* implemented in ipmi_sensor.c	*/
int ohoi_get_sensor_data(ipmi_sensor_id_t sensor_id, SaHpiSensorReadingT *data);
int ohoi_get_sensor_thresholds(ipmi_sensor_id_t sensor_id, SaHpiSensorThresholdsT *thres);
int ohoi_set_sensor_thresholds(ipmi_sensor_id_t                 sensor_id, 
                               const SaHpiSensorThresholdsT     *thres);
int ohoi_get_sensor_event_enables(ipmi_sensor_id_t              sensor_id,
			          SaHpiSensorEvtEnablesT        *enables);
int ohoi_set_sensor_event_enables(ipmi_sensor_id_t              sensor_id,
			          const SaHpiSensorEvtEnablesT  *enables);

void ohoi_get_sel_time(ipmi_mcid_t mc_id, SaHpiTimeT *time);
void ohoi_set_sel_time(ipmi_mcid_t mc_id, const struct timeval *time);
void ohoi_get_sel_updatetime(ipmi_mcid_t mc_id, SaHpiTimeT *time);
void ohoi_get_sel_count(ipmi_mcid_t mc_id, int *count);
void ohoi_get_sel_overflow(ipmi_mcid_t mc_id, char *overflow);
void ohoi_get_sel_support_del(ipmi_mcid_t mc_id, char *support_del);
SaErrorT ohoi_clear_sel(ipmi_mcid_t mc_id);
void ohoi_get_sel_first_entry(ipmi_mcid_t mc_id, ipmi_event_t *event);
void ohoi_get_sel_last_entry(ipmi_mcid_t mc_id, ipmi_event_t *event);
void ohoi_get_sel_next_recid(ipmi_mcid_t mc_id, 
                             const ipmi_event_t *event,
                             unsigned int *record_id);
void ohoi_get_sel_prev_recid(ipmi_mcid_t mc_id, 
                             const ipmi_event_t *event, 
                             unsigned int *record_id);
void ohoi_get_sel_by_recid(ipmi_mcid_t mc_id, SaHpiSelEntryIdT entry_id, ipmi_event_t *event);

/* This is used to help plug-in to find resource in rptcache by entity_id */
SaHpiRptEntryT *ohoi_get_resource_by_entityid(RPTable                *table,
                                              const ipmi_entity_id_t *entity_id);

/* This is used for OpenIPMI to notice sensor change */
void ohoi_sensor_event(enum ipmi_update_e op,
                       ipmi_entity_t      *ent,
                       ipmi_sensor_t      *sensor,
                       void               *cb_data);

/* This is used for OpenIPMI to notice control change */
void ohoi_control_event(enum ipmi_update_e op,
                        ipmi_entity_t      *ent,
                        ipmi_control_t     *control,
                        void		   *cb_data);

/* This is used for OpenIPMI to notice mc change */
void ohoi_mc_event(enum ipmi_update_e op,
                   ipmi_domain_t      *domain,
                   ipmi_mc_t          *mc,
                   void               *cb_data);

/* This is used for OpenIPMI to noice inventroy change */
void ohoi_inventory_event(enum ipmi_update_e    op,
                          ipmi_entity_t         *entity,
                          void                  *cb_data);

/* This is used for OpenIPMI to notice entity change */
void ohoi_entity_event(enum ipmi_update_e       op,
                       ipmi_domain_t            *domain,
                       ipmi_entity_t            *entity,
                       void                     *cb_data);

int ohoi_loop(int *done_flag);
/**
 * loop_indicator_cb:
 * @cb_data: callback data
 *
 * Use to indicate if the loop 
 * can end
 *
 * Return value: non-zero means 
 * end.
 **/
typedef int (*loop_indicator_cb)(const void *cb_data);
int ohoi_loop_until(loop_indicator_cb indicator, const void *cb_data, int timeout); 

/*
 * ABI stub functions
 */ 
SaErrorT ohoi_get_inventory_size(void *hnd, SaHpiResourceIdT id,
                          SaHpiEirIdT num, /* yes, they don't call it a
                                            * num, but it still is one
                                            */
                          SaHpiUint32T *size);

SaErrorT ohoi_get_inventory_info(void *hnd, SaHpiResourceIdT id,
                          SaHpiEirIdT num,
                          SaHpiInventoryDataT *data);

SaErrorT ohoi_get_hotswap_state(void *hnd, SaHpiResourceIdT id, 
                                SaHpiHsStateT *state);

SaErrorT ohoi_set_hotswap_state(void *hnd, SaHpiResourceIdT id, 
                                SaHpiHsStateT state);

SaErrorT ohoi_request_hotswap_action(void *hnd, SaHpiResourceIdT id, 
                                     SaHpiHsActionT act);

SaErrorT ohoi_get_indicator_state(void *hnd, SaHpiResourceIdT id, 
                                  SaHpiHsIndicatorStateT *state);

SaErrorT ohoi_set_indicator_state(void *hnd, SaHpiResourceIdT id, 
				  SaHpiHsIndicatorStateT state);

/* misc macros for debug */
#define dump_entity_id(s, x) \
        do { \
                dbg("%s domain id: %p, entity id: %x, entity instance: %x, channel: %x, address: %x, seq: %lx", \
                     s,                         \
                     (x).domain_id.domain,      \
                     (x).entity_id,             \
                     (x).entity_instance,       \
                     (x).channel,               \
                     (x).address,               \
                     (x).seq);                  \
        } while(0)

