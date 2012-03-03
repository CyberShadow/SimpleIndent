/*
    Simple Indent plugin for FAR Manager
    
    Based on:

    Block Indent plugin for FAR Manager
    Copyright (C) 2001-2004 Alex Yaroslavsky

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "plugin.hpp"

#if defined(__GNUC__)
#ifdef __cplusplus
extern "C"{
#endif
  BOOL WINAPI DllMainCRTStartup(HANDLE hDll,DWORD dwReason,LPVOID lpReserved);
#ifdef __cplusplus
};
#endif

BOOL WINAPI DllMainCRTStartup(HANDLE hDll,DWORD dwReason,LPVOID lpReserved)
{
  (void) hDll;
  (void) dwReason;
  (void) lpReserved;
  return TRUE;
}
#endif


static struct PluginStartupInfo Info;

void *memcpy(void *dst, const void *src, size_t count)
{
  void *ret = dst;

  while (count--)
  {
    *(char *)dst = *(char *)src;
    dst = (char *)dst + 1;
    src = (char *)src + 1;
  }
  return (ret);
}

char *strcpy(char *dst, const char *src)
{
  char *cp = dst;

  while(*cp++ = *src++)
    ;

  return (dst);
}

/*
inline int IsWhite(unsigned char c)
{
  return ((c == 32) || (c == 9))?TRUE:FALSE;
}
*/

void WINAPI _export SetStartupInfo(const struct PluginStartupInfo *psi)
{
  Info=*psi;
}

void WINAPI _export GetPluginInfo(struct PluginInfo *pi)
{
  pi->StructSize=sizeof(struct PluginInfo);
  pi->Flags=PF_DISABLEPANELS;
  pi->PluginMenuStrings=NULL;
  pi->PluginMenuStringsNumber=0;
}

HANDLE WINAPI _export OpenPlugin(int OpenFrom, int Item)
{
  return INVALID_HANDLE_VALUE;
}

int WINAPI _export ProcessEditorInput(const INPUT_RECORD *Rec)
{
  if (Rec->EventType==KEY_EVENT && 
      Rec->Event.KeyEvent.bKeyDown && 
     (Rec->Event.KeyEvent.wVirtualKeyCode & 0xFFFF) == 9 &&
     (Rec->Event.KeyEvent.dwControlKeyState & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED | RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) == 0)
  {
    bool rev = !(Rec->Event.KeyEvent.dwControlKeyState & (SHIFT_PRESSED));
    
    struct EditorInfo ei;
    Info.EditorControl(ECTL_GETINFO,&ei);

    {
      struct EditorGetString egs;
      egs.StringNumber = -1;
      Info.EditorControl(ECTL_GETSTRING,&egs);

      if (ei.BlockType != BTYPE_STREAM ||
          egs.SelStart != 0 ||
          ei.CurPos != 0)
        return 0;
    }

    char IndentStr[2];
    IndentStr[0] = 9;
    IndentStr[1] = '\0';
    int IndentSize = ei.TabSize;

    int loop = TRUE;
    int line = ei.BlockStartLine;

    do
    {
      struct EditorGetString egs;
      if (line < ei.TotalLines)
      {
        struct EditorSetPosition esp;
        esp.CurLine = line++;
        esp.CurPos = esp.Overtype = 0;
        esp.CurTabPos = esp.TopScreenLine = esp.LeftPos = -1;
        Info.EditorControl(ECTL_SETPOSITION,&esp);
        egs.StringNumber = -1;
        Info.EditorControl(ECTL_GETSTRING,&egs);
        if (loop && ((egs.SelStart == -1) || (egs.SelStart == egs.SelEnd)))
          break;
      }
      else
        break;
      int j = 0;
      while ((egs.StringText[j]==9 || egs.StringText[j]==32) && (j < egs.StringLength))
        j++;
      if ((j || rev) && (j < egs.StringLength))
      {
        int x;
        {
          struct EditorConvertPos ecp;
          ecp.StringNumber = -1;
          ecp.SrcPos = j+1;
          Info.EditorControl(ECTL_REALTOTAB,&ecp);
          x = (--ecp.DestPos)/IndentSize;
          if (!(ecp.DestPos%IndentSize) && !rev)
            x--;
          if (rev)
            x++;
        }
        {
          struct EditorSetString ess;
          ess.StringNumber = -1;
          ess.StringText = &egs.StringText[j];
          ess.StringEOL = egs.StringEOL;
          ess.StringLength = egs.StringLength - j;
          Info.EditorControl(ECTL_SETSTRING,&ess);
        }
        if (x)
        {
          for (int i=0; i<x; i++)
            Info.EditorControl(ECTL_INSERTTEXT,IndentStr);
        }
      }
    } while (loop);

    {
      struct EditorSetPosition esp;
      esp.CurLine = ei.CurLine;
      esp.CurPos = ei.CurPos;
      esp.TopScreenLine = ei.TopScreenLine;
      esp.LeftPos = ei.LeftPos;
      esp.Overtype = ei.Overtype;
      esp.CurTabPos = -1;
      Info.EditorControl(ECTL_SETPOSITION,&esp);
    }

    Info.EditorControl(ECTL_REDRAW, NULL);

    return 1;
  }
  return 0;
}
