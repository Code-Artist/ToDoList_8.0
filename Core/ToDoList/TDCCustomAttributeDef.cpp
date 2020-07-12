// TDCCustomAttributeDef.cpp: implementation of the TDCCUSTOMATTRIBUTEDEFINITION class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdccustomattributeDef.h"
#include "tdccustomattribdata.h"

#include "..\Shared\Misc.h"
#include "..\Shared\GraphicsMisc.h"

/////////////////////////////////////////////////////////////////////////////////////////////


TDCCUSTOMATTRIBUTEDEFINITION::TDCCUSTOMATTRIBUTEDEFINITION(LPCTSTR szLabel)
	:
	sLabel(szLabel),
	dwAttribType(TDCCA_STRING),
	nTextAlignment(DT_LEFT),
	dwFeatures(TDCCAF_SORT),
	nColID(TDCC_NONE),
	nAttribID(TDCA_NONE),
	bEnabled(TRUE)
{

}

TDCCUSTOMATTRIBUTEDEFINITION::TDCCUSTOMATTRIBUTEDEFINITION(const TDCCUSTOMATTRIBUTEDEFINITION& otherDef)
{
	*this = otherDef;
}

TDCCUSTOMATTRIBUTEDEFINITION& TDCCUSTOMATTRIBUTEDEFINITION::operator=(const TDCCUSTOMATTRIBUTEDEFINITION& attribDef)
{
	dwAttribType = attribDef.dwAttribType;
	sUniqueID = attribDef.sUniqueID;
	sColumnTitle = attribDef.sColumnTitle;
	sLabel = attribDef.sLabel;
	nTextAlignment = attribDef.nTextAlignment;
	dwFeatures = attribDef.dwFeatures;
	nColID = attribDef.nColID;
	nAttribID = attribDef.nAttribID;
	bEnabled = attribDef.bEnabled;

	if (attribDef.IsList())
	{
		aDefaultListData.Copy(attribDef.aDefaultListData);

		if (attribDef.IsAutoList())
			aAutoListData.Copy(attribDef.aAutoListData);
	}

	return *this;
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::operator==(const TDCCUSTOMATTRIBUTEDEFINITION& attribDef) const
{
	// NOTE: we ignore auto data which is temporary
	return ((dwAttribType == attribDef.dwAttribType) &&
			(sUniqueID.CompareNoCase(attribDef.sUniqueID) == 0) &&
			(sColumnTitle == attribDef.sColumnTitle) &&
			(sLabel == attribDef.sLabel) &&
			(nTextAlignment == attribDef.nTextAlignment) &&
			(dwFeatures == attribDef.dwFeatures) &&
			(nColID == attribDef.nColID) &&
			(nAttribID == attribDef.nAttribID) &&
			(bEnabled == attribDef.bEnabled) &&
			Misc::MatchAll(aDefaultListData, attribDef.aDefaultListData, TRUE, TRUE));
}

CString TDCCUSTOMATTRIBUTEDEFINITION::GetColumnTitle() const
{
	return (sColumnTitle.IsEmpty() ? sLabel : sColumnTitle);
}

CString TDCCUSTOMATTRIBUTEDEFINITION::GetToolTip() const
{
	return (sColumnTitle.GetLength() > sLabel.GetLength() ? sColumnTitle : sLabel);
}

UINT TDCCUSTOMATTRIBUTEDEFINITION::GetColumnHeaderAlignment() const
{
	switch (nTextAlignment)
	{
	case DT_CENTER: return HDF_CENTER;
	case DT_RIGHT:	return HDF_RIGHT;
	case DT_LEFT:	return HDF_LEFT;
	}

	// all else
	ASSERT(0);
	return HDF_LEFT;
}

UINT TDCCUSTOMATTRIBUTEDEFINITION::GetDefaultTextAlignment(DWORD dwAttribType)
{
	switch (dwAttribType & TDCCA_DATAMASK)
	{
	case TDCCA_DATE:
	case TDCCA_INTEGER:
	case TDCCA_DOUBLE:
	case TDCCA_TIMEPERIOD:
	case TDCCA_FRACTION:
		return TA_RIGHT;
	}

	// All else
	return TA_LEFT;
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::HasDefaultTextAlignment() const
{
	return (nTextAlignment == GetDefaultTextAlignment(dwAttribType));
}

void TDCCUSTOMATTRIBUTEDEFINITION::SetAttributeType(DWORD dwType)
{
	SetTypes((dwType & TDCCA_DATAMASK), (dwType & TDCCA_LISTMASK));
}

void TDCCUSTOMATTRIBUTEDEFINITION::SetDataType(DWORD dwDataType, BOOL bUpdateDefaultAlignment)
{
	DWORD dwPrevType = GetDataType();

	SetTypes((dwDataType & TDCCA_DATAMASK), GetListType());

	// Set default alignment if the previous also had the default
	if (bUpdateDefaultAlignment)
	{
		if (nTextAlignment == GetDefaultTextAlignment(dwPrevType))
			nTextAlignment = GetDefaultTextAlignment(dwAttribType);
	}
}

void TDCCUSTOMATTRIBUTEDEFINITION::SetListType(DWORD dwListType)
{
	SetTypes(GetDataType(), (dwListType & TDCCA_LISTMASK));
}

int TDCCUSTOMATTRIBUTEDEFINITION::GetUniqueListData(CStringArray& aData) const
{
	aData.Copy(aAutoListData);
	Misc::AddUniqueItems(aDefaultListData, aData);

	return aData.GetSize();
}

CString TDCCUSTOMATTRIBUTEDEFINITION::EncodeListData() const
{
	if (!IsList())
		return _T("");

	CString sListData = Misc::FormatArray(aDefaultListData, '\n');

	if (IsAutoList())
	{
		sListData += '\t';
		sListData += Misc::FormatArray(aAutoListData, '\n');
	}

	return sListData;
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::DecodeListData(const CString& sListData)
{
	if (IsList() && !sListData.IsEmpty())
	{
		CString sDefData(sListData), sAutoData;

		Misc::Split(sDefData, sAutoData, '\t');
		Misc::Split(sDefData, aDefaultListData, '\n');

		if (IsAutoList())
			Misc::Split(sAutoData, aAutoListData, '\n');

		return TRUE;
	}

	// else
	return FALSE;
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::SupportsFeature(DWORD dwFeature) const
{
	switch (dwFeature)
	{
		// sorting and inheritance works on all data types
	case TDCCAF_SORT:
	case TDCCAF_INHERITPARENTCHANGES:
		return TRUE;

	case TDCCAF_FILTER:
		return IsList();

	case TDCCAF_EXCLUDEBLANKITEM:
		return (IsList() && !IsMultiList());

	default:
		// calculations not supported on multi-list types
		if (IsMultiList())
			return FALSE;
		break;
	}

	DWORD dwDataType = GetDataType();

	switch (dwDataType)
	{
	case TDCCA_DOUBLE:
	case TDCCA_INTEGER:
	case TDCCA_TIMEPERIOD:
		return ((dwFeature == TDCCAF_ACCUMULATE) ||
				(dwFeature == TDCCAF_MAXIMIZE) ||
				(dwFeature == TDCCAF_MINIMIZE) ||
				(dwFeature == TDCCAF_HIDEZERO));

	case TDCCA_DATE:
		return ((dwFeature == TDCCAF_MAXIMIZE) ||
				(dwFeature == TDCCAF_MINIMIZE) ||
				(dwFeature == TDCCAF_SHOWTIME));

	case TDCCA_STRING:
	case TDCCA_FILELINK:
	case TDCCA_BOOL:
	case TDCCA_ICON:
		break;

	case TDCCA_FRACTION:
		return ((dwFeature == TDCCAF_DISPLAYASPERCENT) ||
				(dwFeature == TDCCAF_MAXIMIZE) ||
				(dwFeature == TDCCAF_MINIMIZE) ||
				(dwFeature == TDCCAF_HIDEZERO));

	default:
		ASSERT(0);
		break;
	}

	return FALSE;
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::SupportsCalculation() const
{
	return SupportsFeature(TDCCAF_ACCUMULATE) ||
			SupportsFeature(TDCCAF_MAXIMIZE) ||
			SupportsFeature(TDCCAF_MINIMIZE);
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::IsCalculated() const
{
	return HasFeature(TDCCAF_ACCUMULATE) ||
			HasFeature(TDCCAF_MAXIMIZE) ||
			HasFeature(TDCCAF_MINIMIZE);
}

CString TDCCUSTOMATTRIBUTEDEFINITION::GetNextListItem(const CString& sItem, BOOL bNext) const
{
	DWORD dwListType = GetListType();

	ASSERT(dwListType != TDCCA_NOTALIST);

	switch (aDefaultListData.GetSize())
	{
	case 0:
		return sItem;

	default:
		{
			int nFind = Misc::Find(sItem, aDefaultListData, FALSE, FALSE);
			ASSERT((nFind != -1) || sItem.IsEmpty());

			if (bNext)
			{
				nFind++;

				if (nFind < aDefaultListData.GetSize())
					return aDefaultListData[nFind];
			}
			else // prev
			{
				if (nFind == -1)
					nFind = aDefaultListData.GetSize();

				nFind--;

				if (nFind >= 0)
					return aDefaultListData[nFind];
			}

			// all else
			return _T("");
		}
		break;
	}
}

CString TDCCUSTOMATTRIBUTEDEFINITION::GetImageName(const CString& sImage) const
{
	CString sName;
	int nTag = Misc::Find(sImage, aDefaultListData);

	if (nTag != -1)
	{
		CString sTag = aDefaultListData[nTag], sDummy;
		VERIFY(DecodeImageTag(sTag, sDummy, sName));
	}

	return sName;
}

int TDCCUSTOMATTRIBUTEDEFINITION::CalcLongestListItem(CDC* pDC) const
{
	if (!IsList())
	{
		ASSERT(0);
		return 0;
	}

	int nItem = aDefaultListData.GetSize(), nLongest = 0;

	while (nItem--)
	{
		const CString& sItem = Misc::GetItem(aDefaultListData, nItem);
		int nItemLen = 0;

		switch (GetDataType())
		{
		case TDCCA_STRING:
		case TDCCA_INTEGER:
		case TDCCA_DOUBLE:
		case TDCCA_FRACTION:
		case TDCCA_DATE:
		case TDCCA_BOOL:
		case TDCCA_TIMEPERIOD:
			nItemLen = pDC->GetTextExtent(sItem).cx;
			break;

		case TDCCA_ICON:
			{
				nItemLen = GraphicsMisc::ScaleByDPIFactor(16); // for the icon

				// check for trailing text
				CString sDummy, sName;

				if (DecodeImageTag(sItem, sDummy, sName) && !sName.IsEmpty())
				{
					nItemLen += GraphicsMisc::ScaleByDPIFactor(2); // offset
					nItemLen += pDC->GetTextExtent(sName).cx;
				}
			}
			break;

		default:
			ASSERT(0);
			break;
		}

		nLongest = max(nLongest, nItemLen);
	}

	return nLongest;
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::IsEncodedImageTag(const CString& sImage)
{
	return (sImage.Find(':') != -1);
}

CString TDCCUSTOMATTRIBUTEDEFINITION::EncodeImageTag(const CString& sImage, const CString& sName)
{
	if (IsEncodedImageTag(sImage))
		return sImage;

	return (sImage + ':' + sName);
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::DecodeImageTag(const CString& sTag, CString& sImage, CString& sName)
{
	sImage.Empty();
	sName.Empty();

	CStringArray aParts;
	int nNumParts = Misc::Split(sTag, aParts, ':');

	switch (nNumParts)
	{
	case 2:
		sName = aParts[1];
		// fall thru
	case 1:
		sImage = aParts[0];
		break;

	case 0:
		break;
	}

	return !sImage.IsEmpty();
}

void TDCCUSTOMATTRIBUTEDEFINITION::SetTypes(DWORD dwDataType, DWORD dwListType)
{
	dwAttribType = (dwDataType | ValidateListType(dwDataType, dwListType));
}

DWORD TDCCUSTOMATTRIBUTEDEFINITION::ValidateListType(DWORD dwDataType, DWORD dwListType)
{
	// ensure list-type cannot be applied to dates or times,
	// or only partially to icons, flags and file links
	switch (dwDataType)
	{
	case TDCCA_STRING:
	case TDCCA_INTEGER:
	case TDCCA_DOUBLE:
		// all types supported
		break;

	case TDCCA_DATE:
	case TDCCA_TIMEPERIOD:
		dwListType = TDCCA_NOTALIST;
		break;

	case TDCCA_FRACTION:
		if (dwListType)
		{
			if ((dwListType != TDCCA_FIXEDLIST) && (dwListType != TDCCA_AUTOLIST))
				dwListType = TDCCA_FIXEDLIST;
		}
		break;

	case TDCCA_BOOL:
		if (dwListType)
		{
			dwListType = TDCCA_FIXEDLIST;
		}
		break;

	case TDCCA_ICON:
		if (dwListType)
		{
			if ((dwListType != TDCCA_FIXEDLIST) && (dwListType != TDCCA_FIXEDMULTILIST))
				dwListType = TDCCA_FIXEDLIST;
		}
		break;

	case TDCCA_FILELINK:
		if (dwListType)
		{
			dwListType = TDCCA_AUTOLIST;
		}
		break;

	default:
		ASSERT(0);
		break;
	}

	return dwListType;
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::IsCustomAttribute(TDC_ATTRIBUTE nAttribID)
{
	if ((nAttribID >= TDCA_CUSTOMATTRIB_FIRST) && (nAttribID <= TDCA_CUSTOMATTRIB_LAST))
		return TRUE;

	return (nAttribID == TDCA_CUSTOMATTRIB_ALL);
}

BOOL TDCCUSTOMATTRIBUTEDEFINITION::IsCustomColumn(TDC_COLUMN nColID)
{
	return (nColID >= TDCC_CUSTOMCOLUMN_FIRST && nColID <= TDCC_CUSTOMCOLUMN_LAST);
}

CString TDCCUSTOMATTRIBUTEDEFINITION::FormatData(const TDCCADATA& data, BOOL bISODates, TCHAR cListSep) const
{
	if (IsList())
		return data.FormatAsArray(cListSep);

	// else
	switch (GetDataType())
	{
	case TDCCA_DATE:
		return data.FormatAsDate(bISODates, HasFeature(TDCCAF_SHOWTIME));

	case TDCCA_DOUBLE:
	case TDCCA_INTEGER:
		return FormatNumber(data.AsDouble());

	case TDCCA_FRACTION:
		return FormatNumber(data.AsFraction());

	case TDCCA_TIMEPERIOD:
		if (data.IsTimePeriod())
			return data.FormatAsTimePeriod();
		break;
	}

	// all else
	return data.AsString();
}

CString TDCCUSTOMATTRIBUTEDEFINITION::FormatNumber(double dValue) const
{
	switch (GetDataType())
	{
	case TDCCA_FRACTION:
		if (HasFeature(TDCCAF_DISPLAYASPERCENT))
			return Misc::Format((dValue * 100), 1, _T("%"));
		// else fall thru

	case TDCCA_DOUBLE:
		return Misc::Format(dValue, 2);

	case TDCCA_INTEGER:
		return Misc::Format(dValue, 0);
	}

	// All else
	ASSERT(0);
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////////////

CTDCCustomAttribDefinitionArray::CTDCCustomAttribDefinitionArray()
{

}

CTDCCustomAttribDefinitionArray::CTDCCustomAttribDefinitionArray(const CTDCCustomAttribDefinitionArray& other)
{
	Copy(other);
}

int CTDCCustomAttribDefinitionArray::Add(TDCCUSTOMATTRIBUTEDEFINITION& newElement)
{
	int nIndex = CArray<TDCCUSTOMATTRIBUTEDEFINITION, TDCCUSTOMATTRIBUTEDEFINITION&>::Add(newElement);
	RebuildIDs();

	return nIndex;
}

void CTDCCustomAttribDefinitionArray::InsertAt(int nIndex, TDCCUSTOMATTRIBUTEDEFINITION& newElement, int nCount)
{
	CArray<TDCCUSTOMATTRIBUTEDEFINITION, TDCCUSTOMATTRIBUTEDEFINITION&>::InsertAt(nIndex, newElement, nCount);
	RebuildIDs();
}

void CTDCCustomAttribDefinitionArray::RemoveAt(int nIndex, int nCount)
{
	CArray<TDCCUSTOMATTRIBUTEDEFINITION, TDCCUSTOMATTRIBUTEDEFINITION&>::RemoveAt(nIndex, nCount);
	RebuildIDs();
}

BOOL CTDCCustomAttribDefinitionArray::MatchAny(const CTDCCustomAttribDefinitionArray& aAttribDefs) const
{
	for (int nAttrib = 0; nAttrib < aAttribDefs.GetSize(); nAttrib++)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& attribDef = aAttribDefs.GetData()[nAttrib];

		if (Find(attribDef.sUniqueID) != -1)
			return TRUE;
	}

	return FALSE;
}

int CTDCCustomAttribDefinitionArray::Find(const CString& sAttribID, int nIgnore) const
{
	ASSERT(!sAttribID.IsEmpty());

	if (!sAttribID.IsEmpty())
	{
		int nAttrib = GetSize();

		while (nAttrib--)
		{
			const TDCCUSTOMATTRIBUTEDEFINITION& attribDef = GetData()[nAttrib];

			if ((nAttrib != nIgnore) && (attribDef.sUniqueID.CompareNoCase(sAttribID) == 0))
				return nAttrib;
		}
	}

	return -1;
}

int CTDCCustomAttribDefinitionArray::Find(TDC_ATTRIBUTE nAttribID, int nIgnore) const
{
	ASSERT(nAttribID != TDCA_NONE);

	int nAttrib = GetSize();

	while (nAttrib--)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& attribDef = GetData()[nAttrib];

		if ((nAttrib != nIgnore) && (attribDef.GetAttributeID() == nAttribID))
			return nAttrib;
	}

	return -1;
}

int CTDCCustomAttribDefinitionArray::Append(const CTDCCustomAttribDefinitionArray& aSrc)
{
	int nOrgSize = GetSize();

	for (int nAttrib = 0; nAttrib < aSrc.GetSize(); nAttrib++)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& attribDef = aSrc.GetData()[nAttrib];

		// Append unique items only
		if (Find(attribDef.sUniqueID) == -1)
		{
			TDCCUSTOMATTRIBUTEDEFINITION def = aSrc[nAttrib];
			Add(def);
		}
		// else skip
	}

	return (GetSize() - nOrgSize);
}

BOOL CTDCCustomAttribDefinitionArray::AnyHasFeature(DWORD dwFeature) const
{
	int nDef = GetSize();

	while (nDef--)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& def = GetData()[nDef];

		if (def.HasFeature(dwFeature))
			return TRUE;
	}

	return FALSE;
}


CString CTDCCustomAttribDefinitionArray::GetAttributeTypeID(TDC_COLUMN nColID) const
{
	TDCCUSTOMATTRIBUTEDEFINITION attribDef;

	if (GetAttributeDef(nColID, attribDef))
		return attribDef.sUniqueID;

	// all else
	return _T("");
}

TDC_ATTRIBUTE CTDCCustomAttribDefinitionArray::GetAttributeID(TDC_COLUMN nColID) const
{
	if (nColID < TDCC_CUSTOMCOLUMN_FIRST || nColID > TDCC_CUSTOMCOLUMN_LAST)
		return TDCA_NONE;

	int nDef = GetSize();

	while (nDef--)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& def = GetData()[nDef];

		if (def.GetColumnID() == nColID)
			return def.GetAttributeID();
	}

	// all else
	return TDCA_NONE;
}

TDC_ATTRIBUTE CTDCCustomAttribDefinitionArray::GetAttributeID(const CString& sUniqueID) const
{
	int nDef = GetSize();

	while (nDef--)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& def = GetData()[nDef];

		if (sUniqueID.CompareNoCase(def.sUniqueID) == 0)
			return def.GetAttributeID();
	}

	// all else
	return TDCA_NONE;
}

CString CTDCCustomAttribDefinitionArray::GetAttributeTypeID(TDC_ATTRIBUTE nAttribID) const
{
	TDCCUSTOMATTRIBUTEDEFINITION attribDef;

	if (GetAttributeDef(nAttribID, attribDef))
		return attribDef.sUniqueID;

	// all else
	return _T("");
}

int CTDCCustomAttribDefinitionArray::GetVisibleColumnIDs(CTDCColumnIDMap& mapCols, BOOL bAppend) const
{
	if (!bAppend)
		mapCols.RemoveAll();

	int nColsSize = mapCols.GetCount();
	int nDef = GetSize();

	while (nDef--)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& def = GetData()[nDef];

		if (def.bEnabled)
			mapCols.Add(def.GetColumnID());
	}

	return (mapCols.GetCount() - nColsSize);
}

BOOL CTDCCustomAttribDefinitionArray::GetAttributeDef(TDC_ATTRIBUTE nAttribID, TDCCUSTOMATTRIBUTEDEFINITION& attribDef) const
{
	if (!TDCCUSTOMATTRIBUTEDEFINITION::IsCustomAttribute(nAttribID))
		return FALSE;

	int nDef = GetSize();

	while (nDef--)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& def = GetData()[nDef];

		if (def.GetAttributeID() == nAttribID)
		{
			attribDef = def;
			return TRUE;
		}
	}

	// all else
	return FALSE;
}

BOOL CTDCCustomAttribDefinitionArray::GetAttributeDef(const CString& sUniqueID, TDCCUSTOMATTRIBUTEDEFINITION& attribDef) const
{
	int nAttrib = Find(sUniqueID);

	if (nAttrib != -1)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& def = GetData()[nAttrib];
		attribDef = def;
		return TRUE;
	}

	// all else
	return FALSE;
}

BOOL CTDCCustomAttribDefinitionArray::GetAttributeDef(TDC_COLUMN nColID, TDCCUSTOMATTRIBUTEDEFINITION& attribDef) const
{
	if (!TDCCUSTOMATTRIBUTEDEFINITION::IsCustomColumn(nColID))
		return FALSE;

	int nDef = GetSize();

	while (nDef--)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& def = GetData()[nDef];

		if (def.GetColumnID() == nColID)
		{
			attribDef = def;
			return TRUE;
		}
	}

	// all else
	return FALSE;
}

