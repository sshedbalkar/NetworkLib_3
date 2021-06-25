#include "MMO_Common.h"
#include "../include/Logging.h"
#include "../include/asyncgetline.h"
#include <iostream>
#include <string>
#include <thread>
#include <curses.h>

class MMOClient : public olc::net::client_interface<GameMsg> {
   private:
    std::unordered_map<uint32_t, sPlayerDescription> mapObjects;
    uint32_t nPlayerID = 0;
    bool bWaitingForConnection = true;
    sPlayerDescription descPlayer;

   public:
    MMOClient() {
        if (Connect("127.0.0.1", 60000)) {
            merci::logging::INFO()
                << "Successfully connected to: 127.0.0.1:60000" << std::endl;
        }
        merci::logging::INFO()
            << "Could not connect to: 127.0.0.1:60000" << std::endl;
    }

    void run() {
        merci::thread::AsyncGetline ag;
        std::string input;
        merci::logging::TRACE() << "Type msg to send to server: " << std::endl;
        while (true) {
            if (IsConnected()) {
                while (!Incoming().empty()) {
                    auto msg = Incoming().pop_front().msg;
                    switch (msg.header.id) {
                        case (GameMsg::Client_Accepted): {
                            merci::logging::INFO()
                                << "Server accepted client - you're in!"
                                << std::endl;
                            olc::net::message<GameMsg> msg;
                            msg.header.id = GameMsg::Client_RegisterWithServer;
                            descPlayer.vPos = {3.0f, 3.0f};
                            msg << descPlayer;
                            Send(msg);
                            break;
                        }

                        case (GameMsg::Client_AssignID): {
                            // Server is assigning us OUR id
                            msg >> nPlayerID;
                            merci::logging::INFO()
                                << "Assigned Client ID = " << nPlayerID
                                << std::endl;
                            break;
                        }

                        case (GameMsg::Game_AddPlayer): {
                            sPlayerDescription desc;
                            msg >> desc;
                            mapObjects.insert_or_assign(desc.nUniqueID, desc);

                            if (desc.nUniqueID == nPlayerID) {
                                // Now we exist in game world
                                bWaitingForConnection = false;
                            }
                            break;
                        }

                        case (GameMsg::Game_RemovePlayer): {
                            uint32_t nRemovalID = 0;
                            msg >> nRemovalID;
                            mapObjects.erase(nRemovalID);
                            break;
                        }

                        case (GameMsg::Game_UpdatePlayer): {
                            sPlayerDescription desc;
                            msg >> desc;
                            mapObjects.insert_or_assign(desc.nUniqueID, desc);
                            break;
                        }

                        case GameMsg::Dummy_Message: {
                            std::string str;
                            msg >> str;
                            merci::logging::INFO()
                                << "[Client][Dummy_Message]: " << str
                                << std::endl;
                            break;
                        }

                        default: {
                            merci::logging::INFO()
                                << "ID: " << uint32_t(msg.header.id) 
                                << std::endl;
                            break;
                        }
                    }
                }
                input = ag.GetLine();

                // Check to see if there was any input.
                if (!input.empty()) {
                    // Print out the user's input to demonstrate it being
                    // processed.
                    merci::logging::TRACE() << "Client: {" << input << "}" << std::endl;;
                    // Check for the exit condition.
                    if (input == "exit") {
                        break;
                    }
                    // Send the user msg to server
                    olc::net::message<GameMsg> m;
                    m.header.id = GameMsg::Dummy_Message;
                    m << input;
                    Send(m);
                }
            }
        }
    }
};

int main() {
    MMOClient client;
    if (client.IsConnected()) {
        std::thread t1(&MMOClient::run, &client);
        t1.join();
    }
    merci::logging::INFO() << "Execution done" << std::endl;
}