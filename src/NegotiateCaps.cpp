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
#include <List.h>

#include "ServerAgent.h"
#include "Vision.h"

#include <stdio.h>

#include <unordered_map>
#include <algorithm>
#include <string>
#include <iostream>

const char* SUPPORTED[] = {
	"message-tags", // enables IRCv3 message tags
	"server-time",
};

typedef std::unordered_map<std::string, std::string> capmap;

/// @brief parse a CAP LIST message into a map of capapbilities (and their 
/// 	   values, if they exist)
/// @param data the final param of the CAP LIST message
capmap GetCaps(BString data)
{
	BStringList split;
	data.Split(" ", false, split);

	capmap caps;

	for (int idx = 0; idx < split.CountStrings(); idx++) {
		BString cap = split.StringAt(idx);

		BString key;
		BString value;

		// check if there's an = somewhere in the cap name
		int eqPos = cap.FindFirst('=');

		if (eqPos != B_ERROR) {
			// if there is, split the key and value into two variables
			cap.MoveInto(key, 0, eqPos);
			cap.MoveInto(value, 1, cap.CountChars());

		} else {
			// otherwise put the whole thing in the key
			key = cap;
		}

		// slap the capability in the map
		caps[key.String()] = value.String();
	}

	return caps;
}

void ServerAgent::NegotiateCaps(BString data)
{
	capmap caps = GetCaps(data);

	if (vision_app->fDebugSend) {
		std::cout << "CAPS: ";
		for (const auto& [k, v] : caps) {
			std::cout << k;
			if (v != "") std::cout << "=" << v;
			std::cout << " ";
		}
		std::cout << std::endl;
	}

	BString stuff_to_send = "CAP REQ :";

	bool blank = true;
	for (const auto& cap : SUPPORTED) {
		bool found = caps.find(cap) != caps.end();

		if (!found) continue;

		blank = false;

		stuff_to_send += cap;
		stuff_to_send += " ";
	}

	if (!blank) SendData(stuff_to_send.Trim().String());
}

void ServerAgent::CapsAcked(BString data)
{
	capmap caps = GetCaps(data);
	fAvailableCaps.merge(caps);
}