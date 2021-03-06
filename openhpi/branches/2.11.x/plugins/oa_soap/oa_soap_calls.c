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
 *     Bryan Sutula <Bryan.Sutula@hp.com>
 *     Raghavendra PG <raghavendra.pg@hp.com>
 *     Raghavendra MS <raghavendra.ms@hp.com>
 *     Anand S <S.Anand@hp.com>
 */


#include <stdlib.h>
#include <string.h>
#include <oh_error.h>
#define OA_SOAP_CALLS_FILE              /* Defines ENUM strings in this file */
#include <oa_soap_calls.h>


/* Macros used in this file, to simplify common code */
#define SOAP_PARM_CHECK \
        int     ret; \
        if ((con == NULL) || (request == NULL) || (response == NULL)) { \
                err("NULL parameter"); \
                return -1; \
        }

#define SOAP_PARM_CHECK_NRQ \
        int     ret; \
        if ((con == NULL) || (response == NULL)) { \
                err("NULL parameter"); \
                return -1; \
        }

#define SOAP_PARM_CHECK_NRS \
        if ((con == NULL) || (request == NULL)) { \
                err("NULL parameter"); \
                return -1; \
        }


/* Helper functions used by the main OA SOAP calls for code reuse.  They are
 * not intended to be called by users.
 *
 * Please note that the function comment block format has not been followed
 * for these.  The functions are very repetitive, and their parameters are
 * all predefined structures, well-documented in external documents.  Trying
 * to document that here would duplicate that information, leading to future
 * maintainability problems.
 */

/* parse_xsdBoolean - Parses xsdBoolean strings, returning an hpoa_boolean */
static enum hpoa_boolean parse_xsdBoolean(char *str)
{
        if ((! strcmp(str, "true")) ||
            (! strcmp(str, "1")))
                return(HPOA_TRUE);

        return(HPOA_FALSE);
}

/* parse_eventPid - Parses an eventPid response structure */
static void     parse_eventPid(xmlNode *node, struct eventPid *response)
{
        response->pid = atoi(soap_tree_value(node, "pid"));
}

/* parse_bladeInfo - Parses a bladeInfo response structure */
static void     parse_bladeInfo(xmlNode *node, struct bladeInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->bladeType =
                soap_enum(bladeType_S, soap_tree_value(node, "bladeType"));
        response->width = atoi(soap_tree_value(node, "width"));
        response->height = atoi(soap_tree_value(node, "height"));
        response->name = soap_tree_value(node, "name");
        response->manufacturer = soap_tree_value(node, "manufacturer");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->sparePartNumber = soap_tree_value(node, "sparePartNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->serverName = soap_tree_value(node, "serverName");
        response->uuid = soap_tree_value(node, "uuid");
        response->rbsuOsName = soap_tree_value(node, "rbsuOsName");
        response->assetTag = soap_tree_value(node, "assetTag");
        response->romVersion = soap_tree_value(node, "romVersion");
        response->numberOfCpus = atoi(soap_tree_value(node, "numberOfCpus"));
        response->cpus = soap_walk_tree(node, "cpus:bladeCpuInfo");
        response->memory = atoi(soap_tree_value(node, "memory"));
        response->numberOfNics = atoi(soap_tree_value(node, "numberOfNics"));
        response->nics = soap_walk_tree(node, "nics:bladeNicInfo");
        response->mmHeight = atoi(soap_tree_value(node, "mmHeight"));
        response->mmWidth = atoi(soap_tree_value(node, "mmWidth"));
        response->mmDepth = atoi(soap_tree_value(node, "mmDepth"));
        response->deviceId = atoi(soap_tree_value(node, "deviceId"));
        response->productId = atoi(soap_tree_value(node, "productId"));
}

/* parse_diagnosticChecks - Parses a diagnosticChecks response structure */
static void     parse_diagnosticChecks(xmlNode *node,
                                       struct diagnosticChecks *response)
{
        response->internalDataError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "internalDataError"));
        response->managementProcessorError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "managementProcessorError"));
        response->thermalWarning =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "thermalWarning"));
        response->thermalDanger =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "thermalDanger"));
        response->ioConfigurationError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "ioConfigurationError"));
        response->devicePowerRequestError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "devicePowerRequestError"));
        response->insufficientCooling =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "insufficientCooling"));
        response->deviceLocationError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "deviceLocationError"));
        response->deviceFailure =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "deviceFailure"));
        response->deviceDegraded =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "deviceDegraded"));
        response->acFailure = soap_enum(diagnosticStatus_S,
                                        soap_tree_value(node, "acFailure"));
        response->i2cBuses = soap_enum(diagnosticStatus_S,
                                       soap_tree_value(node, "i2cBuses"));
        response->redundancy = soap_enum(diagnosticStatus_S,
                                         soap_tree_value(node, "redundancy"));
}

/* parse_syslog - Parses a syslog response structure */
static void     parse_syslog(xmlNode *node, struct syslog *response)
{
        char    *str;

        if ((str = soap_tree_value(node, "bayNumber")))
                response->bayNumber = atoi(str);
        else
                response->bayNumber = -1;

        if ((str = soap_tree_value(node, "syslogStrlen")))
                response->syslogStrlen = atoi(str);
        else
                response->syslogStrlen = -1;

        response->logContents = soap_tree_value(node, "logContents"); /* May be
                                                                       * NULL
                                                                       */
}

