# ToDoList_8.0

## Visual Studio 2019 Compilation Guide
* Open and upgrade .\Core\3rdParty\Detours.vcxproj project
* Recompile Detours project
* Open .\Core\ToDoList_Core.sln
* Upgrade Visual Studio project and compile.
* Perform the following fix if you get compilation error.
* Then, get your coffee while waiting for the build to complete.

#### C2011: 'DPI_AWARENESS_CONTEXT_': 'struct' type redefinition
**Solution:** Comment out <code>DECLARE_HANDLE(DPI_AWARENESS_CONTEXT) in GraphicsMisc.h
```cs
//#ifndef DPI_AWARENESS_CONTEXT
//DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
//
//#	define DPI_AWARENESS_CONTEXT_UNAWARE              ((DPI_AWARENESS_CONTEXT)-1)
//#	define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE         ((DPI_AWARENESS_CONTEXT)-2)
//#	define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT)-3)
//#	define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
//#	define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED    ((DPI_AWARENESS_CONTEXT)-5)
//#endif
```

#### C2668: 'GraphicsMisc::Dwmxxxx': ambigiuous call to overloaded function
**Solution:** Add 'GraphicsMisc::' to DwmSet and DwmGet method in GraphicsMisc.cpp<br>
```cs
 BOOL GraphicsMisc::GetExtendedFrameBounds(HWND hWnd, CRect& rBounds)
 {
 #ifndef DWMWA_EXTENDED_FRAME_BOUNDS
 #define DWMWA_EXTENDED_FRAME_BOUNDS 9
 #endif

 	return GraphicsMisc::DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rBounds, sizeof(rBounds));
 }
 ```
#### C2059: syntax error 'constant'
**Solution:** Comment out <code>RC_DOWN</code> and <code>RC_UP</code> in struct <code>Corner</code> RoundCorner.h
```cs
enum
{
	RC_NORMAL		= 0x0000, // standard windows 2-pixel wide
	RC_THIN			= 0x0010, // single pixel wide
	RC_RENDER		= 0x0020, // full 3d style. requires RC_EDGES too.
	RC_EDGES		= 0x0040, // draw lines between corners if there is any space
//	RC_DOWN			= 0x0100, // reverses the colors
//	RC_UP			= 0x0200,
//	RC_DEFBTN		= 0x0400,
};

struct Corner;
```

#### LINK2026: module unsafe for SAFESH image
**Solution:**Disable option /SAFESH in Configuration > Linker > Advanced
