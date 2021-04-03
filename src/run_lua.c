//ＵＴＦ８
//////////////////////////////////////////
// run LUAfile for MinGW
//////////////////////////////////////////


// 修正履歴
//
//
#include <unistd.h>
#include <lua.hpp>
#include <stdlib.h>

void my_system_register(lua_State *L);

#if 0
int setLuaPath( lua_State* L, const char* path )
{
    const char *cur_path;
    char* mypath;
    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" );
    cur_path = lua_tostring( L, -1);

	printf("package,path=[%s]\n", cur_path );

    mypath = (char *)malloc(strlen(cur_path) + strlen(path) + 9);
	if( NULL == mypath ){ perror("out of memory"); exit(1); }
    strcpy(mypath,cur_path);
    strcat(mypath,";");
    strcat(mypath,path);
    strcat(mypath,"?.lua;");
    lua_pop( L, 1 );
    lua_pushstring( L, mypath) ;
    lua_setfield( L, -2, "path" );
    lua_pop( L, 1 );
    return 0;
}
#endif


int run_lua(const char *filename)
{
	int ret;
	lua_State *L;

	L = luaL_newstate();
	luaL_openlibs(L); 		// use lua standard libs.
	my_system_register(L);	//register NMR functions.

	ret = luaL_dofile(L, filename); // execute lua file
	if( ret ){
		printf("run_lua:error\n");
	}
	lua_close(L);

    return 0;
}


/*** EOF ***/
