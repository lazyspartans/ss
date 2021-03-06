/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.18.2.2 2004/02/09 18:21:47 rstory Exp $
 *
 * $Id$
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef SAHPIUSEREVENTTABLE_H
#define SAHPIUSEREVENTTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

        /** Index saHpiDomainId is external */
        /** Index saHpiEventSeverity is external */
        /** Index saHpiUserEventEntryId is internal */

typedef struct saHpiUserEventTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
    /** TODO: add storage for external index(s)! */
        /** SaHpiEntryId = ASN_UNSIGNED */
            unsigned long saHpiUserEventEntryId;

        /** SaHpiTime = ASN_COUNTER64 */
    /** TODO: Is this type correct? */
            long saHpiUserEventTimestamp;

        /** SaHpiTextType = ASN_INTEGER */
            long saHpiUserEventTextType;

        /** SaHpiTextLanguage = ASN_INTEGER */
            long saHpiUserEventTextLanguage;

        /** SaHpiText = ASN_OCTET_STR */
            unsigned char saHpiUserEventText[65535];
            long saHpiUserEventText_len;

        /** RowStatus = ASN_INTEGER */
            long saHpiUserEventRowStatus;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */

} saHpiUserEventTable_context;

/*************************************************************
 * function declarations
 */
void init_saHpiUserEventTable(void);
void initialize_table_saHpiUserEventTable(void);
const saHpiUserEventTable_context * saHpiUserEventTable_get_by_idx(netsnmp_index *);
const saHpiUserEventTable_context * saHpiUserEventTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int saHpiUserEventTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);


/*************************************************************
 * oid declarations
 */
extern oid saHpiUserEventTable_oid[];
extern size_t saHpiUserEventTable_oid_len;

#define saHpiUserEventTable_TABLE_OID 1,3,6,1,4,1,18568,2,1,1,3,1,30
    
/*************************************************************
 * column number definitions for table saHpiUserEventTable
 */
#define COLUMN_SAHPIUSEREVENTENTRYID 1
#define COLUMN_SAHPIUSEREVENTTIMESTAMP 2
#define COLUMN_SAHPIUSEREVENTTEXTTYPE 3
#define COLUMN_SAHPIUSEREVENTTEXTLANGUAGE 4
#define COLUMN_SAHPIUSEREVENTTEXT 5
#define COLUMN_SAHPIUSEREVENTROWSTATUS 6
#define saHpiUserEventTable_COL_MIN 2
#define saHpiUserEventTable_COL_MAX 6

/* comment out the following line if you don't handle SET-REQUEST for saHpiUserEventTable */
#define saHpiUserEventTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define saHpiUserEventTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define saHpiUserEventTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define saHpiUserEventTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef saHpiUserEventTable_SET_HANDLING

int saHpiUserEventTable_extract_index( saHpiUserEventTable_context * ctx, netsnmp_index * hdr );

void saHpiUserEventTable_set_reserve1( netsnmp_request_group * );
void saHpiUserEventTable_set_reserve2( netsnmp_request_group * );
void saHpiUserEventTable_set_action( netsnmp_request_group * );
void saHpiUserEventTable_set_commit( netsnmp_request_group * );
void saHpiUserEventTable_set_free( netsnmp_request_group * );
void saHpiUserEventTable_set_undo( netsnmp_request_group * );

saHpiUserEventTable_context * saHpiUserEventTable_duplicate_row( saHpiUserEventTable_context* );
netsnmp_index * saHpiUserEventTable_delete_row( saHpiUserEventTable_context* );

int saHpiUserEventTable_can_activate(saHpiUserEventTable_context *undo_ctx,
                      saHpiUserEventTable_context *row_ctx,
                      netsnmp_request_group * rg);
int saHpiUserEventTable_can_deactivate(saHpiUserEventTable_context *undo_ctx,
                        saHpiUserEventTable_context *row_ctx,
                        netsnmp_request_group * rg);
int saHpiUserEventTable_can_delete(saHpiUserEventTable_context *undo_ctx,
                    saHpiUserEventTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
#ifdef saHpiUserEventTable_ROW_CREATION
saHpiUserEventTable_context * saHpiUserEventTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef saHpiUserEventTable_IDX2
saHpiUserEventTable_context * saHpiUserEventTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** SAHPIUSEREVENTTABLE_H */
