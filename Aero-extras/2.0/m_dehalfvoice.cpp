
#include "inspircd.h"

/** Handle /DEVOICE
 */
class CommandDehalfvoice : public Command
{
 public:
        CommandDehalfvoice(Module* Creator) : Command(Creator,"DEHALFVOICE", 1)
        {
                syntax = "<channel>";
                TRANSLATE2(TR_TEXT, TR_END);
        }

        CmdResult Handle (const std::vector<std::string> &parameters, User *user)
        {
                Channel* c = ServerInstance->FindChan(parameters[0]);
                if (c && c->HasUser(user))
                {
                        std::vector<std::string> modes;
                        modes.push_back(parameters[0]);
                        modes.push_back("-E");
                        modes.push_back(user->nick);

                        ServerInstance->SendGlobalMode(modes, ServerInstance->FakeClient);
                        return CMD_SUCCESS;
                }

                return CMD_FAILURE;
        }
};

class ModuleDeHalfVoice : public Module
{
        CommandDehalfvoice cmd;
 public:
        ModuleDeHalfVoice() : cmd(this)
        {
        }

        void init()
        {
                ServerInstance->Modules->AddService(cmd);
        }

        virtual ~ModuleDeHalfVoice()
        {
        }

        virtual Version GetVersion()
        {
                return Version("Provides halfvoiced users with the ability to dehalfvoice themselves.", VF_VENDOR);
        }
};

MODULE_INIT(ModuleDeHalfVoice)
