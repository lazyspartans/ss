/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.18.2.2 2004/02/09 18:21:47 rstory Exp $
 *
 * $Id$
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef SAHPIANNOUNCEMENTTABLE_H
#define SAHPIANNOUNCEMENTTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

        /** Index saHpiDomainId is external */
        /** Index saHpiResourceId is external */
        /** Index saHpiDomainAlarmId is external */
        /** Index saHpiEntryId is external */

typedef struct saHpiAnnouncementTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
    /** TODO: add storage for external index(s)! */
        /** SaHpiTime = ASN_COUNTER64 */
    /** TODO: Is this type correct? */
            long saHpiAnnuoncementTimestamp;

        /** TruthValue = ASN_INTEGER */
            long saHpiAnnuoncementAddedByUser;

        /** INTEGER = ASN_INTEGER */
            long saHpiAnnouncementSeverity;

        /** TruthValue = ASN_INTEGER */
            long saHpiAnnuoncementAcknowledged;

        /** INTEGER = ASN_INTEGER */
            long saHpiAnnouncementStatusCondType;

        /** OCTETSTR = ASN_OCTET_STR */
            unsigned char saHpiAnnouncementEntityPath[65535];
            long saHpiAnnouncementEntityPath_len;

        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long saHpiAnnouncementSensorNum;

        /** SaHpiEventState = ASN_OCTET_STR */
            unsigned char saHpiAnnouncementEventState[65535];
            long saHpiAnnouncementEventState_len;

        /** OCTETSTR = ASN_OCTET_STR */
            unsigned char saHpiAnnouncementName[65535];
            long saHpiAnnouncementName_len;

        /** SaHpiManufacturerId = ASN_UNSIGNED */
            unsigned long saHpiAnnouncementMid;

        /** SaHpiTextType = ASN_INTEGER */
            long saHpiAnnouncementTextType;

        /** SaHpiTextLanguage = ASN_INTEGER */
            long saHpiAnnouncementTextLanguage;

        /** SaHpiText = ASN_OCTET_STR */
            unsigned char saHpiAnnouncementText[65535];
            long saHpiAnnouncementText_len;

        /** RowStatus = ASN_INTEGER */
            long saHpiAnnouncementDelete;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */

} saHpiAnnouncementTable_context;

/*************************************************************
 * function declarations
 */
void init_saHpiAnnouncementTable(void);
void initialize_table_saHpiAnnouncementTable(void);
const saHpiAnnouncementTable_context * saHpiAnnouncementTable_get_by_idx(netsnmp_index *);
const saHpiAnnouncementTable_context * saHpiAnnouncementTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int saHpiAnnouncementTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);


/*************************************************************
 * oid declarations
 */
extern oid saHpiAnnouncementTable_oid[];
extern size_t saHpiAnnouncementTable_oid_len;

#define saHpiAnnouncementTable_TABLE_OID 1,3,6,1,4,1,18568,1,1,1,6,2,1,22
    
/*************************************************************
 * column number definitions for table saHpiAnnouncementTable
 */
#define COLUMN_SAHPIANNUONCEMENTTIMESTAMP 1
#define COLUMN_SAHPIANNUONCEMENTADDEDBYUSER 2
#define COLUMN_SAHPIANNOUNCEMENTSEVERITY 3
#define COLUMN_SAHPIANNUONCEMENTACKNOWLEDGED 4
#define COLUMN_SAHPIANNOUNCEMENTSTATUSCONDTYPE 5
#define COLUMN_SAHPIANNOUNCEMENTENTITYPATH 6
#define COLUMN_SAHPIANNOUNCEMENTSENSORNUM 7
#define COLUMN_SAHPIANNOUNCEMENTEVENTSTATE 8
#define COLUMN_SAHPIANNOUNCEMENTNAME 9
#define COLUMN_SAHPIANNOUNCEMENTMID 10
#define COLUMN_SAHPIANNOUNCEMENTTEXTTYPE 11
#define COLUMN_SAHPIANNOUNCEMENTTEXTLANGUAGE 12
#define COLUMN_SAHPIANNOUNCEMENTTEXT 13
#define COLUMN_SAHPIANNOUNCEMENTDELETE 14
#define saHpiAnnouncementTable_COL_MIN 1
#define saHpiAnnouncementTable_COL_MAX 14

/* comment out the following line if you don't handle SET-REQUEST for saHpiAnnouncementTable */
#define saHpiAnnouncementTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define saHpiAnnouncementTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define saHpiAnnouncementTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define saHpiAnnouncementTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef saHpiAnnouncementTable_SET_HANDLING

int saHpiAnnouncementTable_extract_index( saHpiAnnouncementTable_context * ctx, netsnmp_index * hdr );

void saHpiAnnouncementTable_set_reserve1( netsnmp_request_group * );
void saHpiAnnouncementTable_set_reserve2( netsnmp_request_group * );
void saHpiAnnouncementTable_set_action( netsnmp_request_group * );
void saHpiAnnouncementTable_set_commit( netsnmp_request_group * );
void saHpiAnnouncementTable_set_free( netsnmp_request_group * );
void saHpiAnnouncementTable_set_undo( netsnmp_request_group * );

saHpiAnnouncementTable_context * saHpiAnnouncementTable_duplicate_row( saHpiAnnouncementTable_context* );
netsnmp_index * saHpiAnnouncementTable_delete_row( saHpiAnnouncementTable_context* );

int saHpiAnnouncementTable_can_activate(saHpiAnnouncementTable_context *undo_ctx,
                      saHpiAnnouncementTable_context *row_ctx,
                      netsnmp_request_group * rg);
int saHpiAnnouncementTable_can_deactivate(saHpiAnnouncementTable_context *undo_ctx,
                        saHpiAnnouncementTable_context *row_ctx,
                        netsnmp_request_group * rg);
int saHpiAnnouncementTable_can_delete(saHpiAnnouncementTable_context *undo_ctx,
                    saHpiAnnouncementTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
#ifdef saHpiAnnouncementTable_ROW_CREATION
saHpiAnnouncementTable_context * saHpiAnnouncementTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef saHpiAnnouncementTable_IDX2
saHpiAnnouncementTable_context * saHpiAnnouncementTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** SAHPIANNOUNCEMENTTABLE_H */