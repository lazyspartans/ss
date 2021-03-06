/*
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *   Konrad Rzeszutek <konradr@us.ibm.com>

 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.18 2003/11/07 17:03:52 rstory Exp $
 *
 * $Id$
 *
 */
#ifndef SAHPISENSORREADINGCURRENTTABLE_H
#define SAHPISENSORREADINGCURRENTTABLE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
#include <SaHpi.h>
#include <hpiSubagent.h>


	/** Index saHpiDomainID is external */
	/** Index saHpiResourceID is external */
	/** Index saHpiSensorIndex is external */

  typedef struct saHpiSensorReadingCurrentTable_context_s
  {
    netsnmp_index index;
	/** INTEGER = ASN_INTEGER */
    long saHpiSensorReadingCurrentValuesPresent;

	/** UNSIGNED32 = ASN_UNSIGNED */
    unsigned long saHpiSensorReadingCurrentRaw;

	/** OCTETSTR = ASN_OCTET_STR */
    unsigned char
      saHpiSensorReadingCurrentInterpreted[SENSOR_READING_INTER_MAX];
    size_t saHpiSensorReadingCurrentInterpreted_len;

	/** INTEGER = ASN_INTEGER */
    long saHpiSensorReadingCurrentStatus;

	/** OCTETSTR = ASN_OCTET_STR */
    unsigned char
      saHpiSensorReadingCurrentEventStatus[SENSOR_READING_EVENT_MAX];
    size_t saHpiSensorReadingCurrentEventStatus_len;
   
    SaHpiEventCategoryT sensor_category;
    long sensor_id;
    long resource_id;
    long domain_id;
    long hash;


  } saHpiSensorReadingCurrentTable_context;

/*************************************************************
 * function declarations
 */

  void initialize_table_saHpiSensorReadingCurrentTable (void);

  int
    delete_ReadingCurrent_row (SaHpiDomainIdT domain_id,
			       SaHpiResourceIdT resource_id,
			       SaHpiSensorNumT num);

  int populate_ReadingCurrent (SaHpiDomainIdT domain_id,
			       SaHpiResourceIdT resource_id,
			       SaHpiSensorNumT sensor_id,
			       SaHpiEventCategoryT sensor_category,
			       SaHpiSensorReadingT * reading);


  int
    saHpiSensorReadingCurrentTable_get_value (netsnmp_request_info *,
					      netsnmp_index *,
					      netsnmp_table_request_info *);


/*************************************************************
 * oid declarations
 */
  extern oid saHpiSensorReadingCurrentTable_oid[];
  extern size_t saHpiSensorReadingCurrentTable_oid_len;

#define saHpiSensorReadingCurrentTable_TABLE_OID hpiSensor_OID,1

/*************************************************************
 * column number definitions for table saHpiSensorReadingCurrentTable
 */
#define COLUMN_SAHPISENSORREADINGCURRENTVALUESPRESENT 1
#define COLUMN_SAHPISENSORREADINGCURRENTRAW 2
#define COLUMN_SAHPISENSORREADINGCURRENTINTERPRETED 3
#define COLUMN_SAHPISENSORREADINGCURRENTSTATUS 4
#define COLUMN_SAHPISENSORREADINGCURRENTEVENTSTATUS 5
#define saHpiSensorReadingCurrentTable_COL_MIN 1
#define saHpiSensorReadingCurrentTable_COL_MAX 5

  int
    saHpiSensorReadingCurrentTable_extract_index
    (saHpiSensorReadingCurrentTable_context * ctx, netsnmp_index * hdr);

  void saHpiSensorReadingCurrentTable_set_reserve1 (netsnmp_request_group *);
  void saHpiSensorReadingCurrentTable_set_reserve2 (netsnmp_request_group *);
  void saHpiSensorReadingCurrentTable_set_action (netsnmp_request_group *);
  void saHpiSensorReadingCurrentTable_set_commit (netsnmp_request_group *);
  void saHpiSensorReadingCurrentTable_set_free (netsnmp_request_group *);
  void saHpiSensorReadingCurrentTable_set_undo (netsnmp_request_group *);

    saHpiSensorReadingCurrentTable_context
    * saHpiSensorReadingCurrentTable_duplicate_row
    (saHpiSensorReadingCurrentTable_context *);
    netsnmp_index
    * saHpiSensorReadingCurrentTable_delete_row
    (saHpiSensorReadingCurrentTable_context *);

  int
    saHpiSensorReadingCurrentTable_can_delete
    (saHpiSensorReadingCurrentTable_context * undo_ctx,
     saHpiSensorReadingCurrentTable_context * row_ctx,
     netsnmp_request_group * rg);



    saHpiSensorReadingCurrentTable_context
    * saHpiSensorReadingCurrentTable_create_row (netsnmp_index *);

#ifdef __cplusplus
};
#endif

#endif /** SAHPISENSORREADINGCURRENTTABLE_H */
