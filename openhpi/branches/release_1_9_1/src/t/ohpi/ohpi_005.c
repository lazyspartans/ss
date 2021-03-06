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
 * Authors:
 *     Renier Morales <renierm@users.sf.net>
 */
 
#include <stdlib.h>
#include <SaHpi.h>
#include <oHpi.h>

/**
 * Load 'libdummy', create two handlers, get plugin info and
 * compare with known value (3), destroy handlers, get info again
 * and compare with known value (1), unload plugin.
 * Pass on sucess, otherwise failure.
 **/
 
int main(int argc, char **argv)
{
        SaHpiSessionIdT sid = 0;
        char *config_file = NULL;
        oHpiHandlerIdT hid0, hid1;
        GHashTable *h0 = g_hash_table_new(g_str_hash, g_str_equal),
                   *h1 = g_hash_table_new(g_str_hash, g_str_equal);
        oHpiPluginInfoT pinfo;
        
        /* Save config file env variable and unset it */
        config_file = getenv("OPENHPI_CONF");
        setenv("OPENHPI_CONF","./noconfig", 1);
        
        if (saHpiSessionOpen(1, &sid, NULL))
                return -1;
                    
        if (oHpiPluginLoad("libdummy"))
                return -1;
                
        /* Set configuration for two handlers and create them. */
        g_hash_table_insert(h0, "plugin", "libdummy");
        g_hash_table_insert(h0, "entity_root", "{SYSTEM_CHASSIS,1}");
        g_hash_table_insert(h0, "name", "test0");
        g_hash_table_insert(h0, "addr", "0");
        
        g_hash_table_insert(h1, "plugin", "libdummy");
        g_hash_table_insert(h1, "entity_root", "{SYSTEM_CHASSIS,2}");
        g_hash_table_insert(h1, "name", "test1");
        g_hash_table_insert(h1, "addr", "1");
                
        if (oHpiHandlerCreate(h0,&hid0) || oHpiHandlerCreate(h1,&hid1))
                return -1;
                
        if (oHpiPluginInfo("libdummy",&pinfo))
                return -1;
                
        if (pinfo.refcount != 3)
                return -1;
                
        if (oHpiHandlerDestroy(hid0) || oHpiHandlerDestroy(hid1))
                return -1;
                
        if (oHpiPluginInfo("libdummy",&pinfo))
                return -1;
                
        if (pinfo.refcount != 1)
                return -1;
                
        /* Restore config file env variable */
        setenv("OPENHPI_CONF",config_file,1);                           
        
        return oHpiPluginUnload("libdummy");
}
