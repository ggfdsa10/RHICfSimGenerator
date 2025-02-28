/**
   @file      histogram.hh
   @author    Christian Holm Christensen <cholmcc@gmail.com>
   @date      18 Aug 2021
   @copyright (c) 2021 Christian Holm Christensen. 
   @license   LGPL-3
   @brief     A versatile mutli-dimensional histogram 
   @see       @ref nd_histogram

   A generic histogram in any number of dimensions

   This provide histogramming in any number of dimension using really
   simple classes with minimal overhead (except that sparse histograms
   are not implemented).  The main part of the code is only some 250
   lines of code, with under 200 lines of supplementary code. 

   Scaling of histograms _will not_ be supported.  A scaled histogram
   is no longer a histogram. Instead, retrieve the data and work on that.

   Multiplication, division, and subtraction of histograms _will not_
   be supported.  The product, ratio, or difference of a histogram
   with _any thing_ is no longer a histogram.  Instead, retrieve the
   data and work on that.

   Addition of histograms _will_ be supported.  This corresponds to
   adding more observations to an existing histogram.

   This is available from https://gitlab.com/cholmcc/nd_histogram
*/
#ifndef ND_HISTOGRAM_HISTOGRAM
#define ND_HISTOGRAM_HISTOGRAM
#include <valarray>
#include <array>
#include <numeric>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <vector>

/** 
   @defgroup nd_histogram Generic N-dimensional histogram classes
   
   @see @ref index
*/
/** 
   Namespace for histogramming code 

   @ingroup nd_histogram 
*/
namespace nd_histogram {

  //==================================================================
  /** 
      
     Histogram axis 
      
     Instances of this template defines a class for a one-dimensional
     axis of bins.  These bins can be equidistant (of same width) or
     of variable width.  The instances of this class template defines
     member functions to

     - Look up a bin index (nd_histogram::axis::find_bin)

     - Get the widths (nd_histogram::axis::widths) and centers
       (nd_histogram::axis::centers) of all bins.

     - Get the number of bins (nd_histogram::axis::size).

     Objects of instances of the class template nd_histogram::axis
     can be shared among different uses - once defined the objects are
     immutable.

     @see @ref index

     @ingroup nd_histogram 
  */
  template <typename X=double>
  struct axis {
    /** Value type */
    using x_type=X;
    
    /** Value array */
    using x_array=std::valarray<x_type>;
    
    /** 
	Constructor.  This will create equidistant bins
	
	@param nbins Number of bins 
	@param low   Low edge 
	@param high  High edge 
    */
    axis(size_t nbins, x_type low, x_type high)
      : _bounds(nbins+1) {
      x_type  dx = (high - low) / nbins;
      
      std::iota(std::begin(_bounds),std::end(_bounds),x_type());
      
      _bounds *= dx;
      _bounds += low;
    }
    /** 
	Constructor.  Initialize from an initializer list 
	
	@param bounds bin boundaries 
    */
    axis(std::initializer_list<x_type> bounds)
      : _bounds(bounds) {}
    /** 
	Constructor.  Initialize from an initializer list 
	
	@param bounds bin boundaries 
    */
    axis(const x_array& bounds)
      : _bounds(bounds) {}
    /** 
	Find bin index corresponding to @a x 
	
	@param x  Value to find bin index for 
	
	@return bin index or -1 if out of bounds 
    */
    int find_bin(const x_type x) const {
      if (x == _bounds[size()])
	return size()-1;
      
      auto s = std::begin(_bounds);
      auto e = std::end(_bounds);
      auto i = std::upper_bound(s,e,x);
      if (i == e) return -1;
      
      return std::distance(s,i)-1;
    }
    
    /** Bin widts */
    x_array dx() const {
      size_t n = size();
      return _bounds[std::slice(1,n,1)]-_bounds[std::slice(0,n,1)];
    }
    
    /** Bin centres */
    x_array x() const {
      size_t n = size();
      return (_bounds[std::slice(1,n,1)]+_bounds[std::slice(0,n,1)])/2;
    }
    
    /** Number of bins */
    size_t size() const { return _bounds.size() - 1; }

    /** Bin boundaries */
    const x_array& bounds() const { return _bounds; }

