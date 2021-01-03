// infocut.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "shlobj.h"
#include "resource.h"


typedef struct _NAME
{
	_TCHAR *szShort; // 8.3 story file name
	_TCHAR *szName;  // User friendly name
	int idi;         // resource ID for an Icon
} NAME;

NAME rgname[] =
{
	{ L"trinity.z4", L"Trinity", IDI_TRINITY },
	{ L"arthur.z6", L"Arthur", IDI_ARTHUR },
	{ L"ballyhoo.z3", L"Ballyhoo", IDI_BALLYHOO },
	{ L"ADVENT.Z5", L"Colossal Cave Adventure", IDI_ADVENT },
	{ L"AMFV.z4", L"A Mind Forever Voyaging", IDI_AMFV },
	{ L"arthur.z6", L"Arthur", IDI_ARTHUR },
	{ L"BALLYHOO.z3", L"Ballyhoo", IDI_BALLYHOO },
	{ L"BEYONDZO.z5", L"Beyond Zork", IDI_BEYONDZORK },
	{ L"BORDERZO.z5", L"Border Zone", IDI_BORDERZONE },
	{ L"BUREAUCR.z4", L"Bureaucracy", IDI_BUREAUCRACY },
	{ L"CUTTHROA.z3", L"Cutthroats", IDI_CUTTHROATS },
	{ L"DEADLINE.z3", L"Deadline", IDI_DEADLINE },
	{ L"ENCHANTE.z3", L"Enchanter", IDI_ENCHANTER },
	{ L"hitchhik.z5", L"Hitchhiker's Guide to the Galaxy", IDI_HGTTG },
	{ L"HOLLYWOO.z3", L"Hollywood Hijinks", IDI_HOLLYWOOD },
	{ L"INFIDEL.z3", L"Infidel", IDI_INFIDEL },
	{ L"JOURNEY.z6", L"Journey", IDI_JOURNEY },
	{ L"LEATHER.z3", L"Leather Goddesses of Phobos", IDI_LEATHER },
	{ L"leather.z5", L"Leather Goddesses of Phobos z5", IDI_LEATHER2 },
	{ L"LURKING.z3", L"Lurking Horror", IDI_LURKING },
	{ L"MOONMIST.z3", L"Moonmist", IDI_MOONMIST },
	{ L"NORDANDB.z4", L"Nord and Bert", IDI_NORDANDBERT },
	{ L"PLANETFA.z5", L"Planetfall", IDI_PLANETFALL },
	{ L"PLUNDERE.z3", L"Plundered Hearts", IDI_PLUNDERED },
	{ L"SEASTALK.z3", L"Seastalker", IDI_SEASTALKER },
	{ L"SHERLOCK.z5", L"Sherlock", IDI_SHERLOCK },
	{ L"SHOGUN.Z6", L"Shogun", IDI_SHOGUN },
	{ L"SORCERER.z3", L"Sorcerer", IDI_SORCERER },
	{ L"SPELLBRE.z3", L"Spellbreaker", IDI_SPELLBREAKER },
	{ L"STARCROS.z3", L"Starcross", IDI_STARCROSS },
	{ L"STATIONF.z3", L"Stationfall", IDI_STATIONFALL },
	{ L"SUSPECT.z3", L"Suspect", IDI_SUSPECT },
	{ L"SUSPEND.z3", L"Suspended", IDI_SUSPENDED },
	{ L"TRINITY.z4", L"Trinity", IDI_TRINITY },
	{ L"wishbrin.z5", L"Wishbringer", IDI_WISHBRINGER },
	{ L"WITNESS.z3", L"Witness", IDI_WITNESS },
	{ L"zork0.z6", L"Zork Zero", IDI_ZORKZERO },
	{ L"zork1.z5", L"Zork I", IDI_ZORKI },
	{ L"ZORK2.z3", L"Zork II", IDI_ZORKII },
	{ L"ZORK3.z3", L"Zork III", IDI_ZORKIII },
	{ L"ztuu.z5", L"Zork - The Undiscovered Underground", IDI_ZORKTUU },
};

