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

#include "jobsxmlparser.h"

#include <vector>

// список загружаемых карт
extern std::vector<TMapParameters> g_Maps;
// количество одновременных запросов
extern int g_SimultaneousRequestsCount;

//------------------------------------------------------------------
bool TJobsXMLParser::startDocument(void)
{
return true;
}

//------------------------------------------------------------------
bool TJobsXMLParser::startElement(const QString&, const QString&,
                                  const QString& Name_, 
                                  const QXmlAttributes& Attr_)
{
if(Name_ == SPIDER_TAG_JOBDOCUMENT) {
   if(m_TagsStack.empty()) {
      m_CurrentTag = Name_;
      m_TagsStack.push(Name_);
      if(Attr_.length() == 1 &&
         Attr_.qName(0) == SPIDER_PARAMETER_REQUESTSCOUNT &&
         Attr_.value(0).toInt() > 0) {
         g_SimultaneousRequestsCount = Attr_.value(0).toInt();
         }
      else {
         m_ErrorString = "TJobsXMLParser: The tag \"" + Name_ + 
                         "\" is expected parameter \"" + SPIDER_PARAMETER_REQUESTSCOUNT + "\"";
         return false;
         }
      }
   else {
      m_ErrorString = "TJobsXMLParser: Tag name \"" + Name_ + "\" mast be the root tag.";
      return false;
      }
   }
else if(Name_ == SPIDER_TAG_MAP) {
   if(m_CurrentTag == SPIDER_TAG_JOBDOCUMENT) {
      m_CurrentTag = Name_;
      m_TagsStack.push(Name_);
      if(Attr_.length() == 1 && Attr_.qName(0) == SPIDER_PARAMETER_PLUGINNAME) {
         g_Maps.push_back(TMapParameters());
         m_CurrentMapParameters = g_Maps.end() - 1;
         m_CurrentMapParameters->setPluginName(Attr_.value(0));
         }
      else {
         m_ErrorString = "TJobsXMLParser: The tag \"" + Name_ + 
                         "\" is expected parameter \"" + SPIDER_PARAMETER_PLUGINNAME + "\"";
         return false;
         }
      }
   else {
      m_ErrorString = 
         "TJobsXMLParser: Tag name \"" + Name_ + "\" must be inside a tag \"" + SPIDER_TAG_JOBDOCUMENT + "\"";
      return false;
      }
   }
else if(Name_ == SPIDER_TAG_AREA) {
   if(m_CurrentTag == SPIDER_TAG_MAP) {
      m_CurrentTag = Name_;
      m_TagsStack.push(Name_);
      if(Attr_.length() == 1 && Attr_.qName(0) == SPIDER_PARAMETER_ZOOM) {
         m_CurrentMapParameters->m_Areas.push_back(TAreaParameters());
         m_CurrentAreaParameters = m_CurrentMapParameters->m_Areas.end() - 1;
         m_CurrentAreaParameters->setZoom(Attr_.value(0).toInt());
         }
      else {
         m_ErrorString = "TJobsXMLParser: The tag \"" + Name_ + 
                         "\" is expected parameter \"" + SPIDER_PARAMETER_ZOOM + "\"";
         return false;
         }
      }
   else {
      m_ErrorString = 
         "TJobsXMLParser: Tag name \"" + Name_ + "\" must be inside a tag \"" + SPIDER_TAG_MAP + "\"";
      return false;
      }
   }
else if(Name_ == SPIDER_TAG_POINT) {
   if(m_CurrentTag == SPIDER_TAG_AREA) {
      if(!m_CurrentAreaParameters->point1Initialized() || 
         !m_CurrentAreaParameters->point2Initialized()) {
         m_CurrentTag = Name_;
         m_TagsStack.push(Name_);
         if(Attr_.length() == 2 && 
            ((Attr_.qName(0) == SPIDER_PARAMETER_LONGITUDE && Attr_.qName(1) == SPIDER_PARAMETER_LATITUDE) ||
             (Attr_.qName(0) == SPIDER_PARAMETER_LATITUDE  && Attr_.qName(1) == SPIDER_PARAMETER_LONGITUDE))) {
            if(Attr_.qName(0) == SPIDER_PARAMETER_LONGITUDE) {
               if(!m_CurrentAreaParameters->point1Initialized())
                    m_CurrentAreaParameters->setPoint1(Attr_.value(0).toDouble(), Attr_.value(1).toDouble());
               else m_CurrentAreaParameters->setPoint2(Attr_.value(0).toDouble(), Attr_.value(1).toDouble());
               }
            else {
               if(!m_CurrentAreaParameters->point1Initialized())
                    m_CurrentAreaParameters->setPoint1(Attr_.value(1).toDouble(), Attr_.value(0).toDouble());
               else m_CurrentAreaParameters->setPoint2(Attr_.value(1).toDouble(), Attr_.value(0).toDouble());
               }
            }
         else {
            m_ErrorString = "TJobsXMLParser: The tag \"" + Name_ + "\" is expected parameters";
            return false;
            }
         }
      else {
         m_ErrorString = "TJobsXMLParser: The tag \"" + Name_ +
                         "\" should be introduced only 2 times in the tag \"" + SPIDER_TAG_AREA + "\"";
         return false;
         }
      }
   else {
      m_ErrorString = 
         "TJobsXMLParser: Tag name \"" + Name_ + "\" must be inside a tag \"" + SPIDER_TAG_AREA + "\"";
      return false;
      }
   }
else {
   m_ErrorString = "TJobsXMLParser: Tag name \"" + Name_ + "\" not known.";
   return false;
   }

return true;
}

//------------------------------------------------------------------
bool TJobsXMLParser::endElement(const QString&, const QString&, const QString& Name_)
{
if(m_CurrentTag == Name_) {
   // проверяем на правильную инициализацию
   if((Name_ == SPIDER_TAG_MAP  && !m_CurrentMapParameters->isInitialized()) ||
      (Name_ == SPIDER_TAG_AREA && !m_CurrentAreaParameters->isInitialized())) {
      m_ErrorString = "TJobsXMLParser: Tag \"" + Name_ + "\" parameters are incorrect.";
      return false;
      }
   // восстанавливаем родительсктй тег
   m_TagsStack.pop();
   if(m_TagsStack.empty()) m_CurrentTag = "";
   else                    m_CurrentTag = m_TagsStack.top();
   }
else {
   m_ErrorString = "TJobsXMLParser: Broken XML-file structure. "
                   "Open \"" + m_CurrentTag + "\", close \"" + Name_ + "\"";
   return false;
   }

return true;
}

//------------------------------------------------------------------
QString TJobsXMLParser::errorString(void)
{
QString ErrorString;

if(m_ErrorString != "") ErrorString = m_ErrorString;
else                    ErrorString = QXmlDefaultHandler::errorString();

return ErrorString;
}

//------------------------------------------------------------------
bool TJobsXMLParser::fatalError(const QXmlParseException &Exception_)
{ 
qWarning("Line %d, column %d: %s",
         Exception_.lineNumber(), 
         Exception_.columnNumber(),
         qPrintable(Exception_.message()));
return false; 
}
