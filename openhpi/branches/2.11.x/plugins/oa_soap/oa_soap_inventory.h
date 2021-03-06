/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Raghavendra M.S. <raghavendra.ms@hp.com>
 */

#ifndef _OA_SOAP_INVENTORY_H
#define _OA_SOAP_INVENTORY_H

/* Include files */
#include "oa_soap.h"

/* cClass resource inventory string */
#define ENCLOSURE_INVENTORY_STRING "Enclosure Inventory"
#define OA_INVENTORY_STRING "OA Inventory"
#define SERVER_INVENTORY_STRING "Server Inventory"
#define INTERCONNECT_INVENTORY_STRING "Interconnect Inventory"
#define FAN_INVENTORY_STRING "Fan Inventory"
#define POWER_SUPPLY_INVENTORY_STRING "Power Supply Inventory"
#define POWER_SUPPLY_RDR_STRING "Power Supply"

/* Inventory data field structure */
struct oa_soap_field
{
        SaHpiIdrFieldT      field;
        struct oa_soap_field *next_field;
};

/* Inventory data area structure */
struct oa_soap_area
{
        SaHpiIdrAreaHeaderT idr_area_head;
        struct oa_soap_field *field_list;
        struct oa_soap_area *next_area;
};

struct oa_soap_inventory_info
{
        SaHpiIdrInfoT       idr_info;
        struct oa_soap_area  *area_list;
};

/* Inventory data respository header structure */
struct oa_soap_inventory
{
        SaHpiInventoryRecT        inv_rec;
        struct oa_soap_inventory_info info;
        char *comment;
};

/* Inventory function declarations */
SaErrorT oa_soap_get_idr_info(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiIdrIdT idr_id,
                              SaHpiIdrInfoT *idr_info);

SaErrorT oa_soap_get_idr_area_header(void *oh_handler,
                                    SaHpiResourceIdT resource_id,
                                    SaHpiIdrIdT idr_id,
                                    SaHpiIdrAreaTypeT area_type,
                                    SaHpiEntryIdT area_id,
                                    SaHpiEntryIdT *next_area_id,
                                    SaHpiIdrAreaHeaderT *area_header);

SaErrorT oa_soap_add_idr_area(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiIdrIdT idr_id,
                              SaHpiIdrAreaTypeT area_type,
                              SaHpiEntryIdT *area_id);

SaErrorT oa_soap_add_idr_area_by_id(void *oh_handler,
                                    SaHpiResourceIdT resource_id,
                                    SaHpiIdrIdT idr,
                                    SaHpiIdrAreaTypeT area_type,
                                    SaHpiEntryIdT area_id);

SaErrorT oa_soap_del_idr_area(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiIdrIdT idr_id,
                              SaHpiEntryIdT area_id);

SaErrorT oa_soap_get_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr_id,
                               SaHpiEntryIdT area_id,
                               SaHpiIdrFieldTypeT field_type,
                               SaHpiEntryIdT field_id,
                               SaHpiEntryIdT *next_field_id,
                               SaHpiIdrFieldT *field);

SaErrorT oa_soap_add_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr_id,
                               SaHpiIdrFieldT *field);

SaErrorT oa_soap_add_idr_field_by_id(void *oh_handler,
                                     SaHpiResourceIdT resource_id,
                                     SaHpiIdrIdT idr_id,
                                     SaHpiIdrFieldT *field);

SaErrorT oa_soap_set_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr_id,
                               SaHpiIdrFieldT *field);

SaErrorT oa_soap_del_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr_id,
                               SaHpiEntryIdT area_id,
                               SaHpiEntryIdT field_id);

SaErrorT build_enclosure_inv_rdr(struct oh_handler_state *oh_handler,
                                 struct enclosureInfo *response,
                                 SaHpiRdrT *rdr,
                                 struct oa_soap_inventory **pinv);

SaErrorT build_oa_inv_rdr(struct oh_handler_state *oh_handler,
                          struct oaInfo *response,
                          SaHpiRdrT *rdr,
                          struct oa_soap_inventory **pinv);

