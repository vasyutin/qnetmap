#ifndef QNETMAP_HTTPPARAMETERS_H
#define QNETMAP_HTTPPARAMETERS_H

#include "qnetmap_global.h"

class QString;

namespace qnetmap 
{
   //! \class THttpParameters
   //! \brief controls the parameters of http-requests
   class THttpParameters
   {
   public:
      THttpParameters();
      virtual ~THttpParameters() {}

      // methods that return the required properties
      //! \brief return the field User-Agent
      const QString& userAgent(void) const { return m_UserAgent; }
      // methods of establishing the properties with the necessary checks
      bool setUserAgent(const QString& UserAgent_);
      //! \brief validation of input data
      bool isValid(void) const;

   private:
      //! \var field User-Agent
      QString m_UserAgent;
   };

   typedef QList<THttpParameters> THttpParametersList;
}

#endif
