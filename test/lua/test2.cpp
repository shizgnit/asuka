#include <asuka.hpp>
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

int main()
{
  AT::Field args;
  args.push(5);
  args.push(4);
  args.push(3);

  args["nothing"] = "something";

  args[3][1] = 75;

  lua_State* state = lua_open();
  
  openstdlibs( state );
  int status = luaL_loadfile( state, "args.lua" );
  lua_setfield(state, args);
  lua_setglobal(state,"arg");
  
  int result = 0;
  if(status == 0)
    result = lua_pcall( state, 0, LUA_MULTRET, 0 );
  else
    std::cout << "bad" << std::endl;
   
  if (result != 0)
    std::cerr << "[C++] script failed" << std::endl;

  std::cout << "[C++] These values were returned from the script" << std::endl;

  AT::Field list = state;

  std::cout<<"list[0][1] '"<<list[0][1]<<"'"<<std::endl;
  std::cout<<"list[0][2] '"<<list[0][2]<<"'"<<std::endl;
  std::cout<<"list[0][3] '"<<list[0][3]<<"'"<<std::endl;
  std::cout<<"list[0][4] '"<<list[0][4]<<"'"<<std::endl;
  std::cout<<"list[0][5] '"<<list[0][5]<<"'"<<std::endl;
  std::cout<<"list[0][6][1] '"<<list[0][6][1]<<"'"<<std::endl;
  std::cout<<"list[0][6][2] '"<<list[0][6][2]<<"'"<<std::endl;
  std::cout<<"list[1] '"<<list[1]<<"'"<<std::endl;
  std::cout<<"list[2] '"<<list[2]<<"'"<<std::endl;
  
  std::cout<<"list[0]['something'] '"<<list[0]["something"]<<"'"<<std::endl;
  
//  AT::Field value;
//  foreach(value, list) {
//    std::cout<<"value '"<<value<<"'"<<std::endl;
//  }

  std::cout<<"finished"<<std::endl;

  return 0;
}
