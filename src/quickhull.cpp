#include "quickhull.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <valarray>
#include <chrono>

#include <cstdlib>
#include <cstdio>

#include <iostream>

#include <cstdlib>
#include <cassert>

int main()
{
    using G = double;
    using point_type = std::valarray< G >;
    using H = convex_hull< point_type >;
    using size_type = std::size_t;

    std::ifstream ifs_;
    ifs_.open("points.txt"); // rbox n D3 s 100 > points.txt
    if (!ifs_.is_open()) {
        std::cerr << "file is not open" << std::endl;
        return EXIT_FAILURE;
    }
    std::string line_;
    if (!std::getline(ifs_, line_)) {
        std::cerr << "no dim at first line" << std::endl;
        return EXIT_FAILURE;
    }
    size_type const dim_ = std::stoll(line_);
    if (!std::getline(ifs_, line_)) {
        std::cerr << "no count at second line" << std::endl;
        return EXIT_FAILURE;
    }
    size_type const count_ = std::stoll(line_);
    std::deque< point_type > points_;
    while (std::getline(ifs_, line_)) {
        points_.emplace_back(dim_);
        point_type & point_ = points_.back();
        std::istringstream iss(line_);
        for (size_type i = 0; i < dim_; ++i) {
            if (!(iss >> point_[i])) {
                std::cerr << "bad value faced at line " << points_.size() << " of data" << std::endl;
                return EXIT_FAILURE;
            }
        }
    }
    assert(points_.size() == count_);
    if (count_ != points_.size()) {
        std::cerr << "input file format error" << std::endl;
        return EXIT_FAILURE;
    }
    std::ofstream ofs_;
    ofs_.open("script.txt"); // gnuplot> load 'script.txt'
    if (!ofs_.is_open()) {
        std::cerr << "output file cannot be truncated" << std::endl;
        return EXIT_FAILURE;
    }
    ofs_ << "reset" << std::endl;
    ofs_ << "set autoscale" << std::endl;
    switch (dim_) {
    case 1 : {
        ofs_ << "plot";
        break;
    }
    case 2 : {
        ofs_ << "plot";
        break;
    }
    case 3 : {
        ofs_ << "splot";
        break;
    }
    default : {
        std::cerr << "dimensionality value (" << dim_ << ") is out of supported range" << std::endl;
        return EXIT_FAILURE;
    }
    }
    std::cout.rdbuf()->pubsetbuf(nullptr, 0);
    std::cout << "D = " << dim_ << std::endl;
    std::cout << "N = " << count_ << std::endl;
    H convex_hull_(dim_, points_.cbegin(), points_.cend());
    {
        using std::chrono::duration_cast;
        using std::chrono::microseconds;
        using std::chrono::steady_clock;
        steady_clock::time_point const start = steady_clock::now();
        {
            convex_hull_.create_convex_hull();
        }
        steady_clock::time_point const end = steady_clock::now();
        std::cout << "time = " << duration_cast< microseconds >(end - start).count() << std::endl;
    }
    auto const & facets_ = convex_hull_.facets_;
    std::cout << "number of facets created = " << facets_.size() << std::endl;
    ofs_ << " '-' with points notitle, '-' with labels offset 0,char 1 notitle";
    for (std::size_t i = 0; i < facets_.size(); ++i) {
        ofs_ << ", '-' with lines notitle";
    }
    ofs_ << std::endl;
    for (point_type const & point_ : points_) {
        for (G const & coordinate_ : point_) {
            ofs_ << coordinate_ << ' ';
        }
        ofs_ << std::endl;
    }
    ofs_ << 'e' << std::endl;
    std::size_t i = 0;
    for (point_type const & point_ : points_) {
        for (G const & coordinate_ : point_) {
            ofs_ << coordinate_ << ' ';
        }
        ofs_ << i << std::endl;
        ++i;
    }
    ofs_ << 'e' << std::endl;
    for (auto const & facet_ : facets_) {
        auto const & vertices_ = facet_.second.vertices_;
        for (size_type const vertex_ : vertices_) {
            for (G const & coordinate_ : points_.at(vertex_)) {
                ofs_ << coordinate_ << ' ';
            }
            ofs_ << std::endl;
        }
        point_type const & first_vertex_ = points_.at(vertices_.front());
        for (G const & coordinate_ : first_vertex_) {
            ofs_ << coordinate_ << ' ';
        }
        ofs_ << std::endl;
        ofs_ << 'e' << std::endl;
    }
    return EXIT_SUCCESS;
}
