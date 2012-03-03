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
#include "plugin_123.hpp"
#include "version.hpp"

// {52d8eecb-acae-42de-9b2f-f1e909948272}
static const GUID MainGuid =
{ 0x52d8eecb, 0xacae, 0x42de, { 0x9b, 0x2f, 0xf1, 0xe9, 0x09, 0x94, 0x82, 0x72 } };

ADD_GETGLOBALINFO;

static struct PluginStartupInfo Info;

void WINAPI WIDE_SUFFIX(SetStartupInfo)(const struct PluginStartupInfo *Info)
{
  ::Info=*Info;
}

void WINAPI WIDE_SUFFIX(GetPluginInfo)(struct PluginInfo *Info)
{
  Info->StructSize=sizeof(*Info);
  Info->Flags=PF_DISABLEPANELS;
  Info->PluginMenuStringsNumber=0;
}

OPENPLUGIN
{
  return INVALID_HANDLE_VALUE;
}

PROCESSEDITORINPUT
{
  const INPUT_RECORD* Rec = PROCESSEDITORINPUT_REC;

  if (Rec->EventType==KEY_EVENT &&
      Rec->Event.KeyEvent.bKeyDown &&
     (Rec->Event.KeyEvent.wVirtualKeyCode & 0x7FFF) == 9 &&
     (Rec->Event.KeyEvent.dwControlKeyState & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED | RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) == 0)
  {
    bool rev = !!(Rec->Event.KeyEvent.dwControlKeyState & (SHIFT_PRESSED));

    struct EditorInfo ei;
    Info.EditorControl(ECTL_GETINFO, &ei);

    {
      struct EditorGetString egs;
      egs.StringNumber = -1;
      Info.EditorControl(ECTL_GETSTRING, &egs);

      if (ei.BlockType != BTYPE_STREAM ||  // Ignore block selection
          egs.SelStart != 0 ||             // Only activate if selection starts on a line start
          ei.CurPos != 0)                  // Only activate if the cursor is at the line start
        return 0;
    }

#if FARMANAGERVERSION_MAJOR >= 2
    struct EditorUndoRedo eur;
    eur.Command = EUR_BEGIN;
    Info.EditorControl(ECTL_UNDOREDO, &eur);
#endif

    TCHAR IndentStr[2];
    IndentStr[0] = '\t';
    IndentStr[1] = '\0';

    int line;
    for (line = ei.BlockStartLine; line < ei.TotalLines; line++)
    {
      struct EditorSetPosition esp;
      esp.CurLine = line;
      esp.CurPos = esp.Overtype = 0;
      esp.CurTabPos = esp.TopScreenLine = esp.LeftPos = -1;
      Info.EditorControl(ECTL_SETPOSITION, &esp);

      struct EditorGetString egs;
      egs.StringNumber = -1;
      Info.EditorControl(ECTL_GETSTRING, &egs);
      if (egs.SelStart == -1 || egs.SelStart == egs.SelEnd)
        break; // Stop when reaching the end of the text selection

      if (!rev) // Indent
      {
        Info.EditorControl(ECTL_INSERTTEXT, IndentStr);
      }
      else      // Unindent
      {
        if (egs.StringLength > 0)
        {
          int n = 0;
          if (egs.StringText[0]=='\t')
            n = 1;
          else
            while (egs.StringText[n]==' ' && n < ei.TabSize)
              n++;
          for (int i=0; i<n; i++)
            Info.EditorControl(ECTL_DELETECHAR, NULL);
        }
      }
    }

    {
      struct EditorSetPosition esp;
      esp.CurLine = ei.CurLine;
      esp.CurPos = ei.CurPos;
      esp.TopScreenLine = ei.TopScreenLine;
      esp.LeftPos = ei.LeftPos;
      esp.Overtype = ei.Overtype;
      esp.CurTabPos = -1;
      Info.EditorControl(ECTL_SETPOSITION, &esp);
    }

    {
      // Restore selection to how it was before the insertion
      struct EditorSelect es;
      es.BlockType = ei.BlockType;
      es.BlockStartLine = ei.BlockStartLine;
      es.BlockStartPos = 0;
      es.BlockWidth = 0;
      es.BlockHeight = line - ei.BlockStartLine + 1;
      Info.EditorControl(ECTL_SELECT, &es);
    }

#if FARMANAGERVERSION_MAJOR >= 2
    eur.Command = EUR_END;
    Info.EditorControl(ECTL_UNDOREDO, &eur);
#endif

    Info.EditorControl(ECTL_REDRAW, NULL);

    return 1;
  }
  return 0;
}
