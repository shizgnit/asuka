#include <asuka.hpp>
#include <iostream>

static const luaL_reg lualibs[] = {
  {"base", luaopen_base},
  {"table", luaopen_table},
  {"io", luaopen_io},
  {"string", luaopen_string},
  {"math", luaopen_math},
  {"debug", luaopen_debug},
  {NULL, NULL}
};

static void lua_loadlibs(lua_State *state) {
  const luaL_reg *lib = lualibs;
  for(; lib->func; lib++) {
    lib->func(state);
    lua_settop(state, 0);
  }
}

int main() {
  try {
    AT::Field arguments;
    arguments[0] = 20;
    arguments["nothing"] = "something";
    lua_State* state = luaL_newstate();
//    lua_loadlibs(state);
    luaL_openlibs(state);
    if(luaL_loadfile(state, "example.lua")) {
      throw("failed to load example.lua");
    }
    lua_setfield(state, arguments);
    lua_setglobal(state,"arg");
    if(lua_pcall(state, 0, LUA_MULTRET, 0)) {
      throw("script failure");
    }
    AT::Field value;
    foreach(value, state) {
      std::cout<<"rval '"<<value<<"'"<<std::endl;
    }
  }
  catch(...) {
    std::cout<<"-exception-"<<std::endl;
  };
  std::cout<<"-finished-"<<std::endl;
  return 0;
}
