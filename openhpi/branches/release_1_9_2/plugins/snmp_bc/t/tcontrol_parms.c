/* -*- linux-c -*-
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
 *     Peter D Phan <pdphan@users.sourceforge.net>
 */


#include <snmp_bc_plugin.h>
#include <sahpimacros.h>
#include <tsetup.h>

int main(int argc, char **argv) 
{

	/* ************************
	 * Local variables
	 * ***********************/	 
	int testfail = 0;
	SaHpiResourceIdT  id;
	SaHpiParmActionT  act;
	SaErrorT          err;
	SaErrorT expected_err;
        SaHpiDomainIdT did;
        struct oh_domain *d;					

        SaHpiSessionIdT sessionid;
	 
	/* ************************	 	 
	 * Find a resource with Control type rdr
	 * ***********************/
        struct oh_handler l_handler;
	struct oh_handler *h= &l_handler;
        SaHpiRptEntryT rptentry;
	
	err = tsetup(&sessionid);
	if (err != SA_OK) {
		printf("Error! bc_control_parm, can not setup test environment\n");
		return -1;

	}
	err = tfind_resource(&sessionid, (SaHpiCapabilitiesT) SAHPI_CAPABILITY_CONTROL, h, &rptentry);
	if (err != SA_OK) {
		printf("Error! bc_control_parm, can not setup test environment\n");
		err = tcleanup(&sessionid);
		return -1;

	}

	/************************** 
	 * Test 1: Invalid Control Action
	 **************************/
	id = rptentry.ResourceId;
	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	act = 0xFF;
																																														
	err = snmp_bc_control_parm((void *)h->hnd, id, act);
	checkstatus(&err, &expected_err, &testfail);
	
	/************************** 
	 * Test 2: Invalid ResourceId
	 **************************/
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;
	act = SAHPI_RESTORE_PARM;
	id = 5000; /* set it way out */

	err = snmp_bc_control_parm((void *)h->hnd, id, act);
	checkstatus(&err, &expected_err, &testfail);

	id = rptentry.ResourceId;
	struct oh_handler_state *handle = (struct oh_handler_state *)h->hnd;		
	/************************** 
	 * Test 3: 
	 *************************/
        OH_GET_DID(sessionid, did);
	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities |= SAHPI_CAPABILITY_CONFIGURATION;  
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */
	
	expected_err = SA_ERR_HPI_INTERNAL_ERROR;

	err = snmp_bc_control_parm((void *)h->hnd, id, act);
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test 4: 
	 **************************/

	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities &=  !SAHPI_CAPABILITY_CONFIGURATION;
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */

	expected_err = SA_ERR_HPI_CAPABILITY;

	err = snmp_bc_control_parm((void *)h->hnd, id, act);
	checkstatus(&err, &expected_err, &testfail);
	
	/***************************
	 * Cleanup after all tests
	 ***************************/

	 err = tcleanup(&sessionid);
	 return testfail;

}

#include <tsetup.c>
