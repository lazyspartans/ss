/*
 * marshaling/demarshaling of hpi data types
 *
 * Copyright (c) 2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     W. David Ashley <dashley@us.ibm.com.com>
 */

#include "marshal_hpi_types.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


// text buffer
static cMarshalType SaHpiTextBufferDataArray = dArray( SaHpiUint8Type, SAHPI_MAX_TEXT_BUFFER_LENGTH );

static cMarshalType SaHpiTextBufferElements[] =
{
  dStructElement( SaHpiTextBufferT, DataType, SaHpiTextTypeType ),
  dStructElement( SaHpiTextBufferT, Language, SaHpiLanguageType ),
  dStructElement( SaHpiTextBufferT, DataLength, SaHpiUint8Type ),
  dStructElement( SaHpiTextBufferT, Data, SaHpiTextBufferDataArray ),
  dStructElementEnd()
};

cMarshalType SaHpiTextBufferType = dStruct( SaHpiTextBufferT, SaHpiTextBufferElements );


// oHpi text buffer
static cMarshalType oHpiTextBufferDataArray = dArray( SaHpiUint8Type, OH_MAX_TEXT_BUFFER_LENGTH );

static cMarshalType oHpiTextBufferElements[] =
{
  dStructElement( oHpiTextBufferT, DataLength, SaHpiUint16Type ),
  dStructElement( oHpiTextBufferT, Data, oHpiTextBufferDataArray ),
  dStructElementEnd()
};

cMarshalType oHpiTextBufferType = dStruct( oHpiTextBufferT, oHpiTextBufferElements );


// entity
static cMarshalType SaHpiEntityElements[] =
{
  dStructElement( SaHpiEntityT, EntityType, SaHpiEntityTypeType ),
  dStructElement( SaHpiEntityT, EntityLocation, SaHpiEntityLocationType ),
  dStructElementEnd()
};

cMarshalType SaHpiEntityType = dStruct( SaHpiEntityT, SaHpiEntityElements );

// entity path
static cMarshalType SaHpiEntityPathEntryArray = dArray( SaHpiEntityType, SAHPI_MAX_ENTITY_PATH );


static cMarshalType SaHpiEntityPathElements[] =
{
  dStructElement( SaHpiEntityPathT, Entry, SaHpiEntityPathEntryArray ),
  dStructElementEnd()
};

cMarshalType SaHpiEntityPathType = dStruct( SaHpiEntityPathT, SaHpiEntityPathElements );


// sensors
static cMarshalType SaHpiSensorInterpretedUnionBufferArray = dArray( SaHpiUint8Type, SAHPI_SENSOR_BUFFER_LENGTH );

static cMarshalType SaHpiSensorReadingUnionElements[] =
{
  dUnionElement( SAHPI_SENSOR_READING_TYPE_INT64,   SaHpiInt64Type ),
  dUnionElement( SAHPI_SENSOR_READING_TYPE_UINT64,  SaHpiUint64Type ),
  dUnionElement( SAHPI_SENSOR_READING_TYPE_FLOAT64, SaHpiFloat64Type ),
  dUnionElement( SAHPI_SENSOR_READING_TYPE_BUFFER , SaHpiSensorInterpretedUnionBufferArray ),
  dUnionElementEnd()
};

static cMarshalType SaHpiSensorReadingUnionType = dUnion( 1,
							  SaHpiSensorReadingUnionT,
                                                          SaHpiSensorReadingUnionElements );

// sensor reading
static cMarshalType SaHpiSensorReadingTElements[] =
{
  dStructElement( SaHpiSensorReadingT, IsSupported, SaHpiBoolType ),
  dStructElement( SaHpiSensorReadingT, Type, SaHpiSensorReadingTypeType ),
  dStructElement( SaHpiSensorReadingT, Value, SaHpiSensorReadingUnionType ),
  dStructElementEnd()
};

cMarshalType SaHpiSensorReadingType = dStruct( SaHpiSensorReadingT, SaHpiSensorReadingTElements );


// sensor threshold values

static cMarshalType SaHpiSensorThresholdsElements[] =
{
  dStructElement( SaHpiSensorThresholdsT, LowCritical     , SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorThresholdsT, LowMajor        , SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorThresholdsT, LowMinor        , SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorThresholdsT, UpCritical      , SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorThresholdsT, UpMajor         , SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorThresholdsT, UpMinor         , SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorThresholdsT, PosThdHysteresis, SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorThresholdsT, NegThdHysteresis, SaHpiSensorReadingType ),
  dStructElementEnd()
};

cMarshalType SaHpiSensorThresholdsType = dStruct( SaHpiSensorThresholdsT, SaHpiSensorThresholdsElements );


// sensor range

static cMarshalType SaHpiSensorRangeElements[] =
{
  dStructElement( SaHpiSensorRangeT, Flags    , SaHpiSensorRangeFlagsType ),
  dStructElement( SaHpiSensorRangeT, Max      , SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorRangeT, Min      , SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorRangeT, Nominal  , SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorRangeT, NormalMax, SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorRangeT, NormalMin, SaHpiSensorReadingType ),
  dStructElementEnd()
};


cMarshalType SaHpiSensorRangeType = dStruct( SaHpiSensorRangeT, SaHpiSensorRangeElements );


// sensor units

