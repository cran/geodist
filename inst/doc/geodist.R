## ----pkg-load, echo = FALSE, message = FALSE, eval = FALSE--------------------
#  devtools::load_all (".", export_all = FALSE)

## ----intro, eval = FALSE------------------------------------------------------
#  n <- 50
#  x <- cbind (-10 + 20 * runif (n), -10 + 20 * runif (n))
#  y <- cbind (-10 + 20 * runif (2 * n), -10 + 20 * runif (2 * n))
#  colnames (x) <- colnames (y) <- c ("x", "y")
#  d0 <- geodist (x) # A 50-by-50 matrix
#  d1 <- geodist (x, y) # A 50-by-100 matrix
#  d2 <- geodist (x, sequential = TRUE) # Vector of length 49
#  d2 <- geodist (x, sequential = TRUE, pad = TRUE) # Vector of length 50

## ----tibble, eval = FALSE-----------------------------------------------------
#  n <- 1e1
#  x <- tibble::tibble (
#      x = -180 + 360 * runif (n),
#      y = -90 + 180 * runif (n)
#  )
#  dim (geodist (x))
#  #> [1] 10 10
#  y <- tibble::tibble (
#      x = -180 + 360 * runif (2 * n),
#      y = -90 + 180 * runif (2 * n)
#  )
#  dim (geodist (x, y))
#  #> [1] 10 20
#  x <- cbind (
#      -180 + 360 * runif (n),
#      -90 + 100 * runif (n),
#      seq (n), runif (n)
#  )
#  colnames (x) <- c ("lon", "lat", "a", "b")
#  dim (geodist (x))
#  #> [1] 10 10

## ----geodist_benchmark, eval = FALSE------------------------------------------
#  geodist_benchmark (lat = 30, d = 1000)
#  #>            haversine    vincenty       cheap
#  #> absolute 0.836551561 0.836551562 0.594188257
#  #> relative 0.002155514 0.002155514 0.001616718

## ----plot, eval = FALSE, echo = FALSE-----------------------------------------
#  lat <- 30
#  d <- 10^(1:35 / 5) # 1m to 100 km
#  y <- lapply (d, function (i) geodist_benchmark (lat = lat, d = i))
#  yabs <- do.call (rbind, lapply (y, function (i) i [1, ]))
#  yrel <- 100 * do.call (rbind, lapply (y, function (i) i [2, ]))
#  
#  yvals <- list (yabs, yrel)
#  cols <- c ("skyblue", "lawngreen", "tomato")
#  par (mfrow = c (1, 2))
#  ylabs <- c ("Absolute error (m)", "Relative error (%)")
#  ylims <- list (range (yvals [[1]]), c (min (yvals [[2]]), 1))
#  for (i in 1:2)
#  {
#      plot (NULL, NULL,
#          xlim = range (d / 1000), ylim = ylims [[i]],
#          bty = "l", log = "xy", xaxt = "n", yaxt = "n",
#          xlab = "distance (km)", ylab = ylabs [i]
#      )
#      axis (d / 1000,
#          side = 1, at = c (0.001, 0.1, 10, 1e3, 1e4),
#          labels = c ("0.001", "0.1", "10", "1000", "")
#      )
#      if (i == 1) {
#          yl <- 10^(-3:5)
#          axis (yvals [[i]],
#              side = 2, at = c (0.001, 0.1, 10, 100, 10000),
#              labels = c ("0.001", "0.1", "10", "100", "1000")
#          )
#      } else {
#          yl <- c (0.1, 0.2, 0.3, 0.4, 0.5, 1, 2)
#          axis (yvals [[i]],
#              side = 2, at = yl,
#              labels = c ("0.1", "0.2", "0.3", "0.4", "0.5", "1", "2")
#          )
#      }
#      junk <- sapply (yl, function (j) {
#          lines (range (d / 1000), rep (j, 2),
#              col = "grey", lty = 2
#          )
#      })
#  
#      xl <- 10^(-3:6)
#      junk <- sapply (xl, function (j) {
#          lines (rep (j, 2), range (yvals [[i]]),
#              col = "grey", lty = 2
#          )
#      })
#  
#      for (j in 1:3) {
#          lines (d / 1000, yvals [[i]] [, j], col = cols [j])
#      }
#      legend ("topleft",
#          lwd = 1, col = cols, bty = "n",
#          legend = colnames (yvals [[i]])
#      )
#  }

