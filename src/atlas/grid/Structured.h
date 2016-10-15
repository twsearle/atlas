/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef Structured_h
#define Structured_h

#include "eckit/config/Parametrisation.h"
#include "eckit/memory/Builder.h"
#include "atlas/grid/Grid.h"


namespace atlas {
namespace grid {


/**
 * @brief Structured Grid
 *
 * This class is a base class for all grids that can be described by
 * constant latitudes with a uniform distribution of points per latitude
 * in zonal direction.
 * This means any full grid and reduced grid, both regular, gaussian or other
 * such distribution can be represented with this class
 */
class Structured : public Grid {
  public:

    typedef eckit::SharedPtr<Structured> Ptr;

    static Structured* create(const eckit::Parametrisation&);

    static Structured* create(const std::string& shortName);

  public:

    static std::string className();

    static std::string grid_type_str();

  public:

    Structured();

    virtual ~Structured();

    virtual size_t npts() const {
        return npts_;
    }

    virtual void lonlat(std::vector<Point>&) const;

    virtual eckit::Properties spec() const = 0;

    virtual std::string getOptimalMeshGenerator() const {
        return "Structured";
    }

    virtual size_t N() const {
        return N_;
    }

    inline size_t nlat() const {
        return lat_.size();
    }

    inline size_t nlon( size_t jlat ) const {
        return static_cast<size_t>(pl_[jlat]);
    }

    inline size_t nlonmax() const {
        return nlonmax_;
    }

    inline size_t nlonmin() const {
        return nlonmin_;
    }

    inline const std::vector<long>& pl() const {
        return pl_;
    }

    inline const std::vector<double>& latitudes() const {
        return lat_;
    }

    inline double lon( const size_t jlat, const size_t jlon ) const {
        return lonmin_[jlat] + static_cast<double>(jlon) * lon_inc_[jlat];
    }

    inline double lat( const size_t jlat ) const {
        return lat_[jlat];
    }

    inline void lonlat( const size_t jlat, const size_t jlon, double crd[] ) const {
        crd[0] = lon(jlat,jlon);
        crd[1] = lat(jlat);
    }

    inline bool reduced() const {
        return nlonmax() != nlonmin();
    }

  protected: // methods

    virtual size_t copyLonLatMemory(double* pts, size_t size) const;

    virtual void print(std::ostream&) const;

    /// Hash of the PL array
    virtual void hash(eckit::MD5&) const;

    void setup(const size_t nlat, const double lats[], const long pl[], const double lonmin[], const double lonmax[]);

    static void setup_lon_limits(const size_t nlat, const long pl[], const Domain& dom, double lonmin[], double lonmax[]);

    void setup_lat_hemisphere(const size_t N, const double lat[], const long lon[]);

  protected:

    /// Number of latitudes in hemisphere
    size_t N_;

    // TODO: document
    size_t nlonmin_;

    // TODO: document
    size_t nlonmax_;

    /// Total number of unique points in the grid
    size_t npts_;

    /// Latitude values
    std::vector<double> lat_;

    /// Number of points per latitude
    std::vector<long> pl_;

    /// Value of minimum longitude per latitude [default=0]
    std::vector<double> lonmin_;

    /// Value of maximum longitude per latitude [default=0]
    std::vector<double> lonmax_;

    /// Value of longitude increment
    std::vector<double> lon_inc_;

};


#define CONFIG eckit::Parametrisation
extern "C"
{
    void atlas__grid__Structured__delete(Structured* This);
    Structured* atlas__grid__Structured(char* identifier);
    Structured* atlas__grid__Structured__config(CONFIG* conf);
    Structured* atlas__grid__CustomStructured_int(size_t nlat, double lat[], int nlon[]);
    Structured* atlas__grid__CustomStructured_long(size_t nlat, double lat[], long nlon[]);
    Structured* atlas__grid__CustomStructured_lonmin_lonmax_int(size_t nlat, double lat[], int nlon[], double lonmin[], double lonmax[]);
    Structured* atlas__grid__CustomStructured_lonmin_lonmax_long(size_t nlat, double lat[], long nlon[], double lonmin[], double lonmax[]);
    Structured* atlas__grid__gaussian__RegularGaussian(size_t N);
    Structured* atlas__grid__gaussian__ReducedGaussian_int(size_t N, int nlon[]);
    Structured* atlas__grid__gaussian__ReducedGaussian_long(size_t N, long nlon[]);
    Structured* atlas__grid__lonlat__RegularLonLat(size_t nlon, size_t nlat);
    Structured* atlas__grid__lonlat__ShiftedLonLat(size_t nlon, size_t nlat);
    Structured* atlas__grid__lonlat__ShiftedLon(size_t nlon, size_t nlat);
    Structured* atlas__grid__lonlat__ShiftedLat(size_t nlon, size_t nlat);

    void   atlas__grid__Structured__pl        (Structured* This, const long* &pl, size_t &size);
    size_t atlas__grid__Structured__N         (Structured* This);
    size_t atlas__grid__Structured__nlat      (Structured* This);
    size_t atlas__grid__Structured__nlon      (Structured* This, size_t jlat);
    size_t atlas__grid__Structured__nlonmin   (Structured* This);
    size_t atlas__grid__Structured__nlonmax   (Structured* This);
    size_t atlas__grid__Structured__npts      (Structured* This);
    double atlas__grid__Structured__lat       (Structured* This, size_t jlat);
    double atlas__grid__Structured__lon       (Structured* This, size_t jlat, size_t jlon);
    void   atlas__grid__Structured__lonlat    (Structured* This, size_t jlat, size_t jlon, double crd[]);
    void   atlas__grid__Structured__latitudes (Structured* This, const double* &lats, size_t &size);
    int    atlas__grid__Structured__reduced   (Structured* This);
}
#undef CONFIG


}  // namespace grid
}  // namespace atlas


#endif