define({'Absolute Limits Test':{d:'Assesses whether each variable attains, at least once in the current time series, a value that is outside a user-defined plausible range.',l:''},'Acquisition Frequency':{d:'The number of samples per second (Hz) recorded by the eddy covariance system. Sometimes called Update Rate (in the LI‑7500RS and LI‑7200RS software) or Output Rate (in the LI‑7700 software).',l:''},'Actinic Light':{d:'Electromagnetic radiation that is capable of causing photochemical reactions in chlorophyll.',l:''},'Advanced Mode':{d:'An EddyPro run mode that enables many advanced processing options for atypical sites and comparison of methods.',l:''},'Aggregation':{d:'With respect to eddy flux footprint analysis, it is the sum of probabilities that flux originated a specific area.',l:''},'Altitude':{d:'The elevation above sea level of the site (m).',l:''},'Amplitude Resolution Test':{d:'A statistical test that detects records with weak variance (weak winds and stable conditions), in which the amplitude resolution of the recorded data may not be sufficient to capture the fluctuations, by assessing whether the number of different values each variable takes throughout the time series covers its range of variation with sufficient homogeneity.',l:''},'Angle of Attack':{d:'The angle that the wind vector forms with respect to the horizontal plane of the sonic anemometer (the one defined by the U and V components). It is positive if the vertical wind component is negative, i.e. the wind goes in the downward direction. Angles-of-attack that are different from zero are thought to lead a distortion of the wind field due to interaction with the anemometer’s transducers. Flow distortion corrections address this issue for some anemometer models.',l:''},'Angle of Attack Correction':{d:'Correction for the flow distortion due to the interaction between the wind flow and the anemometer’s structures (e.g., transducers). Corrections are available for post-mounted Gill Instruments sonic anemometer such as R3 and WindMaster. The relevance of this effect of c-clamp anemometer models is debated (Kochendorfer et al. 2012; Mauder, 2013).',l:''},'Angle of Attack Test':{d:'Calculates sample-wise angle of attacks throughout the current flux averaging period, and flags it if the percentage of angles of attack exceeding a user-defined range is beyond a (user-defined) threshold.',l:''},'ASCII File':{d:'A format for raw eddy covariance data. Data are most commonly stored in a column-wise fashion, and columns are usually separated by commas, tabs, semicolons or blank spaces. ASCII files are human-readable using any text editor and can feature a header stating the variable meanings and their units.',l:''},'Auto Control':{d:'A user-settable control algorithm that adjusts instrument settings.',l:''},'Auto Program':{d:'A predefined program that controls the instrument and logs data according to the settings.',l:''},'Axis Rotation for Tilt Correction':{d:'Method for compensating anemometer tilt with respect to local streamlines.',l:''},'Biomet Data':{d:'Shorthand for biological and meteorological data. May include slow, accurate measurements of air properties (temperature, pressure, water content, etc.), radiation components (direct, indirect, reflected, and sorted in spectral bands), soil and vegetation properties (biomass, water content, temperature, water and carbon flux, LAI, etc.). A few of these measurements can be used to improve flux computation, or to calculate other flux contributions such as storage fluxes, advection fluxes, or soil fluxes.',l:''},'Bootstrap':{d:'A statistical technique used to estimate the properties of a statistical estimator.',l:''},'Canopy Height':{d:'The height (m) of the canopy above the ground at eddy covariance site.',l:''},'Computational Grid':{d:'Specifies a geographical area around the sensor platform (tower) over which the footprint computation will be performed. The resolution of the computation can be set as distance in meters or points that make up the grid. The higher the number of points the longer the computational time. The grid must be a square or rectangle.',l:''},'Conductance':{d:'Also known as stomatal conductance, the transfer rate through stomata in the leaf.',l:''},'Covariance Maximization':{d:'Procedure to estimate time lags between two time series. In eddy covariance, it is most commonly used to estimate time lags between vertical wind speed and other variables. It is based on the assumption that any setup limitation (physical separations, sampling lines, etc) reduces the correlation between time series, that would be observed in the environment. In a cross-correlation analysis between vertical wind speed and a variable, the time lag for which the resulting covariance is maximal is taken to be the timelag for that variable. The procedure is performed for all possible time lags in a in a ‘time lag plausibility window’.',l:''},'Coverage':{d:'Represents the completeness of a variable or group of variables on a time series. Gaps in the coverage represent missing data.',l:''},'Cut-off Frequency':{d:'In eddy-covariance, the term is most often used to quantify the spectral features of an eddy flux system. The larger the cut-off frequency, the greater the ability of the system to measure flux contributions occurring at small spatial scales (small eddies). Typical realistic values range from 0.1 to 5-8 Hz. Open-path systems tend to have higher cut-offs than closed-path systems, due to attenuations of high-frequency fluctuations of gas concentrations resulting from the sampling line. Shorter sampling lines mitigate the problem. EddyPro can calculate your system’s cut-off if you don’t know how to estimate it.',l:''},'Desiccant':{d:'A scrubbing chemical that removes water vapor from the air stream.',l:''},'Detrending':{d:'Refers to the operation of establishing and removing a trend in raw (high-frequency) time series before calculating turbulent fluctuations. Using the ‘block average’ procedure, no detrending is performed, and any instantaneous variation with respect to the global mean of a variable (evaluated over the flux averaging interval) is considered as a turbulent fluctuation. On the contrary, ‘linear detrending’ and ‘(exponential) running mean’ methods remove a trend before evaluating fluctuations. With the latter methods, the trend can be modulated to follow the time series more or less closely, by respectively reducing or increasing the method’s time constant.',l:''},'Discontinuities Test':{d:'To detect discontinuities that lead to semi-permanent changes, as opposed to sharp changes associated with smaller-scale fluctuations.',l:''},'Displacement Height':{d:'The height at which the wind speed would go to zero if the logarithmic wind profile was maintained from the outer flow all the way down to the surface (also known as zero plane displacement height).',l:''},'Domain Configuration':{d:'A specified region with an overlaid grid for the footprint\nanalysis.',l:''},'Double Rotation':{d:'Aligns the x-axis of the anemometer to the current mean streamlines, nullifying the vertical and crosswind components.',l:''},'Download Data':{d:'Refers to a permission that is available to Station Administrators and General Users who are assigned a station. For General Users, the permission is made available when a Station Administrator assigns a station to a user. With this permission, users can download a summary file from the flux system to a computer.',l:''},'Drop-outs Test':{d:'Attempts to detect (relatively) short periods in which the time series sticks to some value that is statistically different from the average value calculated over the whole period.',l:''},'Dynamic Metadata File':{d:'For EC datasets not in the GHG format, the dynamic metadata file can be used to provide EddyPro with information concerning time-varying site, station, and instrumentation parameters, such as changing canopy height and roughness length, or instrument separations and height. A dynamic metadata file can also be used with GHG files, in case these do not contain accurate information (e.g. if a changing canopy height was not updated in the GHG software during data collection).',l:''},'Early Match':{d:'An option under Auto Programs that enables the instrument to match IRGA readings as soon as the reference CO2 and H2O concentrations have stabilized. This option can save time under most configurations.',l:''},'Eastward Separation':{d:'East/west distance between the gas analyzer and the reference anemometer; positive values if east and negative values if west of the anemometer.',l:''},'Edit Station Information':{d:'Refers to a permission that is available to Station Administrators and General Users who are assigned a station. For General Users, the permission is made available when a Station Administrator assigns a station to a user. With this permission, users can edit selected details about a flux station.',l:''},'Embedded Biomet File':{d:'File containing Biomet data collected with LI-COR’s biomet system and included (embedded) in the .ghg files. Each .ghg file contains all eddy-covariance and biomet measurements corresponding to a specific time period, and can be automatically processed in EddyPro, to refine computation of turbulent fluxes.',l:''},'Energy Balance':{d:'A method of computing temperature that uses the difference between energy in and energy consumed.',l:''},'Express Mode':{d:'A setting in EddyPro that uses commonly-accepted settings that should apply to most well-designed eddy covariance setups.',l:''},'External Biomet File(s)':{d:'For users of Biomet systems other than the LI-COR system, EddyPro allows the usage of Biomet data in flux computation. Data must be included in one or more external files with a prescribed CSV format, which can then be loaded in EddyPro.',l:''},'External Data':{d:'Data that is provided from nearby weather stations or available in gridded data products available everywhere in the world. In order to load external data, the location must be locked, which happens when you establish the coordinates, either by entering them, or by loading the .metadata file from the EddyPro project and clicking Lock Information.',l:''},'Extinction coefficient in oxygen Ko':{d:'In Krypton or Lyman-α hygrometers, the extinction coefficients for oxygen of the hygrometers, associated with the third-order Taylor expansion of the Lambert–Beer law around reference conditions.',l:''},'Extinction coefficient in water Kw':{d:'In Krypton or Lyman-α hygrometers, the extinction coefficients for water vapor of the hygrometers, associated with the third-order Taylor expansion of the Lambert–Beer law around reference conditions.',l:''},'Fast Fourier Transform':{d:'Fast Fourier Transform (FFT) is used for the frequency domain analysis (spectra and co-spectra) of the time series data.',l:''},'Fluorescence':{d:'The emission of energy as photons. In leaves, this occurs when the leaf has absorbed an excess of energy. The energy is dissipated as a fluorescence emission, to prevent the energy from damaging the leaf.',l:''},'Flux Averaging Interval':{d:'Time span over which fluxes and all other results are calculated. Most typical is 30 minutes, but longer or shorter averaging intervals may be needed in specific conditions (see Finnigan et al. 2003). You can look into the ogives output by EddyPro to determine the most appropriate averaging interval for a given site. The flux averaging interval does not need to match the duration of the collected raw files.',l:''},'Footprint Analysis':{d:'Tovi\u0027s tool to evaulate the probability that a region contributed to the measured flux.',l:''},'Footprint Estimation':{d:'Flux crosswind-integrated footprints are distances from the tower contributing 10%, 30%, 50%, 70% and 90% to measured fluxes.',l:''},'Gain Value':{d:'The gain (slope) of the linear relation between input and output units.',l:''},'General User':{d:'A FluxSuite user who does not have Station Administration or User Administration permissions. A general user may have permissions for specific stations to edit station information, download data, manage users, and manage alerts.',l:''},'Generic Binary File':{d:'Raw eddy covariance data can be efficiently stored in a plain binary format. EddyPro can handle generic binary files (both little and big endian) as long as the length of binary words used to store each variable is constant throughout the file. Binary files are not human-readable and, although they are fast to import for processing, do not constitute a robust way of storing eddy covariance data, unless they are accompanied by appropriate metadata.',l:''},'ghg File':{d:'A custom LI-COR file format that includes complete eddy covariance data sets and is denoted by a .ghg file extension. At a minimum, .ghg files contain a raw, high-frequency data file (the extension is .data) and the paired metadata file (the extension is .metadata). If a biomet system was part of the eddy covariance setup, .ghg files can also contain a file with biomet data (ending in ‘-biomet.data’) and the paired metadata file (ending in ‘-biomet_metadata’). All files are stored in ASCII text format.',l:''},'GHG Software':{d:'The name of the desktop and embedded software on LI‑COR LI‑7500RS and LI‑7200RS; used to log .ghg files.',l:''},'Height':{d:'Under instruments tab in the metadata file editor; the distance between the ground and the sample path of the sensor (m).',l:''},'High-pass Filtering Effects':{d:'Applies a correction for flux spectral losses in the low frequency range, due to the finite averaging time and dependent on the detrending method selected.',l:''},'Ignore':{d:'In the metadata file editor, tells EddyPro to ignore a variable.',l:''},'Input Unit':{d:'Related to re-scaling a variable in the metadata file editor, this specifies the input units.',l:''},'IRGA':{d:'Infrared Gas Analyzer',l:''},'Library':{d:'A collection of data files that have been added to a site in Tovi. Compatible data files include EddyPro output files and SmartFlux summary files.',l:''},'Linear Scaling':{d:'Used to rescale values (from voltage, for example) to meaningful units (such as m/s).',l:''},'Longitudinal Path Length (anemometer)':{d:'Distance between a pair of anemometer transducers.',l:''},'Longitudinal Path Length (gas analyzer)':{d:'The path length of the gas analyzer sample volume.',l:''},'Low-pass Filtering':{d:'Applies a correction for flux spectral losses in the high frequency range.',l:''},'Magnetic Declination':{d:'The angle between magnetic (uncorrected compass) North and geographic (true). This angle changes slightly over time, according to variations of the earth’s magnetic field. Based on Latitude, Longitude and date, EddyPro can retrieve the magnetic declination at your site from the dedicated NOAA web-service.',l:''},'Manage Alerts':{d:'Refers to a permission that is available to Station Administrators or General Users who have permission to assigned to them at the station level. With this permission, users can configure alert thresholds and activate/deactivate alerts.',l:''},'Manage Users':{d:'Refers to a permission that is available to Station Administrators and General Users who are assigned to a station. For General Users, the permission is made available when a Station Administrator assigns a station to a user. With this permission, users can change user information and settings.',l:''},'Master Anemometer':{d:'In an eddy covariance site with two or more anemometers, the master anemometer is the one used in flux calculations.',l:''},'Match':{d:'A procedure that ensures both gas analyzers provide the same reading when at the same CO2 and H2O concentrations.',l:''},'Maximum Time Lag':{d:'The maximum expected time lag for the current variable, with respect to anemometric measurements.',l:''},'Measurment Type':{d:'The description of the concentration measurement (either Molar/Mass density, Mole fraction, or Mixing ratio).',l:''},'Metadata':{d:'Data about a data set.',l:''},'Metadata File':{d:'The metadata file (extension .metadata) contains all meta information relevant to the collected raw data and necessary to process it appropriately. Metadata include site, station and instrumentation description, as well as meaning and units of all variables available in the corresponding data files. For maximal robustness, each EC raw data file should be accompanied by its own metadata, as is the case with GHG files.',l:''},'Minimum Time Lag':{d:'The minimum expected time lag for the current variable, with respect to anemometric measurements.',l:''},'Mixing Fan':{d:'A blower fan that ensures mixing of air in the leaf chamber.',l:''},'Net Photosynthesis':{d:'The mass balance of CO2 in an open system.',l:''},'Nominal Time Lag':{d:'The expected (nominal) time lag of the variable, with respect to the measurements of the sonic anemometer.',l:''},'North Alignment':{d:'For Gill sonic anemometers, this specifies whether the U wind component is aligned to the North spar of the anemometer (the spar marked with an N or a notch) or with the transducer closest to it. There is an offset of 30 degress between the two. For most models, wind components V and W will be perpendicular to U in a right handed coordinate system.',l:''},'North Offset':{d:'The anemometer\u0027s yaw offset with respect to local magnetic North (positive eastward). Magnetic North is the direction in which the north end of a compass needle or other freely suspended magnet will point in response to the earth\u0027s magnetic field.',l:''},'North Reference':{d:'The ‘North’ to which EddyPro should refer the wind direction. It can be magnetic (the direction that the north end of a compass needle points in response to the earth\u0027s magnetic field) or geographic (the direction along the earth\u0027s surface towards the geographic North Pole). The difference between the two is the magnetic declination.',l:''},'Northward Separation':{d:'North/south distance between the gas analyzer and the reference anemometer; positive values if north and negative values if south of the anemometer.',l:''},'Numeric (yes/no)':{d:'Specifies whether a variable is purely numeric or not.',l:''},'Offset':{d:'The offset (y-axis intercept) of the linear relation between input and output units.',l:''},'OJIP':{d:'Origin,  inflection, intermediary peak, and peak; A plot of induction kinetics. Also known as the Kautsky effect.',l:''},'Output Directory':{d:'The directory in the file system where your flux results will be written.',l:''},'Output ID':{d:'An identifier in the name of the output file that specifies the results of a project.',l:''},'Output Unit':{d:'The units of values that are rescaled using the linear scaling feature.',l:''},'Pall Stuttgarter Masse':{d:'Porous ceramic substrate used to hold liquid water, to add water vapor to the air stream.',l:''},'PAR':{d:'Photosynthetically Active Radiation; light that drives photosynthesis reactions. Typically this is a square-shaped waveband with wavelengths between 400 and 700 nm, measured in quanta per unit area per second.',l:''},'Planar Fit':{d:'Aligns the anemometer coordinate system to local streamlines assessed on a long time period (e.g., 2 weeks or more). Can be performed sector-wise, meaning that different rotation angles are calculated for different wind sectors.',l:''},'Planar Fit with no Velocity Bias':{d:'Similar to classic planar fit, but assumes that the any bias in the measurement of vertical wind is compensated, and forces the fitting plane to pass through the origin (that is, such that if average u and v are zero, also average w is zero).',l:''},'PPFD':{d:'Photosynthetic Photon Flux Density, a measure of photosynthetically active radiation.',l:''},'Quality Flagging Region':{d:'Used to define a general area of interest from which the majority of flux contributions are expected to arise. The aggregated footprint contribution probability for this area can be used as a quality screening filter to exclude data when that probability falls below a user-set threshold.',l:''},'Random Uncertainty':{d:'Uncertainty is a quantification of the precision of a measurement. Sources of uncertainties can be traced to biogeochemical (source/sink), transport, and instrument factors. Random uncertainties due to sampling errors are a consequence of the limited number of independent samples that contribute substantially to a flux during any fixed sampling period (Finkelstein and Sims, 2001). Contrary to systematic uncertainties, which introduce a bias into the resulting fluxes and shall be minimized by accurate experimental design and instrument deployment, random uncertainties do not introduce a bias, rather, they reduce our confidence that the reported number is the true value (Billesbach, 2011).',l:''},'Regions of Interest':{d:'A collection of areas where the computed footprint contribution probability is summed to give one value that represents the entire region of interest.',l:''},'Roles':{d:'Roles include Station Administration and User Administration. These are set by users with User Administration permission.',l:''},'Roughness Length':{d:'The height at which wind speed is zero (indicated by z0).',l:''},'Site':{d:'A project in Tovi is referred to as a Site. Think of the site as the geographical location of your tower and a repository of all data collected at that location.',l:''},'Site Information':{d:'Describes characteristics of the site. Site information is provided via the EddyPro .metadata file or entered manually. Additional information that is relevant to the dataset is provided via the .eddypro project file, including error codes. The default list of error codes represents common codes used by EddyPro and recognized by Tovi. Tovi will examine your data for error codes.',l:''},'Skewness and Kurtosis Test':{d:'Excessive skewness and kurtosis may help detect periods of instrument malfunction. Third and fourth order moments are calculated on the whole time series and variables are flagged if their values exceed user-selected thresholds.',l:''},'SLT File':{d:'A format for raw eddy covariance data, often created by or used in conjunction with the EdiRe/EdiSol or EddySoft software packages. It is a fixed-length binary format, with or without a binary header describing the file content. EddyPro can handle both types of SLT files, but you must specify whether it is associated to EdiRe/EdiSol or to EddySoft.',l:''},'Soda Lime':{d:'A scrubbing chemical that removes CO2 from the air stream.',l:''},'Span':{d:'A user calibration that adjusts the slope of the instrument\u0027s reading, to ensure that a non-zero reading corresponds with the actual concentration.',l:''},'Spectral Correction Factor':{d:'Multiplicative factors (typically \u003e 1) used to correct tentative flux estimates for flux losses that can occur at both low and high frequencies. Spectral correction factors are calculated for each flux based on a spectral correction scheme. EddyPro supports different schemes, to suit different EC setups. Depending on the type of gas analyzer deployed, spectral corrections must be applied to fluxes before (open-path) or after (closed-path) consideration of WPL effects.',l:''},'Spectral Corrections':{d:'Procedure to calculate spectral correction factors. In EddyPro, we distinguish between analytic and in-situ methods. Analytic methods do not require analysis of (co)spectral features derived from the data and are easy to implement, fast to perform and robust. However, they may underestimate spectral attenuations, especially for closed-path systems. In-situ methods are more appropriate to closed-path systems, but they require long (e.g., \u003e1 month) datasets to analyze spectra features or raw time series, estimate cut-off frequencies and compute correction factors. In-situ methods, involving non-linear regressions, may also fail in certain conditions. Note: We use the wording “(co)spectra” to refer to both spectra and cospectra.',l:''},'Spike Count/Removal':{d:'A statistical technique that detects the number of spikes in each time series and removes them based upon your settings.',l:''},'Station Administrator':{d:'Station Administrators have permission to add and remove stations and edit information for all stations.',l:''},'Steadiness of Horizontal Wind Test':{d:'Assesses whether the along-wind and crosswind components of the wind vector undergo a systematic reduction (or increase) throughout the file.',l:''},'Stomatal Ratio':{d:'A dimensionless ratio of the estimated stomatal conductances of one side of the leaf to the other.',l:''},'Stuttgarter Masse':{d:'A ceramic substrate that holds liquid water.',l:''},'Time Lag':{d:'In eddy-covariance, the term ‘time lag’ refers to the time misalignment between different high-frequency time series. Time lags arise due to physical distances between instruments, to electronic delays and, in closed-path setups, to the passage of air through sampling lines.',l:''},'Time Lag Compensation':{d:'Procedure to compensate for time lags between anemometric measurements and any other high frequency measurements included in the raw files, notably those used for fluxes. In EddyPro time lags can be ignored, set to a constant value, calculated with the automatic procedure called covariance maximization, or optimized by means of the time lag optimization procedure.',l:''},'Time Lag Optimization':{d:'Time lags of water vapor in closed-path systems often depend on relative humidity (and secondarily on air temperature), on account of enhanced sorption processes occurring at the walls of the sampling line when RH increases. The time lag optimization procedure consists of automatically defining the most appropriate time lag plausibility window and nominal time lag for water vapor for a specified number of relative humidity classes. The time lag optimization can also be used to derive plausibility windows for passive gases, as a special case.',l:''},'Time Lag Plausibility Window':{d:'The range of physically plausible time lags, within which the actual time is expected to fall. Within this window, the ‘nominal time lag’ is the time lag of maximal likelihood, the one that is expected to occur most often. The minimum and maximum of the plausibility window, as well as the nominal time lags, are automatically calculated by EddyPro if not explicitly provided, with algorithms that depend on whether the EC system is open- or closed-path. The covariance maximization procedure (when requested) is applied only within the time lag plausibility window.',l:''},'Time Lags Test':{d:'Flags the scalar time series if the maximal w-covariances, determined via the covariance maximization procedure and evaluated over a predefined time-lag window, are too different from those calculated for the user-suggested time lags. That is, the mismatch between fluxes calculated with the expected time lags and with the “actual” time lags is too large.',l:''},'Time Response (anemometer)':{d:'Time response of the anemometer; its inverse defines the maximum frequency of the atmospheric turbulence that the instrument is able to resolve. Consult the anemometer’s specifications or user manual.',l:''},'Time Response (gas analyzer)':{d:'Time response of the gas analyzer; its inverse defines the maximum frequency of the atmospheric turbulent concentration fluctuations that the instrument is able to resolve. Consult the analyzer’s specifications or user manual.',l:''},'Time Step':{d:'The averaging period of an eddy covariance data set (typically 30 minutes).',l:''},'Timeline':{d:'A way to organize time-series data based on the included time stamps.',l:''},'TOB1 File':{d:'A format for raw eddy covariance data, often used for storing data obtained from Campbell® Scientific dataloggers. It is a fixed-length binary format, featuring a ASCII (readable) header stating the collected variables, their units, and numerical format.',l:''},'Transmittance':{d:'In the LI-6800, a value that represents the transparency of the film on the chamber top and bottom of the light source.',l:''},'Transversal Path Length (anemometer)':{d:'Distance transverse to a pair of transducers on the anemometer.',l:''},'Transversal Path Length (gas analyzer)':{d:'Path length in the distance orthagonal to the longitudinal path length.',l:''},'Triple Rotation':{d:'Double rotations plus a third rotation that nullifies the cross-stream stress. Not suitable in situations where the cross-stream stress is not expected to vanish, e.g., over water surfaces.',l:''},'Tube Diameter':{d:'For closed path gas analyzers, the diameter of the intake tube (cm).',l:''},'Tube Length':{d:'For closed path gas analyzers, this specifies the length (cm) of the intake tube.',l:''},'User Administrator':{d:'User Administrators have permission to add/remove user profiles, change user information for all users, and assign stations to other users.',l:''},'Vapor Pressure Deficit':{d:'The difference between vapor pressure in the leaf and vapor pressure in the air surrounding the leaf.',l:''},'Vertical Separation':{d:'Vertical distance between the gas analyzer and the reference anemometer; this value is negative if the center of the analyzer sample volume (LI‑7500RS, LI‑7700) or intake tube inlet (LI‑7200RS) is below the center of the reference anemometer sample volume and positive if the gas sample is above.',l:''}});