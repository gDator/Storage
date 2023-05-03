#ifndef FILE_DIALOG_HPP
#define FILE_DIALOG_HPP

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <string>


class FileDialog
{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
};


#endif /*FILE_DIALOG_HPP*/