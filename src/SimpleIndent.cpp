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
#include <CRT/crt.hpp>
#include "plugin.hpp"
#include "version.hpp"

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

// {52d8eecb-acae-42de-9b2f-f1e909948272}
static const GUID MainGuid =
{ 0x52d8eecb, 0xacae, 0x42de, { 0x9b, 0x2f, 0xf1, 0xe9, 0x09, 0x94, 0x82, 0x72 } };


static struct PluginStartupInfo Info;

void WINAPI GetGlobalInfoW(struct GlobalInfo *Info)
{
  Info->StructSize=sizeof(GlobalInfo);
  Info->MinFarVersion=FARMANAGERVERSION;
  Info->Version=PLUGIN_VERSION;
  Info->Guid=MainGuid;
  Info->Title=PLUGIN_NAME;
  Info->Description=PLUGIN_DESC;
  Info->Author=PLUGIN_AUTHOR;
}

void WINAPI SetStartupInfoW(const struct PluginStartupInfo *Info)
{
  ::Info=*Info;
}

void WINAPI GetPluginInfoW(struct PluginInfo *Info)
{
  Info->StructSize=sizeof(*Info);
  Info->Flags=PF_DISABLEPANELS;
  Info->PluginMenu.Count=0;
}

HANDLE WINAPI OpenW(const struct OpenInfo *OInfo)
{
  return INVALID_HANDLE_VALUE;
}

int WINAPI ProcessEditorInputW(const struct ProcessEditorInputInfo *Info)
{
  if (Info->Rec.EventType==KEY_EVENT &&
      Info->Rec.Event.KeyEvent.bKeyDown &&
     (Info->Rec.Event.KeyEvent.wVirtualKeyCode & 0x7FFF) == 9 &&
     (Info->Rec.Event.KeyEvent.dwControlKeyState & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED | RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) == 0)
  {
    bool rev = !(Info->Rec.Event.KeyEvent.dwControlKeyState & (SHIFT_PRESSED));

    struct EditorInfo ei;
    ::Info.EditorControl(-1, ECTL_GETINFO, 0, &ei);

    {
      struct EditorGetString egs;
      egs.StringNumber = -1;
      ::Info.EditorControl(-1, ECTL_GETSTRING, 0, &egs);

      if (ei.BlockType != BTYPE_STREAM ||  // Ignore block selection
          egs.SelStart != 0 ||             // Only activate if selection starts on a line start
          ei.CurPos != 0)                  // Only activate if the cursor is at the line start
        return 0;
    }

    TCHAR IndentStr[2];
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
        ::Info.EditorControl(-1, ECTL_SETPOSITION, 0, &esp);
        egs.StringNumber = -1;
        ::Info.EditorControl(-1, ECTL_GETSTRING, 0, &egs);
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
          ::Info.EditorControl(-1, ECTL_REALTOTAB, 0, &ecp);
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
          ::Info.EditorControl(-1, ECTL_SETSTRING, 0, &ess);
        }
        if (x)
        {
          for (int i=0; i<x; i++)
            ::Info.EditorControl(-1, ECTL_INSERTTEXT, 0, IndentStr);
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
      ::Info.EditorControl(-1, ECTL_SETPOSITION, 0, &esp);
    }

    ::Info.EditorControl(-1, ECTL_REDRAW, 0, NULL);

    return 1;
  }
  return 0;
}
