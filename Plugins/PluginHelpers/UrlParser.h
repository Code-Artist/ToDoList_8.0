#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////

using namespace System;
using namespace System::Collections::Generic;

////////////////////////////////////////////////////////////////////////////////////////////////

class CUrlParser;

////////////////////////////////////////////////////////////////////////////////////////////////

namespace Abstractspoon
{
	namespace Tdl
	{
		namespace PluginHelpers
		{
			public ref class UrlItem
			{
			public:
				UrlItem(int nStart, int nEnd, String^ sUrl);

				int StartPos, EndPos;
				String^ Url;
			};

			public ref class UrlParser
			{
			public:
				UrlParser();
				virtual ~UrlParser();

				List<UrlItem^>^ ParseText(String^ sText);
				bool IsFileProtocol(String^ sUrl);
				int GetUrlCount(String^ sText);

			protected:
				CUrlParser* m_parser;
			};

		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////

