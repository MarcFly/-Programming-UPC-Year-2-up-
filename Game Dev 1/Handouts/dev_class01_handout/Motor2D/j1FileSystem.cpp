#include "p2Defs.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1FileSystem.h"
#include "SDL/include/SDL_rwops.h"
#include "SDL/include/SDL_filesystem.h"
#include "PhysFS/include/physfs.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

j1FileSystem::j1FileSystem(const char* game_path) : j1Module()
{
	name.create("file_system");

	// TODO 1: Init PhysFS lib

	PHYSFS_init(game_path);

	if (GetLastError() != NULL)
		LOG("O Shit Waddup - INIT");
	else
		LOG("Dank INIT");
	// TODO 2: Mount directory ".", then mount "data.zip"
	// files in the folder should take precendence over the zip file!
	PHYSFS_mount(".", NULL, 0);
	if (GetLastError() != NULL)
		LOG("O Shit Waddup - Game Dir");
	else
		LOG("Dank Mount Dir");

	PHYSFS_mount("data.zip", NULL, 1);
	if (GetLastError() != NULL)
		LOG("O Shit Waddup - data.zip");
	else
		LOG("Dank Mount Data");
}

// Destructor
j1FileSystem::~j1FileSystem()
{
	// TODO 1: stop PhysFS lib
	PHYSFS_deinit();
	if (GetLastError() != NULL)
		LOG("O Shit Waddup - DEINIT");
	else
		LOG("Dank DEINIT");
}

// Called before render is available
bool j1FileSystem::Awake()
{
	LOG("Loading File System");
	bool ret = true;

	// This is a good place to setup the write dir (homework)

	return ret;
}

// Read a whole file and put it in a new buffer
unsigned int j1FileSystem::Load(const char* file, char** buffer) const
{
	unsigned int ret = 0;

	// TODO 3: Read a file and allocate needed bytes to buffer and write all data into it
	// return the size of the data
	
	PHYSFS_File* handle_temp = PHYSFS_openRead(file);

	ret = PHYSFS_fileLength(handle_temp);

	*buffer = new char[ret];

	PHYSFS_read(handle_temp, *buffer, ret, 1 );

	PHYSFS_close(handle_temp);

	if (GetLastError() != NULL)
		LOG("O Shit Waddup - read %s", file);
	else
		LOG("Dank Test.png %s", file);

	return ret;
}

// Read a whole file and put it in a new buffer
SDL_RWops* j1FileSystem::Load(const char* file) const
{
	// TODO 4: Using our previous Load method
	// create a sdl rwops using SDL_RWf_From_ConstMem()
	// and return it if everything goes well

	char** buffer = new char*;

	SDL_RWops* RWops_File = NULL;

	unsigned int size = Load(file, buffer);

	RWops_File = SDL_RWFromConstMem(*buffer, size);

	RWops_File->close = close_sdl_rwops; //Makes the RWops do a specific function rather than the standard one

	if (GetLastError() != NULL) {
		LOG("O Shit Waddup - Create RWops");
		return NULL;
	}
	else {
		LOG("Dank RWops Creation?");
		return RWops_File;
	}
}

// TODO 4: you will need to use this small helper function
// to make sure all data is freed automatically
int close_sdl_rwops(SDL_RWops *rw)
{
	RELEASE(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

/*
unsigned int j1FileSystem::Load(const char file, char* buffer) const
{
	unsigned int ret = 0;

	// TODO 3: Read a file and allocate needed bytes to buffer and write all data into it
	// return the size of the data
	PHYSFS_File* tmp_file;
	if ((tmp_file = PHYSFS_openRead(file)) == NULL)
		LOG("File System Open File Error: %s", PHYSFS_getLastError());
	else {
		ret = Size(tmp_file);
		if (ret == -1) LOG("File System Get Lenght Error: %s", PHYSFS_getLastError());
		else {
			*buffer = new char[ret];
			if (PHYSFS_read(tmp_file, *buffer, ret, 1) == -1)
				LOG("File System Read Error: %s", PHYSFS_getLastError());
			if (PHYSFS_close(tmp_file) == 0)
				LOG("File System Close File Error: %s", PHYSFS_getLastError());
		}
	}
	return ret;
}

// Read a whole file and put it in a new buffer
SDL_RWops j1FileSystem::Load(const char file) const
{
	// TODO 4: Using our previous Load method
	// create a sdl rwops using SDL_RWFromConstMem()
	// and return it if everything goes well
	char** buffer = new char*;
	SDL_RWops* rw = NULL;
	unsigned int size = Load(file, buffer);
	if (size != -1) {
		if ((rw = SDL_RWFromConstMem(*buffer, size)) == NULL) {
			LOG("File System Error Creating RWoops: %s", SDL_GetError());
			return NULL;
		}
		rw->close = close_sdl_rwops;

	}

	delete buffer;

	return rw;
}*/