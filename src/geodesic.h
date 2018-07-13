/**
 * \file geodesic.h
 * \brief API for the geodesic routines in C
 *
 * This an implementation in C of the geodesic algorithms described in
 * - C. F. F. Karney,
 *   <a href="https://doi.org/10.1007/s00190-012-0578-z">
 *   Algorithms for geodesics</a>,
 *   J. Geodesy <b>87</b>, 43--55 (2013);
 *   DOI: <a href="https://doi.org/10.1007/s00190-012-0578-z">
 *   10.1007/s00190-012-0578-z</a>;
 *   addenda: <a href="https://geographiclib.sourceforge.io/geod-addenda.html">
 *   geod-addenda.html</a>.
 * .
 * The principal advantages of these algorithms over previous ones (e.g.,
 * Vincenty, 1975) are
 * - accurate to round off for |<i>f</i>| &lt; 1/50;
 * - the solution of the inverse problem is always found;
 * - differential and integral properties of geodesics are computed.
 *
 * The shortest path between two points on the ellipsoid at (\e lat1, \e
 * lon1) and (\e lat2, \e lon2) is called the geodesic.  Its length is
 * \e s12 and the geodesic from point 1 to point 2 has forward azimuths
 * \e azi1 and \e azi2 at the two end points.
 *
 * Traditionally two geodesic problems are considered:
 * - the direct problem -- given \e lat1, \e lon1, \e s12, and \e azi1,
 *   determine \e lat2, \e lon2, and \e azi2.  This is solved by the function
 *   geod_direct().
 * - the inverse problem -- given \e lat1, \e lon1, and \e lat2, \e lon2,
 *   determine \e s12, \e azi1, and \e azi2.  This is solved by the function
 *   geod_inverse().
 *
 * The ellipsoid is specified by its equatorial radius \e a (typically in
 * meters) and flattening \e f.  The routines are accurate to round off with
 * double precision arithmetic provided that |<i>f</i>| &lt; 1/50; for the
 * WGS84 ellipsoid, the errors are less than 15 nanometers.  (Reasonably
 * accurate results are obtained for |<i>f</i>| &lt; 1/5.)  For a prolate
 * ellipsoid, specify \e f &lt; 0.
 *
 * The routines also calculate several other quantities of interest
 * - \e S12 is the area between the geodesic from point 1 to point 2 and the
 *   equator; i.e., it is the area, measured counter-clockwise, of the
 *   quadrilateral with corners (\e lat1,\e lon1), (0,\e lon1), (0,\e lon2),
 *   and (\e lat2,\e lon2).
 * - \e m12, the reduced length of the geodesic is defined such that if
 *   the initial azimuth is perturbed by \e dazi1 (radians) then the
 *   second point is displaced by \e m12 \e dazi1 in the direction
 *   perpendicular to the geodesic.  On a curved surface the reduced
 *   length obeys a symmetry relation, \e m12 + \e m21 = 0.  On a flat
 *   surface, we have \e m12 = \e s12.
 * - \e M12 and \e M21 are geodesic scales.  If two geodesics are
 *   parallel at point 1 and separated by a small distance \e dt, then
 *   they are separated by a distance \e M12 \e dt at point 2.  \e M21
 *   is defined similarly (with the geodesics being parallel to one
 *   another at point 2).  On a flat surface, we have \e M12 = \e M21
 *   = 1.
 * - \e a12 is the arc length on the auxiliary sphere.  This is a
 *   construct for converting the problem to one in spherical
 *   trigonometry.  \e a12 is measured in degrees.  The spherical arc
 *   length from one equator crossing to the next is always 180&deg;.
 *
 * If points 1, 2, and 3 lie on a single geodesic, then the following
 * addition rules hold:
 * - \e s13 = \e s12 + \e s23
 * - \e a13 = \e a12 + \e a23
 * - \e S13 = \e S12 + \e S23
 * - \e m13 = \e m12 \e M23 + \e m23 \e M21
 * - \e M13 = \e M12 \e M23 &minus; (1 &minus; \e M12 \e M21) \e
 *   m23 / \e m12
 * - \e M31 = \e M32 \e M21 &minus; (1 &minus; \e M23 \e M32) \e
 *   m12 / \e m23
 *
 * The shortest distance returned by the solution of the inverse problem is
 * (obviously) uniquely defined.  However, in a few special cases there are
 * multiple azimuths which yield the same shortest distance.  Here is a
 * catalog of those cases:
 * - \e lat1 = &minus;\e lat2 (with neither point at a pole).  If \e azi1 = \e
 *   azi2, the geodesic is unique.  Otherwise there are two geodesics and the
 *   second one is obtained by setting [\e azi1, \e azi2] &rarr; [\e azi2, \e
 *   azi1], [\e M12, \e M21] &rarr; [\e M21, \e M12], \e S12 &rarr; &minus;\e
 *   S12.  (This occurs when the longitude difference is near &plusmn;180&deg;
 *   for oblate ellipsoids.)
 * - \e lon2 = \e lon1 &plusmn; 180&deg; (with neither point at a pole).  If \e
 *   azi1 = 0&deg; or &plusmn;180&deg;, the geodesic is unique.  Otherwise
 *   there are two geodesics and the second one is obtained by setting [\e
 *   azi1, \e azi2] &rarr; [&minus;\e azi1, &minus;\e azi2], \e S12 &rarr;
 *   &minus;\e S12.  (This occurs when \e lat2 is near &minus;\e lat1 for
 *   prolate ellipsoids.)
 * - Points 1 and 2 at opposite poles.  There are infinitely many geodesics
 *   which can be generated by setting [\e azi1, \e azi2] &rarr; [\e azi1, \e
 *   azi2] + [\e d, &minus;\e d], for arbitrary \e d.  (For spheres, this
 *   prescription applies when points 1 and 2 are antipodal.)
 * - \e s12 = 0 (coincident points).  There are infinitely many geodesics which
 *   can be generated by setting [\e azi1, \e azi2] &rarr; [\e azi1, \e azi2] +
 *   [\e d, \e d], for arbitrary \e d.
 *
 * These routines are a simple transcription of the corresponding C++ classes
 * in <a href="https://geographiclib.sourceforge.io"> GeographicLib</a>.  The
 * "class data" is represented by the structs geod_geodesic, geod_geodesicline,
 * geod_polygon and pointers to these objects are passed as initial arguments
 * to the member functions.  Most of the internal comments have been retained.
 * However, in the process of transcription some documentation has been lost
 * and the documentation for the C++ classes, GeographicLib::Geodesic,
 * GeographicLib::GeodesicLine, and GeographicLib::PolygonAreaT, should be
 * consulted.  The C++ code remains the "reference implementation".  Think
 * twice about restructuring the internals of the C code since this may make
 * porting fixes from the C++ code more difficult.
 *
 * Copyright (c) Charles Karney (2012-2017) <charles@karney.com> and licensed
 * under the MIT/X11 License.  For more information, see
 * https://geographiclib.sourceforge.io/
 *
 * This library was distributed with
 * <a href="../index.html">GeographicLib</a> 1.49.
 **********************************************************************/

