//////////////////////////////////////////
// run LUAfile for MinGW
//////////////////////////////////////////


// 修正履歴
//
//
#include <unistd.h>


#include <lua.hpp>

//#define lua_open()	luaL_newstate()
void my_system_register(lua_State *L);


int run_lua(const char *filename)
{
	char *ans;
	int n;
	lua_State *L = luaL_newstate();
	luaL_openlibs(L); 		// use lua standard libs.
	my_system_register(L);	//register NMR functions.

	luaL_dofile(L, filename); // execute lua file

	lua_close(L);
    return 0;
}

/*** EOF ***/
