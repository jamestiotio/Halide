#ifndef HALIDE_PARAMETER_H
#define HALIDE_PARAMETER_H

/** \file
 * Defines the internal representation of parameters to halide piplines
 */

#include <string>

namespace Halide {
namespace Internal {

class ParameterContents;

/** A reference-counted handle to a parameter to a halide
 * pipeline. May be a scalar parameter or a buffer */
class Parameter {
    IntrusivePtr<ParameterContents> contents;

    void check_defined() const;
    void check_is_buffer() const;
    void check_is_scalar() const;
    void check_dim_ok(int dim) const;

public:
    /** Construct a new undefined handle */
    Parameter() : contents(NULL) {}

    /** Construct a new parameter of the given type. If the second
     * argument is true, this is a buffer parameter, otherwise, it is
     * a scalar parameter. The parameter will be given a unique
     * auto-generated name. */
    Parameter(Type t, bool is_buffer);

    /** Construct a new parameter of the given type with name given by
     * the third argument. If the second argument is true, this is a
     * buffer parameter, otherwise, it is a scalar parameter. The
     * parameter will be given a unique auto-generated name. */
    Parameter(Type t, bool is_buffer, const std::string &name);

    /** Get the type of this parameter */
    EXPORT Type type() const;

    /** Get the name of this parameter */
    EXPORT const std::string &name() const;

    /** Does this parameter refer to a buffer/image? */
    EXPORT bool is_buffer() const;

    /** If the parameter is a scalar parameter, get its currently
     * bound value. Only relevant when jitting */
    template<typename T>
    NO_INLINE T get_scalar() {
        user_assert(type() == type_of<T>())
            << "Can't get Param<" << type()
            << "> as scalar of type " << type_of<T>() << "\n";
        return *((T *)(get_scalar_address()));
    }

    /** If the parameter is a scalar parameter, set its current
     * value. Only relevant when jitting */
    template<typename T>
    NO_INLINE void set_scalar(T val) {
        user_assert(type() == type_of<T>())
            << "Can't set Param<" << type()
            << "> to scalar of type " << type_of<T>() << "\n";
        *((T *)(get_scalar_address())) = val;
    }

    /** If the parameter is a buffer parameter, get its currently
     * bound buffer. Only relevant when jitting */
    Buffer get_buffer() const;

    /** If the parameter is a buffer parameter, set its current
     * value. Only relevant when jitting */
    void set_buffer(Buffer b);

    /** Get the pointer to the current value of the scalar
     * parameter. For a given parameter, this address will never
     * change. Only relevant when jitting. */
    EXPORT const void *get_scalar_address() const;

    /** Tests if this handle is the same as another handle */
    bool same_as(const Parameter &other) const;

    /** Tests if this handle is non-NULL */
    EXPORT bool defined() const;

    /** Get and set constraints for the min, extent, and stride (see
     * ImageParam::set_extent) */
    //@{
    void set_min_constraint(int dim, Expr e);
    void set_extent_constraint(int dim, Expr e);
    void set_stride_constraint(int dim, Expr e);
    Expr min_constraint(int dim) const;
    Expr extent_constraint(int dim) const;
    Expr stride_constraint(int dim) const;
    //@}

    /** Get and set constraints for scalar parameters. These are used
     * directly by Param, so they must be exported. */
    // @{
    EXPORT void set_min_value(Expr e);
    EXPORT Expr get_min_value();
    EXPORT void set_max_value(Expr e);
    EXPORT Expr get_max_value();
    // @}
};

/** Validate arguments to a call to a func, image or imageparam. */
void check_call_arg_types(const std::string &name, std::vector<Expr> *args, int dims);

}
}

#endif
