#include "StdAfx.h"
#include "FileOperations.h"
#ifdef USE_NET_LIBS
#include "globalmembers.h"
#endif

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

bool CreateFolderIfNotExists(const string& folderName)
{
	path p(folderName);

	path currDir;
	for(path::iterator iter = p.begin(); iter != p.end(); ++iter)
	{
		currDir /= (*iter);
		if(!exists(currDir))
		{
			try
			{
				create_directory(currDir);
			}
			catch (const filesystem_error& ex)
			{
#ifdef USE_NET_LIBS
				logger.Error(ex.what());
#endif
				return false;
			}
		}
	}
	
	return true;
}
