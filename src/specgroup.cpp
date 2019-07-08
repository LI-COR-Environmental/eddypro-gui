/***************************************************************************
  specgroup.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2018, LI-COR Biosciences
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

#include "specgroup.h"

#include <QStringList>

// custom relation (not useful for lexical ordering)
bool SpecGroup::operator<(const SpecGroup &sg) const
{
    // exclude negative values
    if ((toSpecGroupCode(start_) < 0)
        || (toSpecGroupCode(stop_) < 0)
        || (toSpecGroupCode(sg.start_) < 0)
        || (toSpecGroupCode(sg.stop_) < 0))
        return true;

    return ((toSpecGroupCode(start_) < toSpecGroupCode(sg.start_))
              && (toSpecGroupCode(stop_) < toSpecGroupCode(sg.stop_))
              && (toSpecGroupCode(sg.stop_) < toSpecGroupCode(start_)));
}

const QString SpecGroup::getJAN_STRING()
{
    static const QString s(tr("January"));
    return s;
}

const QString SpecGroup::getFEB_STRING()
{
    static const QString s(tr("February"));
    return s;
}

const QString SpecGroup::getMAR_STRING()
{
    static const QString s(tr("March"));
    return s;
}

const QString SpecGroup::getAPR_STRING()
{
    static const QString s(tr("April"));
    return s;
}

const QString SpecGroup::getMAY_STRING()
{
    static const QString s(tr("May"));
    return s;
}

const QString SpecGroup::getJUN_STRING()
{
    static const QString s(tr("June"));
    return s;
}

const QString SpecGroup::getJUL_STRING()
{
    static const QString s(tr("July"));
    return s;
}

const QString SpecGroup::getAUG_STRING()
{
    static const QString s(tr("August"));
    return s;
}

const QString SpecGroup::getSEP_STRING()
{
    static const QString s(tr("September"));
    return s;
}

const QString SpecGroup::getOCT_STRING()
{
    static const QString s(tr("October"));
    return s;
}

const QString SpecGroup::getNOV_STRING()
{
    static const QString s(tr("November"));
    return s;
}

const QString SpecGroup::getDEC_STRING()
{
    static const QString s(tr("December"));
    return s;
}

SpecGroup::SpecGroup() :
    start_(tr("PLEASE SPECIFY A BEGINNING")),
    stop_(tr("PLEASE SPECIFY A END"))
{ ; }

SpecGroup::SpecGroup(const QString &start, const QString &stop) :
    start_(start),
    stop_(stop)
{ ; }

SpecGroup::~SpecGroup() { ; }

SpecGroup::SpecGroup(const SpecGroup &sg) :
    start_(sg.start_),
    stop_(sg.stop_)
{ ; }

SpecGroup& SpecGroup::operator=(const SpecGroup &sg)
{
    if (this != &sg)
    {
        start_ = sg.start_;
        stop_ = sg.stop_;
    }
    return *this;
}

bool SpecGroup::operator==(const SpecGroup &sg) const
{
    return (start_ == sg.start_)
        && (stop_ == sg.stop_);
}

// NOTE: never used
const QStringList SpecGroup::monthStringList()
{
    return (QStringList()
            << getJAN_STRING()
            << getFEB_STRING()
            << getMAR_STRING()
            << getAPR_STRING()
            << getMAY_STRING()
            << getJUN_STRING()
            << getJUL_STRING()
            << getAUG_STRING()
            << getSEP_STRING()
            << getOCT_STRING()
            << getNOV_STRING()
            << getDEC_STRING()
            );
}

// convert numeric spectra group code in string var
// NOTE: never used
const QString SpecGroup::toStringSpecGroup(qint8 i)
{
    switch (i)
    {
        case 0:
            return SpecGroup::getJAN_STRING();
        case 1:
            return SpecGroup::getFEB_STRING();
        case 2:
            return SpecGroup::getMAR_STRING();
        case 3:
            return SpecGroup::getAPR_STRING();
        case 4:
            return SpecGroup::getMAY_STRING();
        case 5:
            return SpecGroup::getJUN_STRING();
        case 6:
            return SpecGroup::getJUL_STRING();
        case 7:
            return SpecGroup::getAUG_STRING();
        case 8:
            return SpecGroup::getSEP_STRING();
        case 9:
            return SpecGroup::getOCT_STRING();
        case 10:
            return SpecGroup::getNOV_STRING();
        case 11:
            return SpecGroup::getDEC_STRING();
        default:
            return QString();
    }
}

int SpecGroup::toSpecGroupCode(const QString& s)
{
    if (s == SpecGroup::getJAN_STRING())
    {
        return 0;
    }
    else if (s == SpecGroup::getFEB_STRING())
    {
        return 1;
    }
    else if (s == SpecGroup::getMAR_STRING())
    {
        return 2;
    }
    else if (s == SpecGroup::getAPR_STRING())
    {
        return 3;
    }
    else if (s == SpecGroup::getMAY_STRING())
    {
        return 4;
    }
    else if (s == SpecGroup::getJUN_STRING())
    {
        return 5;
    }
    else if (s == SpecGroup::getJUL_STRING())
    {
        return 6;
    }
    else if (s == SpecGroup::getAUG_STRING())
    {
        return 7;
    }
    else if (s == SpecGroup::getSEP_STRING())
    {
        return 8;
    }
    else if (s == SpecGroup::getOCT_STRING())
    {
        return 9;
    }
    else if (s == SpecGroup::getNOV_STRING())
    {
        return 10;
    }
    else if (s == SpecGroup::getDEC_STRING())
    {
        return 11;
    }
    else
    {
        return -1;
    }
}
