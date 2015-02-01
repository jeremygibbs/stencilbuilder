#define restrict __restrict__

namespace StencilBuilder
{
  typedef const int UnitVec[3];
  constexpr UnitVec ivec = {1, 0, 0};
  constexpr UnitVec jvec = {0, 1, 0};
  constexpr UnitVec kvec = {0, 0, 1};

  struct Grid
  {
    Grid(const int itot, const int jtot, const int ktot, const int gc) :
      itot(itot), jtot(jtot), ktot(ktot), gc(gc),
      ntot((itot+2*gc)*(jtot+2*gc)*(ktot+2*gc)),
      istart(gc),
      jstart(gc),
      kstart(gc),
      iend(itot+gc),
      jend(jtot+gc),
      kend(ktot+gc),
      icells(itot+2*gc),
      ijcells((itot+2*gc)*(jtot+2*gc)) {}

    const int itot;
    const int jtot;
    const int ktot;
    const int gc;
    const int ntot;
    const int istart;
    const int jstart;
    const int kstart;
    const int iend;
    const int jend;
    const int kend;
    const int icells;
    const int ijcells;
  };

  // STENCIL OPERATORS.
  // Fourth order interpolation.
  struct Interp
  {
    static inline const double apply(const double a, const double b, const double c, const double d)
    { return (9./16.)*(b+c) - (1./16.)*(a+d); }
  };

  // Fourth order gradient.
  struct Grad
  {
    static inline const double apply(const double a, const double b, const double c, const double d)
    { return (27./24.)*(c-b) - (1./24)*(d-a); }
  };

  // STENCIL NODE CLASS
  // Stencil node in expression tree.
  template<int loc, class Inner, class Op, UnitVec vec>
  struct Stencil
  {
    Stencil(const Inner& inner) : inner_(inner) {}

    const Inner& inner_;

    inline const double operator()(const int i, const int j, const int k) const
    {
      return Op::apply(inner_(i + vec[0]*(-2+loc), j + vec[1]*(-2+loc), k + vec[2]*(-2+loc)),
                       inner_(i + vec[0]*(-1+loc), j + vec[1]*(-1+loc), k + vec[2]*(-1+loc)),
                       inner_(i + vec[0]*(   loc), j + vec[1]*(   loc), k + vec[2]*(   loc)),
                       inner_(i + vec[0]*( 1+loc), j + vec[1]*( 1+loc), k + vec[2]*( 1+loc)));
    }
  };

  // Stencil generation operator for interpolation.
  template<class Inner>
  inline Stencil<1, Inner, Interp, ivec> Ix(const Inner& inner)
  { return Stencil<1, Inner, Interp, ivec>(inner); }

  template<class Inner>
  inline Stencil<1, Inner, Interp, jvec> Iy(const Inner& inner)
  { return Stencil<1, Inner, Interp, jvec>(inner); }

  template<class Inner>
  inline Stencil<1, Inner, Interp, kvec> Iz(const Inner& inner)
  { return Stencil<1, Inner, Interp, kvec>(inner); }

  template<class Inner>
  inline Stencil<0, Inner, Interp, ivec> Ix_h(const Inner& inner)
  { return Stencil<0, Inner, Interp, ivec>(inner); }

  template<class Inner>
  inline Stencil<0, Inner, Interp, jvec> Iy_h(const Inner& inner)
  { return Stencil<0, Inner, Interp, jvec>(inner); }

  template<class Inner>
  inline Stencil<0, Inner, Interp, kvec> Iz_h(const Inner& inner)
  { return Stencil<0, Inner, Interp, kvec>(inner); }

  // Stencil generation operator for gradient.
  template<class Inner>
  inline Stencil<1, Inner, Grad, ivec> Gx(const Inner& inner)
  { return Stencil<1, Inner, Grad, ivec>(inner); }

  template<class Inner>
  inline Stencil<1, Inner, Grad, jvec> Gy(const Inner& inner)
  { return Stencil<1, Inner, Grad, jvec>(inner); }

  template<class Inner>
  inline Stencil<1, Inner, Grad, kvec> Gz(const Inner& inner)
  { return Stencil<1, Inner, Grad, kvec>(inner); }