SaErrorT build_server_inv_rdr(struct oh_handler_state *oh_handler,
                              SOAP_CON *con,
                              SaHpiInt32T bay_number,
                              SaHpiRdrT *rdr,
                              struct oa_soap_inventory **pinv);

SaErrorT build_inserted_server_inv_rdr(struct oh_handler_state *oh_handler,
                                       SaHpiInt32T bay_number,
                                       SaHpiRdrT *rdr,
                                       struct oa_soap_inventory **inventory);

SaErrorT build_server_inventory(struct bladeInfo *response,
                                SaHpiRdrT *rdr,
                                struct oa_soap_inventory **inventory);


SaErrorT build_interconnect_inv_rdr(struct oh_handler_state *oh_handler,
                                    SOAP_CON *con,
                                    SaHpiInt32T bay_number,
                                    SaHpiRdrT *rdr,
                                    struct oa_soap_inventory **pinv);

SaErrorT build_fan_inv_rdr(struct oh_handler_state *oh_handler,
                           struct fanInfo *response,
                           SaHpiRdrT *rdr,
                           struct oa_soap_inventory **pinv);

SaErrorT build_power_inv_rdr(struct oh_handler_state *oh_handler,
                             struct powerSupplyInfo *response,
                             SaHpiRdrT *rdr,
                             struct oa_soap_inventory **pinv);

SaErrorT build_server_inventory_area(struct bladeInfo *response,
                                     SaHpiRdrT *rdr,
                                     struct oa_soap_inventory **inventory);

SaErrorT add_product_area(struct oa_soap_area **parea,
                          char *name,
                          char *manufacturer,
                          SaHpiInt32T *success_flag);

SaErrorT add_chassis_area(struct oa_soap_area **parea,
                          char *part_number,
                          char *serial_number,
                          SaHpiInt32T *success_flag);

SaErrorT add_board_area(struct oa_soap_area **parea,
                        char *part_number,
                        char *serial_number,
                        SaHpiInt32T *success_flag);

SaErrorT add_internal_area(struct oa_soap_area **parea,
                           char *manufacturer,
                           char *name,
                           char *part_number,
                           char *serial_number,
                           SaHpiInt32T *success_flag);

SaErrorT  idr_area_add(struct oa_soap_area **area_ptr,
                       SaHpiIdrAreaTypeT area_type,
                       struct oa_soap_area **return_area);

SaErrorT  idr_area_add_by_id(struct oa_soap_area **head_area,
                             SaHpiIdrAreaTypeT area_type,
                             SaHpiEntryIdT area_id);

SaErrorT idr_area_delete(struct oa_soap_area **area_ptr,
                         SaHpiEntryIdT area_id);

SaErrorT fetch_idr_area_header(struct oa_soap_inventory_info *inv_ptr,
                             SaHpiEntryIdT area_id,
                             SaHpiIdrAreaTypeT area_type,
                             SaHpiIdrAreaHeaderT *area_header,
                             SaHpiEntryIdT *next_area_id);

SaErrorT  idr_field_add(struct oa_soap_field **field_ptr,
                        SaHpiIdrFieldT *field);

SaErrorT  idr_field_add_by_id(struct oa_soap_field **head_field,
                              SaHpiEntryIdT area_id,
                              SaHpiIdrFieldTypeT field_type,
                              char *field_data,
                              SaHpiEntryIdT field_id);

SaErrorT idr_field_delete(struct oa_soap_field **field_ptr,
                          SaHpiEntryIdT field_id);

SaErrorT idr_field_update(struct oa_soap_field *field_ptr,
                          SaHpiIdrFieldT *field);

SaErrorT fetch_idr_field(struct oa_soap_inventory_info *inv_ptr,
                       SaHpiEntryIdT area_id,
                       SaHpiIdrFieldTypeT field_type,
                       SaHpiEntryIdT field_id,
                       SaHpiEntryIdT *next_field_id,
                       SaHpiIdrFieldT *field);

SaErrorT free_inventory_info(struct oh_handler_state *handler,
             SaHpiResourceIdT resource_id);

#endif
