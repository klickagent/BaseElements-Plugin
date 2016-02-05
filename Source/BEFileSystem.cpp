/*
 BEFileSystem.cpp
 BaseElements Plug-In
 
 Copyright 2011-2016 Goya. All rights reserved.
 For conditions of distribution and use please see the copyright notice in BEPlugin.cpp
 
 http://www.goya.com.au/baseelements/plugin
 
 */


#include "BEFileSystem.h"
#include "BEPluginException.h"


using namespace std;
using namespace boost::filesystem;


bool recursive_directory_copy ( const path & from, const path & to  ) {
	
	path base = from;

	if ( is_directory ( from ) ) {

		copy_directory ( from, to );

		directory_iterator end_it; // default construction yields past-the-end
		
		for ( directory_iterator it ( from ); it != end_it; ++it ) {
			
			path relative = from;
			
			path target = to;
			target /= it->path().filename();
			
			bool failed = recursive_directory_copy ( it->path(), target );
			if ( !failed ) {
				return false;
			}

		}
		
	} else {
		
		// note: aliases & symlinks are not handled correctly
		
		copy ( from, to );
	}
	
	return true;
	
} // recursive_directory_copy


BEValueListWideStringAutoPtr list_files_in_directory ( const boost::filesystem::path & directory, const long file_type_wanted = kBE_FileType_ALL, const bool recurse = false )
{
	
	BEValueListWideStringAutoPtr list_of_files ( new BEValueList<wstring> );
	
	try {
		
		path directory_path = directory;
		directory_path.make_preferred(); // force the correct path separator for the platform
		
		recursive_directory_iterator end_it; // default construction yields past-the-end
		recursive_directory_iterator it ( directory_path, symlink_option::recurse );
			
		while ( it != end_it ) {
				
			if ( !((recurse == false) && (it.level() > 0)) ) {

				bool is_folder = is_directory ( it->status() );
					
				if (
						(!is_folder && (file_type_wanted == kBE_FileType_File)) ||
						(is_folder && (file_type_wanted == kBE_FileType_Folder)) ||
						(file_type_wanted == kBE_FileType_ALL)
					) {
					list_of_files->append ( it->path().wstring() );
				}
				
			}
				
			++it;

		}
		
	} catch ( filesystem_error& e ) {
		throw BEPlugin_Exception ( e.code().value() );
	}
	
	return list_of_files;
	
} // list_files_in_directory