  template<class Inner>
  inline Stencil<0, Inner, Grad, ivec> Gx_h(const Inner& inner)
  { return Stencil<0, Inner, Grad, ivec>(inner); }

  template<class Inner>
  inline Stencil<0, Inner, Grad, jvec> Gy_h(const Inner& inner)
  { return Stencil<0, Inner, Grad, jvec>(inner); }

  template<class Inner>
  inline Stencil<0, Inner, Grad, kvec> Gz_h(const Inner& inner)
  { return Stencil<0, Inner, Grad, kvec>(inner); }

  // SCALAR OPERATORS
  // Multiplication operator.
  struct Multiply
  {
    static inline const double apply(const double left, const double right) { return left*right; }
  };

  // Addition operator.
  struct Add
  {
    static inline const double apply(const double left, const double right) { return left+right; }
  };

  // OPERATOR NODE CLASS
  // Operator node in expression tree.
  template<class Left, class Op, class Right>
  struct Operator
  {
    Operator(const Left& left, const Right& right) : left_(left), right_(right) {}

    const Left& left_;
    const Right& right_;

    // inline double operator[](const int i) const { return Op::apply(left_[i], right_[i]); }
    inline const double operator()(const int i, const int j, const int k) const
    { return Op::apply(left_(i, j, k), right_(i, j, k)); }
  };

  // Operator aggregation class, specialization for left scalar multiplication
  template<class Op, class Right>
  struct Operator<double, Op, Right>
  {
    Operator(const double& left, const Right& right) : left_(left), right_(right) {}

    const double& left_;
    const Right& right_;

    // inline double operator[](const int i) const { return Op::apply(left_, right_[i]); }
    inline const double operator()(const int i, const int j, const int k) const
    { return Op::apply(left_, right_(i, j, k)); }
  };

  // Template classes for the multiplication operator.
  template<class Left, class Right>
  inline Operator<Left, Multiply, Right> operator*(const Left& left, const Right& right)
  {
    return Operator<Left, Multiply, Right>(left, right);
  }

  // Template classes for the addition operators.
  template<class Left, class Right>
  inline Operator<Left, Add, Right> operator+(const Left& left, const Right& right)
  {
    return Operator<Left, Add, Right>(left, right);
  }

  // Field class representing the field, whose operations expand compile time.
  class Field
  {
    public:
      Field(const Grid& grid) :
        grid_(grid),
        data_(new double[grid_.ntot]) {}

      ~Field() { delete[] data_; }

      inline double& operator[](const int i) { return data_[i]; }
      inline const double operator[](const int i) const { return data_[i]; }

      inline const double operator()(const int i, const int j, const int k) const
      { return data_[i + j*grid_.icells + k*grid_.ijcells]; }

      inline double& operator()(const int i, const int j, const int k)
      { return data_[i + j*grid_.icells + k*grid_.ijcells]; }

      // Assignment operator, this operator starts the inline expansion.
      template<class T>
      inline Field& operator= (const T& restrict expression)
      {
        for (int k=grid_.kstart; k<grid_.kend; ++k)
          for (int j=grid_.jstart; j<grid_.jend; ++j)
            #pragma ivdep
            for (int i=grid_.istart; i<grid_.iend; ++i)
              (*this)(i, j, k) = expression(i, j, k);

        return *this;
      }

      // Overload, NOT specialization, for assignment with a constant.
      inline Field& operator= (const double& restrict expression)
      {
        for (int k=grid_.kstart; k<grid_.kend; ++k)
          for (int j=grid_.jstart; j<grid_.jend; ++j)
            #pragma ivdep
            for (int i=grid_.istart; i<grid_.iend; ++i)
              (*this)(i, j, k) = expression;

        return *this;
      }

      // Compound assignment operator, this operator starts the inline expansion.
      template<class T>
      inline Field& operator+=(const T& restrict expression)
      {
        for (int k=grid_.kstart; k<grid_.kend; ++k)
          for (int j=grid_.jstart; j<grid_.jend; ++j)
            #pragma ivdep
            for (int i=grid_.istart; i<grid_.iend; ++i)
              (*this)(i, j, k) += expression(i, j, k);

        return *this;
      }

    private:
      // Reference to the grid on which the field is created
      const Grid& grid_;
      // Pointer to the raw data.
      double* restrict data_;
  };
}
