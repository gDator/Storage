#include "FileDialog.hpp"
#include "program.hpp"

#include <windows.h>
#include <commdlg.h>
#include "program.hpp"

std::string FileDialog::OpenFile(const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = {0};
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = app.getRenderWindow().getSystemHandle(); //sfml
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if(GetOpenFileNameA(&ofn) == TRUE)
		return ofn.lpstrFile;
	return std::string();
}

std::string FileDialog::OpenFolder()
{
	// OFNOTIFYEXA of;
	// CHAR szFile[260] = {0};
	// ZeroMemory(&ofn, sizeof(OPENFILENAME));
	// of.lpOFN.lStructSize = sizeof(OPENFILENAME);
	// of.lpOFN.hwndOwner = app.getRenderWindow().getSystemHandle(); //sfml
	// of.lpOFN.lpstrFile = szFile;
	// of.lpOFN.nMaxFile = sizeof(szFile);
	// of.lpOFN.lpstrFilter = filter;
	// of.lpOFN.nFilterIndex = 1;
	// of.lpOFN.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	// if(GetOpenFileNameA(&ofn) == TRUE)
	// 	return ofn.lpstrFile;
	// return std::string();
}

std::string FileDialog::SaveFile(const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = {0};
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = app.getRenderWindow().getSystemHandle(); //sfml
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if(GetSaveFileNameA(&ofn) == TRUE)
		return ofn.lpstrFile;
	return std::string();
}