## ----benchmark-measures, eval = FALSE-----------------------------------------
#  n <- 1e3
#  dx <- dy <- 0.01
#  x <- cbind (-100 + dx * runif (n), 20 + dy * runif (n))
#  y <- cbind (-100 + dx * runif (2 * n), 20 + dy * runif (2 * n))
#  colnames (x) <- colnames (y) <- c ("x", "y")
#  rbenchmark::benchmark (
#      replications = 10, order = "test",
#      d1 <- geodist (x, measure = "cheap"),
#      d2 <- geodist (x, measure = "haversine"),
#      d3 <- geodist (x, measure = "vincenty"),
#      d4 <- geodist (x, measure = "geodesic")
#  ) [, 1:4]
#  #>                                      test replications elapsed relative
#  #> 1     d1 <- geodist(x, measure = "cheap")           10   0.058    1.000
#  #> 2 d2 <- geodist(x, measure = "haversine")           10   0.185    3.190
#  #> 3  d3 <- geodist(x, measure = "vincenty")           10   0.276    4.759
#  #> 4  d4 <- geodist(x, measure = "geodesic")           10   3.106   53.552

## ----x_to_sf, eval = FALSE----------------------------------------------------
#  require (magrittr)
#  x_to_sf <- function (x) {
#      sapply (seq (nrow (x)), function (i) {
#          sf::st_point (x [i, ]) %>%
#              sf::st_sfc ()
#      }) %>%
#          sf::st_sfc (crs = 4326)
#  }

## ----benchmark-sf, eval = FALSE-----------------------------------------------
#  n <- 1e2
#  x <- cbind (-180 + 360 * runif (n), -90 + 180 * runif (n))
#  colnames (x) <- c ("x", "y")
#  xsf <- x_to_sf (x)
#  sf_dist <- function (x) sf::st_distance (x, x)
#  geo_dist <- function (x) geodist (x, measure = "geodesic")
#  rbenchmark::benchmark (
#      replications = 10, order = "test",
#      sf_dist (xsf),
#      geo_dist (x)
#  ) [, 1:4]
#  #> Linking to GEOS 3.6.2, GDAL 2.3.0, proj.4 5.0.1
#  #>           test replications elapsed relative
#  #> 2  geo_dist(x)           10   0.066    1.000
#  #> 1 sf_dist(xsf)           10   0.210    3.182

## ----benchmark-sf-accuracy, eval = FALSE--------------------------------------
#  ds <- matrix (as.numeric (sf_dist (xsf)), nrow = length (xsf))
#  dg <- geodist (x, measure = "geodesic")
#  formatC (max (abs (ds - dg)), format = "e")
#  #> [1] "7.4506e-09"

## ----echo = FALSE-------------------------------------------------------------
n <- 1e4
x <- cbind (-180 + 360 * runif (n), -90 + 180 * runif (n))
colnames (x) <- c ("x", "y")

## ----sequential, eval = FALSE-------------------------------------------------
#  fgeodist <- function () geodist (x, measure = "vincenty", sequential = TRUE)
#  fgeosph <- function () geosphere::distVincentySphere (x)
#  rbenchmark::benchmark (
#      replications = 10, order = "test",
#      fgeodist (),
#      fgeosph ()
#  ) [, 1:4]
#  #>         test replications elapsed relative
#  #> 1 fgeodist()           10   0.022    1.000
#  #> 2  fgeosph()           10   0.048    2.182