    /** 
	Is other axix compatible? 

	- Absolute tolerance is @f$ 10^{-8}@f$ 
	- Relative tolerance is @f$ 10^{-5}@f$

	@return true if compatible (same bin boundaries)
    */
    template <typename T1>
    bool operator==(const axis<T1>& other) const {
      return std::abs(_bounds - other._bounds).max()
	< (1e-8 + 1e-5 * std::abs(other._bounds).max());
    }
  protected:
    x_array _bounds;
  };


  //==================================================================
  /** 
      A histogram in any dimension. 

      Objects of instances of this class template has a fixed
      dimensionality (second template parameter) and a fixed data type
      (first template parameter).  

      @see @ref index 

      @ingroup nd_histogram
  */
  template <typename T, size_t N>
  struct histogram
  {
    /** Type of sum of weights */
    using w_type = std::valarray<T>;

    /** Type of x values */
    using x_type = std::valarray<std::array<T,N>>;

    /** Axis type */
    using axis_type=axis<T>;
    /** Type of container of references to axes */
    using axes_type = std::array<std::reference_wrapper<const axis_type>,N>;

    /** The number of dimensions */
    const size_t dimension;

    /** 
	Constructor 

	Construct histogram from @f$ N@f$ axis objects 

	@param axes The axis objects 
    */
    template <typename ... Axes>
    histogram(const Axes& ... axes)
      : dimension(N),
	_axes{axes...},
	_w(_size(axes...))
    {
      static_assert(N == sizeof...(axes));
    }
    /** Construct from array of axis objects */
    histogram(const std::array<axis_type,N>& axes)
      : dimension(N),
	_axes{axes},
	_w(_size(_axes))
    {}
    /** No default constructor */
    histogram() = delete;
    
    /** 
	@{ 
	@name Filling in observations 

	@return true if observation was within bounds
    */
    /** 
	Fill in an observersion 

	@param x Observation 
    */
    template <typename ... X>
    bool fill(const X& ... x) {
      static_assert(sizeof...(x) == N);

      int idx = _find_bin(0,x...);
      if (idx < 0) return false;

      _w[idx] += 1;

      return true;
    }
    /** 
	Fill in an observersion 

	@param w Weight
	@param x Observation 

	@return true if observation was within bounds
    */
    template <typename ... X>
    bool wfill(const T& w, const X& ... x) {
      static_assert(sizeof...(x) == N);

      int idx = _find_bin(0,x...);
      if (idx < 0) return false;

      _w[idx] += w;
      return true;
    }
    /** @} */
    
    /** 
	@{ 
	@name Indexing 
    */
    /** 
	Find bin index correspnding to values.  Returns -1 if out of bounds. 

	@param x Values 
	
	@returns Bin index or -1 if out of bounds 
    */
    template <typename ... X>
    int find_bin(const X& ... x) {
      static_assert(sizeof...(x) == N);
      
      return _find_bin(0,x...);
    }

    /** Get real index */
    template <typename ... I>
    int index(const I& ... i) const {
      static_assert(sizeof...(I) == N);

      return _index(0,i...);
    }
    /** 
	Get array of axes references 
    */
    const axes_type& axes() const { return _axes; }
    /** @} */
    
    /** 
	@{ 
	@name Data 
    */
    /** 
	Get bin width @f$\Delta@f$ in each bin
    */
    w_type dx() const {
      w_type ret(1,_w.size());
      size_t stride = 1;
      for (auto ia = _axes.begin(); ia != _axes.end(); ++ia) {
	_aw(ret, stride, ia->get());
	stride *= ia->get().size();
      }

      return ret;
    }

    /** 
	Get bin centre @f$ x@f$ in each bin 
    */
    x_type x() const {
      x_type ret(_w.size());
      size_t stride = 1;
      size_t na     = 0;
      for (auto ia = _axes.begin(); ia != _axes.end(); ++ia, ++na) {
	_ac(ret, na, stride, ia->get());
	stride *= ia->get().size();
      }
      
      return ret;
    }

    /** 
	Get sum of weights 

	@f[\sum_i w_i\quad,@f]

	in each bin.
    */
    const w_type& w() const { return _w;  }

