/*
 * (C) Copyright IBM Corp. 2003
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
 *
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id$
 *
 */
#ifndef SAHPIRDRTABLE_H
#define SAHPIRDRTABLE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
#include <SaHpi.h>
#include <hpiSubagent.h>

  /*
   * Number of index values in this table.
   * Consult the HPI-MIB
   *
   * If this number changes, look in the src code for this 
   * define, and make sure to add/remove the new index value(s).
   */
#define RDR_INDEX_NR 4


  typedef struct saHpiRdrTable_context_s
  {
    netsnmp_index index;

       /** UNSIGNED32 = ASN_UNSIGNED */
    unsigned long saHpiResourceID;

	/** UNSIGNED32 = ASN_UNSIGNED */
    unsigned long saHpiRdrRecordId;

	/** INTEGER = ASN_INTEGER */
    long saHpiRdrType;

	/** OCTETSTR = ASN_OCTET_STR */
    unsigned char saHpiRdrEntityPath[SNMP_MAX_MSG_SIZE];
    long saHpiRdrEntityPath_len;

	/** RowPointer = ASN_OBJECT_ID */
    oid saHpiRdr[MAX_OID_LEN];
    long saHpiRdr_len;

	/** COUNTER = ASN_COUNTER */
    // It is  the child_id.
    unsigned long saHpiRdrId;

	/** RowPointer = ASN_OBJECT_ID */
    oid saHpiRdrRTP[MAX_OID_LEN];
    long saHpiRdrRTP_len;

	/** OCTETSTR = ASN_OCTET_STR */
    unsigned char saHpiRdrIdString[SNMP_MAX_MSG_SIZE];
    long saHpiRdrIdString_len;

    SaHpiDomainIdT domain_id;
    long hash;
    unsigned int dirty_bit;
  } saHpiRdrTable_context;

/*************************************************************
 * function declarations
 */

  void initialize_table_saHpiRdrTable (void);

  int saHpiRdrTable_get_value (netsnmp_request_info *,
			       netsnmp_index *, netsnmp_table_request_info *);

  int populate_rdr (SaHpiRptEntryT * rpt_entry,
		    oid * rpt_oid, size_t rpt_oid_len,
		    oid * resource_oid, size_t resource_oid_len);

  unsigned long purge_rdr (void);
  int
    delete_rdr_row (SaHpiDomainIdT domain_id,
		    SaHpiResourceIdT resource_id,
		    SaHpiEntryIdT num, SaHpiRdrTypeT type);


/*************************************************************
 * oid declarations
 */
//    extern oid      saHpiRdrTable_oid[];
  //   extern size_t   saHpiRdrTable_oid_len;
//1,3,6,1,3,90,3,2
#define saHpiRdrTable_TABLE_OID hpiResources_OID,2


/*************************************************************
 * column number definitions for table saHpiRdrTable
 */
#define COLUMN_SAHPIRDRRESOURCEID 1
#define COLUMN_SAHPIRDRRECORDID 1
#define COLUMN_SAHPIRDRTYPE 2
#define COLUMN_SAHPIRDRENTITYPATH 3
#define COLUMN_SAHPIRDR 4
#define COLUMN_SAHPIRDRID 5
#define COLUMN_SAHPIRDRRTP 6
#define COLUMN_SAHPIRDRIDSTRING 7
#define saHpiRdrTable_COL_MIN 1
#define saHpiRdrTable_COL_MAX 7

#define SCALAR_COLUMN_SAHPIRDRCOUNT 1


  int saHpiRdrTable_extract_index (saHpiRdrTable_context *
				   ctx, netsnmp_index * hdr);


  saHpiRdrTable_context *saHpiRdrTable_create_row (netsnmp_index *);

  netsnmp_index *saHpiRdrTable_delete_row (saHpiRdrTable_context *);


#ifdef __cplusplus
};
#endif

#endif /** SAHPIRDRTABLE_H */
