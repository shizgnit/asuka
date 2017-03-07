/*
================================================================================
  $RCSfile: db.hpp,v $ $Revision: 1.2 $ $State: Exp $
--------------------------------------------------------------------------------

  Copyright (c) 2005, Dee E. Abbott
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

================================================================================
*/


#include "asuka.hpp"

#ifndef __ASUKA_DB_HPP
#define __ASUKA_DB_HPP

__ASUKA_BEGIN_NAMESPACE

/*
for query

//      case(CR_COMMANDS_OUT_OF_SYNC): cerr<<"Commands were executed in an improper order."<<endl;
//      case(CR_SERVER_GONE_ERROR): cerr<<"The MySQL server has gone away."<<endl;
//      case(CR_SERVER_LOST): cerr<<"The connection to the server was lost during the query."<<endl;
//      case(CR_UNKNOWN_ERROR): cerr<<"An unknown error occurred."<<endl;

*/

/*
For connect

CR_CONN_HOST_ERROR
Failed to connect to the MySQL server.
CR_CONNECTION_ERROR
Failed to connect to the local MySQL server.
CR_IPSOCK_ERROR
Failed to create an IP socket.
CR_OUT_OF_MEMORY
Out of memory.
CR_SOCKET_CREATE_ERROR
Failed to create a Unix socket.
CR_UNKNOWN_HOST
Failed to find the IP address for the hostname.
CR_VERSION_ERROR
A protocol mismatch resulted from attempting to connect to a server with a client library that uses a different protocol version. This can happen if you use a very old client library to connect to a new server that wasn't started with the --old-protocol option.
CR_NAMEDPIPEOPEN_ERROR
Failed to create a named pipe on Windows.
CR_NAMEDPIPEWAIT_ERROR
Failed to wait for a named pipe on Windows.
CR_NAMEDPIPESETSTATE_ERROR
Failed to get a pipe handler on Windows.
CR_SERVER_LOST
If connect_timeout > 0 and it took longer than connect_timeout seconds to connect to the server or if the server died while executing the init-command.
*/

/*
for ping

CR_COMMANDS_OUT_OF_SYNC
Commands were executed in an improper order.
CR_SERVER_GONE_ERROR
The MySQL server has gone away.
CR_UNKNOWN_ERROR
An unknown error occurred.

*/

namespace DB {

  void *pthread_pre(void *i);
  void pthread_post(void *i);
  
  class Interface;

  class Connection : public AT::Resource::Limited {
    friend class Query;
    public:
      Connection(Interface *spec);
      ~Connection(void);

      MYSQL *handle(void) { return(&m_mysql); }

    private:
      Connection(void) { /*NULL*/ }

    private:
      Interface *m_specification;
      MYSQL m_mysql;
  };

  class Query {
    
    public:
      Query(Interface *spec);
      Query(const Query &copy);
      ~Query(void);

      Query &operator=(Query &copy);

      Query &prepare(AT::Field qstr=undef, AT::Field args=undef);
      Query &exec(AT::Field qstr=undef, AT::Field args=undef);

      AT::Integer32 status(AT::Boolean block=true);

      AT::Boolean connect(AT::Boolean reconnect=false);
      AT::Boolean disconnect(void);

      AT::Field escape(AT::Field str);

      AT::Field metadata(void);

      operator AT::Field(void);

    private:
      Query(void);

    public:
      class instance {
        friend class Query;
        private:
          instance(void);
          ~instance(void);

          AT::Field query(AT::Field qstr=undef, AT::Field args=undef);

        public:
          void *thread_exec(void);
          pthread_t *thread(void) { return(&m_handle); }
          pthread_mutex_t *mutex(void) { return(&m_running); }

        private:
          AT::Unsigned32 m_count;

          AT::Field m_qstr;
          AT::Field m_args;

          Interface *m_specification;
          Connection *m_connection;

          pthread_t m_handle;
          pthread_mutex_t m_running;

          AT::Integer32 m_status;

          AT::Field m_results;
          AT::Field m_error;
      };
      
      instance *m_instance;
  };

  class Interface {
    public:
      Interface(AT::Field hostname, AT::Field username, AT::Field password, AT::Field database, AT::Resource::Limiter *pool=NULL);
      ~Interface(void);

      Query prepare(AT::Field qstr, AT::Field args=undef);
      Query exec(AT::Field qstr=undef, AT::Field args=undef);
      
      AT::Field hostname(void) { return(m_hostname); }
      AT::Field username(void) { return(m_username); }
      AT::Field password(void) { return(m_password); }
      AT::Field database(void) { return(m_database); }

      AT::Resource::Limiter *pool(void) { return(m_pool); }
      
    private:
      AT::Field m_hostname;
      AT::Field m_username;
      AT::Field m_password;
      AT::Field m_database;

      AT::Resource::Limiter *m_pool;
  };

}


__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:

