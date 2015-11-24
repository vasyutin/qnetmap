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

#ifndef QNETMAP_UNIVERSALTILEMAPXMLPARSER_H
#define QNETMAP_UNIVERSALTILEMAPXMLPARSER_H

#include "qnetmap_global.h"
#include "qnetmap_tilemapadapterparameters.h"
#include "qnetmap_httpparameters.h"

#include <QXmlDefaultHandler>
#include <deque>

class QString;

namespace qnetmap 
{
   class TUniversalTileMapXMLHandler : public QXmlDefaultHandler
   {
   public:
      TUniversalTileMapXMLHandler(TTileMapAdapterParametersList* TileMapAdapterParametersList_, 
                                  THttpParametersList& HttpParametersList_)
         : m_ErrorString(""), m_CurrentTag(""), m_CurrentMapAdapter(""),
           w_TileMapAdapterParametersList(TileMapAdapterParametersList_),
           w_HttpParametersList(HttpParametersList_)           
      {
      assert(TileMapAdapterParametersList_);
      }
      virtual ~TUniversalTileMapXMLHandler() {}

      //! \brief implementation of the method parser
      bool startDocument(void) { 
         return true; 
         }
      //! \brief implementation of the method parser
      bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
      //! \brief implementation of the method parser
      bool endElement(const QString&, const QString&, const QString&);
      //! \brief implementation of the method parser
      bool characters(const QString&);
      //! \brief implementation of the method parser
      QString errorString(void);
      //! \brief implementation of the method parser
      bool error(const QXmlParseException &Exception_);
      //! \brief implementation of the method parser
      bool fatalError(const QXmlParseException &Exception_);

   private:
      //! \brief Returns the current sequence of nested tags
      QString sequenceTags(void) const
      {
      QString Sequence;
      for(std::deque<QString>::const_iterator it = m_TagsStack.begin(); it != m_TagsStack.end(); ++it) {
         Sequence += "::";
         Sequence += *it;
         }
      return Sequence.mid(2);
      }

      //! \var the text of the last error
      QString m_ErrorString;
      //! \var processed tag
      QString m_CurrentTag;
      //! \var processed card adapter
      QString m_CurrentMapAdapter;
      //! \var parameter list adapter cards
      TTileMapAdapterParametersList* w_TileMapAdapterParametersList;
      //! \var parameter list http requests
      THttpParametersList& w_HttpParametersList;
      //! \var stack tag (only append and extract out the same)
      std::deque<QString> m_TagsStack;
   };
}
#endif