    /** 
	Get 

	@f[ \frac{\mathrm{d}w}{\mathrm{d}x} 
	= \frac{\sum_i w_i}{\Delta}\quad, 
	@f]

	in each bin. That is, the sum of weights divided by the bin widths.
    */
    w_type dwdx() const { return _w / dx(); }
    /** 
	Get uncertainty on @f$ dw/dx@f$ - that is, the square root sum
	of weights divided by the bin widths.
    */
    w_type edwdx() const { return std::sqrt(_w) / dx(); }
    /** @} */

    /** 
	@{ 
	@name Other histograms 
    */
    /** 
	Check if other histogram is compatible 

	@param h Other histogram to check for compatibility 

	@return true if other is compatible with this
    */
    template <typename T1>
    bool is_compatible(const histogram<T1,N>& h) const {
      auto oia = h._axes.begin();
      auto tia = _axes.begin();
      for (; tia != _axes.end(); ++oia, ++tia)
	if (not (tia->get() == oia->get())) return false;
      return true;
    }
    /** 
	Add another histogram to this
	
	- Absolute tolerance is @f$ 10^{-8}@f$ 
	- Relative tolerance is @f$ 10^{-5}@f$
	
	@param other Right-hand side 

	@return Sum of the this and other histogram 

	@throws std::logic_error in case the histograms are not compatible 
    */
    template <typename T1>
    histogram& operator+=(const histogram<T1,N>& other) {
      if (not is_compatible(other))
	throw std::invalid_argument("+=: incompatible histograms");

      _w += other._w;
      return *this;
    }
    /** 
	Compare this to another histogram 
	
	@param other Right-hand side 
	
	@return true if the histograms are compatible and have same
	sum of weights 
    */
    template <typename T1>
    bool operator==(const histogram<T1,N>& other) const {
      if (not is_compatible(other)) return false;

      return std::abs(_w - other._w).max()
	< (1e-8 + 1e-5 * std::abs(other._w).max());
    }
    /** @} */
  protected:
    /** 
	@{ 
	@name Number of bins 
    */
    /** 
      Calculate total size (number of bins) of the histogram using a
      recursive template expansion (terminating template)

      @param axes  Axis objects 

      @returns Number of bins 
    */
    size_t _size() const {
      size_t ret = 1;
      for (auto a : axes) ret *= a.get().size();
      return ret;
    }
    /** 
      Calculate total size (number of bins) of the histogram using a
      recursive template expansion.

      @param a  Axis object 
      @param as Remaining axes objects 

      @returns Number of bins 
    */
    template <typename A, typename ... As>
    size_t _size(const A& a, const As& ... as) const {
      return a.size() * _size(as...);
    }
     /** 
       Calculate total size (number of bins) of the histogram using a
       recursive template expansion (terminating template)
       
       @param a  Axis object 
       
       @returns Number of bins 
     */
    template <typename A>
    size_t _size(const A& a) const { return a.size(); }
    /** @} */

    /** 
	@{
	@name Bin widths 
    */
    /** 
	For a given axis, multiply widths by axis widths 
	
	@param ret    Return array 
	@param stride Stride to use 
	@param a      Axis 
    */
    void _aw(w_type& ret, size_t stride, const axis_type& a) const {
      auto      dx      = a.dx();
      size_t    chunk   = stride * dx.size();
      size_t    repeat  = ret.size() / chunk;

      for (size_t i = 0; i < repeat; i++) {
	size_t off = i * chunk;
	for (size_t j = 0; j < stride; j++) {
	  ret[std::slice(off+j,dx.size(),stride)] *= dx;
	}
      }
    }
    /** @} */

    /** 
	@{
	@name Bin centres
    */
    /** 
	For a given axis, set X field 
	
	@param ret    Return array 
	@param ia     Axis index 
	@param stride Stride to use 
	@param a      Axis 
    */
    void _ac(x_type& ret, size_t ia, size_t stride, const axis_type& a) const {
      auto      x       = a.x();
      size_t    chunk   = stride * x.size();
      size_t    repeat  = ret.size() / chunk;

      for (size_t i = 0; i < repeat; i++) {
	size_t off = i * chunk;
	for (size_t j = 0; j < stride; j++) {
	  for (size_t k = 0; k < x.size(); k++) {
	    ret[off+j+stride*k][ia] = x[k];
	  }
	}
      }
    }
    /** @} */

