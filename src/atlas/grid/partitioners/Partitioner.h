/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef atlas_Partitioner_h
#define atlas_Partitioner_h

#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"

namespace atlas {
namespace grid {
class Grid;
}
namespace grid {
class GridDistribution;
}
}

namespace atlas {
namespace grid {
namespace partitioners {

class Partitioner : public eckit::Owned {
  public:

    typedef eckit::SharedPtr<Partitioner> Ptr;

  public:

    Partitioner(const grid::Grid& grid);
    Partitioner(const grid::Grid& grid, const size_t nb_partitions);
    virtual ~Partitioner();

    virtual void partition( int part[] ) const = 0;

    virtual GridDistribution* distribution() const;

  public:

    size_t nb_partitions() const;
    const grid::Grid& grid() const {
        return grid_;
    }

  private:

    size_t nb_partitions_;
    const grid::Grid& grid_;
};


// ------------------------------------------------------------------

class PartitionerFactory {
  public:
    /*!
     * \brief build Partitioner with factory key, constructor arguments
     * \return Partitioner
     */
    static Partitioner* build(const std::string&, const grid::Grid& grid);
    static Partitioner* build(const std::string&, const grid::Grid& grid, const size_t nb_partitions);

    /*!
     * \brief list all registered field creators
     */
    static void list(std::ostream &);
    static bool has(const std::string& name);

  private:
    std::string name_;
    virtual Partitioner* make(const grid::Grid& grid) = 0 ;
    virtual Partitioner* make(const grid::Grid& grid, const size_t nb_partitions) = 0 ;
  protected:

    PartitionerFactory(const std::string&);
    virtual ~PartitionerFactory();
};

// ------------------------------------------------------------------

template<class T>
class PartitionerBuilder : public PartitionerFactory {
    virtual Partitioner* make(const grid::Grid& grid) {
        return new T(grid);
    }
    virtual Partitioner* make(const grid::Grid& grid, const size_t nb_partitions) {
        return new T(grid, nb_partitions);
    }
  public:
    PartitionerBuilder(const std::string& name) : PartitionerFactory(name) {}
};

// ------------------------------------------------------------------

} // namespace partitioners
} // namespace grid
} // namespace atlas

#endif // atlas_Partitioner_h