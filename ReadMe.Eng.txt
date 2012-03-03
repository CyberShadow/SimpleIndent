Simple Indent plugin for FAR Manager
Copyright (C) 2012 Vladimir Panteleev
Based on Block Indent plugin
Copyright (C) 2001-2004 Alex Yaroslavsky

-------------------------------------------------------------------------------
License:

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

-------------------------------------------------------------------------------
Usage:

Select some lines, and press Tab to indent and Shift+Tab to unindent. The
plugin will insert tabs, or spaces if the "Expand tabs to spaces" editor option
is enabled. The number of spaces added or removed is set by the tab size.

The plugin will only activate if the cursor is at the beginning of the line,
and at the edge or inside the selection. Otherwise, the key press will be
processed as usual (it will probably insert a tab character or a few spaces).
