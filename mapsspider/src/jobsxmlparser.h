/*
* This file is part of MapsSpider,
* an open-source cross-platform software for downloading maps,
* based on the library QNetMap
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
* along with MapsSpider. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAPSSPIDER_JOBSXMLPARSER_H
#define MAPSSPIDER_JOBSXMLPARSER_H

#include "parameters.h"
#include <qnetmap_xmltagnames.h>

#include <QXmlDefaultHandler>
#include <stack>

class QString;

class TJobsXMLParser : public QXmlDefaultHandler
{
public:
   TJobsXMLParser() : m_ErrorString(""), m_CurrentTag("") {}
   virtual ~TJobsXMLParser() {}

   //! \brief реализация метода парсера
   bool startDocument(void);
   //! \brief реализация метода парсера
   bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
   //! \brief реализация метода парсера
   bool endElement(const QString&, const QString&, const QString&);
   //! \brief реализация метода парсера
   QString errorString(void);
   //! \brief реализация метода парсера
   bool fatalError(const QXmlParseException &Exception_);

private:
   // \var строка с последней ошибкой
   QString m_ErrorString;
   // \var обрабатываемый тег
   QString m_CurrentTag;
   // \var стек тегов
   std::stack<QString> m_TagsStack;
   // \var указатель на параметры обрабатываемой области
   std::vector<TAreaParameters>::iterator m_CurrentAreaParameters;
   // \var указатель на параметры обрабатываемой карты
   std::vector<TMapParameters>::iterator m_CurrentMapParameters;
};

#endif