    /** 
	@{ 
	@name Index from indexes 
    */
    /** 
	Get index correspnding to axes indexes. 

	@param ai   Axes number 
	@param i    First axis index 
	@param is   Remaming axes indexes 
	
	@return Global index 
    */
    template <typename I, typename ... Is>
    int _index(size_t ai, const I& i, const Is& ... is) const {
      if (i < 0 or i >= _axes[ai].get().size()) return -1;
      
      int j = _index(ai+1, is...);
      if (j < 0) return -1;
	
      return _axes[ai].get().size() * j + i;
    }
    /** 
	Get index correspnding to axes indexes. 

	@param ai   Axes number 
	@param i    First axis index 
	
	@return Global index 
    */
    template <typename I>
    int _index(size_t ai, const I& i) const { return i;  }
    /** 
	@{ 
	@name Bin index 
    */
    /** 
	Find bin index of values (recursive template expansion) 

	@param ai   Axes index 
	@param x    Value 
	@param xs   Remaning values 

	@return bin index or -1 if out of bounds 
    */
    template <typename X, typename ... Xs>
    int _find_bin(size_t ai, const X& x, const Xs& ... xs) const {
      int j = _find_bin(ai+1,xs...);
      int k = _axes[ai].get().find_bin(x);
      
      if (j < 0 or k < 0) return -1;

      return _axes[ai].get().size() * j + k;
    }
    /** 
	Find bin index of values (terminal of recursive template
	expansion)

	@param ai   Axes index 
	@param x    Value 

	@return bin index or -1 if out of bounds 
    */
    template <typename X>
    size_t _find_bin(size_t ai, const X& x) const {
      return _axes[ai].get().find_bin(x);
    }
    /** @} */

    /** 
	@{ 
	@name Data members 
    */
    /** Sum of weights in each bin */
    w_type _w;

    /** References to axes objects used */
    axes_type _axes;
    /** @} */
    /** 
	IO structure `io` is a friend 
    */
    friend struct io;
  };

  /** 
      Add to histograms together and return the sum 
      
      @param lhs Left-hand side 
      @param rhs Right-hand side 

      @return Sum of the two histograms 

      @throws std::logic_error in case the histograms are not compatible 

      @ingroup nd_histogram 
  */
  template <typename T, size_t N>
  histogram<T,N> operator+(const histogram<T,N>& lhs,
			   const histogram<T,N>& rhs)
  {
    histogram<T,N> ret(lhs);
    ret += rhs;

    return ret;
  }
  /** 
      Compare two histograms for equality.  Histograms are considered
      equal if and only if

      - The have the same bin boundaries 
      - The have the same sum of weights 
      
      @param lhs Left-hand side 
      @param rhs Right-hand side 

      @return true if equal 
  */
  template <typename T, size_t N>
  bool operator==(const histogram<T,N>& lhs,
		  const histogram<T,N>& rhs)
  {
    return lhs.operator==(rhs);
  }

  //------------------------------------------------------------------
  /** 
      A histogram in one dimension. 
      
      @ingroup nd_histogram 
  */
  template <typename T>
  struct histogram<T,1>
  {
    /** Type of sum of weights */
    using w_type = std::valarray<T>;

    /** Type of x values */
    using x_type = std::valarray<T>;

    /** Axis type */
    using axis_type=axis<T>;

    /** Axes type */
    using axes_type = std::array<std::reference_wrapper<const axis_type>,1>;
    
    /** The number of dimensions */
    const size_t dimension;

    /** 
	Constructor 

	Construct histogram from @f$ N@f$ axis objects 

	@param axis The axis objects 
    */
    histogram(const axis_type& axis)
      : dimension(1),
	_axis(axis),
	_w(axis.size())
    {
    }
    /** No default constructor */
    histogram() = delete;
    
    /** 
	@{ 
	@name Filling in observations 
	
	@return true if observation was within bounds
    */
    /** 
	Fill in an observersion 

	@param x Observation 
    */
    bool fill(const T& x) {
      int idx = find_bin(x);
      if (idx < 0) return false;

      _w[idx] += 1;

      return true;
    }
    /** 
	Fill in an observersion 

	@param w Weight
	@param x Observation 
	
	@return true if observation was within bounds
    */
    template <typename ... X>
    bool wfill(const T& w, const T& x) {
      int idx = find_bin(x);
      if (idx < 0) return false;
      
      _w[idx] += w;
      return true;
    }
    /** @} */
    
