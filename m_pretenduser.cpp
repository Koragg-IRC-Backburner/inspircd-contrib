/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2008 InspIRCd Development Team
 * See: http://www.inspircd.org/wiki/index.php/Credits
 *
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

/* 
   by Chernov-Phoenix Alexey (Phoenix@RusNet) mailto:phoenix /email address separator/ pravmail.ru */

/* $ModDesc: Inject a command from another user */
/* $ModAuthor: Alexey */
/* $ModAuthorMail: Phoenix@RusNet */
/* $ModDepends: core 1.2 */
/* $ModVersion: $Rev: 78 $ */

#include "inspircd.h"

class CommandPretenduser : public Command
{
 public:
        CommandPretenduser (InspIRCd* Instance) : Command(Instance, "PRETENDUSER", "o", 2)
        {
                this->source = "m_pretenduser.so";
                syntax = "<nick> <a line>";
                TRANSLATE3(TR_NICK, TR_TEXT, TR_END);
        }

        CmdResult Handle (const std::vector<std::string>& parameters, User *user)
        {
		User * u=ServerInstance->FindNick(parameters[0]);
		if (u==NULL)
		    return CMD_FAILURE;

		std::string cmd="";
		for (unsigned int i=1; i<parameters.size(); ++i)
		    {
		    if (i>1)
			{
			cmd.append(" ");
			}
		    cmd.append(parameters[i]);
		    }
/*		ServerInstance->Logs->Log(this->source,DEBUG,"loggind fake command: %s",cmd.c_str());*/
		ServerInstance->Parser->ProcessBuffer(cmd,u);
                return CMD_SUCCESS;
        }
};

class ModuleCodepage : public Module
{
	private:
		InspIRCd* ServerInstance;
		CommandPretenduser*  mycommand;
	public:
		ModuleCodepage(InspIRCd* Me)
			: Module(Me)
		{
			ServerInstance=Me;
            		mycommand = new CommandPretenduser(ServerInstance);
            		ServerInstance->AddCommand(mycommand);
		}
                virtual Version GetVersion()
                {
                        return Version(0,0,0,0,VF_VENDOR,API_VERSION);
                }
};


MODULE_INIT(ModuleCodepage)