#if !defined(GEODESIC_H)
#define GEODESIC_H 1

/**
 * The major version of the geodesic library.  (This tracks the version of
 * GeographicLib.)
 **********************************************************************/
#define GEODESIC_VERSION_MAJOR 1
/**
 * The minor version of the geodesic library.  (This tracks the version of
 * GeographicLib.)
 **********************************************************************/
#define GEODESIC_VERSION_MINOR 49
/**
 * The patch level of the geodesic library.  (This tracks the version of
 * GeographicLib.)
 **********************************************************************/
#define GEODESIC_VERSION_PATCH 0

/**
 * Pack the version components into a single integer.  Users should not rely on
 * this particular packing of the components of the version number; see the
 * documentation for GEODESIC_VERSION, below.
 **********************************************************************/
#define GEODESIC_VERSION_NUM(a,b,c) ((((a) * 10000 + (b)) * 100) + (c))

/**
 * The version of the geodesic library as a single integer, packed as MMmmmmpp
 * where MM is the major version, mmmm is the minor version, and pp is the
 * patch level.  Users should not rely on this particular packing of the
 * components of the version number.  Instead they should use a test such as
 * @code{.c}
   #if GEODESIC_VERSION >= GEODESIC_VERSION_NUM(1,40,0)
   ...
   #endif
 * @endcode
 **********************************************************************/
