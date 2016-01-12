/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will `be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QNetMap. If not, see <http://www.gnu.org/licenses/>.
*/

#include "qnetmap_version.h"

namespace qnetmap {

// -----------------------------------------------------------------------
int QNETMAP_EXPORT VersionMinor(void)
{
return QNETMAP_VERSION_MINOR;
}

// -----------------------------------------------------------------------
int QNETMAP_EXPORT VersionMajor(void)
{
return QNETMAP_VERSION_MAJOR;
}

// -----------------------------------------------------------------------
int QNETMAP_EXPORT VersionRevision(void)
{
return 
#include "qnetmap_revision.h"
;
}

// -----------------------------------------------------------------------
QString QNETMAP_EXPORT VersionString(void)
{
QString Result(QString::number(VersionMajor()));
Result += '.';
Result += QString::number(VersionMinor());
Result += '.';
Result += QString::number(VersionRevision());
return Result;
}

// -----------------------------------------------------------------------
QString QNETMAP_EXPORT VersionHash(void)
{
return QString::number((quint64)
	#include "qnetmap_revision.hash" 
	, 16);
}

} // namespace qnetmap {
