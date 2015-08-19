/***************************************************************************
  specgroup.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
  Author: Antonio Forgione

  This file is part of EddyPro (R).

  EddyPro (R) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (R) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (R). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef SPECGROUP_H
#define SPECGROUP_H

#include <QCoreApplication> // for Q_DECLARE_TR_FUNCTIONS
#include <QDebug>

////////////////////////////////////////////////////////////////////////////////
/// \file src/specgroup.h
/// \brief
/// \version
/// \date
/// \author      Antonio Forgione
/// \note
/// \sa
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

class QString;
class QStringList;

/// \class SpecGroup
/// \brief Class representing the spectral month groups
class SpecGroup
{
    Q_DECLARE_TR_FUNCTIONS(SpecGroup)

public:
    // anem measures strings
    static const QString getJAN_STRING();
    static const QString getFEB_STRING();
    static const QString getMAR_STRING();
    static const QString getAPR_STRING();
    static const QString getMAY_STRING();
    static const QString getJUN_STRING();
    static const QString getJUL_STRING();
    static const QString getAUG_STRING();
    static const QString getSEP_STRING();
    static const QString getOCT_STRING();
    static const QString getNOV_STRING();
    static const QString getDEC_STRING();

    SpecGroup();
    SpecGroup(const QString &start, const QString &stop);
    SpecGroup(const SpecGroup &sg);

    SpecGroup& operator=(const SpecGroup &sg);
    bool operator==(const SpecGroup &sg) const;
    bool operator<(const SpecGroup &sg) const;

    ~SpecGroup();

    // get/set start
    const QString &start() const;
    void setStart(const QString &month);

    // get/set stop
    const QString &stop() const;
    void setStop(const QString &month);

    // return a list of type strings
    static const QStringList monthStringList();

    static const QString toStringSpecGroup(qint8 i);
    static int toSpecGroupCode(const QString& s);

private:
    QString start_;
    QString stop_;
};

inline void SpecGroup::setStart(const QString &month)
    { start_ = month; }

inline const QString &SpecGroup::start() const
    { return start_; }

inline void SpecGroup::setStop(const QString &month)
    { stop_ = month; }

inline const QString &SpecGroup::stop() const
    { return stop_; }

#endif // SPECGROUP_H