#define GEODESIC_VERSION \
 GEODESIC_VERSION_NUM(GEODESIC_VERSION_MAJOR, \
                      GEODESIC_VERSION_MINOR, \
                      GEODESIC_VERSION_PATCH)

#if defined(__cplusplus)
extern "C" {
#endif

  /**
   * The struct containing information about the ellipsoid.  This must be
   * initialized by geod_init() before use.
   **********************************************************************/
  struct geod_geodesic {
    double a;                   /**< the equatorial radius */
    double f;                   /**< the flattening */
    /**< @cond SKIP */
    double f1, e2, ep2, n, b, c2, etol2;
    double A3x[6], C3x[15], C4x[21];
    /**< @endcond */
  };

  /**
   * Initialize a geod_geodesic object.
   *
   * @param[out] g a pointer to the object to be initialized.
   * @param[in] a the equatorial radius (meters).
   * @param[in] f the flattening.
   **********************************************************************/
  void geod_init(struct geod_geodesic* g, double a, double f);

  /**
   * Solve the direct geodesic problem.
   *
   * @param[in] g a pointer to the geod_geodesic object specifying the
   *   ellipsoid.
   * @param[in] lat1 latitude of point 1 (degrees).
   * @param[in] lon1 longitude of point 1 (degrees).
   * @param[in] azi1 azimuth at point 1 (degrees).
   * @param[in] s12 distance from point 1 to point 2 (meters); it can be
   *   negative.
   * @param[out] plat2 pointer to the latitude of point 2 (degrees).
   * @param[out] plon2 pointer to the longitude of point 2 (degrees).
   * @param[out] pazi2 pointer to the (forward) azimuth at point 2 (degrees).
   *
   * \e g must have been initialized with a call to geod_init().  \e lat1
   * should be in the range [&minus;90&deg;, 90&deg;].  The values of \e lon2
   * and \e azi2 returned are in the range [&minus;180&deg;, 180&deg;].  Any of
   * the "return" arguments \e plat2, etc., may be replaced by 0, if you do not
   * need some quantities computed.
   *
   * If either point is at a pole, the azimuth is defined by keeping the
   * longitude fixed, writing \e lat = &plusmn;(90&deg; &minus; &epsilon;), and
   * taking the limit &epsilon; &rarr; 0+.  An arc length greater that 180&deg;
   * signifies a geodesic which is not a shortest path.  (For a prolate
   * ellipsoid, an additional condition is necessary for a shortest path: the
   * longitudinal extent must not exceed of 180&deg;.)
   *
   * Example, determine the point 10000 km NE of JFK:
   @code{.c}
   struct geod_geodesic g;
   double lat, lon;
   geod_init(&g, 6378137, 1/298.257223563);
   geod_direct(&g, 40.64, -73.78, 45.0, 10e6, &lat, &lon, 0);
   printf("%.5f %.5f\n", lat, lon);
   @endcode
   **********************************************************************/
  void geod_direct(const struct geod_geodesic* g,
                   double lat1, double lon1, double azi1, double s12,
                   double* plat2, double* plon2, double* pazi2);

  /**
   * The general direct geodesic problem.
   *
   * @param[in] g a pointer to the geod_geodesic object specifying the
   *   ellipsoid.
   * @param[in] lat1 latitude of point 1 (degrees).
   * @param[in] lon1 longitude of point 1 (degrees).
   * @param[in] azi1 azimuth at point 1 (degrees).
   * @param[in] flags bitor'ed combination of geod_flags(); \e flags &
   *   GEOD_ARCMODE determines the meaning of \e s12_a12 and \e flags &
   *   GEOD_LONG_UNROLL "unrolls" \e lon2.
   * @param[in] s12_a12 if \e flags & GEOD_ARCMODE is 0, this is the distance
   *   from point 1 to point 2 (meters); otherwise it is the arc length
   *   from point 1 to point 2 (degrees); it can be negative.
   * @param[out] plat2 pointer to the latitude of point 2 (degrees).
   * @param[out] plon2 pointer to the longitude of point 2 (degrees).
   * @param[out] pazi2 pointer to the (forward) azimuth at point 2 (degrees).
   * @param[out] ps12 pointer to the distance from point 1 to point 2
   *   (meters).
   * @param[out] pm12 pointer to the reduced length of geodesic (meters).
   * @param[out] pM12 pointer to the geodesic scale of point 2 relative to
   *   point 1 (dimensionless).
   * @param[out] pM21 pointer to the geodesic scale of point 1 relative to
   *   point 2 (dimensionless).
   * @param[out] pS12 pointer to the area under the geodesic
   *   (meters<sup>2</sup>).
   * @return \e a12 arc length from point 1 to point 2 (degrees).
   *
   * \e g must have been initialized with a call to geod_init().  \e lat1
   * should be in the range [&minus;90&deg;, 90&deg;].  The function value \e
   * a12 equals \e s12_a12 if \e flags & GEOD_ARCMODE.  Any of the "return"
   * arguments, \e plat2, etc., may be replaced by 0, if you do not need some
   * quantities computed.
   *
   * With \e flags & GEOD_LONG_UNROLL bit set, the longitude is "unrolled" so
   * that the quantity \e lon2 &minus; \e lon1 indicates how many times and in
   * what sense the geodesic encircles the ellipsoid.
   **********************************************************************/
  double geod_gendirect(const struct geod_geodesic* g,
                        double lat1, double lon1, double azi1,
                        unsigned flags, double s12_a12,
                        double* plat2, double* plon2, double* pazi2,
                        double* ps12, double* pm12, double* pM12, double* pM21,
                        double* pS12);

  /**
   * Solve the inverse geodesic problem.
   *
   * @param[in] g a pointer to the geod_geodesic object specifying the
   *   ellipsoid.
   * @param[in] lat1 latitude of point 1 (degrees).
   * @param[in] lon1 longitude of point 1 (degrees).
   * @param[in] lat2 latitude of point 2 (degrees).
   * @param[in] lon2 longitude of point 2 (degrees).
   * @param[out] ps12 pointer to the distance from point 1 to point 2
   *   (meters).
   * @param[out] pazi1 pointer to the azimuth at point 1 (degrees).
   * @param[out] pazi2 pointer to the (forward) azimuth at point 2 (degrees).
   *
   * \e g must have been initialized with a call to geod_init().  \e lat1 and
   * \e lat2 should be in the range [&minus;90&deg;, 90&deg;].  The values of
   * \e azi1 and \e azi2 returned are in the range [&minus;180&deg;, 180&deg;].
   * Any of the "return" arguments, \e ps12, etc., may be replaced by 0, if you
   * do not need some quantities computed.
   *
   * If either point is at a pole, the azimuth is defined by keeping the
   * longitude fixed, writing \e lat = &plusmn;(90&deg; &minus; &epsilon;), and
   * taking the limit &epsilon; &rarr; 0+.
   *
   * The solution to the inverse problem is found using Newton's method.  If
   * this fails to converge (this is very unlikely in geodetic applications
   * but does occur for very eccentric ellipsoids), then the bisection method
   * is used to refine the solution.
   *
   * Example, determine the distance between JFK and Singapore Changi Airport:
   @code{.c}
   struct geod_geodesic g;
   double s12;
   geod_init(&g, 6378137, 1/298.257223563);
   geod_inverse(&g, 40.64, -73.78, 1.36, 103.99, &s12, 0, 0);
   printf("%.3f\n", s12);
   @endcode
   **********************************************************************/
  void geod_inverse(const struct geod_geodesic* g,
                    double lat1, double lon1, double lat2, double lon2,
                    double* ps12, double* pazi1, double* pazi2);

  /**
   * The general inverse geodesic calculation.
   *
   * @param[in] g a pointer to the geod_geodesic object specifying the
   *   ellipsoid.
   * @param[in] lat1 latitude of point 1 (degrees).
   * @param[in] lon1 longitude of point 1 (degrees).
   * @param[in] lat2 latitude of point 2 (degrees).
   * @param[in] lon2 longitude of point 2 (degrees).
   * @param[out] ps12 pointer to the distance from point 1 to point 2
   *  (meters).
   * @param[out] pazi1 pointer to the azimuth at point 1 (degrees).
   * @param[out] pazi2 pointer to the (forward) azimuth at point 2 (degrees).
   * @param[out] pm12 pointer to the reduced length of geodesic (meters).
   * @param[out] pM12 pointer to the geodesic scale of point 2 relative to
   *   point 1 (dimensionless).
   * @param[out] pM21 pointer to the geodesic scale of point 1 relative to
   *   point 2 (dimensionless).
   * @param[out] pS12 pointer to the area under the geodesic
   *   (meters<sup>2</sup>).
   * @return \e a12 arc length from point 1 to point 2 (degrees).
   *
   * \e g must have been initialized with a call to geod_init().  \e lat1 and
   * \e lat2 should be in the range [&minus;90&deg;, 90&deg;].  Any of the
   * "return" arguments \e ps12, etc., may be replaced by 0, if you do not need
   * some quantities computed.
   **********************************************************************/
  double geod_geninverse(const struct geod_geodesic* g,
                         double lat1, double lon1, double lat2, double lon2,
                         double* ps12, double* pazi1, double* pazi2,
                         double* pm12, double* pM12, double* pM21,
                         double* pS12);

  /**
   * mask values for the \e caps argument to geod_lineinit().
   **********************************************************************/
  enum geod_mask {
    GEOD_NONE         = 0U,                    /**< Calculate nothing */
    GEOD_LATITUDE     = 1U<<7  | 0U,           /**< Calculate latitude */
    GEOD_LONGITUDE    = 1U<<8  | 1U<<3,        /**< Calculate longitude */
    GEOD_AZIMUTH      = 1U<<9  | 0U,           /**< Calculate azimuth */
    GEOD_DISTANCE     = 1U<<10 | 1U<<0,        /**< Calculate distance */
    GEOD_DISTANCE_IN  = 1U<<11 | 1U<<0 | 1U<<1,/**< Allow distance as input  */
    GEOD_REDUCEDLENGTH= 1U<<12 | 1U<<0 | 1U<<2,/**< Calculate reduced length */
    GEOD_GEODESICSCALE= 1U<<13 | 1U<<0 | 1U<<2,/**< Calculate geodesic scale */
    GEOD_AREA         = 1U<<14 | 1U<<4,        /**< Calculate reduced length */
    GEOD_ALL          = 0x7F80U| 0x1FU         /**< Calculate everything */
  };

  /**
   * flag values for the \e flags argument to geod_gendirect() and
   * geod_genposition()
   **********************************************************************/
  enum geod_flags {
    GEOD_NOFLAGS      = 0U,     /**< No flags */
    GEOD_ARCMODE      = 1U<<0,  /**< Position given in terms of arc distance */
    GEOD_LONG_UNROLL  = 1U<<15  /**< Unroll the longitude */
  };

#if defined(__cplusplus)
}
#endif

#endif