/* parse_rackTopology - Parses a rackTopology response structure */
static void     parse_rackTopology(xmlNode *node,
                                   struct rackTopology *response)
{
        response->ruid = soap_tree_value(node, "ruid"); /* May be NULL */
        response->enclosures = soap_walk_tree(node, "enclosures:enclosure");
}

/* parse_rackTopology2 - Parses rackTopology2 response structure */
static void     parse_rackTopology2(xmlNode *node,
                                    struct rackTopology2 *response)
{
        response->ruid = soap_tree_value(node, "ruid");
        response->enclosures = soap_walk_tree(node, "enclosures:enclosure");
}

/* parse_enclosureInfo - Parses a enclosureInfo response structure */
static void     parse_enclosureInfo(xmlNode *node,
                                    struct enclosureInfo *response)
{
        response->rackName = soap_tree_value(node, "rackName");
        response->enclosureName = soap_tree_value(node, "enclosureName");
        response->hwVersion = soap_tree_value(node, "hwVersion");
        response->bladeBays = atoi(soap_tree_value(node, "bladeBays"));
        response->fanBays = atoi(soap_tree_value(node, "fanBays"));
        response->powerSupplyBays =
                atoi(soap_tree_value(node, "powerSupplyBays"));
        response->thermalSensors =
                atoi(soap_tree_value(node, "thermalSensors"));
        response->interconnectTrayBays =
                atoi(soap_tree_value(node, "interconnectTrayBays"));
        response->oaBays = atoi(soap_tree_value(node, "oaBays"));
        response->name = soap_tree_value(node, "name");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->uuid = soap_tree_value(node, "uuid");
        response->assetTag = soap_tree_value(node, "assetTag");
        response->manufacturer = soap_tree_value(node, "manufacturer");
        response->chassisSparePartNumber =
                soap_tree_value(node, "chassisSparePartNumber");
        response->interposerManufacturer =
                soap_tree_value(node, "interposerManufacturer");
        response->interposerName = soap_tree_value(node, "interposerName");
        response->interposerPartNumber =
                soap_tree_value(node, "interposerPartNumber");
        response->interposerSerialNumber =
                soap_tree_value(node, "interposerSerialNumber");
        response->pduType = soap_tree_value(node, "pduType");
        response->mmHeight = atoi(soap_tree_value(node, "mmHeight"));
        response->mmWidth = atoi(soap_tree_value(node, "mmWidth"));
        response->mmDepth = atoi(soap_tree_value(node, "mmDepth"));
        response->pduPartNumber = soap_tree_value(node, "pduPartNumber");
        response->pduSparePartNumber =
                soap_tree_value(node, "pduSparePartNumber");
}

