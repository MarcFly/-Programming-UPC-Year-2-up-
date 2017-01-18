#include "p2Defs.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1FileSystem.h"
#include "PhysFS/include/physfs.h"
#include "SDL/include/SDL.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

j1FileSystem::j1FileSystem() : j1Module()
{
	name.create("file_system");

	// need to be created before Awake so other modules can use it
	//H1 TODO 1 - Init PhysFS lib, without given path
	char* game_path = SDL_GetBasePath();
	PHYSFS_init(game_path);
	SDL_free(game_path);
	//

	// By default we include executable's own directory
	// without this we won't be able to find config.xml :-(

	//H1 TODO 2 - Add exucatble’s directory, or mount base directory and data.zip
	
	AddPath("."); // This is Rick’s way in H11, used everywhere else

	/*In my H1 Handout, works...
	PHYSFS_mount(".", NULL, 0);
	PHYSFS_mount("data.zip", NULL, 1);
	*/
}

// Destructor
j1FileSystem::~j1FileSystem()
{
	//H1 TODO 1.1 - Deinit PhysFS lib
	PHYSFS_deinit();
	//
}

// Called before render is available
bool j1FileSystem::Awake(pugi::xml_node& config)
{
	LOG("Loading File System");
	bool ret = true;

	// Add all paths in configuration in order
	for(pugi::xml_node path = config.child("path"); path; path = path.next_sibling("path"))
	{
		AddPath(path.child_value());
	}

	// Ask SDL for a write dir
	char* write_path = SDL_GetPrefPath(App->GetOrganization(), App->GetTitle());

	if(PHYSFS_setWriteDir(write_path) == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());
	else
	{
		// We add the writing directory as a reading directory too with speacial mount point
		LOG("Writing directory is %s\n", write_path);
		AddPath(write_path, GetSaveDirectory());
	}

	SDL_free(write_path);

	return ret;
}

// Called before quitting
bool j1FileSystem::CleanUp()
{
	//LOG("Freeing File System subsystem");
	return true;
}

// Add a new zip file or folder
bool j1FileSystem::AddPath(const char* path_or_zip, const char* mount_point)
{
	bool ret = false;

	if(PHYSFS_mount(path_or_zip, mount_point, 1) == 0)
		LOG("File System error while adding a path or zip(%s): %s\n", path_or_zip, PHYSFS_getLastError());
	else
		ret = true;

	return ret;
}

// Check if a file exists
bool j1FileSystem::Exists(const char* file) const
{
	return PHYSFS_exists(file) != 0;
}

// Check if a file is a directory
bool j1FileSystem::IsDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;
}

// Read a whole file and put it in a new buffer
unsigned int j1FileSystem::Load(const char* file, char** buffer) const
{
	unsigned int ret = 0;

	//H1 TODO 3: Read a file and allocate needed bytes to buffer and write all data into it
	// return the size of the data

	PHYSFS_file* fs_file = PHYSFS_openRead(file); //Puts 'physical' file into a variable here

	if(fs_file != NULL)
	{
		PHYSFS_sint64 size = PHYSFS_fileLength(fs_file);

		if(size > 0)
		{
			*buffer = new char[(uint)size]; //New buffer based on size of the file
			PHYSFS_sint64 readed = PHYSFS_read(fs_file, *buffer, 1, (PHYSFS_sint32)size); //checks size
			if(readed != size)
			{
				LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE(buffer); //if error, buffer is eliminated
			}
			else
				ret = (uint)readed; //puts size into ret
		}

		if(PHYSFS_close(fs_file) == 0) //Close file, then checks for error
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());
	//

	return ret;
}

// Read a whole file and put it in a new buffer
SDL_RWops* j1FileSystem::Load(const char* file) const
{
	//H1 TODO 4: Using our previous Load method
	// create a sdl rwops using SDL_RWf_From_ConstMem()
	// and return it if everything goes well

	char* buffer; //placeholder buffer, not initiated because it will kill itself
	int size = Load(file, &buffer); //preloads file to get a size

	if(size > 0) //checks that it exists through size
	{
		SDL_RWops* r = SDL_RWFromConstMem(buffer, size); //creates the SDLRwops
		if(r != NULL)
			r->close = close_sdl_rwops;

		return r;
	}
	else
		return NULL;
}

//H1 TODO 4: you will need to use this small helper function
// to make sure all data is freed automatically
int close_sdl_rwops(SDL_RWops *rw)
{
	RELEASE(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}
//This function closes Rwops files, when closing or in case of error

// Save a whole buffer to disk
unsigned int j1FileSystem::Save(const char* file, const char* buffer, unsigned int size) const
{
	unsigned int ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openWrite(file);

	if(fs_file != NULL)
	{
		PHYSFS_sint64 written = PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if(written != size)
			LOG("File System error while writing to file %s: %s\n", file, PHYSFS_getLastError());
		else
			ret = (uint) written;

		if(PHYSFS_close(fs_file) == 0)
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}