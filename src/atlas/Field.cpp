// (C) Copyright 1996-2014 ECMWF.

#include <typeinfo>       // std::bad_cast
#include <sstream>
#include <stdexcept>

#include "atlas/Field.hpp"
#include "atlas/FunctionSpace.hpp"

namespace atlas {

Field::Field(const std::string& name, const int nb_vars, FunctionSpace& function_space) :
    name_(name), nb_vars_(nb_vars), function_space_(function_space)
{
}

template <>
std::vector< int >& Field::data<int>()
{
  try {
    return dynamic_cast< FieldT<int>& >(*this).data();
  } 
  catch (std::bad_cast& e) {
    std::stringstream msg;
    msg << "Could not cast Field " << name() 
        << " with data_type " << data_type() << " to int32";
    throw std::runtime_error(msg.str());
  }
}

template <>
std::vector< float >& Field::data<float>()
{
  try {
    return dynamic_cast< FieldT<float>& >(*this).data();
  } 
  catch (std::bad_cast& e) {
    std::stringstream msg;
    msg << "Could not cast Field " << name() 
        << " with data_type " << data_type() << " to real32";
    throw std::runtime_error(msg.str());
  }
}

template <>
std::vector< double >& Field::data<double>()
{
  try {
    return dynamic_cast< FieldT<double>& >(*this).data();
  } 
  catch (std::bad_cast& e) {
    std::stringstream msg;
    msg << "Could not cast Field " << name() 
        << " with data_type " << data_type() << " to real64";
    throw std::runtime_error(msg.str());
  }
}

template<>
std::string FieldT<int>::data_type() const { return "int32"; }
template<>
std::string FieldT<float>::data_type() const { return "real32"; }
template<>
std::string FieldT<double>::data_type() const { return "real64"; }

template<>
void FieldT<int>::halo_exchange() { function_space().halo_exchange(data_.data(),data_.size()); }
template<>
void FieldT<float>::halo_exchange() { function_space().halo_exchange(data_.data(),data_.size()); }
template<>
void FieldT<double>::halo_exchange() { function_space().halo_exchange(data_.data(),data_.size()); }


// ------------------------------------------------------------------
// C wrapper interfaces to C++ routines

const char* atlas__Field__name (Field* This)
{
  return This->name().c_str();
}

const char* atlas__Field__data_type (Field* This)
{
  return This->data_type().c_str();
}

int atlas__Field__nb_vars (Field* This)
{
  return This->nb_vars();
}

Metadata* atlas__Field__metadata (Field* This)
{
  return &This->metadata();
}

FunctionSpace* atlas__Field__function_space (Field* This)
{
  return &This->function_space();
}


void atlas__Field__data_double (Field* This, double* &field_data, int* &field_bounds, int &rank)
{
  field_data = &This->data<double>()[0];
  field_bounds = const_cast<int*>(&(This->bounds()[0]));
  rank = This->bounds().size();
}

void atlas__Field__data_float (Field* This, float* &field_data, int* &field_bounds, int &rank)
{
  field_data = &This->data<float>()[0];
  field_bounds = const_cast<int*>(&(This->bounds()[0]));
  rank = This->bounds().size();
}

void atlas__Field__data_int (Field* This, int* &field_data, int* &field_bounds, int &rank)
{
  field_data = &This->data<int>()[0];
  field_bounds = const_cast<int*>(&(This->bounds()[0]));
  rank = This->bounds().size();
}

// ------------------------------------------------------------------

} // namespace atlas

