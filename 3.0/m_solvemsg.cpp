/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2015-2016 Peter Powell <petpow@saberuk.com>
 *
 * This file is part of InspIRCd.  InspIRCd is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// $ModAuthor: Peter "SaberUK" Powell
/// $ModAuthorMail: petpow@saberuk.com
/// $ModDepends: core 3
/// $ModDesc: Requires users to solve a basic maths problem before messaging others.


#include "inspircd.h"

struct Problem
{
	int first;
	int second;
	bool warned;
};

class CommandSolve : public SplitCommand
{
 private:
	SimpleExtItem<Problem>& ext;

 public:
	CommandSolve(Module* Creator, SimpleExtItem<Problem>& Ext)
		: SplitCommand(Creator, "SOLVE", 1, 1)
		, ext(Ext)
	{
	}

	CmdResult HandleLocal(LocalUser* user, const Params& parameters) CXX11_OVERRIDE
	{
		if (user->exempt)
		{
			user->WriteNotice("*** You do not need to solve a problem!");
			return CMD_FAILURE;
		}

		Problem* problem = ext.get(user);
		if (!problem)
		{
			user->WriteNotice("** You have already solved your problem!");
			return CMD_FAILURE;
		}

		int result = ConvToNum<int>(parameters[0]);
		if (result != (problem->first + problem->second))
		{
			user->WriteNotice(InspIRCd::Format("*** %s is not the correct answer.", parameters[0].c_str()));
			user->CommandFloodPenalty += 10000;
			return CMD_FAILURE;
		}

		ext.unset(user);
		user->WriteNotice(InspIRCd::Format("*** %s is the correct answer!", parameters[0].c_str()));
		return CMD_SUCCESS;
	}
};

class ModuleSolveMessage : public Module
{
 private:
	SimpleExtItem<Problem> ext;
	CommandSolve cmd;

 public:
	ModuleSolveMessage()
		: ext("solve-message", ExtensionItem::EXT_USER, this)
		, cmd(this, ext)
	{
	}

	void OnUserPostInit(LocalUser* user) CXX11_OVERRIDE
	{
		Problem problem;
		problem.first = ServerInstance->GenRandomInt(9);
		problem.second = ServerInstance->GenRandomInt(9);
		problem.warned = false;
		ext.set(user, problem);
	}

	ModResult OnUserPreMessage(User* user, const MessageTarget& msgtarget, MessageDetails& details) CXX11_OVERRIDE
	{
		LocalUser* source = IS_LOCAL(user);
		if (!source || source->exempt || msgtarget.type != MessageTarget::TYPE_USER)
			return MOD_RES_PASSTHRU;

		User* target = msgtarget.Get<User>();
		if (target->server->IsULine())
			return MOD_RES_PASSTHRU;

		Problem* problem = ext.get(user);
		if (!problem)
			return MOD_RES_PASSTHRU;

		if (problem->warned)
			return MOD_RES_DENY;

		user->WriteNotice("*** Before you can send messages you must solve the following problem:");
		user->WriteNotice(InspIRCd::Format("*** What is %d + %d?", problem->first, problem->second));
		user->WriteNotice("*** You can enter your answer using /QUOTE SOLVE <answer>");
		problem->warned = true;
		return MOD_RES_DENY;
	}

	Version GetVersion() CXX11_OVERRIDE
	{
		return Version("Requires users to solve a basic maths problem before messaging others.");
	}
};

MODULE_INIT(ModuleSolveMessage)
