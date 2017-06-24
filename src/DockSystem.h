#ifndef DockSystenH
#define DockSystenH
/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


#if defined(ADS_DLL)
#if defined(ADS_EXPORT)
#define ASD_API __declspec(dllexport)
#else
#define ASD_API __declspec(dllimport)
#endif
#else
#define ASD_API
#endif

#include <QFlags>

namespace ads
{
enum DockWidgetArea
{
	NoDockWidgetArea = 0x00,
	LeftDockWidgetArea = 0x01,
	RightDockWidgetArea = 0x02,
	TopDockWidgetArea = 0x04,
	BottomDockWidgetArea = 0x08,
	CenterDockWidgetArea = 0x10,

	InvalidDockWidgetArea = NoDockWidgetArea,
	OuterDockAreas = TopDockWidgetArea | LeftDockWidgetArea | RightDockWidgetArea | BottomDockWidgetArea,
	AllDockAreas = OuterDockAreas | CenterDockWidgetArea
};
Q_DECLARE_FLAGS(DockWidgetAreas, DockWidgetArea)
} // namespace ads


//---------------------------------------------------------------------------
#endif // DockSystenH
