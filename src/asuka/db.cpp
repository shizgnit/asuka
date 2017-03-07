/*
================================================================================
  $RCSfile: db.cpp,v $ $Revision: 1.2 $ $State: Exp $
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


#include "db.hpp"

#if defined(__mysql__) && defined(__pthread__)

__ASUKA_BEGIN_NAMESPACE


#define LAME_PTHREAD_WORKAROUND }


DB::Connection::Connection(DB::Interface *spec) {
  m_specification = spec;
  mysql_init(&m_mysql);
  //mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"your_prog_name");

  //cerr<<"*** connecting to: ["<<m_specification->hostname()<<":"<<m_specification->username()<<":"<<m_specification->password()<<":"<<m_specification->database()<<"]"<<endl;

  MYSQL *connect = mysql_real_connect(
    &m_mysql,
    m_specification->hostname(),
    m_specification->username(),
    m_specification->password(),
    m_specification->database(),
    0,
    NULL,
    0
  );

  if (connect != &m_mysql) {
    AT::Field error;
    error[0] = mysql_errno(&m_mysql);
    error[1] = mysql_error(&m_mysql);
    throw(error);
  }
}

DB::Connection::~Connection(void) {

}


DB::Query::Query(DB::Interface *spec) {
  m_instance = new instance;
  m_instance->m_specification = spec;
}

DB::Query::Query(const DB::Query &copy) {
  m_instance = copy.m_instance;
  if(m_instance) {
    m_instance->m_count++;
  }
}

DB::Query::~Query(void) {
  if(m_instance && --m_instance->m_count <= 0) {
    disconnect();
    delete(m_instance);
    m_instance = NULL;
  }
}

DB::Query &DB::Query::operator=(DB::Query &copy) {
  if(m_instance && --m_instance->m_count <= 0) {
    delete(m_instance);
    m_instance = NULL;
  }
  m_instance = copy.m_instance;
  if(m_instance) {
    m_instance->m_count++;
  }
  return(*this);
}

DB::Query &DB::Query::prepare(AT::Field qstr, AT::Field args) {
  m_instance->query(qstr, args);
  return(*this);
}

DB::Query &DB::Query::exec(AT::Field qstr, AT::Field args) {
  if(!m_instance) {
    return(*this);
  }

  m_instance->m_status = -1;

  if(!connect()) {
    m_instance->m_status = 0;
    return(*this);
  }

  if(qstr) {
    prepare(qstr, args);
  }
  else {
    if(!m_instance->query()) {
      m_instance->m_status = 0;
      return(*this);
    }
  }

  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_mutex_lock(m_instance->mutex());
  AT::Integer32 rval = ::pthread_create(m_instance->thread(), &attr, DB::pthread_pre, (void *)this->m_instance);
  if(rval) {
    AT::Field error;
    error[0] = rval;
    switch(rval) {
      case(EINVAL):
        error[1] = "pthread_create, The value specified for the argument is not correct";
        break;
      case(EAGAIN):
        error[1] = "pthread_create, The system didn't have enough resources to create another thread or the maximum number of threads for this job has been reached";
        break;
      case(EBUSY):
        error[1] = "pthread_create, The system cannot allow thread creation in this process at this time";
        break;
      default:
        error[1] = "pthread_create, Unknown error";
        break;
    };
    pthread_mutex_unlock(m_instance->mutex());
    m_instance->m_status = 0;
    throw(error);
  }

  pthread_attr_destroy(&attr);

  return(*this);
}

AT::Integer32 DB::Query::status(AT::Boolean block) {
  if(!m_instance) {
    return(0);
  }
  if(m_instance->m_status == -1) {
    if(block) {
      pthread_join(*m_instance->thread(), (void **)&m_instance->m_status);
    }
    else {
      if(pthread_mutex_trylock(m_instance->mutex()) != EBUSY) {
        pthread_join(*m_instance->thread(), (void **)&m_instance->m_status);
      }
    }
  }
  if(m_instance->m_error.defined()) {
    throw(m_instance->m_error);
  }
  return(m_instance->m_status);
}

AT::Boolean DB::Query::connect(AT::Boolean reconnect) {
  if(!m_instance) {
    return(false);
  }

  Resource::Limiter *pool = m_instance->m_specification->pool();

  if(m_instance->m_connection) {
    if(reconnect) {
      disconnect();
    }
    else {
//
// Not sure why mysql_ping isn't working... this will have to be fixed.
//
//      if(!mysql_ping(m_instance->m_connection->handle())) {
//        std::cout<<"*** failed to ping"<<std::endl;
//        return(false);
//      }
      return(true);
    }
  }

  if(pool) {
    while(!(m_instance->m_connection = (Connection *)pool->reserve()) && pool->count() < 10) {
      pool->release(new Connection(m_instance->m_specification));
    }
    if(!m_instance->m_connection) {
      return(false);
    }
  }
  else {
    if(!m_instance->m_connection) {
      m_instance->m_connection = new Connection(m_instance->m_specification);
    }
    if(!m_instance->m_connection) {
      return(false);
    }
  }

  return(true);
}

AT::Boolean DB::Query::disconnect() {
  Resource::Limiter *pool = m_instance->m_specification->pool();

  if(!m_instance->m_connection) {
    return(true);
  }

  if(pool) {
    pool->release(m_instance->m_connection);
  }
  else {
    delete(m_instance->m_connection);
  }

  m_instance->m_connection = NULL;
  return(true);
}

DB::Query::operator AT::Field(void) {
  AT::Field results;
  AT::Integer32 val;
  if(val = status(true)) {
    results = &m_instance->m_results;
  }
  return(results);
}

AT::Field DB::Query::escape(AT::Field in_str) {
  AT::Character *str = in_str;
  AT::Unsigned32 length = strlen(str);

  AT::Field value;

  AT::Character *escaped = (AT::Character *)malloc((length*2) + 1);
  mysql_real_escape_string(m_instance->m_connection->handle(), escaped, str, length);
  value = escaped;

  return(value);
}

void *DB::pthread_pre(void *i) {
  Query::instance *instance = (Query::instance *)i;
  pthread_cleanup_push(DB::pthread_post, i); LAME_PTHREAD_WORKAROUND
  return(instance->thread_exec());
}

void DB::pthread_post(void *i) {
  Query::instance *instance = (Query::instance *)i;
  pthread_mutex_unlock(instance->mutex());
};

DB::Query::instance::instance(void) {
  pthread_mutex_init(&m_running, NULL);
  m_count = 1;
  m_specification = NULL;
  m_connection = NULL;
  m_status = 0;
}

DB::Query::instance::~instance(void) {
  pthread_mutex_destroy(&m_running);
}

AT::Field DB::Query::instance::query(AT::Field qstr, AT::Field args) {
  if(qstr.defined()) {
    m_qstr = qstr;
    m_args = args;
  }
  return(m_qstr);
}

void *DB::Query::instance::thread_exec(void) {
  AT::Integer32 num_rows = 0;

  MYSQL_RES *result;
  MYSQL_ROW row;

  AT::Field qstr;

  if(m_args.defined()) {
    qstr = AT::substitute(m_qstr, m_args);
  }
  else {
    qstr = m_qstr;
  }
  
  if(mysql_real_query(m_connection->handle(), qstr, strlen(qstr))) {
    m_error[0] = mysql_errno(m_connection->handle());
    m_error[1] = mysql_error(m_connection->handle());
  }
  else {
    m_results.reset();
    result = mysql_store_result(m_connection->handle());

    if(result) {
      MYSQL_FIELD *field;

      AT::Field metadata;
      while((field = mysql_fetch_field(result))) {
        AT::Field entry;

        entry["name"]       = field->name;
        entry["table"]      = field->table;
        entry["org_table"]  = field->org_table;
        entry["db"]         = field->db;
        entry["def"]        = field->def;
        entry["length"]     = field->length;
        entry["max_length"] = field->max_length;
        entry["flags"]      = field->flags;
        entry["decimals"]   = field->decimals;
        entry["type"]       = field->type;

        metadata.push(&entry);
      }

      AT::Integer32 num_fields;
      AT::Integer32 i;
      
      num_fields = mysql_num_fields(result);
      while (row = mysql_fetch_row(result)) {
        AT::Field entry;
        num_rows++;
        AT::Unsigned32 *lengths;
        lengths = mysql_fetch_lengths(result);
        for(i = 0; i < num_fields; i++) {
          entry[metadata[i]["name"]] = row[i];
        }
        m_results.push(&entry);
      }
    }
    else {
      if(mysql_field_count(m_connection->handle()) == 0) {
        num_rows = mysql_affected_rows(m_connection->handle());
      }
      else {
        AT::Field error;
        error[0] = mysql_errno(m_connection->handle());
        error[1] = mysql_error(m_connection->handle());
        throw(error);
      }
    }
  }

  pthread_exit((void*) num_rows);
  return(NULL);
}

DB::Interface::Interface(AT::Field hostname, AT::Field username, AT::Field password, AT::Field database, Resource::Limiter *pool) {
  m_hostname = hostname;
  m_username = username;
  m_password = password;
  m_database = database;
  
  if(pool) {
    m_pool = pool;
  }
  else {
    m_pool = new AT::Resource::Limiter();
  }
}

DB::Interface::~Interface(void) {

}

DB::Query DB::Interface::prepare(AT::Field qstr, AT::Field args) {
  return(Query(this).prepare(qstr, args));
}

DB::Query DB::Interface::exec(AT::Field qstr, AT::Field args) {
  return(Query(this).exec(qstr, args));
}

__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:

