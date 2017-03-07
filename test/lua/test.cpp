extern "C"
{
   #include <lua.h>
   #include <lauxlib.h>
   #include <lualib.h>
}
#include <iostream>

// a list of libs to expose for use in scripts
static const luaL_reg lualibs[] = 
  {
    {"base", luaopen_base},
    {"table", luaopen_table},
    {"io", luaopen_io},
    {"string", luaopen_string},
    {"math", luaopen_math},
    {"debug", luaopen_debug},
    {"loadlib", luaopen_loadlib},
    /* add your libraries here */
    {NULL, NULL}
  };

// load the list of libs
static void openstdlibs( lua_State *l ) 
{
  const luaL_reg *lib = lualibs;
  for (; lib->func; lib++) 
    {
      lib->func(l);  /* open library */
      lua_settop(l, 0);  /* discard any results */
    }
}

void resolve(lua_State* state, int index=0) {
  if(index<0) {
    index = lua_gettop(state) + index + 1;
  }
  if(index==0) {
    index = lua_gettop(state);
  }
  
  switch (lua_type(state, index)) {
  case LUA_TNUMBER:
    std::cout << "number(" << lua_tonumber(state, index) <<")";
    break;
  case LUA_TSTRING:
    std::cout << "string(" << lua_tostring(state, lua_gettop(state)) <<")";
    break;
  case LUA_TBOOLEAN:
    std::cout << "boolean(" << lua_toboolean(state, lua_gettop(state)) <<")";
    break;
  case LUA_TTABLE:
    std::cout << "table" << std::endl;
    lua_pushnil(state);
    while (lua_next(state, index) != 0) {
      resolve(state, -2);
      std::cout<<" -> ";
      resolve(state, -1);
      std::cout<<std::endl;
      lua_pop(state, 1);
    }
    lua_pop(state, 1);
    break;
  default:
    std::cout << "script returned unknown param";
    break;
  }
}

int main()
{
  int status;
  lua_State* state = lua_open();

  
  openstdlibs( state );
  status = luaL_loadfile( state, "args.lua" );

  std::cout << "[C++] Passing 'arg' array to script" << std::endl;

  // start array structure
  lua_newtable( state );

  // set first element "1" to value 45
  lua_pushnumber( state, 1 );
  lua_pushnumber( state, 45 );
  lua_rawset( state, -3 );

  // set second element "2" to value 99
  lua_pushnumber( state, 2 );
  lua_pushnumber( state, 99 );
  lua_rawset( state, -3 );
  
  lua_pushnumber( state, 3 );
  lua_newtable( state );
   
  lua_pushnumber( state, 1 );
  lua_pushnumber( state, 75 );
   
  lua_rawset( state, -3 );
  lua_rawset( state, -3 );
   
  // set the number of elements (index to the last array element)
  //lua_pushliteral( state, "n" );
  //lua_pushnumber(state, 4);
  //lua_pushnumber( state, 3 );
  //lua_rawset( state, -3 );

  // set the name of the array that the script will access
  lua_setglobal( state, "arg" );


  std::cout << "[C++] Running script" << std::endl;
   
  int result = 0;
  if (status == 0)
    result = lua_pcall( state, 0, LUA_MULTRET, 0 );
  else
    std::cout << "bad" << std::endl;
   
  if (result != 0)
    std::cerr << "[C++] script failed" << std::endl;

  std::cout << "[C++] These values were returned from the script" << std::endl;
  while (lua_gettop( state ))
    {
      resolve(state);
      std::cout<<std::endl;
      lua_pop(state, 1);
    }
  lua_close(state);
  return 0;
}
