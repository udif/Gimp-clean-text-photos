
#include <Windows.h>
#include <cstdio>
#include "Typedefs.h"
#include "gimp.h"
#include <vector>

#pragma comment(lib, "user32.lib")

typedef PVOID (__cdecl *PFNFUNC)(...);

G_BEGIN_DECLS
	void	Query();
	void	Run(PCSTR psName, int params, const GimpParam *pSrc, int *pReturns, GimpParam **ppDst);
G_END_DECLS

const GimpPlugInInfo	PLUG_IN_INFO =
{
	NULL, NULL, Query, Run
};
PCSTR PLUGIN_NAME = "plug-in-Invert";

void	Msg(PCSTR psFmt, ...)
{
#define MSG_BUF_LEN	0x200
	va_list	ap;
	va_start(ap, psFmt);
	char	sMsg[MSG_BUF_LEN];
	::vsprintf(sMsg, psFmt, ap);
	va_end(ap);
	::MessageBoxA(NULL, sMsg, "DEBUG MSG", MB_OK | MB_ICONEXCLAMATION);
}

class Dll
{
	HMODULE	hModule_;
	bool	bAlreadyLoaded;
	
public:
	bool	Invalid()	{	return hModule_ == NULL;	}
	
	PFNFUNC	GetFunction(PCSTR psFuncName)
	{
		FARPROC	pfnProc;
		
		if( !(pfnProc = ::GetProcAddress(hModule_, psFuncName)) )
		{	::Msg("Not found such function: %s", psFuncName);	}
		return (PFNFUNC)pfnProc;
	}
	Dll(PCSTR psDllName) : bAlreadyLoaded(false)
	{
		if( (hModule_ = ::GetModuleHandle(psDllName)) )
		{
			//::Msg("Already loaded: %s", psDllName);
			bAlreadyLoaded = true;
			return;
		}
	
		if( !(hModule_ = ::LoadLibrary(psDllName)) )
		{	::Msg("Error loading: %s", psDllName);	}
	}
	~Dll()	{	if(!bAlreadyLoaded) ::FreeLibrary(hModule_);	}
};

//MAIN()
int __stdcall	WinMain(HINSTANCE hInst, HINSTANCE, char *psCmdLine, int nCmdShow)
{
	typedef int (*LPFNGIMPMAIN)(const GimpPlugInInfo *info, int argc, char *argv[]);

	// If PATH that includes these dlls is not high priority, loading dlls may fail! 
	Dll	libFFI("libffi-6.dll");	if(libFFI.Invalid())	return -1;
	Dll	libGIo("libgio-2.0-0.dll");	if(libGIo.Invalid())	return -1;
	Dll	libGLib("libglib-2.0-0.dll");	if(libGLib.Invalid())	return -1;
	Dll	libGMod("libgmodule-2.0-0.dll");	if(libGMod.Invalid())	return -1;
	Dll	libGObj("libgobject-2.0-0.dll");	if(libGObj.Invalid())	return -1;
	Dll	libIntl("libintl-8.dll");	if(libIntl.Invalid())	return -1;

	Dll	libGimpBase("libgimpbase-2.0-0.dll");	if(libGimpBase.Invalid())	return -1;
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return -1;
	
	LPFNGIMPMAIN	pfnGimpMain	=  (LPFNGIMPMAIN)libGimp.GetFunction("gimp_main");
	if(!pfnGimpMain)
	{	::MessageBox(0, "Error loading gimp_main", "ERROR", MB_OK);	return -1;	}	
	
	return (*pfnGimpMain)(&PLUG_IN_INFO, __argc, __argv);
}

void	Query()
{
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;

	PFNFUNC pfnGimpInstallProcedure	= libGimp.GetFunction("gimp_install_procedure");
	PFNFUNC pfnGimpPluginMenuRegister	= libGimp.GetFunction("gimp_plugin_menu_register");
	
	static GimpParamDef	args[] =
	{
		{GIMP_PDB_INT32, "run-mode", "Run mode"},
		{GIMP_PDB_IMAGE, "image", "Input image"},
		{GIMP_PDB_DRAWABLE, "drawable", "Input drawable"},
	};
	//static GimpParamDef	results[] =
	(*pfnGimpInstallProcedure)
	(
		PLUGIN_NAME,
		"Invert Color",
		"Invert color.",
		"Um6ra1",
		"Copyright (C) Um6ra1",
		"2016",
		"_Invert",
		"RGB*, GRAY*",
		GIMP_PLUGIN,
		G_N_ELEMENTS(args), 0,
		args, NULL
	);

	int status = (int)(*pfnGimpPluginMenuRegister)(PLUGIN_NAME, "<Image>/Filters/VsNative-Plugin");
	if(!status)	::Msg("gimp_plugin_menu_register() failed!");
}