static cMarshalType SaHpiSensorDataFormatElements[] =
{
	dStructElement( SaHpiSensorDataFormatT, IsSupported, SaHpiBoolType ),
	dStructElement( SaHpiSensorDataFormatT, ReadingType, SaHpiSensorReadingTypeType ),
   	dStructElement( SaHpiSensorDataFormatT, BaseUnits, SaHpiSensorUnitsType ),
	dStructElement( SaHpiSensorDataFormatT, ModifierUnits, SaHpiSensorUnitsType ),
	dStructElement( SaHpiSensorDataFormatT, ModifierUse, SaHpiSensorModUnitUseType ),
	dStructElement( SaHpiSensorDataFormatT, Percentage, SaHpiBoolType ),
        dStructElement( SaHpiSensorDataFormatT, Range, SaHpiSensorRangeType ),
        dStructElement( SaHpiSensorDataFormatT, AccuracyFactor,SaHpiFloat64Type ),
	dStructElementEnd()
};

cMarshalType SaHpiSensorDataFormatType = dStruct( SaHpiSensorDataFormatT, SaHpiSensorDataFormatElements );


// threshold support

static cMarshalType SaHpiSensorThdDefnElements[] =
{
  dStructElement( SaHpiSensorThdDefnT, IsAccessible, SaHpiBoolType ),
  dStructElement( SaHpiSensorThdDefnT, ReadThold, SaHpiSensorThdMaskType ),
  dStructElement( SaHpiSensorThdDefnT, WriteThold, SaHpiSensorThdMaskType ),
  dStructElement( SaHpiSensorThdDefnT, Nonlinear, SaHpiBoolType ),
  dStructElementEnd()
};

cMarshalType SaHpiSensorThdDefnType = dStruct( SaHpiSensorThdDefnT, SaHpiSensorThdDefnElements );


// sensor record

static cMarshalType SaHpiSensorRecElements[] =
{
  dStructElement( SaHpiSensorRecT, Num, SaHpiSensorNumType ),
  dStructElement( SaHpiSensorRecT, Type, SaHpiSensorTypeType ),
  dStructElement( SaHpiSensorRecT, Category, SaHpiEventCategoryType ),
  dStructElement( SaHpiSensorRecT, EnableCtrl,SaHpiBoolType ),
  dStructElement( SaHpiSensorRecT, EventCtrl, SaHpiSensorEventCtrlType ),
  dStructElement( SaHpiSensorRecT, Events, SaHpiEventStateType ),
  dStructElement( SaHpiSensorRecT, DataFormat, SaHpiSensorDataFormatType ),
  dStructElement( SaHpiSensorRecT, ThresholdDefn, SaHpiSensorThdDefnType ),
  dStructElement( SaHpiSensorRecT, Oem, SaHpiUint32Type ),
  dStructElementEnd()
};

cMarshalType SaHpiSensorRecType = dStruct( SaHpiSensorRecT, SaHpiSensorRecElements );


// stream control state

static cMarshalType SaHpiCtrlStateStreamArray = dArray( SaHpiUint8Type, SAHPI_CTRL_MAX_STREAM_LENGTH );


static cMarshalType SaHpiCtrlStateStreamElements[] =
{
  dStructElement( SaHpiCtrlStateStreamT, Repeat, SaHpiBoolType ),
  dStructElement( SaHpiCtrlStateStreamT, StreamLength, SaHpiUint32Type ),
  dStructElement( SaHpiCtrlStateStreamT, Stream, SaHpiCtrlStateStreamArray ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlStateStreamType = dStruct( SaHpiCtrlStateStreamT, SaHpiCtrlStateStreamElements );


// text control state

static cMarshalType SaHpiCtrlStateTextElements[] =
{
  dStructElement( SaHpiCtrlStateTextT, Line, SaHpiTxtLineNumType ),
  dStructElement( SaHpiCtrlStateTextT, Text, SaHpiTextBufferType ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlStateTextType = dStruct( SaHpiCtrlStateTextT, SaHpiCtrlStateTextElements );


// OEM control state

static cMarshalType SaHpiCtrlStateOemBodyArray = dArray( SaHpiUint8Type, SAHPI_CTRL_MAX_OEM_BODY_LENGTH );

static cMarshalType SaHpiCtrlStateOemElements[] =
{
  dStructElement( SaHpiCtrlStateOemT, MId, SaHpiManufacturerIdType ),
  dStructElement( SaHpiCtrlStateOemT, BodyLength, SaHpiUint8Type ),
  dStructElement( SaHpiCtrlStateOemT, Body, SaHpiCtrlStateOemBodyArray ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlStateOemType = dStruct( SaHpiCtrlStateOemT, SaHpiCtrlStateOemElements );


static cMarshalType SaHpiCtrlStateUnionElements[] =
{
  dUnionElement( SAHPI_CTRL_TYPE_DIGITAL, SaHpiCtrlStateDigitalType ),
  dUnionElement( SAHPI_CTRL_TYPE_DISCRETE, SaHpiCtrlStateDiscreteType ),
  dUnionElement( SAHPI_CTRL_TYPE_ANALOG, SaHpiCtrlStateAnalogType ),
  dUnionElement( SAHPI_CTRL_TYPE_STREAM, SaHpiCtrlStateStreamType ),
  dUnionElement( SAHPI_CTRL_TYPE_TEXT, SaHpiCtrlStateTextType ),
  dUnionElement( SAHPI_CTRL_TYPE_OEM, SaHpiCtrlStateOemType ),
  dUnionElementEnd()
};


static cMarshalType SaHpiCtrlStateUnionType = dUnion( 0, SaHpiCtrlStateUnionT, SaHpiCtrlStateUnionElements );

static cMarshalType SaHpiCtrlStateElements[] =
{
  dStructElement( SaHpiCtrlStateT, Type, SaHpiCtrlTypeType ),
  dStructElement( SaHpiCtrlStateT, StateUnion, SaHpiCtrlStateUnionType ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlStateType = dStruct( SaHpiCtrlStateT, SaHpiCtrlStateElements );


// control rdr record types

static cMarshalType SaHpiCtrlRecDigitalElements[] =
{
  dStructElement( SaHpiCtrlRecDigitalT, Default, SaHpiCtrlStateDigitalType ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlRecDigitalType = dStruct( SaHpiCtrlRecDigitalT, SaHpiCtrlRecDigitalElements );


static cMarshalType SaHpiCtrlRecDiscreteElements[] =
{
  dStructElement( SaHpiCtrlRecDiscreteT, Default, SaHpiCtrlStateDiscreteType ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlRecDiscreteType = dStruct( SaHpiCtrlRecDiscreteT, SaHpiCtrlRecDiscreteElements );


static cMarshalType SaHpiCtrlRecAnalogElements[] =
{
  dStructElement( SaHpiCtrlRecAnalogT, Min, SaHpiCtrlStateAnalogType ),
  dStructElement( SaHpiCtrlRecAnalogT, Max, SaHpiCtrlStateAnalogType ),
  dStructElement( SaHpiCtrlRecAnalogT, Default, SaHpiCtrlStateAnalogType ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlRecAnalogType = dStruct( SaHpiCtrlRecAnalogT, SaHpiCtrlRecAnalogElements );


static cMarshalType SaHpiCtrlRecStreamElements[] =
{
  dStructElement( SaHpiCtrlRecStreamT, Default, SaHpiCtrlStateStreamType ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlRecStreamType = dStruct( SaHpiCtrlRecStreamT, SaHpiCtrlRecStreamElements );


static cMarshalType SaHpiCtrlRecTextElements[] =
{
  dStructElement( SaHpiCtrlRecTextT, MaxChars, SaHpiUint8Type ),
  dStructElement( SaHpiCtrlRecTextT, MaxLines, SaHpiUint8Type ),
  dStructElement( SaHpiCtrlRecTextT, Language, SaHpiLanguageType ),
  dStructElement( SaHpiCtrlRecTextT, DataType, SaHpiTextTypeType ),
  dStructElement( SaHpiCtrlRecTextT, Default, SaHpiCtrlStateTextType ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlRecTextType = dStruct( SaHpiCtrlRecTextT, SaHpiCtrlRecTextElements );


static cMarshalType SaHpiCtrlRecOemConfigDataArray = dArray( SaHpiUint8Type, SAHPI_CTRL_OEM_CONFIG_LENGTH );

static cMarshalType SaHpiCtrlRecOemElements[] =
{
  dStructElement( SaHpiCtrlRecOemT, MId, SaHpiManufacturerIdType ),
  dStructElement( SaHpiCtrlRecOemT, ConfigData, SaHpiCtrlRecOemConfigDataArray ),
  dStructElement( SaHpiCtrlRecOemT, Default, SaHpiCtrlStateOemType ),
  dStructElementEnd()
};

cMarshalType SaHpiCtrlRecOemType = dStruct( SaHpiCtrlRecOemT, SaHpiCtrlRecOemElements );


static cMarshalType SaHpiCtrlRecUnionElements[] =
{
	dUnionElement( SAHPI_CTRL_TYPE_DIGITAL, SaHpiCtrlRecDigitalType ),
	dUnionElement( SAHPI_CTRL_TYPE_DISCRETE, SaHpiCtrlRecDiscreteType ),
	dUnionElement( SAHPI_CTRL_TYPE_ANALOG, SaHpiCtrlRecAnalogType ),
	dUnionElement( SAHPI_CTRL_TYPE_STREAM, SaHpiCtrlRecStreamType ),
	dUnionElement( SAHPI_CTRL_TYPE_TEXT, SaHpiCtrlRecTextType ),
	dUnionElement( SAHPI_CTRL_TYPE_OEM, SaHpiCtrlRecOemType ),
	dUnionElementEnd()
};

static cMarshalType SaHpiCtrlRecUnionType = dUnion( 2, SaHpiCtrlRecUnionT, SaHpiCtrlRecUnionElements );


// control rdr mode

static cMarshalType SaHpiCtrlDefaultModeElements[] =
{
	dStructElement( SaHpiCtrlDefaultModeT, Mode, SaHpiCtrlModeType ),
	dStructElement( SaHpiCtrlDefaultModeT, ReadOnly, SaHpiBoolType),
	dStructElementEnd()
};

cMarshalType SaHpiCtrlDefaultModeType = dStruct( SaHpiCtrlDefaultModeT, SaHpiCtrlDefaultModeElements );


static cMarshalType SaHpiCtrlRecElements[] =
{
	dStructElement( SaHpiCtrlRecT, Num, SaHpiCtrlNumType ),

	dStructElement( SaHpiCtrlRecT, OutputType, SaHpiCtrlOutputTypeType ),
	dStructElement( SaHpiCtrlRecT, Type, SaHpiCtrlTypeType ),
	dStructElement( SaHpiCtrlRecT, TypeUnion, SaHpiCtrlRecUnionType ),
	dStructElement( SaHpiCtrlRecT, DefaultMode, SaHpiCtrlDefaultModeType ),
	dStructElement( SaHpiCtrlRecT, WriteOnly, SaHpiBoolType ),
	dStructElement( SaHpiCtrlRecT, Oem, SaHpiUint32Type),
	dStructElementEnd()
};

cMarshalType SaHpiCtrlRecType = dStruct( SaHpiCtrlRecT, SaHpiCtrlRecElements );


// entity inventory data

static cMarshalType SaHpiIdrFieldTElements[] =
{
  dStructElement( SaHpiIdrFieldT, AreaId, SaHpiEntryIdType ),
  dStructElement( SaHpiIdrFieldT, FieldId, SaHpiEntryIdType ),
  dStructElement( SaHpiIdrFieldT, Type, SaHpiIdrFieldTypeType ),
  dStructElement( SaHpiIdrFieldT, ReadOnly, SaHpiBoolType ),
  dStructElement( SaHpiIdrFieldT, Field, SaHpiTextBufferType ),
  dStructElementEnd()
};

cMarshalType SaHpiIdrFieldType = dStruct( SaHpiIdrFieldT, SaHpiIdrFieldTElements );

static cMarshalType SaHpiIdrAreaHeaderTElements[] =
{
  dStructElement( SaHpiIdrAreaHeaderT, AreaId, SaHpiEntryIdType ),
  dStructElement( SaHpiIdrAreaHeaderT, Type, SaHpiIdrAreaTypeType ),
  dStructElement( SaHpiIdrAreaHeaderT, ReadOnly, SaHpiBoolType ),
  dStructElement( SaHpiIdrAreaHeaderT, NumFields, SaHpiUint32Type ),
  dStructElementEnd()
};

cMarshalType SaHpiIdrAreaHeaderType = dStruct( SaHpiIdrAreaHeaderT, SaHpiIdrAreaHeaderTElements );

static cMarshalType SaHpiIdrInfoTElements[] =
{
  dStructElement( SaHpiIdrInfoT, IdrId, SaHpiIdrIdType ),
  dStructElement( SaHpiIdrInfoT, UpdateCount, SaHpiUint32Type ),
  dStructElement( SaHpiIdrInfoT, ReadOnly, SaHpiBoolType ),
  dStructElement( SaHpiIdrInfoT, NumAreas, SaHpiUint32Type ),
  dStructElementEnd()
};

cMarshalType SaHpiIdrInfoType = dStruct( SaHpiIdrInfoT, SaHpiIdrInfoTElements );


// inventory resource data records

static cMarshalType SaHpiInventoryRecElements[] =
{
  dStructElement( SaHpiInventoryRecT, IdrId, SaHpiUint32Type ),
  dStructElement( SaHpiInventoryRecT, Persistent, SaHpiBoolType ),
  dStructElement( SaHpiInventoryRecT, Oem, SaHpiUint32Type ),
  dStructElementEnd()
};

cMarshalType SaHpiInventoryRecType = dStruct( SaHpiInventoryRecT, SaHpiInventoryRecElements );


// watchdogs

static cMarshalType SaHpiWatchdogElements[] =
{
  dStructElement( SaHpiWatchdogT, Log, SaHpiBoolType ),
  dStructElement( SaHpiWatchdogT, Running, SaHpiBoolType ),
  dStructElement( SaHpiWatchdogT, TimerUse, SaHpiWatchdogTimerUseType ),
  dStructElement( SaHpiWatchdogT, TimerAction, SaHpiWatchdogActionType ),
  dStructElement( SaHpiWatchdogT, PretimerInterrupt, SaHpiWatchdogPretimerInterruptType ),
  dStructElement( SaHpiWatchdogT, PreTimeoutInterval, SaHpiUint32Type ),
  dStructElement( SaHpiWatchdogT, TimerUseExpFlags, SaHpiWatchdogExpFlagsType ),
  dStructElement( SaHpiWatchdogT, InitialCount, SaHpiUint32Type ),
  dStructElement( SaHpiWatchdogT, PresentCount, SaHpiUint32Type ),
  dStructElementEnd()
};

cMarshalType SaHpiWatchdogType = dStruct( SaHpiWatchdogT, SaHpiWatchdogElements );


// watchdog resource data records

static cMarshalType SaHpiWatchdogRecElements[] =
{
  dStructElement( SaHpiWatchdogRecT, WatchdogNum, SaHpiWatchdogNumType ),
  dStructElement( SaHpiWatchdogRecT, Oem, SaHpiUint32Type ),
  dStructElementEnd()
};

cMarshalType SaHpiWatchdogRecType = dStruct( SaHpiWatchdogRecT, SaHpiWatchdogRecElements );


// annunciators

static cMarshalType SaHpiNameDataArray = dArray( SaHpiUint8Type, SA_HPI_MAX_NAME_LENGTH );

static cMarshalType SaHpiNameElements[] =
{
  dStructElement( SaHpiNameT, Length, SaHpiUint16Type ),
  dStructElement( SaHpiNameT, Value, SaHpiNameDataArray ),
  dStructElementEnd()
};

cMarshalType SaHpiNameType = dStruct( SaHpiNameT, SaHpiNameElements );



static cMarshalType SaHpiConditionTypeElements[] =
{
  dStructElement( SaHpiConditionT, Type, SaHpiStatusCondTypeType ),
  dStructElement( SaHpiConditionT, Entity, SaHpiEntityPathType ),
  dStructElement( SaHpiConditionT, DomainId, SaHpiDomainIdType ),
  dStructElement( SaHpiConditionT, ResourceId, SaHpiResourceIdType ),
  dStructElement( SaHpiConditionT, SensorNum, SaHpiSensorNumType ),
  dStructElement( SaHpiConditionT, EventState, SaHpiEventStateType ),
  dStructElement( SaHpiConditionT, Name, SaHpiNameType ),
  dStructElement( SaHpiConditionT, Mid, SaHpiManufacturerIdType ),
  dStructElement( SaHpiConditionT, Data, SaHpiTextBufferType ),
  dStructElementEnd()
};

cMarshalType SaHpiConditionType = dStruct( SaHpiConditionT, SaHpiConditionTypeElements );


static cMarshalType SaHpiAnnouncementTypeElements[] =
{
  dStructElement( SaHpiAnnouncementT, EntryId, SaHpiEntryIdType ),
  dStructElement( SaHpiAnnouncementT, Timestamp, SaHpiTimeType ),
  dStructElement( SaHpiAnnouncementT, AddedByUser, SaHpiBoolType ),
  dStructElement( SaHpiAnnouncementT, Severity, SaHpiSeverityType ),
  dStructElement( SaHpiAnnouncementT, Acknowledged, SaHpiBoolType ),
  dStructElement( SaHpiAnnouncementT, StatusCond, SaHpiConditionType ),
  dStructElementEnd()
};

cMarshalType SaHpiAnnouncementType = dStruct( SaHpiAnnouncementT, SaHpiAnnouncementTypeElements );


// annunciators rdr

static cMarshalType SaHpiAnnunciatorRecElements[] =
{
	dStructElement( SaHpiAnnunciatorRecT, AnnunciatorNum, SaHpiAnnunciatorNumType ),
	dStructElement( SaHpiAnnunciatorRecT, AnnunciatorType, SaHpiAnnunciatorTypeType ),
	dStructElement( SaHpiAnnunciatorRecT, ModeReadOnly, SaHpiBoolType ),
	dStructElement( SaHpiAnnunciatorRecT, MaxConditions, SaHpiUint32Type ),
	dStructElement( SaHpiAnnunciatorRecT, Oem, SaHpiUint32Type ),
	dStructElementEnd()
};

cMarshalType SaHpiAnnunciatorRecType = dStruct( SaHpiAnnunciatorRecT, SaHpiAnnunciatorRecElements );


// rdr

static cMarshalType SaHpiRdrTypeUnionTypeElements[] =
{
        dUnionElement( SAHPI_NO_RECORD, SaHpiVoidType ),
        dUnionElement( SAHPI_CTRL_RDR, SaHpiCtrlRecType ),
        dUnionElement( SAHPI_SENSOR_RDR, SaHpiSensorRecType ),
        dUnionElement( SAHPI_INVENTORY_RDR, SaHpiInventoryRecType ),
        dUnionElement( SAHPI_WATCHDOG_RDR, SaHpiWatchdogRecType ),
        dUnionElement( SAHPI_ANNUNCIATOR_RDR, SaHpiAnnunciatorRecType ),
        dUnionElementEnd()
};

static cMarshalType SaHpiRdrTypeUnionType = dUnion( 1, SaHpiRdrTypeUnionT, SaHpiRdrTypeUnionTypeElements );

static cMarshalType SaHpiRdrElements[] =
{
	dStructElement( SaHpiRdrT, RecordId, SaHpiEntryIdType ),
	dStructElement( SaHpiRdrT, RdrType, SaHpiRdrTypeType ),
	dStructElement( SaHpiRdrT, Entity, SaHpiEntityPathType ),
	dStructElement( SaHpiRdrT, IsFru, SaHpiBoolType ),
	dStructElement( SaHpiRdrT, RdrTypeUnion, SaHpiRdrTypeUnionType ),
	dStructElement( SaHpiRdrT, IdString, SaHpiTextBufferType ),
	dStructElementEnd()
};

cMarshalType SaHpiRdrType = dStruct( SaHpiRdrT, SaHpiRdrElements );


// events part 2

static cMarshalType SaHpiResourceEventTypeElements[] =
{
  dStructElement( SaHpiResourceEventT, ResourceEventType, SaHpiResourceEventTypeType ),
  dStructElementEnd()
};

cMarshalType SaHpiResourceEventType = dStruct( SaHpiResourceEventTypeT, SaHpiResourceEventTypeElements );


static cMarshalType SaHpiDomainEventTypeElements[] =
{
  dStructElement( SaHpiDomainEventT, Type, SaHpiDomainEventTypeType ),
  dStructElement( SaHpiDomainEventT, DomainId, SaHpiDomainIdType ),
  dStructElementEnd()
};

cMarshalType SaHpiDomainEventType = dStruct( SaHpiDomainEventT, SaHpiDomainEventTypeElements );


static cMarshalType SaHpiSensorEventElements[] =
{
  dStructElement( SaHpiSensorEventT, SensorNum, SaHpiSensorNumType ),
  dStructElement( SaHpiSensorEventT, SensorType, SaHpiSensorTypeType ),
  dStructElement( SaHpiSensorEventT, EventCategory, SaHpiEventCategoryType ),
  dStructElement( SaHpiSensorEventT, Assertion, SaHpiBoolType ),
  dStructElement( SaHpiSensorEventT, EventState, SaHpiEventStateType ),
  dStructElement( SaHpiSensorEventT, OptionalDataPresent, SaHpiSensorOptionalDataType ),
  dStructElement( SaHpiSensorEventT, TriggerReading, SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorEventT, TriggerThreshold, SaHpiSensorReadingType ),
  dStructElement( SaHpiSensorEventT, PreviousState, SaHpiEventStateType ),
  dStructElement( SaHpiSensorEventT, CurrentState, SaHpiEventStateType ),
  dStructElement( SaHpiSensorEventT, Oem, SaHpiUint32Type ),
  dStructElement( SaHpiSensorEventT, SensorSpecific, SaHpiUint32Type ),
  dStructElementEnd()
};

cMarshalType SaHpiSensorEventType = dStruct( SaHpiSensorEventT, SaHpiSensorEventElements );


static cMarshalType SaHpiSensorEnableChangeEventTElements[] =
{
  dStructElement( SaHpiSensorEnableChangeEventT, SensorNum, SaHpiSensorNumType ),
  dStructElement( SaHpiSensorEnableChangeEventT, SensorType, SaHpiSensorTypeType ),
  dStructElement( SaHpiSensorEnableChangeEventT, EventCategory, SaHpiEventCategoryType ),
  dStructElement( SaHpiSensorEnableChangeEventT, SensorEnable, SaHpiBoolType ),
  dStructElement( SaHpiSensorEnableChangeEventT, SensorEventEnable, SaHpiBoolType ),
  dStructElement( SaHpiSensorEnableChangeEventT, AssertEventMask, SaHpiEventStateType ),
  dStructElement( SaHpiSensorEnableChangeEventT, DeassertEventMask, SaHpiEventStateType ),
  dStructElement( SaHpiSensorEnableChangeEventT, OptionalDataPresent, SaHpiSensorEnableOptDataType ),
  dStructElement( SaHpiSensorEnableChangeEventT, CurrentState, SaHpiEventStateType ),
  dStructElementEnd()
};

cMarshalType SaHpiSensorEnableChangeEventType = dStruct( SaHpiSensorEnableChangeEventT, SaHpiSensorEnableChangeEventTElements );


static cMarshalType SaHpiHotSwapEventElements[] =
{
  dStructElement( SaHpiHotSwapEventT, HotSwapState, SaHpiHsStateType ),
  dStructElement( SaHpiHotSwapEventT, PreviousHotSwapState, SaHpiHsStateType ),
  dStructElementEnd()
};

cMarshalType SaHpiHotSwapEventType = dStruct( SaHpiHotSwapEventT, SaHpiHotSwapEventElements );


static cMarshalType SaHpiWatchdogEventElements[] =
{
  dStructElement( SaHpiWatchdogEventT, WatchdogNum, SaHpiWatchdogNumType ),
  dStructElement( SaHpiWatchdogEventT, WatchdogAction, SaHpiWatchdogActionEventType ),
  dStructElement( SaHpiWatchdogEventT, WatchdogPreTimerAction, SaHpiWatchdogPretimerInterruptType ),
  dStructElement( SaHpiWatchdogEventT, WatchdogUse, SaHpiWatchdogTimerUseType ),
  dStructElementEnd()
};

cMarshalType SaHpiWatchdogEventType = dStruct( SaHpiWatchdogEventT, SaHpiWatchdogEventElements );

static cMarshalType SaHpiHpiSwEventTypeElements[] =
{
  dStructElement( SaHpiHpiSwEventT, MId, SaHpiManufacturerIdType ),
  dStructElement( SaHpiHpiSwEventT, Type, SaHpiSwEventTypeType ),
  dStructElement( SaHpiHpiSwEventT, EventData, SaHpiTextBufferType ),
  dStructElementEnd()
};

cMarshalType SaHpiHpiSwEventType = dStruct( SaHpiHpiSwEventT, SaHpiHpiSwEventTypeElements );

static cMarshalType SaHpiOemEventTypeElements[] =
{
  dStructElement( SaHpiOemEventT, MId, SaHpiManufacturerIdType ),
  dStructElement( SaHpiOemEventT, OemEventData, SaHpiTextBufferType ),
  dStructElementEnd()
};

cMarshalType SaHpiOemEventType = dStruct( SaHpiOemEventT, SaHpiOemEventTypeElements );

static cMarshalType SaHpiUserEventElements[] =
{
  dStructElement( SaHpiUserEventT, UserEventData, SaHpiTextBufferType ),
  dStructElementEnd()
};

cMarshalType SaHpiUserEventType = dStruct( SaHpiUserEventT, SaHpiUserEventElements );


static cMarshalType SaHpiEventUnionElements[] =
{
  dUnionElement( SAHPI_ET_RESOURCE, SaHpiResourceEventType ),
  dUnionElement( SAHPI_ET_DOMAIN, SaHpiDomainEventType ),
  dUnionElement( SAHPI_ET_SENSOR, SaHpiSensorEventType ),
  dUnionElement( SAHPI_ET_SENSOR_ENABLE_CHANGE, SaHpiSensorEnableChangeEventType ),
  dUnionElement( SAHPI_ET_HOTSWAP, SaHpiHotSwapEventType ),
  dUnionElement( SAHPI_ET_WATCHDOG, SaHpiWatchdogEventType ),
  dUnionElement( SAHPI_ET_HPI_SW, SaHpiHpiSwEventType ),
  dUnionElement( SAHPI_ET_OEM, SaHpiOemEventType ),
  dUnionElement( SAHPI_ET_USER, SaHpiUserEventType ),
  dUnionElementEnd()
};

static cMarshalType SaHpiEventUnionType = dUnion( 1, SaHpiEventUnionT, SaHpiEventUnionElements );

static cMarshalType SaHpiEventElements[] =
{
  dStructElement( SaHpiEventT, Source, SaHpiResourceIdType ),
  dStructElement( SaHpiEventT, EventType, SaHpiEventTypeType ),
  dStructElement( SaHpiEventT, Timestamp, SaHpiTimeType ),
  dStructElement( SaHpiEventT, Severity, SaHpiSeverityType ),
  dStructElement( SaHpiEventT, EventDataUnion, SaHpiEventUnionType ),
  dStructElementEnd()
};

cMarshalType SaHpiEventType = dStruct( SaHpiEventT, SaHpiEventElements );


// resource presence table


static cMarshalType GuidDataArray = dArray( SaHpiUint8Type, 16 );

static cMarshalType SaHpiResourceInfoElements[] =
{
	dStructElement( SaHpiResourceInfoT, ResourceRev, SaHpiUint8Type ),
	dStructElement( SaHpiResourceInfoT, SpecificVer, SaHpiUint8Type ),
	dStructElement( SaHpiResourceInfoT, DeviceSupport, SaHpiUint8Type ),
	dStructElement( SaHpiResourceInfoT, ManufacturerId, SaHpiManufacturerIdType ),
	dStructElement( SaHpiResourceInfoT, ProductId, SaHpiUint16Type ),
	dStructElement( SaHpiResourceInfoT, FirmwareMajorRev, SaHpiUint8Type ),
	dStructElement( SaHpiResourceInfoT, FirmwareMinorRev, SaHpiUint8Type ),
	dStructElement( SaHpiResourceInfoT, AuxFirmwareRev, SaHpiUint8Type ),
	dStructElement( SaHpiResourceInfoT, Guid, GuidDataArray ),
	dStructElementEnd()
};

cMarshalType SaHpiResourceInfoType = dStruct( SaHpiResourceInfoT, SaHpiResourceInfoElements );


static cMarshalType SaHpiRptEntryElements[] =
{
	dStructElement( SaHpiRptEntryT, EntryId, SaHpiEntryIdType ),
	dStructElement( SaHpiRptEntryT, ResourceId, SaHpiResourceIdType ),
	dStructElement( SaHpiRptEntryT, ResourceInfo, SaHpiResourceInfoType ),	
	dStructElement( SaHpiRptEntryT, ResourceEntity, SaHpiEntityPathType ),
	dStructElement( SaHpiRptEntryT, ResourceCapabilities,SaHpiCapabilitiesType ),
	dStructElement( SaHpiRptEntryT, HotSwapCapabilities, SaHpiHsCapabilitiesType ),
	dStructElement( SaHpiRptEntryT, ResourceSeverity, SaHpiSeverityType ),
	dStructElement( SaHpiRptEntryT, ResourceFailed, SaHpiBoolType ),
	dStructElement( SaHpiRptEntryT, ResourceTag, SaHpiTextBufferType ),	
	dStructElementEnd()
};

cMarshalType SaHpiRptEntryType = dStruct( SaHpiRptEntryT, SaHpiRptEntryElements );


// domains

static cMarshalType SaHpiDomainInfoTElements[] =
{
	dStructElement( SaHpiDomainInfoT, DomainId, SaHpiDomainIdType ),
	dStructElement( SaHpiDomainInfoT, DomainCapabilities, SaHpiDomainCapabilitiesType ),
	dStructElement( SaHpiDomainInfoT, IsPeer, SaHpiBoolType ),
	dStructElement( SaHpiDomainInfoT, DomainTag, SaHpiTextBufferType ),
	dStructElement( SaHpiDomainInfoT, DrtUpdateCount, SaHpiUint32Type ),
	dStructElement( SaHpiDomainInfoT, DrtUpdateTimestamp, SaHpiTimeType ),
	dStructElement( SaHpiDomainInfoT, RptUpdateCount, SaHpiUint32Type ),
	dStructElement( SaHpiDomainInfoT, RptUpdateTimestamp, SaHpiTimeType ),
	dStructElement( SaHpiDomainInfoT, DatUpdateCount, SaHpiUint32Type ),
	dStructElement( SaHpiDomainInfoT, DatUpdateTimestamp, SaHpiTimeType ),
	dStructElement( SaHpiDomainInfoT, ActiveAlarms, SaHpiUint32Type ),
	dStructElement( SaHpiDomainInfoT, CriticalAlarms, SaHpiUint32Type ),
	dStructElement( SaHpiDomainInfoT, MajorAlarms, SaHpiUint32Type ),
	dStructElement( SaHpiDomainInfoT, MinorAlarms, SaHpiUint32Type ),
	dStructElement( SaHpiDomainInfoT, DatUserAlarmLimit, SaHpiUint32Type ),
	dStructElement( SaHpiDomainInfoT, DatOverflow, SaHpiBoolType ),
	dStructElement( SaHpiDomainInfoT, Guid, GuidDataArray ),
	dStructElementEnd()
};

cMarshalType SaHpiDomainInfoType = dStruct( SaHpiDomainInfoT, SaHpiDomainInfoTElements );

static cMarshalType SaHpiDrtEntryTElements[] =
{
	dStructElement( SaHpiDrtEntryT, EntryId, SaHpiEntryIdType ),
	dStructElement( SaHpiDrtEntryT, DomainId, SaHpiDomainIdType ),
	dStructElement( SaHpiDrtEntryT, IsPeer, SaHpiBoolType ),
	dStructElementEnd()
};

cMarshalType SaHpiDrtEntryType = dStruct( SaHpiDrtEntryT, SaHpiDrtEntryTElements );

static cMarshalType SaHpiAlarmTElements[] =
{
	dStructElement( SaHpiAlarmT, AlarmId, SaHpiAlarmIdType ),
	dStructElement( SaHpiAlarmT, Timestamp, SaHpiTimeType ),
	dStructElement( SaHpiAlarmT, Severity, SaHpiSeverityType ),
	dStructElement( SaHpiAlarmT, Acknowledged, SaHpiBoolType ),
	dStructElement( SaHpiAlarmT, AlarmCond, SaHpiConditionType ),
	dStructElementEnd()
};

cMarshalType SaHpiAlarmType = dStruct( SaHpiAlarmT, SaHpiAlarmTElements );


// event log

static cMarshalType SaHpiEventLogInfoTElements[] =
{
	dStructElement( SaHpiEventLogInfoT, Entries, SaHpiUint32Type ),
	dStructElement( SaHpiEventLogInfoT, Size, SaHpiUint32Type ),
	dStructElement( SaHpiEventLogInfoT, UserEventMaxSize, SaHpiUint32Type ),
	dStructElement( SaHpiEventLogInfoT, UpdateTimestamp, SaHpiTimeType ),
	dStructElement( SaHpiEventLogInfoT, CurrentTime, SaHpiTimeType ),
	dStructElement( SaHpiEventLogInfoT, Enabled, SaHpiBoolType ),
	dStructElement( SaHpiEventLogInfoT, OverflowFlag, SaHpiBoolType ),
	dStructElement( SaHpiEventLogInfoT, OverflowResetable, SaHpiBoolType ),
	dStructElement( SaHpiEventLogInfoT, OverflowAction, SaHpiSelOverflowActionType ),
	dStructElementEnd()
};

cMarshalType SaHpiEventLogInfoType = dStruct( SaHpiEventLogInfoT, SaHpiEventLogInfoTElements );


static cMarshalType SaHpiEventLogEntryElements[] =
{
  dStructElement( SaHpiEventLogEntryT, EntryId, SaHpiEventLogEntryIdType ),
  dStructElement( SaHpiEventLogEntryT, Timestamp, SaHpiTimeType ),
  dStructElement( SaHpiEventLogEntryT, Event, SaHpiEventType ),
  dStructElementEnd()
};

cMarshalType SaHpiEventLogEntryType = dStruct( SaHpiEventLogEntryT, SaHpiEventLogEntryElements );


// plugin info
static cMarshalType oHpiPluginInfoElements[] =
{
  dStructElement( oHpiPluginInfoT, refcount, refcountType ),
  dStructElementEnd()
};

cMarshalType oHpiPluginInfoType = dStruct( oHpiPluginInfoT, oHpiPluginInfoElements );


// handler info
static cMarshalType plugin_nameBufferArray = dArray( SaHpiUint8Type, MAX_PLUGIN_NAME_LENGTH );

static cMarshalType oHpiHandlerInfoElements[] =
{
  dStructElement( oHpiHandlerInfoT, plugin_name, plugin_nameBufferArray ),
  dStructElementEnd()
};

cMarshalType oHpiHandlerInfoType = dStruct( oHpiHandlerInfoT, oHpiHandlerInfoElements );


// global param
static cMarshalType GlobalParamPathArray = dArray( SaHpiUint8Type, OH_MAX_TEXT_BUFFER_LENGTH );
static cMarshalType GlobalParamVarPathArray = dArray( SaHpiUint8Type, OH_MAX_TEXT_BUFFER_LENGTH );
static cMarshalType GlobalParamConfArray = dArray( SaHpiUint8Type, SAHPI_MAX_TEXT_BUFFER_LENGTH );

static cMarshalType oHpiGlobalParamUnionTypeElements[] =
{
  dUnionElement( OHPI_ON_EP,            SaHpiEntityPathType ),
  dUnionElement( OHPI_LOG_ON_SEV,       SaHpiSeverityType ),
  dUnionElement( OHPI_EVT_QUEUE_LIMIT,  SaHpiUint32Type ),
  dUnionElement( OHPI_DEL_SIZE_LIMIT,   SaHpiUint32Type ),
  dUnionElement( OHPI_DEL_SAVE,         SaHpiBoolType ),
  dUnionElement( OHPI_DAT_SIZE_LIMIT,   SaHpiUint32Type ),
  dUnionElement( OHPI_DAT_USER_LIMIT,   SaHpiUint32Type ),
  dUnionElement( OHPI_THREADED,         SaHpiBoolType ),
  dUnionElement( OHPI_PATH,             GlobalParamPathArray ),
  dUnionElement( OHPI_VARPATH,          GlobalParamVarPathArray ),
  dUnionElement( OHPI_CONF,             GlobalParamConfArray ),
  dUnionElementEnd()
};

static cMarshalType oHpiGlobalParamUnionType = dUnion( 0,
				        	  oHpiGlobalParamUnionT,
                                                  oHpiGlobalParamUnionTypeElements );

static cMarshalType oHpiGlobalParamTypeElements[] =
{
  dStructElement( oHpiGlobalParamT, Type, oHpiGlobalParamTypeType ),
  dStructElement( oHpiGlobalParamT, u,    oHpiGlobalParamUnionType ),
  dStructElementEnd()
};

cMarshalType oHpiGlobalParamType = dStruct( oHpiGlobalParamT, oHpiGlobalParamTypeElements );

