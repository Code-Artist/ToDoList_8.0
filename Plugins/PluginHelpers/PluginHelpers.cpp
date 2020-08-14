// PluginHelpers.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "pluginhelpers.h"
#include "win32.h"

#include <Interfaces\IEnums.h>

#include <Shared\Misc.h>

////////////////////////////////////////////////////////////////////////////////////////////////

using namespace System::IO;
using namespace System::Reflection;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;

using namespace Abstractspoon::Tdl::PluginHelpers;

////////////////////////////////////////////////////////////////////////////////////////////////

class ModuleInitialiser
{
public:
	ModuleInitialiser()
	{
		try
		{
			Application::SetCompatibleTextRenderingDefault(false);
		}
		catch (...)
		{

		}
	}
};

static ModuleInitialiser* _init = new ModuleInitialiser();

////////////////////////////////////////////////////////////////////////////////////////////////

MarshalledString::MarshalledString(String^ str) : m_wszGlobal(NULL)
{
	m_wszGlobal = (LPCWSTR)Marshal::StringToHGlobalUni(str).ToPointer();
}

MarshalledString::~MarshalledString()
{
	Marshal::FreeHGlobal((IntPtr)(void*)m_wszGlobal);
}

MarshalledString::operator LPCWSTR() 
{ 
	return m_wszGlobal; 
}

////////////////////////////////////////////////////////////////////////////////////////////////

bool StringUtil::Find(String^ source, String^ findText, bool matchWhole, bool matchCase)
{
	MarshalledString msSearchIn(source), msSearchFor(findText);
	CString sSearchIn(msSearchIn), sSearchFor(msSearchFor);

	return (-1 != Misc::Find(sSearchFor, sSearchIn, (BOOL)matchCase, (BOOL)matchWhole));
}

////////////////////////////////////////////////////////////////////////////////////////////////

void Log::LogText(String^ text)
{
	if (s_sLogPath == nullptr)
	{
		String^ appFolder = Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location);
		s_sLogPath = Path::Combine(appFolder, "ToDoList.log");

		if (!File::Exists(s_sLogPath))
			s_sLogPath = String::Empty;
	}

	if (!String::IsNullOrEmpty(s_sLogPath))
		System::IO::File::AppendAllText(s_sLogPath, text + System::Environment::NewLine);
}


////////////////////////////////////////////////////////////////////////////////////////////////