#define inameMax (sizeof(rgname) / sizeof(NAME))

HRESULT CreateLink(_TCHAR *szName, _TCHAR *szTarget, _TCHAR *szArgs, _TCHAR *szStartDir, _TCHAR *szDesc, _TCHAR *szIcon, int iIcon)
{
	HRESULT hr;
	IShellLink *psl;

	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
	if (FAILED(hr))
		return hr;

	psl->SetPath(szTarget);
	psl->SetArguments(szArgs);
	psl->SetWorkingDirectory(szStartDir);
	psl->SetDescription(szDesc);
	psl->SetIconLocation(szIcon, iIcon);

	IPersistFile *ppf;
	hr = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
	psl->Release();
	if (FAILED(hr))
		return hr;

	hr = ppf->Save(szName, TRUE);
	ppf->Release();

	return hr;
}

int _tmain(int cszArg, _TCHAR* rgszArg[])
{
	HRESULT hr;
	_TCHAR szShortcutFolderPath[MAX_PATH + 10];
	_TCHAR *szFrotz = L"c:\\mark\\z-machine\\frotz.exe";
	_TCHAR *szInfocomDir = L"c:\\mark\\z-machine\\";
	int nRet = 1;
	int fSetSzFrotz = FALSE;
	int fIgnoreSubDirs = FALSE;

	for (int isz = 1; isz < cszArg; isz++)
	{
		_TCHAR *sz = rgszArg[isz];

		if (sz[0] == '//' || sz[0] == '-')
		{
			switch (tolower(sz[1]))
			{
			case '?':
				printf("INFOCUT - create start menu shortCUTs for INFOcom's masterpieces\n\n"
					"Usage: infocut [-?] [-d] [frotz path] [masterpieces dir]\n"
					"    -?   Display this message\n"
					"    -d   All of the data files are in one directory\n");
				return 1;
			case 'd':
				fIgnoreSubDirs = TRUE;
				break;
			}
		}
		else if (!fSetSzFrotz)
		{
			szFrotz = rgszArg[isz];
			fSetSzFrotz = TRUE;
		}
		else
			szInfocomDir = rgszArg[isz];
	}

	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		printf("Unable to initialize COM library.\n");
		return 1;
	}

	PWSTR pszT;
	if (FAILED(SHGetKnownFolderPath(FOLDERID_Programs, 0, NULL, &pszT)))
	{
		printf("Unable to find start menu folder.\n");
		goto LQuit;
	}

	_tcscpy_s(szShortcutFolderPath, pszT);
	_tcscat_s(szShortcutFolderPath, L"\\Infocom\\");
	CreateDirectory(szShortcutFolderPath, NULL);

	for each (NAME name in rgname)
	{
		_tprintf(L"%s - %s\n", name.szName, name.szShort);

		_TCHAR szLinkName[MAX_PATH];
		_tcscpy_s(szLinkName, szShortcutFolderPath);
		_tcscat_s(szLinkName, name.szName);
		_tcscat_s(szLinkName, L".lnk");

#ifdef ICONFILES
		_TCHAR szIcon[MAX_PATH + 32];
		_tcscpy_s(szIcon, szInfocomDir);
		_tcscat_s(szIcon, L"icons256\\");
		_tcscat_s(szIcon, name.szName);
		_tcscat_s(szIcon, L" 256.ico");
#else
		_TCHAR *szIcon = rgszArg[0];
#endif

		hr = CreateLink(szLinkName, szFrotz, name.szShort, szInfocomDir, name.szName, szIcon, name.idi - IDI_MASTERPIECES);
		if (FAILED(hr))
			wprintf(L"Unable to create link %s.\n", name.szName);
	}
	nRet = 0;

LQuit:
	CoUninitialize();

	_tprintf(L"Press any key.");
	_gettch();

	return nRet;
}