/* parse_oaStatus - Parses an oaStatus response structure */
static void     parse_oaStatus(xmlNode *node, struct oaStatus *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->oaName = soap_tree_value(node, "oaName");
        response->oaRole = soap_enum(oaRole_S, soap_tree_value(node, "oaRole"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->uid = soap_enum(uidStatus_S, soap_tree_value(node, "uid"));
        response->restartCause = atoi(soap_tree_value(node, "restartCause"));
        response->oaRedundancy =
                parse_xsdBoolean(soap_tree_value(node, "oaRedundancy"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
#if 0                                   /* TODO: Doesn't work yet */
        response->diagnosticChecksEx =
                soap_walk_tree(node, "diagnosticChecksEx");
#endif
}

/* parse_oaInfo - Parses an oaInfo response structure */
static void     parse_oaInfo(xmlNode *node, struct oaInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->youAreHere =
                parse_xsdBoolean(soap_tree_value(node, "youAreHere"));
        response->name = soap_tree_value(node, "name");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->sparePartNumber = soap_tree_value(node, "sparePartNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->uuid = soap_tree_value(node, "uuid");
        response->assetTag = soap_tree_value(node, "assetTag");
        response->manufacturer = soap_tree_value(node, "manufacturer");
        response->hwVersion = soap_tree_value(node, "hwVersion");
        response->fwVersion = soap_tree_value(node, "fwVersion");
        response->mmHeight = atoi(soap_tree_value(node, "mmHeight"));
        response->mmWidth = atoi(soap_tree_value(node, "mmWidth"));
        response->mmDepth = atoi(soap_tree_value(node, "mmDepth"));
}

/* parse_bladeStatus - Parses a bladeStatus response structure */
static void     parse_bladeStatus(xmlNode *node, struct bladeStatus *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->thermal =
                soap_enum(sensorStatus_S, soap_tree_value(node, "thermal"));
        response->powered =
                soap_enum(power_S, soap_tree_value(node, "powered"));
        response->powerState =
                soap_enum(powerState_S, soap_tree_value(node, "powerState"));
        response->shutdown =
                soap_enum(shutdown_S, soap_tree_value(node, "shutdown"));
        response->uid = soap_enum(uidStatus_S, soap_tree_value(node, "uid"));
        response->powerConsumed = atoi(soap_tree_value(node, "powerConsumed"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
#if 0                                   /* TODO: Doesn't work yet */
        response->diagnosticChecksEx =
                soap_walk_tree(node, "diagnosticChecksEx");
#endif
}

/* parse_fanInfo - Parses a fanInfo response structure */
static void     parse_fanInfo(xmlNode *node, struct fanInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->name = soap_tree_value(node, "name");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->sparePartNumber = soap_tree_value(node, "sparePartNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->powerConsumed = atoi(soap_tree_value(node, "powerConsumed"));
        response->fanSpeed = atoi(soap_tree_value(node, "fanSpeed"));
        response->maxFanSpeed = atoi(soap_tree_value(node, "maxFanSpeed"));
        response->lowLimitFanSpeed =
                atoi(soap_tree_value(node, "lowLimitFanSpeed"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
#if 0                                   /* TODO: Doesn't work yet */
        response->diagnosticChecksEx =
                soap_walk_tree(node, "diagnosticChecksEx");
#endif
}

/* parse_interconnectTrayStatus - Parses an interconnectTrayStatus
 * response structure
 */
static void     parse_interconnectTrayStatus(xmlNode *node,
                                struct interconnectTrayStatus *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->thermal =
                soap_enum(sensorStatus_S, soap_tree_value(node, "thermal"));
        response->cpuFault =
                parse_xsdBoolean(soap_tree_value(node, "cpuFault"));
        response->healthLed =
                parse_xsdBoolean(soap_tree_value(node, "healthLed"));
        response->uid = soap_enum(uidStatus_S, soap_tree_value(node, "uid"));
        response->powered =
                soap_enum(power_S, soap_tree_value(node, "powered"));
        response->ports = soap_walk_tree(node, "ports:port");
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
#if 0                                   /* TODO: Doesn't work yet */
        response->diagnosticChecksEx =
                soap_walk_tree(node, "diagnosticChecksEx");
#endif
}

/* parse_interconnectTrayInfo - Parses an interconnectTrayInfo
 * response structure
 */
static void     parse_interconnectTrayInfo(xmlNode *node,
                                struct interconnectTrayInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->interconnectTrayType =
                soap_enum(interconnectTrayType_S,
                          soap_tree_value(node, "interconnectTrayType"));
        response->passThroughSupport =
                parse_xsdBoolean(soap_tree_value(node, "passThroughSupport"));
        response->portDisableSupport =
                parse_xsdBoolean(soap_tree_value(node, "portDisableSupport"));
        response->temperatureSensorSupport =
                parse_xsdBoolean(soap_tree_value(node,
                                                 "temperatureSensorSupport"));
        response->width = atoi(soap_tree_value(node, "width"));
        response->manufacturer = soap_tree_value(node, "manufacturer");
        response->name = soap_tree_value(node, "name");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->sparePartNumber = soap_tree_value(node, "sparePartNumber");
        response->rs232PortRoute =
                parse_xsdBoolean(soap_tree_value(node, "rs232PortRoute"));
        response->ethernetPortRoute =
                parse_xsdBoolean(soap_tree_value(node, "ethernetPortRoute"));
        response->userAssignedName = soap_tree_value(node, "userAssignedName");
        response->inBandIpAddress = soap_tree_value(node, "inBandIpAddress");
        response->urlToMgmt = soap_tree_value(node, "urlToMgmt");
        response->powerOnWatts = atoi(soap_tree_value(node, "powerOnWatts"));
        response->powerOffWatts = atoi(soap_tree_value(node, "powerOffWatts"));
}

/* parse_powerSupplyInfo - Parses a powerSupplyInfo response structure */
static void     parse_powerSupplyInfo(xmlNode *node,
                                      struct powerSupplyInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->modelNumber = soap_tree_value(node, "modelNumber");
        response->sparePartNumber = soap_tree_value(node, "sparePartNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->capacity = atoi(soap_tree_value(node, "capacity"));
        response->actualOutput = atoi(soap_tree_value(node, "actualOutput"));
}

/* parse_powerSupplyStatus - Parses a powerSupplyStatus response structure */
static void     parse_powerSupplyStatus(xmlNode *node,
                                        struct powerSupplyStatus *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->inputStatus =
                soap_enum(opStatus_S, soap_tree_value(node, "inputStatus"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
#if 0                                   /* TODO: Doesn't work yet */
        response->diagnosticChecksEx =
                soap_walk_tree(node, "diagnosticChecksEx");
#endif
}

/* parse_powerSubsystemInfo - Parses a powerSubsystemInfo response structure */
static void     parse_powerSubsystemInfo(xmlNode *node,
                                         struct powerSubsystemInfo *response)
{
        char    *str;

        response->subsystemType =
                soap_enum(powerSystemType_S,
                          soap_tree_value(node, "subsystemType"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->redundancy =
                soap_enum(redundancy_S, soap_tree_value(node, "redundancy"));
        response->redundancyMode =
                soap_enum(powerRedundancy_S,
                          soap_tree_value(node, "redundancyMode"));
        response->capacity = atoi(soap_tree_value(node, "capacity"));
        response->redundantCapacity =
                atoi(soap_tree_value(node, "redundantCapacity"));
        response->outputPower = atoi(soap_tree_value(node, "outputPower"));
        response->powerConsumed = atoi(soap_tree_value(node, "powerConsumed"));
        response->inputPowerVa = atof(soap_tree_value(node, "inputPowerVa"));
        response->inputPowerCapacityVa =
                atof(soap_tree_value(node, "inputPowerCapacityVa"));
        if ((str = soap_tree_value(node, "inputPower")))
                response->inputPower = atof(str);
        else
                response->inputPower = -1.0;
        if ((str = soap_tree_value(node, "inputPowerCapacity")))
                response->inputPowerCapacity = atof(str);
        else
                response->inputPowerCapacity = -1.0;
        response->goodPowerSupplies =
                atoi(soap_tree_value(node, "goodPowerSupplies"));
        response->wantedPowerSupplies =
                atoi(soap_tree_value(node, "wantedPowerSupplies"));
        response->neededPowerSupplies =
                atoi(soap_tree_value(node, "neededPowerSupplies"));
}

/* parse_thermalInfo - Parses a thermalInfo response structure */
static void     parse_thermalInfo(xmlNode *node, struct thermalInfo *response)
{
        response->sensorType =
                soap_enum(sensorType_S, soap_tree_value(node, "sensorType"));
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->sensorStatus =
                soap_enum(sensorStatus_S,
                          soap_tree_value(node, "sensorStatus"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->temperatureC = atoi(soap_tree_value(node, "temperatureC"));
        response->cautionThreshold =
                atoi(soap_tree_value(node, "cautionThreshold"));
        response->criticalThreshold =
                atoi(soap_tree_value(node, "criticalThreshold"));
}

/* parse_enclosureBaysSelection - Parses an enclosureBaysSelection response
 * structure
 */
static void     parse_enclosureBaysSelection(xmlNode *node,
                        struct enclosureBaysSelection *response)
{
        char    *str;

        if ((str = soap_tree_value(node, "oaAccess")))
                response->oaAccess = parse_xsdBoolean(str);
        else
                response->oaAccess = HPOA_FALSE; /* Default */
        response->bladeBays = soap_walk_tree(node, "bladeBays:blade");
        response->interconnectTrayBays =
                soap_walk_tree(node, "interconnectTrayBays:interconnectTray");
}

/* parse_userInfo - Parses a userInfo response structure */
static void     parse_userInfo(xmlNode *node, struct userInfo *response)
{
        response->username = soap_tree_value(node, "username");
        response->fullname = soap_tree_value(node, "fullname");
        response->contactInfo = soap_tree_value(node, "contactInfo");
        response->isEnabled =
                parse_xsdBoolean(soap_tree_value(node, "isEnabled"));
        response->acl = soap_enum(userAcl_S, soap_tree_value(node, "acl"));
        parse_enclosureBaysSelection(soap_walk_tree(node, "bayPermissions"),
                                     &(response->bayPermissions));
}

/* parse_oaNetworkInfo - Parses a oaNetworkInfo response structure */
static void     parse_oaNetworkInfo(xmlNode *node,
                                    struct oaNetworkInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->dhcpEnabled =
                parse_xsdBoolean(soap_tree_value(node, "dhcpEnabled"));
        response->dynDnsEnabled =
                parse_xsdBoolean(soap_tree_value(node, "dynDnsEnabled"));
        response->macAddress = soap_tree_value(node, "macAddress");
        response->ipAddress = soap_tree_value(node, "ipAddress");
        response->netmask = soap_tree_value(node, "netmask");
        response->gateway = soap_tree_value(node, "gateway");
        response->dns = soap_walk_tree(node, "dns:ipAddress");
        response->elinkIpAddress = soap_tree_value(node, "elinkIpAddress");
        response->linkActive =
                parse_xsdBoolean(soap_tree_value(node, "linkActive"));
}

/* parse_getAllEvents - Parses a getAllEventsResponse structure */
static void     parse_getAllEvents(xmlNode *node,
                                   struct getAllEventsResponse *response)
{
        response->eventInfoArray =
                soap_walk_tree(node, "eventInfoArray:eventInfo");
}


/* User-callable functions that are used to walk the lists of response
 * parameters.  Please note that there are many structures involved, which
 * are derived from the SOAP/XML interface specification.  Instead of trying
 * to document each fully in the comments, please refer to the oa_soap_calls.h
 * file which contains structure definitions, and to the OA SOAP interface
 * documentation (HP Onboard Administrator SOAP Interface Guide).
 */

/* soap_getBladeCpuInfo - Walks list of bladeCpuInfo nodes, providing details
 *      on each CPU.  Used after calling soap_getBladeInfo().
 *
 * Outputs:
 *      cpuType:        String describing CPU
 *      cpuSpeed:       CPU speed in MHz
 */
void    soap_getBladeCpuInfo(xmlNode *cpus, struct bladeCpuInfo *result)
{
        result->cpuType = soap_tree_value(cpus, "cpuType");
        result->cpuSpeed = atoi(soap_tree_value(cpus, "cpuSpeed"));
}

/* soap_getBladeNicInfo - Walks list of bladeNicInfo nodes, providing details
 *      on each NIC.  Used after calling soap_getBladeInfo().
 *
 * Outputs:
 *      port:           String describing NIC port
 *      macAddress:     String containing the NIC's MAC address
 */
void    soap_getBladeNicInfo(xmlNode *nics, struct bladeNicInfo *result)
{
        result->port = soap_tree_value(nics, "port");
        result->macAddress = soap_tree_value(nics, "macAddress");
}

#if 0                                   /* TODO: Not sure the following call
                                         * will work...perhaps same problem
                                         * as extraData
                                         */
/* soap_getDiagnosticData - Gets information from diagnosticData nodes,
 *      providing details on each diagnostic event.  Used after calling
 *      soap_getEventInfo().
 *
 * Outputs:
 *      item:           diagnosticStatus enum
 *      name:           String containing name of diagnostic item
 */
void    soap_getDiagnosticData(xmlNode *data, struct diagnosticData *result)
{
        result->item = soap_enum(diagnosticStatus_S,
                                 soap_tree_value(data, "item"));
        result->name = soap_tree_value(data, "name");
}
#endif

/* soap_getBayAccess - Gets information from bayAccess nodes.
 *
 * Outputs:
 *      bayNumber:      Bay number
 *      access:         True if user can access the bay
 */
void    soap_getBayAccess(xmlNode *bay, struct bayAccess *result)
{
        result->bayNumber = atoi(soap_tree_value(bay, "bayNumber"));
        result->access = parse_xsdBoolean(soap_tree_value(bay, "access"));
}

/* soap_getEncLink - Gets information from encLink nodes, providing enclosure
 *      information.
 *
 * Outputs:
 *      enclosureNumber: Which enclosure is this?
 *      oaName:          Name assigned to the OA
 *      uuid:            UUID
 *      rackName:        Name assigned to the rack
 *      enclosureName:   Name assigned to the enclosure
 *      url:             URL
 *      local:           Local or remote
 */
void    soap_getEncLink(xmlNode *data, struct encLink *result)
{
        result->enclosureNumber =
                atoi(soap_tree_value(data, "enclosureNumber"));
        result->oaName = soap_tree_value(data, "oaName"); /* May be NULL */
        result->uuid = soap_tree_value(data, "uuid"); /* May be NULL */
        result->rackName = soap_tree_value(data, "rackName"); /* May be NULL */
        result->enclosureName =
                soap_tree_value(data, "enclosureName"); /* May be NULL */
        result->url = soap_tree_value(data, "url");
        result->local = parse_xsdBoolean(soap_tree_value(data, "local"));
}

/* soap_getEncLink2 - Gets information from encLink2 nodes, 
 * providing enclosure information.
 *
 * Outputs:
 *      enclosureNumber        : Which enclosure is this?
 *      productId              : Product ID
 *      mfgId                  : Manufacturer ID
 *      enclosureUuid          : Enclosure Universal Unique ID
 *      encloseSerialNumber    : Serial number from enclosure
 *      enclosureName          : Enclosure name
 *      enclosureProductName   : Enclosure product name including mfg name
 *      enclosureStatus        : Enclosure status
 *      enclosureRackIpAddress : Rack IP address
 *      enclosureUrl           : URL for the enclosure
 *      rackName               : Rack name assigned by user
 *      primaryEnclosure       : Flag for identifying primary
 *      encLinkOaArray         : encLinkOa array structure
 */
void    soap_getEncLink2(xmlNode *data, struct encLink2 *result)
{
        result->enclosureNumber = 
                atoi(soap_tree_value(data, "enclosureNumber"));
        result->productId = atoi(soap_tree_value(data, "productId"));
        result->mfgId = atoi(soap_tree_value(data, "mfgId"));
        result->enclosureUuid = soap_tree_value(data, "enclosureUuid");
        result->enclosureSerialNumber = 
                soap_tree_value(data, "enclosureSerialNumber");
        result->enclosureName = soap_tree_value(data, "enclosureName");
        result->enclosureProductName = 
                soap_tree_value(data, "enclosureProductName");
        result->enclosureStatus = 
                soap_enum(opStatus_S, 
                          soap_tree_value(data, "enclosureStatus"));
        result->enclosureRackIpAddress = 
                soap_tree_value(data, "enclosureRackIpAddress");
        result->enclosureUrl = soap_tree_value(data, "enclosureUrl");
        result->rackName = soap_tree_value(data, "rackName");
        result->primaryEnclosure = 
                parse_xsdBoolean(soap_tree_value(data, "primaryEnclosure"));
        result->encLinkOa = soap_walk_tree(data, "encLinkOaArray:encLinkOa");
}

/* soap_getEncLinkOa - Gets information from encLinkOa nodes, 
 * providing enclosure information.
 *
 * Outputs:
 *      activeOa    : Active OA flag indicator
 *      bayNumber   : Bay number of the OA
 *      oaName      : OA name assigned by the user
 *      ipAddress   : IP address for OA ENC Link
 *      macAddress  : MAC address for OA ENC Link
 *      fwVersion   : OA firmware version
 */
void    soap_getEncLinkOa (xmlNode *data, struct encLinkOa *result)
{
        result->activeOa = parse_xsdBoolean(soap_tree_value(data, "activeOa"));
        result->bayNumber = atoi(soap_tree_value(data, "bayNumber"));
        result->oaName = soap_tree_value(data, "oaName");
        result->ipAddress = soap_tree_value(data, "ipAddress");
        result->macAddress = soap_tree_value(data, "macAddress");
        result->fwVersion = soap_tree_value(data, "fwVersion");
}


/* soap_getPortEnabled - Gets information from portEnabled nodes
 *
 * Outputs:
 *      portNumber:     Port number
 *      enabled:        Is it enabled?
 */
void    soap_getPortEnabled(xmlNode *data, struct portEnabled *result)
{
        result->portNumber = atoi(soap_tree_value(data, "portNumber"));
        result->enabled = parse_xsdBoolean(soap_tree_value(data, "enabled"));
}

/* soap_getIpAddress - Gets information from ipAddress nodes
 *
 * Outputs:
 *      result:         String containing IP address
 */
void    soap_getIpAddress(xmlNode *ips, char **result)
{
        *result = soap_value(ips);
}

/* soap_getEventInfo - Walks list of eventInfo nodes, providing details
 *      on each event.  Used after calling soap_getAllEvents().
 *
 * Outputs:
 *      event:          indicates the type of event
 *      eventTimeStamp: the time this event happened
 *      queueSize:      size of event queue, or -1 for not present
 *      eventData:      the event information for each type of event
 */
void    soap_getEventInfo(xmlNode *events, struct eventInfo *result)
{
        char    *str;
        xmlNode *node;

        result->event =
                soap_enum(eventType_S, soap_tree_value(events, "event"));
        result->eventTimeStamp =
                atoi(soap_tree_value(events, "eventTimeStamp"));
        if ((str = soap_tree_value(events, "queueSize")))
                result->queueSize = atoi(str);
        else
                result->queueSize = -1;

        /* The remainder depends on what sort of data is returned by the OA.
         * The current documentation says that only one of these can be
         * returned, and indeed, since we have a union, we can handle only
         * one.  Therefore, we will quit and return early when we get a
         * match.
         *
         * As of this writing, not all events have been handled.  Those that
         * are not handled yet are noted by comments left in the code.  Please
         * note that because some are unimplemented, these events will drop
         * through and be returned as "NOPAYLOAD", an answer that is not
         * correct.
         */
        if ((node = soap_walk_tree(events, "syslog"))) {
                result->enum_eventInfo = SYSLOG;
                parse_syslog(node, &(result->eventData.syslog));
                return;
        }

        if ((node = soap_walk_tree(events, "rackTopology"))) {
                result->enum_eventInfo = RACKTOPOLOGY;
                parse_rackTopology(node, &(result->eventData.rackTopology));
                return;
        }

        /* ENCLOSURESTATUS */

        if ((node = soap_walk_tree(events, "enclosureInfo"))) {
                result->enum_eventInfo = ENCLOSUREINFO;
                parse_enclosureInfo(node, &(result->eventData.enclosureInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "oaStatus"))) {
                result->enum_eventInfo = OASTATUS;
                parse_oaStatus(node, &(result->eventData.oaStatus));
                return;
        }

        if ((node = soap_walk_tree(events, "oaInfo"))) {
                result->enum_eventInfo = OAINFO;
                parse_oaInfo(node, &(result->eventData.oaInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "bladeInfo"))) {
                result->enum_eventInfo = BLADEINFO;
                parse_bladeInfo(node, &(result->eventData.bladeInfo));
                return;
        }

        /* BLADEMPINFO */

        if ((node = soap_walk_tree(events, "bladeStatus"))) {
                result->enum_eventInfo = BLADESTATUS;
                parse_bladeStatus(node, &(result->eventData.bladeStatus));
                return;
        }

        /* BLADEPORTMAP */

        if ((node = soap_walk_tree(events, "fanInfo"))) {
                result->enum_eventInfo = FANINFO;
                parse_fanInfo(node, &(result->eventData.fanInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "interconnectTrayStatus"))) {
                result->enum_eventInfo = INTERCONNECTTRAYSTATUS;
                parse_interconnectTrayStatus(node,
                        &(result->eventData.interconnectTrayStatus));
                return;
        }

        if ((node = soap_walk_tree(events, "interconnectTrayInfo"))) {
                result->enum_eventInfo = INTERCONNECTTRAYINFO;
                parse_interconnectTrayInfo(node,
                        &(result->eventData.interconnectTrayInfo));
                return;
        }

        /* INTERCONNECTTRAYPORTMAP */

        if ((node = soap_walk_tree(events, "powerSupplyInfo"))) {
                result->enum_eventInfo = POWERSUPPLYINFO;
                parse_powerSupplyInfo(node,
                        &(result->eventData.powerSupplyInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "powerSupplyStatus"))) {
                result->enum_eventInfo = POWERSUPPLYSTATUS;
                parse_powerSupplyStatus(node,
                        &(result->eventData.powerSupplyStatus));
                return;
        }

        if ((node = soap_walk_tree(events, "powerSubsystemInfo"))) {
                result->enum_eventInfo = POWERSUBSYSTEMINFO;
                parse_powerSubsystemInfo(node,
                        &(result->eventData.powerSubsystemInfo));
                return;
        }

        /* POWERCONFIGINFO */

        if ((node = soap_walk_tree(events, "thermalInfo"))) {
                result->enum_eventInfo = THERMALINFO;
                parse_thermalInfo(node, &(result->eventData.thermalInfo));
                return;
        }

        /* USERINFOARRAY */

        if ((node = soap_walk_tree(events, "userInfo"))) {
                result->enum_eventInfo = USERINFO;
                parse_userInfo(node, &(result->eventData.userInfo));
                return;
        }

        /* LDAPINFO */
        /* LDAPGROUPINFO */
        /* SNMPINFO */
        /* ENCLOSURENETWORKINFO */

        if ((node = soap_walk_tree(events, "oaNetworkInfo"))) {
                result->enum_eventInfo = OANETWORKINFO;
                parse_oaNetworkInfo(node, &(result->eventData.oaNetworkInfo));
                return;
        }

        /* ENCLOSURETIME */
        /* ALERTMAILINFO */
        /* PASSWORDSETTINGS */
        /* EBIPAINFO */
        /* LCDCHATMESSAGE */
        /* LCDUSERNOTES */
        /* LCDBUTTONEVENT */
        /* LCDSTATUS */
        /* LCDINFO */
        /* HPSIMINFO */
        /* THERMALSUBSYSTEMINFO */
        /* BLADEBOOTINFO */
        /* OAVCMMODE */
        /* POWERREDUCTIONSTATUS */
        /* VIRTUALMEDIASTATUS */
        /* OAMEDIADEVICE */
        /* FANZONE */
        /* EBIPAINFOEX */
        /* CACERTSINFO */

        if ((node = soap_walk_tree (events, "rackTopology2"))) {
                result->enum_eventInfo = RACKTOPOLOGY2;
                parse_rackTopology2(node, &(result->eventData.rackTopology2));
                return;
        }

        /* USERCERTIFICATEINFO */
        /* SYSLOGSETTINGS */
        /* POWERDELAYSETTINGS */
        /* USBMEDIAFIRMWAREIMAGES */
        /* CONFIGSCRIPTS */
        /* NUMVALUE */
        /* STRING */

        if ((result->eventData.message = soap_tree_value(events, "message"))) {
                result->enum_eventInfo = MESSAGE;
                return;
        }

        result->enum_eventInfo = NOPAYLOAD; /* If we get here, we got nothing */
}


/* Finally, the main body of OA SOAP call functions.  These are the ones that
 * a user would expect to call for most things.  Please refer to the associated
 * .h file for parameter details, and to the HP Onboard Administrator SOAP
 * Interface Guide for further call details.
 */
int soap_subscribeForEvents(SOAP_CON *con, struct eventPid *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, SUBSCRIBE_FOR_EVENTS))) {
                parse_eventPid(soap_walk_doc(con->doc, "Body:eventPid"),
                               response);
        }
        return(ret);
}


int soap_unSubscribeForEvents(SOAP_CON *con,
                              struct unSubscribeForEvents *request)
{
        SOAP_PARM_CHECK_NRS
        return(soap_request(con, UN_SUBSCRIBE_FOR_EVENTS, request->pid));
}


int soap_getEvent(SOAP_CON *con,
                  struct getEvent *request,
                  struct eventInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_EVENT,
                           request->pid,
                           request->waitTilEventHappens, /* xsd:boolean */
                           request->lcdEvents))) {       /* xsd:boolean */
               soap_getEventInfo(soap_walk_doc(con->doc,
                                               "Body:"
                                               "getEventResponse:"
                                               "eventInfo"),
                                 response);
        }
        return(ret);
}


int soap_getAllEvents(SOAP_CON *con,
                      struct getAllEvents *request,
                      struct getAllEventsResponse *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_ALL_EVENTS,
                           request->pid,
                           request->waitTilEventHappens, /* xsd:boolean */
                           request->lcdEvents))) {       /* xsd:boolean */
                parse_getAllEvents(soap_walk_doc(con->doc,
                                                 "Body:"
                                                 "getAllEventsResponse"),
                                   response);
        }
        return(ret);
}


int             soap_getBladeInfo(SOAP_CON *con,
                                  struct getBladeInfo *request,
                                  struct bladeInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_BLADE_INFO, request->bayNumber))) {
                parse_bladeInfo(soap_walk_doc(con->doc,
                                              "Body:"
                                              "getBladeInfoResponse:"
                                              "bladeInfo"),
                                response);
        }
        return(ret);
}


int soap_getEnclosureInfo(SOAP_CON *con,
                          struct enclosureInfo *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_ENCLOSURE_INFO))) {
                parse_enclosureInfo(soap_walk_doc(con->doc,
                                                  "Body:"
                                                  "getEnclosureInfoResponse:"
                                                  "enclosureInfo"),
                                    response);
        }
        return(ret);
}


int soap_getOaStatus(SOAP_CON *con,
                     struct getOaStatus *request,
                     struct oaStatus *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_OA_STATUS, request->bayNumber))) {
                parse_oaStatus(soap_walk_doc(con->doc,
                                             "Body:"
                                             "getOaStatusResponse:"
                                             "oaStatus"),
                               response);
        }
        return(ret);
}


