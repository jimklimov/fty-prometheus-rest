/*  =========================================================================
    tntnmlm - class for malamute client connection from tntnet

    Copyright (C) 2017 Eaton.

    This software is confidential and licensed under Eaton Proprietary License
    (EPL or EULA).
    This software is not authorized to be used, duplicated or disclosed to
    anyone without the prior written permission of Eaton.
    Limitations, restrictions and exclusions of the Eaton applicable standard
    terms and conditions, such as its EPL and EULA, apply.
    =========================================================================
*/

#ifndef SRC_SHARED_TNTMLM_H
#define SRC_SHARED_TNTMLM_H

// Original idea of using cxxtools::Pool of mlm_client_t* connections
// by Michal Hrusecky <michal@hrusecky.net>

#include <memory>
#include <string>

#include <cxxtools/pool.h>
#include <malamute.h>

class MlmClient {
    public:
        static const std::string ENDPOINT;

        MlmClient ();
        ~MlmClient ();

        // timeout <0, 300> seconds, greater number trimmed
        // based on specified uuid returns expected message or NULL on expire/interrupt
        zmsg_t*     recv (const std::string& uuid, uint32_t timeout);

        // implements request reply pattern
        // method prepends two frames
        zmsg_t*     requestreply (const std::string& address,
                                  const std::string& subject,
                                  uint32_t timeout,
                                  zmsg_t **content_p);
        int         sendto (const std::string& address,
                            const std::string& subject,
                            uint32_t timeout,
                            zmsg_t **content_p);
        bool        connected () { return mlm_client_connected (_client); }
        const char* subject () { return mlm_client_subject (_client); }
        const char* sender () { return mlm_client_sender (_client); }

    private:
        void connect ();
        mlm_client_t*   _client;
        zuuid_t*        _uuid;
        zpoller_t*      _poller;
};

typedef cxxtools::Pool <MlmClient> MlmClientPool;

extern MlmClientPool mlm_pool;

void tntmlm_test (bool verbose);

#endif // SRC_SHARED_TNTMLM_H
