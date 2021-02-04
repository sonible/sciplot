// sciplot - a modern C++ scientific plotting library powered by gnuplot
// https://github.com/sciplot/sciplot
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
//
// Copyright (c) 2018-2021 Allan Leal
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

// C++ includes
#include <algorithm>
#include <limits>
#include <numeric>

// sciplot includes
#include <sciplot/enums.hpp>
#include <sciplot/specs/FillSpecsOf.hpp>
#include <sciplot/specs/LineSpecsOf.hpp>
#include <sciplot/specs/PointSpecsOf.hpp>
#include <sciplot/util.hpp>

namespace sciplot {

/// The class where options for the plot function are specified.
class PlotSpecs : public LineSpecsOf<PlotSpecs>, public PointSpecsOf<PlotSpecs>, public FillSpecsOf<PlotSpecs>
{
  public:
    /// Undefine / ignore column usage value. See use().
    static constexpr int USE_AUTO = std::numeric_limits<int>::min();

    /// Construct a PlotSpecs instance.
    /// @param what A string representing what to be plot (e.g., "'filename' u 1:2", "sin(x)", etc.)
    PlotSpecs(std::string what, std::string with);

    /// Convert this PlotSpecs object into a gnuplot formatted string.
    auto repr() const -> std::string;

    /// Set the label to be displayed for the plot object.
    auto title(std::string value) -> PlotSpecs&
    {
        m_title = gnuplot::titlestr(value);
        return *this;
    }

    /// Set which columns from the data file to use for plot data or tick labels. Resembles the "using" directive for a plot.
    /// Pass an USE_AUTO in any of these values to "undefine" that value, e.g. to use column 2 for y, do: plot.use(USE_AUTO, 2);
    /// To use strings as tick labels, you can pass them in the corresponding data column in the plot.draw() call.
    auto use(int xcol = USE_AUTO, int ycol = USE_AUTO, int zcol = USE_AUTO, int xtic = USE_AUTO, int x2tic = USE_AUTO, int ytic = USE_AUTO, int y2tic = USE_AUTO, int ztic = USE_AUTO) -> PlotSpecs&
    {
        std::vector<std::pair<unsigned int, std::string>> values = {
            {xcol, std::to_string(xcol)},
            {ycol, std::to_string(ycol)},
            {zcol, std::to_string(zcol)},
            {xtic, "xtic(" + std::to_string(xtic) + ")"},
            {x2tic, "x2tic(" + std::to_string(x2tic) + ")"},
            {ytic, "ytic(" + std::to_string(ytic) + ")"},
            {y2tic, "y2tic(" + std::to_string(y2tic) + ")"},
            {ztic, "ztic(" + std::to_string(ztic) + ")"}};
        std::vector<std::string> strings;
        // filter out all valuzes we don't want to use and put the remaining one int strings
        std::for_each(values.begin(), values.end(), [&strings](const std::pair<unsigned int, std::string>& v) { if (v.first != USE_AUTO) { strings.push_back(v.second); } });
        // join all remaining values using ":"
        m_using = std::accumulate(std::next(strings.begin()), strings.end(), strings[0], [](const std::string& a, const std::string& b) { return a + ":" + b; });
        return *this;
    }

  private:
    /// The what to be plotted as a gnuplot formatted string (e.g., "sin(x)").
    std::string m_what;

    /// The style of the plot (lines, points, linespoints) as a gnuplot formatted string (e.g., "with linespoints").
    std::string m_with;

    /// The title of the plot as a gnuplot formatted string (e.g., "title 'sin(x)'").
    std::string m_title;

    /// Select which columns from the data file to use for plot data or tick labels (e.g. "using 1:xtic(2)").
    std::string m_using;
};

PlotSpecs::PlotSpecs(std::string what, std::string with)
: m_what(what), m_with(with)
{
    lineWidth(internal::DEFAULT_LINEWIDTH);
}

auto PlotSpecs::repr() const -> std::string
{
    std::stringstream ss;
    ss << m_what << " ";
    ss << gnuplot::optionValueStr("using", m_using);
    ss << gnuplot::optionValueStr("title", m_title);
    ss << gnuplot::optionValueStr("with", m_with);
    ss << LineSpecsOf<PlotSpecs>::repr() << " ";
    ss << PointSpecsOf<PlotSpecs>::repr() << " ";
    ss << FillSpecsOf<PlotSpecs>::repr() << " ";
    return internal::removeExtraWhitespaces(ss.str());
}

} // namespace sciplot