DWORD CTDCCustomAttribDefinitionArray::GetAttributeDataType(TDC_ATTRIBUTE nAttribID) const
{
	int nAttrib = Find(nAttribID);

	if (nAttrib != -1)
		return GetData()[nAttrib].GetDataType();

	// all else
	return TDCCA_STRING;
}

DWORD CTDCCustomAttribDefinitionArray::GetAttributeDataType(const CString& sUniqueID) const
{
	int nAttrib = Find(sUniqueID);

	if (nAttrib != -1)
	{
		const TDCCUSTOMATTRIBUTEDEFINITION& def = GetData()[nAttrib];
		return def.GetDataType();
	}

	// all else
	return TDCCA_STRING;
}

BOOL CTDCCustomAttribDefinitionArray::IsColumnSortable(TDC_COLUMN nColID) const
{
	ASSERT(TDCCUSTOMATTRIBUTEDEFINITION::IsCustomColumn(nColID));

	TDCCUSTOMATTRIBUTEDEFINITION attribDef;

	if (GetAttributeDef(nColID, attribDef))
		return attribDef.HasFeature(TDCCAF_SORT);

	// else
	return FALSE;
}

BOOL CTDCCustomAttribDefinitionArray::IsColumnEnabled(TDC_COLUMN nColID) const
{
	TDCCUSTOMATTRIBUTEDEFINITION attribDef;

	return (GetAttributeDef(nColID, attribDef) && attribDef.bEnabled);
}

BOOL CTDCCustomAttribDefinitionArray::IsCustomAttributeEnabled(TDC_ATTRIBUTE nAttribID) const
{
	TDCCUSTOMATTRIBUTEDEFINITION attribDef;

	return (GetAttributeDef(nAttribID, attribDef) && attribDef.bEnabled);
}

void CTDCCustomAttribDefinitionArray::RebuildIDs()
{
	int nColID = TDCC_CUSTOMCOLUMN_FIRST;
	int nAttribID = TDCA_CUSTOMATTRIB_FIRST;

	for (int nAttrib = 0; nAttrib < GetSize(); nAttrib++)
	{
		TDCCUSTOMATTRIBUTEDEFINITION& attribDef = ElementAt(nAttrib);

		attribDef.nColID = (TDC_COLUMN)nColID++;
		attribDef.nAttribID = (TDC_ATTRIBUTE)nAttribID++;
	}
}

/////////////////////////////////////////////////////////////////////////////

