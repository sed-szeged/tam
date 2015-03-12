/*
 * Copyright (C): 2014-2015 Department of Software Engineering, University of Szeged
 *
 * Authors: David Havas <havasd@inf.u-szeged.hu>
 *
 * This file is part of TAM.
 *
 *  TAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with TAM.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CSHOWSCORES_H
#define CSHOWSCORES_H

#include <QWebView>
#include "rapidjson/document.h"
#include "data/CSelectionData.h"

using namespace soda;

/**
 * @brief The CShowScores class implements methods for visualising the results of the fault localization techniques.
 */
class CShowScores
{
public:
    /**
     * @brief Creates a new object.
     */
    CShowScores();
    ~CShowScores();

    /**
     * @brief Visualises the given results.
     * @param view Current QWebView.
     * @param scores Results of computed fault localization techniques
     * @param selection Current test-suite for resolving code element ids.
     */
    void generateResults(QWebView *view, rapidjson::Document *scores, CSelectionData *selection);
};

#endif // CSHOWSCORES_H