int soap_getOaInfo(SOAP_CON *con,
                   struct getOaInfo *request,
                   struct oaInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_OA_INFO, request->bayNumber))) {
                parse_oaInfo(soap_walk_doc(con->doc,
                                           "Body:"
                                           "getOaInfoResponse:"
                                           "oaInfo"),
                             response);
        }
        return(ret);
}


int soap_getInterconnectTrayStatus(SOAP_CON *con,
                                   struct getInterconnectTrayStatus *request,
                                   struct interconnectTrayStatus *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_INTERCONNECT_TRAY_STATUS,
                                  request->bayNumber))) {
                parse_interconnectTrayStatus(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getInterconnectTrayStatusResponse:"
                                      "interconnectTrayStatus"),
                        response);
        }
        return(ret);
}


int soap_getInterconnectTrayInfo(SOAP_CON *con,
                                 struct getInterconnectTrayInfo *request,
                                 struct interconnectTrayInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_INTERCONNECT_TRAY_INFO,
                                  request->bayNumber))) {
                parse_interconnectTrayInfo(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getInterconnectTrayInfoResponse:"
                                      "interconnectTrayInfo"),
                        response);
        }
        return(ret);
}


int soap_getFanInfo(SOAP_CON *con,
                    struct getFanInfo *request,
                    struct fanInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_FAN_INFO, request->bayNumber))) {
                parse_fanInfo(soap_walk_doc(con->doc,
                                            "Body:"
                                            "getFanInfoResponse:"
                                            "fanInfo"),
                              response);
        }
        return(ret);
}


