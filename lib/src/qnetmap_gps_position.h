/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
* Based on QMapControl GPS_Position code by Kai Winter
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QNetMap. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QNETMAP_GPS_POSITION_H
#define QNETMAP_GPS_POSITION_H

#include "qnetmap_global.h"
#include <QString>

namespace qnetmap
{
    //! Represents a coordinate from a GPS receiver
    /*!
     * This class is used to represent a coordinate which has been parsed from a NMEA string.
     * This is not fully integrated in the API. An example which uses this data type can be found under Samples.
     * @author Kai Winter
     */
    class QNETMAP_EXPORT TGPS_Position
    {
    public:
        TGPS_Position(float time, float longitude, QString longitude_dir, float latitude, QString latitude_dir);
        float time; /*!< time of the string*/
        float longitude; /*!< longitude coordinate*/
        float latitude; /*!< latitude coordinate*/

    private:
        QString longitude_dir;
        QString latitude_dir;
    };
}

#endif
