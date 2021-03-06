/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#ifndef __SNMP_BC_HOTSWAP_H
#define __SNMP_BC_HOTSWAP_H

SaErrorT snmp_bc_get_hotswap_state(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiHsStateT *state);

SaErrorT snmp_bc_set_hotswap_state(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiHsStateT state);

SaErrorT snmp_bc_request_hotswap_action(void *hnd,
					SaHpiResourceIdT id,
					SaHpiHsActionT act);

SaErrorT snmp_bc_get_reset_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiResetActionT *act);

SaErrorT snmp_bc_set_reset_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiResetActionT act);

SaErrorT snmp_bc_get_power_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiPowerStateT *state);

SaErrorT snmp_bc_set_power_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiPowerStateT state);

SaErrorT snmp_bc_get_indicator_state(void *hnd,
				     SaHpiResourceIdT id,
				     SaHpiHsIndicatorStateT *state);

SaErrorT snmp_bc_set_indicator_state(void *hnd,
				     SaHpiResourceIdT id,
				     SaHpiHsIndicatorStateT state);

#endif /* SNMP_BC_HOTSWAP_H */