int soap_getPowerSubsystemInfo(SOAP_CON *con,
                               struct powerSubsystemInfo *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_POWER_SUBSYSTEM_INFO))) {
                parse_powerSubsystemInfo(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getPowerSubsystemInfoResponse:"
                                      "powerSubsystemInfo"),
                        response);
        }
        return(ret);
}


int soap_getPowerSupplyInfo(SOAP_CON *con,
                            struct getPowerSupplyInfo *request,
                            struct powerSupplyInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_POWER_SUPPLY_INFO,
                                  request->bayNumber))) {
                parse_powerSupplyInfo(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getPowerSupplyInfoResponse:"
                                      "powerSupplyInfo"),
                        response);
        }
        return(ret);
}


int soap_getOaNetworkInfo(SOAP_CON *con,
                          struct getOaNetworkInfo *request,
                          struct oaNetworkInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_OA_NETWORK_INFO,
                                  request->bayNumber))) {
                parse_oaNetworkInfo(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getOaNetworkInfoResponse:"
                                      "oaNetworkInfo"),
                        response);
        }
        return(ret);
}


int soap_getBladeStatus(SOAP_CON *con,
                        struct getBladeStatus *request,
                        struct bladeStatus *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_BLADE_STATUS, request->bayNumber))) {
                parse_bladeStatus(soap_walk_doc(con->doc,
                                                "Body:"
                                                "getBladeStatusResponse:"
                                                "bladeStatus"),
                                  response);
        }
        return(ret);
}


