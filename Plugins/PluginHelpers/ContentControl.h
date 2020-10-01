#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////

#include "PluginHelpers.h"
#include "Preferences.h"
#include "UITheme.h"

////////////////////////////////////////////////////////////////////////////////////////////////

namespace Abstractspoon
{
	namespace Tdl
	{
		namespace PluginHelpers
		{
			public ref class ContentControlWnd
			{
			public:
				ref class ParentNotify
				{
				public:
					ParentNotify(IntPtr hwndParent);
					ParentNotify(IntPtr hwndParent, IntPtr hwndFrom);

					bool NotifyChange();
					bool NotifyKillFocus();
					bool NotifyFailedLink(String^ sFailedLink);
					bool NotifyTaskLink(String^ sTaskLink);
					bool NotifyWantSpellcheck();
					String^ NotifyWantLinkTooltip(String^ sLink);

					//bool GetClipboard();
					bool HasClipboard();
					bool DoHelp(String^ sHelpKey);

				private:
					HWND m_hwndParent;
					HWND m_hwndFrom;

				private:
					HWND GetFrom();
					String^ ValidateLink(String^ sLink);
				};

				static bool GoToLink(String^ sLink, IntPtr hwndParent, IntPtr hwndFrom);
			};

			public interface class IContentControlWnd
			{
				// custom/binary data format
				cli::array<Byte>^ GetContent();
				bool SetContent(cli::array<Byte>^ content, bool bResetSelection);

				// text content if supported. return false if not supported
				String^ GetTextContent();
				bool SetTextContent(String^ content, bool bResetSelection);

				bool ProcessMessage(IntPtr hwnd, UInt32 message, UInt32 wParam, UInt32 lParam, UInt32 time, Int32 xPos, Int32 yPos);

				bool Undo();
				bool Redo();

				void SetUITheme(UITheme^ theme);
				void SetReadOnly(bool bReadOnly);

				void SavePreferences(Preferences^ prefs, String^ key);
				void LoadPreferences(Preferences^ prefs, String^ key, bool appOnly);

				//virtual ISpellCheck* GetSpellCheckInterface() = 0;
			};
		}
	}
}

