/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 *
 * Contributor(s): Leah Clark <leah@pronounmail.com>
 */

#include <String.h>
#include <StringList.h>

#include "ServerAgent.h"

#include <stdio.h>
#include <unordered_map>
#include <string>

// https://ircv3.net/specs/extensions/capability-negotiation.html if you need it

void EscapeTagValue(BString data)
{
    data.ReplaceAll("\\:", ";");
    data.ReplaceAll("\\s", " ");
    data.ReplaceAll("\\\\", "\\");
    data.ReplaceAll("\\r", "\r");
    data.ReplaceAll("\\n", "\n");
}

std::unordered_map<std::string, std::string> ServerAgent::ParseTags(const char* line)
{
    std::unordered_map<std::string, std::string> tags;

    BString data = line;

    data.RemoveChars(0, 1); // remove leading @
    
    BStringList tagStrings; 
    data.Split(";", true, tagStrings); // split into individual tags

    for (int idx = 0; idx < tagStrings.CountStrings(); idx++) {
        BString tag = tagStrings.StringAt(idx);

		BString key;
		BString value;

        int eqPos = tag.FindFirst('=');

        if (eqPos != B_ERROR) {
			tag.MoveInto(key, 0, eqPos);
			tag.MoveInto(value, 1, tag.CountChars());
		} else {
			key = tag;
		}

        EscapeTagValue(key);
        EscapeTagValue(value);

        tags[key.String()] = value.String();
    }

    return tags;
}