    /** 
	@{ 
	@name Indexing 
    */
    /** 
	Find bin index correspnding to values.  Returns -1 if out of bounds. 

	@param x Values 
	
	@returns Bin index or -1 if out of bounds 
    */
    int find_bin(const T& x)  const { return _axis.find_bin(x); }

    /** Get real index */
    int index(const int& i) const { return i < 0 or i >= _axis.size() ? -1 : i;}
    /** 
	Get array of axes references 
    */
    const axes_type axes() const { return {_axis}; }
    /** @} */
    
    /** 
	@{ 
	@name Data 
    */
    /** 
	Get bin width @f$\Delta@f$ in each bin
    */
    w_type dx() const { return _axis.dx(); }

    /** 
	Get bin centre @f$ x@f$ in each bin 
    */
    x_type x() const { return _axis.x(); }

    /** 
	Get sum of weights 

	@f[\sum_i w_i\quad,@f]

	in each bin.
    */
    const w_type& w() const { return _w;  }

    /** 
	Get 

	@f[ \frac{\mathrm{d}w}{\mathrm{d}x} 
	= \frac{\sum_i w_i}{\Delta}\quad, 
	@f]

	in each bin. That is, the sum of weights divided by the bin widths.
    */
    w_type dwdx() const { return _w / dx(); }
    /** 
	Get uncertainty on @f$ dw/dx@f$ - that is, the square root sum
	of weights divided by the bin widths.
    */
    w_type edwdx() const { return std::sqrt(_w) / dx(); }
    /** @} */

    /** 
	@{ 
	@name Other histograms 
    */
    /** 
	Check if other histogram is compatible 

	@param h Other histogram to check for compatibility 

	@return true if other is compatible with this
    */
    template <typename T1>
    bool is_compatible(const histogram<T1,1>& h) const {
      return _axis == h._axis;
    }
    /** 
	Add another histogram to this
	
	@param other Right-hand side 

	@return Sum of the this and other histogram 

	@throws std::logic_error in case the histograms are not compatible 
    */
    template <typename T1>
    histogram& operator+=(const histogram<T1,1>& other) {
      if (not is_compatible(other))
	throw std::invalid_argument("+=: incompatible histograms");

      _w += other._w;
      return *this;
    }
    /** 
	Compare this to another histogram.  

	- Absolute tolerance is @f$ 10^{-8}@f$ 
	- Relative tolerance is @f$ 10^{-5}@f$
	
	@param other Right-hand side 
	
	@return true if the histograms are compatible and have same
	sum of weights 
    */
    template <typename T1>
    bool operator==(const histogram<T,1>& other) {
      if (not is_compatible(other)) return false;

      return std::abs(_w - other._w).max()
	< (1e-8 + 1e-5 * std::abs(other._w).max());
    }
    /** @} */
  protected:
    /** Axis used */
    const axis_type& _axis;
    /** Sum of weights */
    w_type _w;
    /** 
	IO structure `io` is a friend 
    */
    friend struct io;
  };
  //==================================================================
  /** 
      Extract values from histogram and store as constant values. 
  */
  template <typename T, size_t N>
  struct frozen_histogram {
    /** Histogram type */
    using histogram_type=histogram<T,N>;
    /** Weights type */
    using w_type=typename histogram_type::w_type;
    /** X type */
    using x_type=typename histogram_type::x_type;
    /** Size type */
    using n_type=std::valarray<size_t>;

    /** Create frozen histogram for histogram */
    frozen_histogram(const histogram_type& h)
      : x(h.x()),
	dwdx(h.dwdx()),
	edwdx(h.edwdx()),
	n{_sizes(h.axes())}
    {}

    /** Get index from axes indexes */
    template <typename ...I>
    int index(const I& ...i) const { return _index(0,i...); }

    /** 
	@{ 
	@name Data 
    */
    /** X values */
    const x_type x;
    /** dW/dx values */
    const w_type dwdx;
    /** Uncertainty on dW/dx */
    const w_type edwdx;
    /** Axes sizes */
    const n_type n;
    /** @} *

    /** 
	@{ 
	@name Aliases 
    */
    /** Alias for dwdx */
    const w_type& y=dwdx;
    /** Alias for edwdx */
    const w_type& e=edwdx;
    /** @} */
    