int soap_setBladePower(SOAP_CON *con,
                       struct setBladePower *request)
{
        char    power[POWER_CONTROL_LENGTH];

        SOAP_PARM_CHECK_NRS
        if (soap_inv_enum(power, powerControl_S, request->power)) {
                err("invalid power parameter");
                return(-1);
        }
        return(soap_request(con, SET_BLADE_POWER, request->bayNumber, power));
}


int soap_setInterconnectTrayPower(SOAP_CON *con,
                                  struct setInterconnectTrayPower *request)
{
        SOAP_PARM_CHECK_NRS
        return(soap_request(con, SET_INTERCONNECT_TRAY_POWER,
                            request->bayNumber, request->on));
}


int soap_resetInterconnectTray(SOAP_CON *con,
                               struct resetInterconnectTray *request)
{
        SOAP_PARM_CHECK_NRS
        return(soap_request(con, RESET_INTERCONNECT_TRAY, request->bayNumber));
}


int soap_getThermalInfo(SOAP_CON *con,
                        struct getThermalInfo *request,
                        struct thermalInfo *response)
{
        char    sensor[SENSOR_TYPE_LENGTH];

        SOAP_PARM_CHECK
        if (soap_inv_enum(sensor, sensorType_S, request->sensorType)) {
                err("invalid sensorType parameter");
                return(-1);
        }
        if (! (ret = soap_request(con, GET_THERMAL_INFO,
                                  sensor, request->bayNumber))) {
                parse_thermalInfo(soap_walk_doc(con->doc,
                                                "Body:"
                                                "getThermalInfoResponse:"
                                                "thermalInfo"),
                                  response);
        }
        return(ret);
}


int soap_getUserInfo(SOAP_CON *con,
                     struct getUserInfo *request,
                     struct userInfo *response)
{
        /* On this one, are there some special rules that have to be followed
         * while sending usernames over XML/SOAP?  I could imagine that a user
         * name could contain characters that need to be escaped in XML.
         *
         * TODO: This needs to be checked.
         */
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_USER_INFO, request->username))) {
                parse_userInfo(soap_walk_doc(con->doc,
                                             "Body:"
                                             "getUserInfoResponse:"
                                             "userInfo"),
                               response);
        }
        return(ret);
}


int soap_getRackTopology2(SOAP_CON *con, struct rackTopology2 *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_RACK_TOPOLOGY2))) {
                parse_rackTopology2(soap_walk_doc(con->doc,
                                                  "Body:"
                                                  "getRackTopology2Response:"
                                                  "rackTopology2"),
                                    response);
        }
        return(ret);
}


int soap_isValidSession(SOAP_CON *con)
{
        if (con == NULL) {
                err("NULL parameter");
                return -1;
        }
        return(soap_request(con, IS_VALID_SESSION));
}