void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable);

void	Run(PCSTR psName, int srcNum, const GimpParam *pSrc, int *pDstNum, GimpParam **ppDst)
{
	static GimpParam	values[1];
	Dll	libGimp("libgimp-2.0-0.dll");	if(libGimp.Invalid())	return;

	PFNFUNC pfnGimpDrawableGet	= libGimp.GetFunction("gimp_drawable_get");
	PFNFUNC pfnGimpDrawableDetach = libGimp.GetFunction("gimp_drawable_detach");
	PFNFUNC pfnGimpDisplaysFlush = libGimp.GetFunction("gimp_displays_flush");
	PFNFUNC pfnGimpProgressInit = libGimp.GetFunction("gimp_progress_init");

	*pDstNum	= 1;
	*ppDst		= values;
	values[0].type	= GIMP_PDB_STATUS;
	values[0].data.d_status	= GIMP_PDB_SUCCESS;
	
	//if(values[0].data.d_int32 != GIMP_RUN_NONINTERACTIVE)	::Msg("Salve munde!");

	GimpDrawable *pDrawable = (GimpDrawable *)(*pfnGimpDrawableGet)(pSrc[2].data.d_drawable);

	::ImgProc(libGimp, pDrawable);
	(*pfnGimpDisplaysFlush)();
	(*pfnGimpDrawableDetach)(pDrawable);
}

#define REP(idx, max)	for(int idx = 0, idx##Max = max; idx < idx##Max; idx ++)
#define FOR(idx, min, max)	for(int idx = min, idx##Max = max; idx < idx##Max; idx ++)
#define CEILDIV(n, d)	(((n) + (d) - 1) / (d))
void	ImgProc(Dll &libGimp, GimpDrawable *pDrawable)
{
	int x1, y1, x2, y2;
	GimpPixelRgn	rgnDst, rgnSrc;
	PFNFUNC	pfnGimpMaskBounds = libGimp.GetFunction("gimp_drawable_mask_bounds");
	PFNFUNC	pfnGimpDrawableBpp = libGimp.GetFunction("gimp_drawable_bpp");
	PFNFUNC	pfnGimpPixelRgnInit = libGimp.GetFunction("gimp_pixel_rgn_init");
	PFNFUNC	pfnGimpDrawableFlush = libGimp.GetFunction("gimp_drawable_flush");
	PFNFUNC	pfnGimpDrawableMergeShadow = libGimp.GetFunction("gimp_drawable_merge_shadow");
	PFNFUNC	pfnGimpDrawableUpdate = libGimp.GetFunction("gimp_drawable_update");
	PFNFUNC	pfnGimpPixelRgnGetRect = libGimp.GetFunction("gimp_pixel_rgn_get_rect");
	PFNFUNC	pfnGimpPixelRgnSetRect = libGimp.GetFunction("gimp_pixel_rgn_set_rect");
	PFNFUNC	pfnGimpTileCacheNTiles = libGimp.GetFunction("gimp_tile_cache_ntiles");
	PFNFUNC	pfnGimpTileWidth = libGimp.GetFunction("gimp_tile_width");

	(*pfnGimpMaskBounds)(pDrawable->drawable_id, &x1, &y1, &x2, &y2);
	int channels = (int)(*pfnGimpDrawableBpp)(pDrawable->drawable_id);
	int width = x2 - x1;
	int height = y2 - y1;
	//::Msg("x, y, w, h = %d, %d, %d, %d", x1, y1, width, height);
	//(*pfnGimpTileCacheNTiles)(CEILDIV(pDrawable->width, (int)(*pfnGimpTileWidth)()));
	(*pfnGimpPixelRgnInit)(&rgnDst, pDrawable, x1, y1, width, height, true, true);
	(*pfnGimpPixelRgnInit)(&rgnSrc, pDrawable, x1, y1, width, height, false, false);

	std::vector<BYTE>	buf(width * height * channels);

	(*pfnGimpPixelRgnGetRect)(&rgnSrc, &buf[0], x1, y1, width, height);

	REP(y, height)
	{
		REP(x, width)
		{
			UINT32 &pixel = *(UINT32 *)&buf[channels * (width * y + x)];
			REP(c, channels)
			{
				if (c >= 3)	break;
				BYTE	&p = ((BYTE *)&pixel)[c];
				p = ~p;
			}
		}
	}

	(*pfnGimpPixelRgnSetRect)(&rgnDst, &buf[0], x1, y1, width, height);

	(*pfnGimpDrawableFlush)(pDrawable);
	(*pfnGimpDrawableMergeShadow)(pDrawable->drawable_id, true);
	(*pfnGimpDrawableUpdate)(pDrawable->drawable_id, x1, y1, width, height);
}