  protected:
    /** 
	Extract axes sizes 
    */
    n_type _sizes(const typename histogram_type::axes_type& a) const {
      n_type ret(a.size());
      size_t i = 0;
      for (auto& nn : ret) nn = a[i++].get().size();
      return ret;
    }
    /** 
	Get index correspnding to axes indexes. 

	@param ai   Axes number 
	@param i    First axis index 
	@param is   Remaming axes indexes 
	
	@return Global index 
    */
    template <typename I, typename ...Is>
    int _index(size_t ai, const I& i, const Is& ...is) const {
      if (i < 0 or i >= n[ai]) return -1;

      int j = _index(ai+1, is...);
      if (j < 0) return -1;

      return n[ai] * j + i;
    }
    /** 
	Get index correspnding to axes indexes. 

	@param ai   Axes number 
	@param i    First axis index 
	
	@return Global index 
    */
    template <typename I>
    int _index(size_t ai, const I& i) const { return i; }
  };
    
  //==================================================================
  struct io {
    /** 
	Read in an axis object 

	@param in  Input stream 
	
	@return axis object 
    */
    template <typename T>
    static axis<T> read_axis(std::istream& in) {
      using axis_type = axis<T>;
      using x_array   = typename axis_type::x_array;
      using x_type    = typename axis_type::x_type;
      
      size_t n;
      in >> n;

      x_array a(n);
      std::copy_n(std::istream_iterator<x_type>(in),n,std::begin(a));

      return axis_type(a);
    }
    /** 
	Write out an axis object 

	@param out  Output stream 
	@param a    Axis object 
    */
    template <typename T>
    static void write_axis(std::ostream& out, const axis<T>& a) {
      using axis_type = axis<T>;
      using x_type    = typename axis_type::x_type;

      out << a.bounds().size() << " ";
      std::copy(std::begin(a.bounds()),std::end(a.bounds()),
		std::ostream_iterator<x_type>(out," "));
    }
    /** 
	Read in a histogram 
	
	@param in Input stream 
	
	@return pair of axes and histogram 
    */
    template <typename T, size_t N, typename ... A>
    static histogram<T,N>
    read_histogram(std::istream& in, const A& ... axes) {
      using histogram_type = histogram<T,N>;

      histogram_type hist(axes...);

      auto&  w = hist._w;
      size_t n = w.size();
      std::copy_n(std::istream_iterator<T>(in),n,std::begin(w));

      return hist;
    }
    /** 
	Write histogram to stream 
	
	@param out Output stream
	@param h Histogram to write 
    */
    template <typename T, size_t N>
    static void write_histogram(std::ostream& out, const histogram<T,N>& h) {
      auto  axes           = h.axes();

      for (auto a : axes) write_axis(out, a.get());
      std::copy(std::begin(h.w()),std::end(h.w()),
		std::ostream_iterator<T>(out," "));
    }
  };

  //==================================================================
  /**
     Wrap double for fixed precision and width when streaming
  */
  template <size_t width,
	    size_t precision,
	    std::ios_base::fmtflags fmt=std::ios_base::fixed>
  struct wdouble {
    /** 
	Construct from a value 
	
	@param x Value 
    */
    wdouble(double x) : _x(x) {};
    /** The value */
    double _x;
  };
  //------------------------------------------------------------------
  /**
     Stream out wrapped double
     
     @param o  Output stream 
     @param x  Value 
     
     @return @a o 
  */
  template <size_t width,
	    size_t precision,
	    std::ios_base::fmtflags fmt>
  std::ostream& operator<<(std::ostream& o,
			   const wdouble<width,precision,fmt>& x) {
    auto op = o.precision(precision);
    auto of = o.setf(fmt,std::ios_base::floatfield);
    o << std::setw(width) << x._x;
    o.precision(op);
    o.setf(of,std::ios_base::floatfield);
    return o;
  }
  
  //==================================================================
  /** 
      Base class template of ASCII plotter 
  */
  template <typename T, size_t N>
  struct ascii_plotter;

