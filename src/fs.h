
#ifndef _FS_H_INC_
#define _FS_H_INC_

namespace fs
{
	struct IDirectoryEnum;

	struct IFile
	{
		virtual ~IFile() {}

		virtual bool get_name( char* buf ) = 0;
		virtual IDirectoryEnum* children() = 0;
	};

	struct IDirectoryEnum
	{
		virtual ~IDirectoryEnum() {}

		virtual void reset() = 0;
		virtual bool next( IFile** file ) = 0;
	};

	struct IFileSystem
	{
		virtual ~IFileSystem() {}

		virtual IDirectoryEnum* root_directory() = 0;
	};
}

#endif // _FS_H_INC_
