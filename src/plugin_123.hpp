#include "plugin.hpp"

// Plugin initialization code

#ifdef __cplusplus
 #define EXTERN_C_BEGIN extern "C" { 
 #define EXTERN_C_END   };
#endif

#if defined(__GNUC__)
EXTERN_C_BEGIN
  BOOL WINAPI DllMainCRTStartup(HANDLE hDll,DWORD dwReason,LPVOID lpReserved);
EXTERN_C_END

BOOL WINAPI DllMainCRTStartup(HANDLE hDll,DWORD dwReason,LPVOID lpReserved)
{
  (void) hDll;
  (void) dwReason;
  (void) lpReserved;
  return TRUE;
}
#endif

// ********** Exported plugin API **********

#if FARMANAGERVERSION_MAJOR >= 2
 #define WIDE
#endif

#ifdef WIDE
 #define WIDE_SUFFIX(x) x##W
#else
 #define WIDE_SUFFIX(x) x
#endif

// GetGlobalInfo

#if FARMANAGERVERSION_MAJOR >= 3
 #define ADD_GETGLOBALINFO                                                   \
  void WINAPI GetGlobalInfoW(struct GlobalInfo *Info)                        \
  {                                                                          \
    Info->StructSize=sizeof(GlobalInfo);                                     \
    Info->MinFarVersion=FARMANAGERVERSION;                                   \
    Info->Version=PLUGIN_VERSION;                                            \
    Info->Guid=MainGuid;                                                     \
    Info->Title=PLUGIN_NAME;                                                 \
    Info->Description=PLUGIN_DESC;                                           \
    Info->Author=PLUGIN_AUTHOR;                                              \
  }
#else
 #define ADD_GETGLOBALINFO
#endif

// GetPluginInfo

#if FARMANAGERVERSION_MAJOR >= 3
 #define PluginMenuStringsNumber PluginMenu.Count
#endif

// Open / OpenPlugin

#if FARMANAGERVERSION_MAJOR >= 3
 #define OPENPLUGIN HANDLE WINAPI OpenW(const struct OpenInfo *OInfo)
#else
 #define OPENPLUGIN HANDLE WINAPI WIDE_SUFFIX(OpenPlugin)(int OpenFrom, int Item)
#endif

// ProcessEditorInput

#if FARMANAGERVERSION_MAJOR >= 3
 #define PROCESSEDITORINPUT int WINAPI ProcessEditorInputW(const struct ProcessEditorInputInfo *_Info)
 #define PROCESSEDITORINPUT_REC (&(_Info->Rec))
#else
 #define PROCESSEDITORINPUT int WINAPI WIDE_SUFFIX(ProcessEditorInput)(const INPUT_RECORD *_Rec)
 #define PROCESSEDITORINPUT_REC _Rec
#endif

// ********** Far service API **********

#if FARMANAGERVERSION_MAJOR >= 3
 #define EditorControl(cmd,str) EditorControl(-1,cmd,0,str)
#endif