  //------------------------------------------------------------------
  /** 
      Class template specialisation of ASCII plotter for 1D histograms 
  */
  template <typename T>
  struct ascii_plotter<T,1>
  {
    /** 
	Plot a histogram as ASCII art. 
	
	@tparam XWidth Width of independent variable values 
	@tparam XPrec  Precision of independent variable values 
	@tparam XFmt   Format of independent variable values 
	@tparam YWidth Width of dependent variable values 
	@tparam YPrec  Precision of dependent variable values 
	@tparam YFmt   Format of dependent variable values 

	@param o     Output stream 
	@param f     Frozen histogram to plot 
	@param width Maximum width of histogram bars 
    */
    template <size_t XWidth=4, size_t XPrec=1, 
	      std::ios_base::fmtflags XFmt=std::ios_base::fixed,
	      size_t YWidth=8, size_t YPrec=3, 
	      std::ios_base::fmtflags YFmt=std::ios_base::fixed>
    static void plot(std::ostream& o,
		     const frozen_histogram<T,1>& f,
		     size_t width=50)
    {
      auto fact  = width/f.dwdx.max();

      for (size_t i = 0; i < f.x.size(); i++) {
	size_t estar = std::max(size_t(fact*f.edwdx[i]),0ul);
	size_t nstar = size_t(fact*f.dwdx[i]) - estar;
	o << std::setw(3) << i << " "
	  << wdouble<XWidth,XPrec,XFmt>(f.x[i]) << ": "
	  << wdouble<YWidth,YPrec,YFmt>(f.dwdx[i]) << " +/- "
	  << wdouble<YWidth,YPrec,YFmt>(f.edwdx[i]) << " "
	  << std::string(nstar,'*')
	  << std::string(estar,'-')
	  << std::endl;
      }
    }
    /** 
	Plot a histogram as ASCII art. 
	
	@tparam XWidth Width of independent variable values 
	@tparam XPrec  Precision of independent variable values 
	@tparam XFmt   Format of independent variable values 
	@tparam YWidth Width of dependent variable values 
	@tparam YPrec  Precision of dependent variable values 
	@tparam YFmt   Format of dependent variable values 

	@param o     Output stream 
	@param f     Histogram to plot 
	@param width Maximum width of histogram bars 
    */
    template <size_t XWidth=4, size_t XPrec=1, 
	      std::ios_base::fmtflags XFmt=std::ios_base::fixed,
	      size_t YWidth=8, size_t YPrec=3, 
	      std::ios_base::fmtflags YFmt=std::ios_base::fixed>
    static void plot(std::ostream& o,
		     const histogram<T,1>& h,
		     size_t width=50)
    {
      frozen_histogram<T,1> f(h);
      plot<XWidth,XPrec,XFmt,YWidth,YPrec,YFmt>(o, f, width);
    }
  };
  
  //==================================================================
  /** 
      @example example1d.cc  Example of one-dimensional histogram 

      This example shows the basic API, and the one-dimensional
      specialisation.

      We generate samples of a random variable 

      @f[ X\sim\mathcal{N}[0,1]\quad,@f] 

      and weights  

      @f[ W\sim\mathcal{U}[0,1]\quad,@f] 

      and histogram the sample with the weights given.  
  */
  //------------------------------------------------------------------
  /** 
      @example example2d.cc  Example of two-dimensional histogram 

      This example shows the basic API.

      We generate samples of a random variables 

      @f{align*}{
      X&\sim\mathcal{N}[0,1]\cr
      Y&\sim\mathcal{E}[1]\quad,
      @f} 

      and histogram the sample.  We print out the data of the histogram. 
  */
  //------------------------------------------------------------------
  /** 
      @test Test of 2D histogram 
      @example test2d.cc Test of 2D histogram 

      Tests calculation of bin widths, centres, indexes, and so on.
  */
  //------------------------------------------------------------------
  /** 
      @test Test of 3D histogram 
      @example test3d.cc Test of 3D histogram 

      Tests calculation of bin widths, centres, indexes, and so on.
  */
  //------------------------------------------------------------------
  /** 
      @test Test of adding histogram 
      @example testadd.cc Test of adding histogram 

      Tests addition of histograms. 
  */
  //------------------------------------------------------------------
  /** 
      @test Test of I/O
      @example testio.cc Test of I/O

      Tests of I/O
  */
}
#endif
// Local Variables:
//   compile-command: "g++ -c -std=c++11 histogram.hh"
// End:
