/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *    W. David Ashley <dashley@us.ibm.com>
 */


#ifndef Included_oSaHpiResourceEvent
#define Included_oSaHpiResourceEvent

#include <stdio.h>
extern "C"
{
#include <SaHpi.h>
}


class oSaHpiResourceEvent : public SaHpiResourceEventT {
    public:
        // constructors
        oSaHpiResourceEvent();
        // copy constructor
        oSaHpiResourceEvent(const oSaHpiResourceEvent& buf);
        // destructor
        ~oSaHpiResourceEvent() {
        }
        // other methods
        static bool assignField(SaHpiResourceEventT * ptr,
                                const char *field,
                                const char *value);
        inline bool assignField(const char *field,
                                const char *value) {
            return assignField(this, field, value);
        }
        inline SaHpiResourceEventT *getStruct(void) {
            return this;
        }
        static bool fprint(FILE *stream,
                           const int indent,
                           const SaHpiResourceEventT *buffer);
        inline bool fprint(FILE *stream,
                           const int indent) {
            return fprint(stream, indent, this);
        }
};

#endif

