#include "StdAfx.h"
#include "FileOperations.h"
#include "globalmembers.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem3;

bool CreateFolderIfNotExists(const string& folderName)
{
	path p(folderName);

	for(path::iterator iter = p.begin(); iter != p.end(); ++iter)
	{
		const path& curr = *iter;
		if(!exists(curr))
		{
			try
			{
				create_directory(curr);
			}
			catch (const filesystem_error& ex)
			{
				logger.Error(ex.what());
				return false;
			}
		}
	}
	
	return true;
}
