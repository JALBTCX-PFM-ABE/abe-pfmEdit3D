
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.
*********************************************************************************************/


/****************************************  IMPORTANT NOTE  **********************************

    Comments in this file that start with / * ! or / / ! are being used by Doxygen to
    document the software.  Dashes in these comment blocks are used to create bullet lists.
    The lack of blank lines after a block of dash preceeded comments means that the next
    block of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

*****************************************  IMPORTANT NOTE  **********************************/



#ifndef _PFM_EDIT_3D_DEF_H_
#define _PFM_EDIT_3D_DEF_H_

#include "nvutility.h"
#include "nvutility.hpp"
#include "attributes.hpp"
#include "nvMapGL.hpp"

#include "pfm.h"
#include "pfm_extras.h"

#include "czmil.h"

#include "binaryFeatureData.h"

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <proj_api.h>

#include "gdal_priv.h"

#include <QtCore>
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QSharedMemory>



#define         MARKER_W                          15
#define         MARKER_H                          10
#define         KILL_SWITCH_OFFSET                10000000


//!<  Internal filters

#define         AREA_FILTER                       0
#define         STAT_FILTER                       1
#define         HP_FILTER                         2
#define         CZMIL_URBAN_FILTER                3


#define         HP_MIN_RADIUS                     0.1
#define         HP_MAX_RADIUS                     100.0
#define         HP_MIN_HEIGHT                     0.1
#define         HP_MAX_HEIGHT                     100.0
#define         HP_MIN_NEIGHBORS                  0
#define         HP_MAX_NEIGHBORS                  100


//    Pointer interaction functions.

#define         DELETE_RECTANGLE                  1
#define         DELETE_POLYGON                    2
#define         RESTORE_RECTANGLE                 3
#define         RESTORE_POLYGON                   4
#define         DELETE_POINT                      5
#define         KEEP_POLYGON                      6
#define         UNSET_SINGLE                      7
#define         SET_MULTIPLE                      8
#define         MOVE_FEATURE                      9
#define         DELETE_FEATURE                    10
#define         EDIT_FEATURE                      11
#define         ADD_FEATURE                       12
#define         HOTKEY_POLYGON                    13
#define         SET_REFERENCE                     14
#define         UNSET_REFERENCE                   15
#define         ROTATE                            16
#define         ZOOM                              17
#define         MASK_INSIDE_RECTANGLE             18
#define         MASK_OUTSIDE_RECTANGLE            19
#define         MASK_INSIDE_POLYGON               20
#define         MASK_OUTSIDE_POLYGON              21
#define         MEASURE                           22
#define         POLY_FILTER_MASK                  23
#define         RECT_FILTER_MASK                  24
#define         HIGHLIGHT_POLYGON                 25
#define         CLEAR_POLYGON                     26
#define         CZMIL_REPROCESS_LAND              27
#define         CZMIL_REPROCESS_WATER             28
#define         CZMIL_REPROCESS_SHALLOW_WATER     29
#define         SHIFT_SLICE                       30
#define         CPF_FILTER                        31
#define         CHANGE_LIDAR_CLASS                32


//  Miscellaneous options

#define         NUMSHADES                         100
#define         FREEZE_POPUP                      5
#define         NUM_SCALE_LEVELS                  17
#define         NUM_HIST_BARS                     NUM_SCALE_LEVELS * 2
#define         DEFAULT_SEGMENT_LENGTH            0.25
#define         CHRTRNULL                         10000000000000000.0
#define         LINE_WIDTH                        2
#define         POLYGON_POINTS                    NVMAPGL_POLYGON_POINTS
#define         NUM_TOOLBARS                      11
#define         PRE_ATTR                          4
#define         POST_ATTR                         10
#define         PRE_USER                          6
#define         MAX_SLICE_SIZE                    50
#define         MAX_TRANS_VALUE                   64
#define         CONTOUR_POINTS                    1000
#define         MASK_NONE                         0x0                  //!<  Clear hide mask data (in misc.data[i].mask)
#define         MASK_SHAPE                        0x1                  //!<  Hide mask for shapes (rectangles/polygons, inside/outside)
#define         MASK_TYPE                         0x2                  //!<  Hide mask for types and filter
#define         MASK_FLAG                         0x4                  //!<  Hide mask for PFM user flags and highlighted data.
#define         MASK_ALL                          0x7                  //!<  Combination of all possible hide masks
#define         UNMASK_SHAPE                      0xfe                 //!<  Inverse of shape hide mask
#define         UNMASK_TYPE                       0xfd                 //!<  Inverse of type hide mask
#define         UNMASK_FLAG                       0xfb                 //!<  Inverse of flag hide mask
#define         HIDE_SLOTS                        30                   //!<  Maximum number of hide slots when hide is persistent
#define         LIDAR_CLASSES                     25                   //!<  Number of defined LiDAR classification values (0 is "Any")


/*!
  NUMPOPUPS should always be one more than you think you're using in rightMouse because the last one 
  (i.e. [NUMPOPUPS - 1]) is used for turning off slicing.  You also have to modify slotPopupMenu
  because it checks this value for debug purposes.
*/

#define         NUMPOPUPS                         15


/*!
  Button hotkeys that are editable by the user.  These are used as indexes into the options.buttonAccel, misc.buttonText,
  misc.button, and misc.buttonIcon arrays.  To add or remove from this list you need to change it here, in set_defaults.cpp
  and in pfmEdit3D.cpp (look for the tool buttons that have one of these defined names attached).  If the
  name has ACTION in it then it is a QAction from a menu.  In this case the misc.action item is set to point to the 
  QAction and the misc.button item is set to NULL (vice versa for QToolButtons).  This is how we tell them apart in the
  code.
*/

#define         SAVE_EXIT_KEY                     0
#define         SAVE_EXIT_MASK_KEY                1
#define         NO_SAVE_EXIT_KEY                  2
#define         RESET_KEY                         3
#define         DELETE_POINT_MODE_KEY             4
#define         DELETE_RECTANGLE_MODE_KEY         5
#define         DELETE_POLYGON_MODE_KEY           6
#define         AREA_FILTER_KEY                   7
#define         STAT_FILTER_KEY                   8
#define         HP_FILTER_KEY                     9
#define         ATTR_FILTER_KEY                   10
#define         RECTANGLE_FILTER_MASK_KEY         11
#define         POLYGON_FILTER_MASK_KEY           12
#define         RUN_HOTKEY_POLYGON_MODE_KEY       13
#define         EDIT_FEATURE_MODE_KEY             14
#define         UNDO_KEY                          15
#define         DISPLAY_MULTIPLE_KEY              16
#define         DISPLAY_ALL_KEY                   17
#define         CLEAR_HIGHLIGHT_KEY               18
#define         HIGHLIGHT_POLYGON_MODE_KEY        19
#define         CLEAR_POLYGON_MODE_KEY            20
#define         TOGGLE_CONTOUR_KEY                21
#define         CZMIL_REPROCESS_LAND_KEY          22
#define         CZMIL_REPROCESS_WATER_KEY         23
#define         CZMIL_REPROCESS_SHALLOW_WATER_KEY 24
#define         CZMIL_URBAN_FILTER_KEY            25
#define         SET_VIEW_LEVEL_KEY                26
#define         CHANGE_LIDAR_CLASS_KEY            27
#define         SET_NORTH_UP_KEY                  28
#define         SAVE_NO_EXIT_KEY                  29
#define         HOTKEYS                           30


//!<  Hard-wired hot keys

#define         COLOR_BY_DEPTH                    0
#define         COLOR_BY_LINE                     1
#define         COLOR_BY_HORIZONTAL_ERROR         2
#define         COLOR_BY_VERTICAL_ERROR           3
#define         COLOR_BY_ATTR_00                  4
#define         COLOR_BY_ATTR_01                  5
#define         COLOR_BY_ATTR_02                  6
#define         COLOR_BY_ATTR_03                  7
#define         COLOR_BY_ATTR_04                  8
#define         COLOR_BY_ATTR_05                  9
#define         COLOR_BY_ATTR_06                  10
#define         COLOR_BY_ATTR_07                  11
#define         COLOR_BY_ATTR_08                  12
#define         COLOR_BY_ATTR_09                  13
#define         COLOR_BY_ATTR_10                  14

#define         HARD_KEYS                         15


/*!
  PROGRAM indices into the ancillary program arrays.  If you want to change this (i.e. add or subtract programs
  it needs to be done here, in set_defaults.cpp, and add or subtract buttons in pfmEdit3D.cpp).
*/

#define         EXAMGSF                           0
#define         GSFMONITOR                        1
#define         WAVEFORMMONITOR                   2
#define         LIDARMONITOR                      3
#define         CHARTSPIC                         4
#define         MOSAICVIEW                        5
#define         WAVEWATERFALL_SHALLOW             6
#define         WAVEWATERFALL_DEEP                7
#define         CZMILWAVEMONITOR                  8
#define         LASWAVEMONITOR                    9
#define         HOFRETURNKILL                     10
#define         HOFRETURNKILL_SWA                 11
#define         HOFWAVEFILTER                     12
#define         CPF_DEEP_FILTER                   13
#define         INVALIDATE_FEATURES               14
#define         DELETE_FILTER_HIGHLIGHTED         15
#define         CANCEL_FILTER_HIGHLIGHTED         16
#define         DELETE_SINGLE_POINT               17
#define         FREEZE                            18
#define         FREEZE_ALL                        19

#define         NUMPROGS                          20


//!  IHO order levels for minimum Z window size computation.

#define         IHO_SPECIAL_ORDER                 1
#define         IHO_ORDER_1A                      2
#define         IHO_ORDER_1B                      3
#define         IHO_ORDER_2                       4


//!  Notebook tab definitions

#define         TRACKTAB                          0
#define         HISTTAB                           1
#define         AREATAB                           2
#define         STATTAB                           3
#define         HPTAB                             4
#define         CZMILURBANTAB                     5
#define         ATTRTAB                           6
#define         HOFTAB                            7
#define         CPFTAB                            8
#define         LIDARCLASSTAB                     9


//!  Structures used for filters.

typedef struct
{
  float                 x;
  float                 y;
  int32_t               count;
  uint8_t               killed;
  uint8_t               data;
} FILTER_POS_DATA;

typedef struct
{
  int32_t               count;
  std::vector<int32_t>  data;
} FILTER_BIN_DATA;

typedef struct
{
  double                lat;
  double                lon;
  double                poly_y[2000];
  double                poly_x[2000];
  int32_t               poly_count;
} FEATURE_RECORD;


//!  MAX_STACK_POINTS points nearest to the cursor.  MAX_STACK_POINTS is defined in ABE.h (utility library directory).

typedef struct
{
  int32_t       point[MAX_STACK_POINTS];
  double        dist[MAX_STACK_POINTS];
  int32_t       num_stack_points;
} NEAREST_STACK;


//!  The OPTIONS structure contains all those variables that can be saved to the users pfmEdit3D QSettings.

typedef struct
{
  int32_t     position_form;              //!<  Position format number
  int32_t     contour_width;              //!<  Contour line width/thickness in pixels
  QColor      contour_color;              //!<  Color to be used for contours
  QColor      edit_color;                 //!<  Color to be used for edit rectangles and polygons
  QColor      marker_color;               //!<  Color to be used for markers (highlighted points)
  QColor      ref_color[2];               //!<  Color to be used for reference and null data.
  QColor      tracker_color;              //!<  Color to be used for "other ABE window" tracker
  QColor      background_color;           //!<  Color to be used for background
  QColor      scale_color;                //!<  Color to be used for the "scale"
  QColor      scaled_offset_z_color;      //!<  Color to be used for printing scale or offset Z values in the information box
  QColor      waveColor[MAX_STACK_POINTS];//!<  Colors for point markers and waveforms used in CZMILwaveMonitor and waveWaterfall 
  QFont       font;                       //!<  Font used for all ABE map GUI applications
  int32_t     smoothing_factor;           //!<  Contour smoothing factor (0-10)
  uint8_t     contour_location;           //!<  0 = draw at the top of the 3D space, 1 = draw in the point cloud.
  QColor      feature_color;              //!<  Color to be used for features
  QColor      feature_info_color;         //!<  Color to be used for feature information text display
  QColor      feature_poly_color;         //!<  Color to be used for feature polygon display
  QColor      feature_highlight_color;    //!<  Color to be used for highlighted features (for text search from pfmView)
  QColor      verified_feature_color;     //!<  Color to be used for verified features (confidence = 5)
  float       feature_size;               //!<  Feature size
  float       min_window_size;            //!<  Minimum window size in Z
  int32_t     iho_min_window;             //!<  If not 0 compute minimum window size using IHO order level (special=1, 1=2, 2=3)
  int32_t     color_index;                /*!<  - Color by:
                                                    - 0 = color by depth
                                                    - 1 = color by line
                                                    - 2 = color by horizontal uncertainty
                                                    - 3 = color by vertical uncertainty
                                                    - 4 to 23 = color by numbered PFM attribute (1 through 20)
                                          */
  int32_t     flag_index;                 /*!<  - Flag data points:
                                                    - 0 = no flag
                                                    - 1 = flag suspect
                                                    - 2 = flag selected
                                                    - 3 = flag feature
                                                    - 4 = flag designated
                                                    - 5 = flag invalid
                                                    - 6 to 15 = flag PFM_USER_XX
                                          */
  uint16_t    min_hsv_color[NUM_SCALE_LEVELS];     //!<  HSV color wheel index for color to be used for minimum values (default 0)
  uint16_t    max_hsv_color[NUM_SCALE_LEVELS];     //!<  HSV color wheel index for color to be used for maximum values (default 315)
  float       min_hsv_value[NUM_SCALE_LEVELS];     //!<  Locked minimum data value for minimum HSV color
  float       max_hsv_value[NUM_SCALE_LEVELS];     //!<  Locked maximum data value for maximum HSV color
  uint8_t     min_hsv_locked[NUM_SCALE_LEVELS];    //!<  Set if min HSV is locked to a value
  uint8_t     max_hsv_locked[NUM_SCALE_LEVELS];    //!<  Set if max HSV is locked to a value
  uint8_t     display_contours;           //!<  Display contours flag
  uint8_t     display_man_invalid;        //!<  Display manually invalid data flag
  uint8_t     display_flt_invalid;        //!<  Display filter invalid data flag
  uint8_t     display_null;               //!<  Display null data flag
  int32_t     display_feature;            /*!<  - Display features:
                                                    - 0 = no features
                                                    - 1 = all features
                                                    - 2 = unverified features
                                                    - 3 = verified features
                                          */
  uint8_t     display_children;           //!<  Flag to show grouped feature children locations
  uint8_t     display_reference;          //!<  Display reference data
  uint8_t     display_feature_info;       //!<  Display feature info
  uint8_t     display_feature_poly;       //!<  Display feature polygon
  int32_t     function;                   //!<  Active edit or zoom function
  int32_t     slice_percent;              //!<  Percent of "Z" depth to use for a rotated "slice" view
  int32_t     slice_alpha;                //!<  Alpha (transparency) value for data that falls outside of the slice (0-255).
  uint8_t     slice_gray;                 //!<  Set if the user wants data outside the slice to be gray scale instead of color.
  float       z_factor;                   /*!<  Conversion factor for Z values.  May be used to convert 
                                                m to ft...  Passed from pfm_view.  */
  float       z_offset;                   //!<  Offset value for Z values.  Passed from pfm_view.
  int32_t     point_size;                 //!<  Point size in pixels
  float       rotation_increment;         //!<  Degrees to rotate when pressing the left and right arrow keys
  QString     unload_prog;                //!<  Unload program name
  uint8_t     auto_unload;                /*!<  Set if we wish to automatically unload edits to input files
                                                at the end of the session  */
  int32_t     last_rock_feature_desc;     //!<  Index of last used rock, wreck, obstruction feature description
  int32_t     last_offshore_feature_desc; //!<  Index of last used offshore installation feature description
  int32_t     last_light_feature_desc;    //!<  Index of last used light, buoy, beacon feature description
  int32_t     last_lidar_feature_desc;    //!<  Index of last used lidar feature description
  QString     last_feature_description;   //!<  String containing the last feature descriptor used
  QString     last_feature_remarks;       //!<  String containing the last feature remarks used
  float       nearest_value;              //!<  Value of point nearest to the cursor when moving the cursor
  int32_t     sparse_limit;               //!<  Number of input points to exceed before we zoom and rotate sparse data
  int32_t     zoom_percent;               //!<  Zoom in/out percentage
  float       exaggeration;               //!<  Z exaggeration
  int32_t     screenshot_delay;           //!<  Delay (in seconds) of the screenshot in the editFeature dialog.
  float       zx_rotation;                //!<  Rotation around the ZX axis to be saved for startup.
  float       y_rotation;                 //!<  Rotation around the Y axis to be saved for startup.
  QString     feature_dir;                //!<  Directory that contains feature file.
  int32_t     filter_type;                //!<  Filter type (0 = original area-based filter, 1 = AVT filter, 2 = Hockey Puck filter)
  float       area_filter_std;            //!<  Filter standard deviation for area-based filter
  float       stat_filter_std;            //!<  Filter standard deviation for AVT statistical filter
  double      stat_filter_radius;         //!<  Statistical filter radius for AVT statistical filter
  int32_t     stat_filter_count;          //!<  Minimum number of points within radius to run AVT statistical filter
  int32_t     stat_filter_passes;         //!<  Number of passes for AVT statistical filter (0 for run until done)
  double      hp_filter_radius;           //!<  Hockey puck filter horizontal radius
  double      hp_filter_height;           //!<  Hockey puck filter height 
  int32_t     hp_filter_neighbors;        //!<  Hockey puck filter number of neighbors required
  int32_t     hp_filter_ml_neighbors;     //!<  Hockey puck filter number of neighbors required if more than one line present
  int32_t     hp_filter_passes;           //!<  Number of passes for hockey puck filter (0 for run until done)
  double      czmil_urban_radius[2];      //!<  CZMIL urban noise hockey puck filter horizontal radii (for vicinity search and filter)
  double      czmil_urban_height[2];      //!<  CZMIL urban noise hockey puck filter heights (for vicinity search and filter)
  int32_t     czmil_urban_d_index[2];     //!<  CZMIL urban noise D_index cutoff values (lower and upper)
  uint8_t     filter_apply_z_range;       //!<  Set to apply filter range limitations
  float       filter_min_z;               //!<  Filter minimum Z value
  float       filter_max_z;               //!<  Filter maximum Z value
  uint8_t     deep_filter_only;           //!<  Filter only in the downward direction (assumes positive is depth, area-based filter only)
  float       feature_radius;             //!<  Filter exclusion distance for features
  int32_t     overlap_percent;            //!<  Percentage of overlap to use for Ctrl-arrow key moves
  uint8_t     kill_and_respawn;           /*!<  Set this to NVTrue to kill the ancillary programs on exit and respawn them on restart.
                                                If set to NVFalse then the programs will remain running and reconnect to pfmEdit3D when
                                                it restarts.  This is a blanket setting and will be enforced regardless of the value of
                                                "state[]".  */
  uint8_t     highlight_tip;              //!<  Set this to NVTrue to display the highlighted points tool tip
  int32_t     main_button_icon_size;      //!<  Main button icon size (16, 24, 32)
  uint8_t     draw_scale;                 //!<  Set this to draw the scale in the display.
  QString     buttonAccel[HOTKEYS];       //!<  Accelerator key sequences for some of the buttons or actions
  int32_t     undo_levels;                //!<  Number of undo levels
  QString     attrFilterText[NUM_ATTR];   //!<  Text ranges for the attribute filter (for NDX attributes only, the other three are below).
  QString     lineFilterText;             //!<  Text ranges for line number attribute filter.
  QString     horzFilterText;             //!<  Text ranges for horizontal error attribute filter.
  QString     vertFilterText;             //!<  Text ranges for vertical error attribute filter.
  float       czmil_probability[9];       //!<  Probability of detection for CZMIL data reprocessing (per channel)
  uint8_t     czmil_cpf_filter_inval;     //!<  Set if we want to invalidate deep channel using CPF filter.
  int32_t     czmil_cpf_filter_amp_min;   //!<  Minimum interest point waveform amplitude value for CPF filter when validating deep channel (default is 600).
  uint8_t     czmil_suppress_warnings;    //!<  Set if we don't want to see the warnings from the czmilReprocess app.
  uint8_t     czmil_reprocess_mode;       //!<  True for polygon mode, False for rectangle mode.
  uint8_t     czmil_reprocess_flags[2];   //!<  Flags for CZMIL data types to be reprocessed - Land, Water.
  uint8_t     hide_persistent;            //!<  Set if we want hide options to remain in effect between edit sessions.
  uint8_t     hide_visible;               //!<  Set if we want the "Hide" tab to be displayed on startup if hide persistent is set and some data is hidden.
  uint8_t     hide_flag[HIDE_SLOTS];      //!<  Hide slots to remain checked if hide_show_persistent is set.
  uint8_t     mask_active;                //!<  0 if no data is masked, 1 if rectangle/polygon masked data, 2 if attribute masked data, 3 if both.
  QString     hide_name[HIDE_SLOTS];      //!<  Names of the hide menu entries so that we can make sure they don't change from session to session.
  int32_t     hide_num;                   //!<  Number of hide menu entries.
  int32_t     msg_width;                  //!<  Message window width
  int32_t     msg_height;                 //!<  Message window height
  int32_t     msg_window_x;               //!<  Message window x position
  int32_t     msg_window_y;               //!<  Message window y position
  uint8_t     auto_filter_mask;           //!<  If this is set, filter masking will occur automatically in pfmView if any filtering has been done
  uint8_t     track_tab;                  //!<  If this is set, the default box in the notebook will be the trackMap, otherwise it will be the histogram
  uint8_t     no_color_scale_masked;      /*!<  If this is set, masked data is not used to compute the color range and scale.  If not, masked data is
                                                used for computing the color range and scale.  The default is NVFalse.  This value is settable in the
                                                Preferences dialog.  */
  uint8_t     invalid_edit_color;         //!<  If set, invalid data is not used for color scaling and is displayed in the edit color.
  int32_t     num_hp_threads;             //!<  Number of threads to use for the Hockey Puck filter.
  float       z_orientation;              /*!<  This is set to 1.0 to display numeric data to the user as depths or -1.0 to display them as elevations.
                                                This value is stored in a separate QSettings .ini file because it is used by a number of other programs
                                                (e.g. pfmView).  */
  int32_t     pref_tab;                   //!<  This is the tab number of the last Preferences tab viewed (so we can go back to the same tab to start).
  int32_t     lidar_class;                /*!<  This is the LiDAR classification value (to) used when changing LiDAR classifications.  These values
                                                are based on the ASPRS LAS 1.4 spec and the 2013 ASPRS paper - "LAS Domain Profile Description: Topo-Bathy
                                                Lidar", July 17, 2013.  */
  uint8_t     from_lidar_class[LIDAR_CLASSES];  /*!<  This is an array of flags that will limit the points that can have their classifications
                                                changed.  The way it works is that setting from_lidar_class[0] to NVTrue means that any point 
                                                regardless of its classification can have its classification changed.  Setting from_lidar_class[0]
                                                to NVTrue will cause all other from_lidar_class values to be set to NVFalse.  If from_lidar_class[0]
                                                is set to NVFalse then only points that have classifications that correspond to from_lidar_class array
                                                members other than [0] that are set to NVTrue can be changed.  */
  uint8_t     default_feature_type;       //!<  Default feature type to be used when creating features (BFDATA_HYDROGRAPHIC or BFDATA_INFORMATIONAL).
  uint8_t     lat_or_east;                //!<  Set to 0 to display latitude and longitude, set to 1 to display northing and easting.
  QString     attrStatName[POST_ATTR];    //!<  Names of selected attributes on the Status tab


  //  The following relate to PFM attributes.

  QString     time_attr_filt;
  QString     gsf_attr_filt[GSF_ATTRIBUTES];
  QString     hof_attr_filt[HOF_ATTRIBUTES];
  QString     tof_attr_filt[TOF_ATTRIBUTES];
  QString     czmil_attr_filt[CZMIL_ATTRIBUTES];
  QString     las_attr_filt[LAS_ATTRIBUTES];
  QString     bag_attr_filt[BAG_ATTRIBUTES];
  ATTR_BOUNDS time_attribute;
  int16_t     time_attribute_num;
  ATTR_BOUNDS gsf_attribute[GSF_ATTRIBUTES];
  int16_t     gsf_attribute_num[GSF_ATTRIBUTES];
  ATTR_BOUNDS hof_attribute[HOF_ATTRIBUTES];
  int16_t     hof_attribute_num[HOF_ATTRIBUTES];
  ATTR_BOUNDS tof_attribute[TOF_ATTRIBUTES];
  int16_t     tof_attribute_num[TOF_ATTRIBUTES];
  ATTR_BOUNDS czmil_attribute[CZMIL_ATTRIBUTES];
  int16_t     czmil_attribute_num[CZMIL_ATTRIBUTES];
  ATTR_BOUNDS las_attribute[LAS_ATTRIBUTES];
  int16_t     las_attribute_num[LAS_ATTRIBUTES];
  ATTR_BOUNDS bag_attribute[BAG_ATTRIBUTES];
  int16_t     bag_attribute_num[BAG_ATTRIBUTES];
  int16_t     czmil_flag_count;
  QString     czmil_flag_name[CZMIL_FLAGS];
  int16_t     czmil_flag_num[CZMIL_FLAGS];


  //  These relate to the ancillary programs

  QString     prog[NUMPROGS];             //!<  Ancillary program command strings with keywords
  QString     name[NUMPROGS];             //!<  Ancillary program name
  QString     hotkey[NUMPROGS];           //!<  Ancillary programs hot key (the extra 2 are for the freeze options)
  QString     action[NUMPROGS];           //!<  Ancillary programs action hot keys
  QToolButton *progButton[NUMPROGS];      //!<  If this program is attached to a button, this is the button (otherwise NULL)
  uint8_t     data_type[NUMPROGS][PFM_DATA_TYPES]; //!<  The data types associated with an ancillary program
  uint8_t     hk_poly_eligible[NUMPROGS]; //!<  Whether the key is eligible to be used in a polygon
  int16_t     hk_poly_filter[NUMPROGS];   //!<  Whether the key can only be used for hotkey polygons (1) or external filter (2), otherwise 0
  int32_t     state[NUMPROGS];            /*!<  - Ancillary program state:
                                                    - 0 = program does not respawn on restart
                                                    - 1 = program respawns but was not up when pfmEdit3D last closed
                                                    - 2 = program respawns and was up
                                          */
  uint32_t    kill_switch[NUMPROGS];      /*!<  Kill switch value to be used to terminate "kill and respawn" ancillary programs.
                                                This number will always be above 10,000,000.  */
  QString     description[NUMPROGS];      //!<  Ancillary program description


  uint8_t     auto_scale;                 //!<  NVTrue if we're auto-scaling when we mask or do other operations that show or hide data.
  double      hofWaveFilter_search_radius;//!<  hofWaveFilter search radius in meters
  int32_t     hofWaveFilter_search_width; //!<  Number of points to search before and after selected point on nearby waveforms
  int32_t     hofWaveFilter_rise_threshold;//!<  Number of rise points needed as corroborating data on nearby waveforms
  int32_t     hofWaveFilter_pmt_ac_zero_offset_required;/*!<  If selected point is less than this amount above the
                                                              PMT AC zero offset the point will be marked invalid  */
  int32_t     hofWaveFilter_apd_ac_zero_offset_required;/*!<  If selected point is less than this amount above the
                                                              APD AC zero offset the point will be marked invalid  */
} OPTIONS;


//! The UNDO block structure.

typedef struct
{
  std::vector<uint32_t> val;              //!<  Validity
  std::vector<uint32_t> num;              //!<  Point number in the POINT_CLOUD array
  std::vector<uint8_t> cls;               //!<  Classification value for CZMIL or LAS classification attribute (this will be empty () if not used)
  int32_t     count;                      //!<  Number of points in this undo block
} UNDO;


//! General stuff.

typedef struct
{
  uint8_t     area_drawn;                 //!<  Area has been displayed flag
  uint8_t     busy;                       //!<  Busy drawing or running external process
  int32_t     save_function;              //!<  Saved active function when non-edit functions in use
  int32_t     save_mode;                  //!<  Saved active edit mode for exit (only delete functions)
  int32_t     num_records;                //!<  Number of records per section
  int32_t     poly_count;                 //!<  Polygon point count
  double      polygon_x[POLYGON_POINTS];  //!<  Polygon X
  double      polygon_y[POLYGON_POINTS];  //!<  Polygon Y
  int32_t     hotkey_poly_count;          //!<  hotkey polygon count
  int32_t     *hotkey_poly_x;             //!<  hotkey polygon X points (allocated)
  int32_t     *hotkey_poly_y;             //!<  hotkey polygon Y points (allocated)
  int32_t     rotate_angle;               //!<  Rotation of view angle at 1/10 degree
  float       ortho_scale;                //!<  X scale of rotated view 
  int32_t     ortho_angle;                //!<  Rotation of view angle at 1/10 degree
  int32_t     ortho_min;                  //!<  Minimum "Z" value when rotated
  int32_t     ortho_max;                  //!<  Maximum "Z" value when rotated
  int32_t     ortho_range;                //!<  Max "Z" - min "Z" when rotated
  uint8_t     slice;                      //!<  Set if we're "slicing" the data
  int32_t     slice_min;                  //!<  Min "Z" value in slice
  int32_t     slice_max;                  //!<  Max "Z" value in slice
  int32_t     slice_size;                 //!<  Size of slice in "Z" (0 = off)
  double      slice_meters;               //!<  Size of slice in meters
  float       sin_array[3600];            //!<  Pre-computed sine values at 1/10 degree
  float       cos_array[3600];            //!<  Pre-computed cosine values at 1/10 degree
  int32_t     line_count;                 //!<  Number of lines in the displayed area
  int32_t     line_number[255];           //!<  Line numbers of lines
  int32_t     line_index[255];            //!<  Line index of lines (the count of the line - 0 for first line, 1 for second line...)
  int32_t     line_type[255];             //!<  Data types of lines
  int32_t     num_lines;                  //!<  Number of lines to display, 0-all
  int32_t     line_num[255];              //!<  Line numbers to display after selection
  int32_t     highlight_count;            //!<  Number of highlighted points
  std::vector<int32_t> highlight;         //!<  Highlighted points
  int32_t     maxd;                       //!<  Maximum contour density per grid cell
  uint8_t     reference_flag;             //!<  True if there is reference data present
  int32_t     nearest_feature;            //!<  Nearest feature to the cursor.
  uint8_t     resized;                    //!<  resized flag
  int32_t     hotkey;                     //!<  function number if we just pushed a command hotkey (or -1)
  BFDATA_SHORT_FEATURE *feature;          //!<  Feature (target) data array
  int32_t     bfd_handle;                 //!<  BFD (target) file handle
  BFDATA_HEADER  bfd_header;              //!<  Header for the current BFD file
  BFDATA_RECORD  new_record;              //!<  Record built when adding a new feature
  uint8_t     bfd_open;                   //!<  Set if we have an open bfd file
  int32_t     visible_feature_count;      //!<  visible feature count
  uint8_t     feature_mod;                //!<  If feature data was modified
  uint8_t     drawing_canceled;           //!<  set to cancel drawing
  uint8_t     color_array[NUMSHADES][4];  //!<  Colors for points in the point cloud
  uint8_t     line_color_array[NUMSHADES][4];/*!<  Colors for points in color by line mode (alternate full value and half value - darker)  */
  QColor      track_color_array[NUMSHADES];//!<  Colors for points in the tracker map
  int32_t     draw_area_width;            //!<  Width of map drawing area in pixels
  int32_t     draw_area_height;           //!<  Height of map drawing area in pixels
  NV_I32_COORD2 center_point;             //!<  center of drawing area in pixels
  int32_t     nearest_point;              //!<  point nearest to the cursor
  QIcon       buttonIcon[HOTKEYS];        //!<  Tool button icons for tools having accelerators
  QString     buttonText[HOTKEYS];        //!<  Tooltip text for the buttons that have editable accelerators
  QToolButton *button[HOTKEYS];           //!<  Buttons that have editable accelerators (or NULL)
  QAction     *action[HOTKEYS];           //!<  QActions that have editable accelerators (or NULL)
  QString     html_help_tag;              /*!<  Help tag for the currently running section of code.  This comes from
                                                the PFM_ABE html help documentation.  */
  QString     shared_file;                //!<  Shared file name for hotkey polygons.
  QColor      widgetBackgroundColor;      //!<  The normal widget background color.
  QColor      widgetForegroundColor;      //!<  The normal widget foreground color.
  int32_t     feature_polygon_flag;
  QSharedMemory *abeShare;                //!<  ABE's shared memory pointer.
  ABE_SHARE   *abe_share;                 //!<  Pointer to the ABE_SHARE structure in shared memory.
  QSharedMemory *dataShare;               //!<  Point cloud shared memory.
  POINT_CLOUD *data;                      /*!<  Pointer to POINT_CLOUD structure in point cloud shared memory.  To see what is in the 
                                                POINT_CLOUD structure please see the ABE.h file in the nvutility library.  */
  NV_F64_XYMBR displayed_area;            //!<  Currently displayed area
  NEAREST_STACK nearest_stack;            //!<  Nine points nearest to the cursor
  double      x_grid_size;                //!<  X grid spacing (degrees) for contours
  double      y_grid_size;                //!<  Y grid spacing (degrees) for contours
  float       min_z;                      //!<  Minimum Z value for all points read in get_buffer
  float       max_z;                      //!<  Maximum Z value for all points read in get_buffer
  float       contour_level;              //!<  Top level used for drawing contours at the top of the 3D space
  uint8_t     marker_mode;                /*!<  - Marker mode (only used in DELETE_POINT mode):
                                                    - 0 = normal
                                                    - 1 = center marker is frozen but others are normal (when CZMILwaveMonitor is running)
                                                    - 2 = freeze all markers (when CZMILwaveMonitor is running).
                                          */
  int32_t     frozen_point;               //!<  Use this instead of nearest point if we freeze the marker
  QString     qsettings_app;              //!<  Application name for QSettings
  uint8_t     need_sparse;                //!<  Set to NVTrue if we have more than options.sparse_limit points to display.
  int16_t     unique_type[PFM_DATA_TYPES];//!<  Unique data types being displayed.
  int16_t     unique_count;               //!<  Number of unique data types being displayed.
  int32_t     add_feature_index;          //!<  "data" array index if we added or moved a feature
  int32_t     nearest_feature_point;      /*!<  "data" array index of the nearest point to a feature that we are moving.
                                                 We use this to try to unset PFM_SELECTED_FEATURE on the point after we
                                                 move the feature.  */
  QWidget     *map_widget;                //!<  Map widget
  uint8_t     filter_mask;                //!<  Set if we have any filtermasked points.
  std::vector<int32_t> filter_kill_list;  //!<  List of points that are set to be killed or modified by the filter (if point is set to -1, ignore)
  int32_t     filter_kill_count;          //!<  Number of points that are set to be killed or modified by the filter
  uint8_t     filtered;                   //!<  NVTrue if area was filtered.
  uint8_t     hydro_lidar_present;        //!<  NVTrue if we have any Hydro LIDAR data.
  uint8_t     lidar_present;              //!<  NVTrue if we have any Hydro or Topo LIDAR data.
  uint8_t     hof_present;                //!<  NVTrue if we have any CHARTS HOF data
  uint8_t     gsf_present;                //!<  NVTrue if we have any GSF data.
  uint8_t     las_present;                //!<  NVTrue if we have any LAS data.
  uint8_t     czmil_present;              //!<  NVTrue if we have any CZMIL data.
  int32_t     czmil_urban_attr;           //!<  "CZMIL Urban noise flags" attribute number (or -1)
  int32_t     czmil_d_index_attr;         //!<  "CZMIL D_index" attribute number (or -1)
  int32_t     czmil_proc_attr;            //!<  Attribute number of the CZMIL processing mode in the PFM (-1 if not present)
  int32_t     czmil_chan_attr;            //!<  Attribute number of the CZMIL channel number in the PFM (-1 if not present)
  int32_t     lidar_class_attr;           /*!<  Attribute number of the CZMIL, LAS, CHARTS HOF, or CHARTS TOF classification attribute in the
                                                PFM (-1 if not present)  */
  int32_t     time_attr;                  /*!<  If this is set to >= 0 then this is the attribute number for the 
                                                minutes from 01/01/1970 (i.e. Time (POSIX minutes) attribute)  */
  int32_t     process_id;                 //!<  This program's process ID
  QStatusBar  *progStatus;                //!<  Progress (and message) status bar
  QProgressBar *statusProg;               //!<  Generic progress bar in status area
  QLabel      *statusProgLabel;           //!<  Generic progress bar label
  QPalette    statusProgPalette;          //!<  Background color palette for the progress bar label
  double      map_center_x;               //!<  Center of displayed area (lon)
  double      map_center_y;               //!<  Center of displayed area (lat)
  double      map_center_z;               //!<  Center of displayed area (z)
  std::vector<UNDO> undo;                 //!<  The undo block array
  int32_t     undo_count;                 //!<  The number of undo blocks
  UNDO        filter_undo;                //!<  A temporary filter undo block (used for CPF_DEEP_FILTER for now).
  scaleBox    *scale[NUM_SCALE_LEVELS];
  NV_F64_XYMBC bounds;                    //!<  Currently visible bounds
  NV_F64_XYMBC aspect_bounds;             //!<  Bounds that are passed to nvMapGL in order to keep the proper aspect ratio
  float        color_min_z;               //!<  Minimum value used for color range
  float        color_max_z;               //!<  Maximum value used for color range
  float        color_range_z;             //!<  Color range
  float        attr_color_min;            //!<  Minimum value used for attribute color range
  float        attr_color_max;            //!<  Maximum value used for attribute color range
  float        attr_color_range;          //!<  Attribute color range
  double       avg_bin_size_meters;       //!<  Average bin size in meters of all opened PFMs
  double       avg_x_bin_size_degrees;    //!<  Average X bin size in degrees of all opened PFMs
  double       avg_y_bin_size_degrees;    //!<  Average Y bin size in degrees of all opened PFMs
  projPJ       pj_utm;                    //!<  Proj4 UTM projection structure
  projPJ       pj_latlon;                 //!<  Proj4 latlon projection structure
  float        attr_min;                  //!<  Attribute minimum value.  This is the actual, unadjusted min for all "color by" options (including depth).
  float        attr_max;                  //!<  Attribute maximum value.  This is the actual, unadjusted max for all "color by" options (including depth).
  int32_t      attrStatNum[POST_ATTR];    //!<  Numbers of attributes being displayed on the Status tab
  int32_t      attrBoxFlag[NUM_ATTR];     //!<  Set to 1 if attribute is being displayed on the Status tab, otherwise 0
  uint8_t      czmilReprocessFlag;        /*!<  If we successfully run the czmilReprocess ancillary program we need to force a redraw when we return
                                                to pfmView.  In that case this flag will be set to NVTrue.  */
  char         program_version[256];      //!<  Program version name, based on the ABE_CZMIL environment variable.
  char         progname[256];             /*!<  This is the program name.  It will be used in all output to stderr so that shelling programs (like abe)
                                                will know what program printed out the error message.  */
  QStringList  *messages;                 //!<  Message list
  int32_t      num_messages;              //!<  Number of messages in messageList
  uint8_t      no_invalid;                //!<  This will be set by the calling program with the -n option.  If set, invalid data will not be loaded.
  uint8_t      no_reference;              //!<  This will be set by the calling program with the -r option.  If set, reference data will not be loaded.
  QString      hard_key[HARD_KEYS];       //!<  Hard wired hot keys (mostly for color-by-attributes).
  int32_t      lidar_class_num[LIDAR_CLASSES]; /*!<  This is the array of LiDAR classification numbers used when changing LiDAR classifications.  These
                                                values are based on the ASPRS LAS 1.4 spec and the 2013 ASPRS paper - "LAS Domain Profile Description:
                                                Topo-Bathy Lidar", July 17, 2013.  They are defined in set_defaults.cpp.  */
  QString      lidar_class_name[LIDAR_CLASSES]; /*!<  This is the array of LiDAR classification names used when changing LiDAR classifications.  These
                                                values are based on the ASPRS LAS 1.4 spec and the 2013 ASPRS paper - "LAS Domain Profile Description:
                                                Topo-Bathy Lidar", July 17, 2013.  They are defined in set_defaults.cpp.  */
  QString      lidar_class_desc[LIDAR_CLASSES]; /*!<  This is the array of LiDAR classification descriptions used when changing LiDAR classifications.  These
                                                values are based on the ASPRS LAS 1.4 spec and the 2013 ASPRS paper - "LAS Domain Profile Description:
                                                Topo-Bathy Lidar", July 17, 2013.  They are defined in set_defaults.cpp.  */


  //  The following concern PFMs as layers.  There are a few things from ABE_SHARE that also need to be 
  //  addressed when playing with layers - open_args, display_pfm, and pfm_count.

  NV_F64_XYMBC total_mbr;                 //!<  MBR of all of the displayed PFMs
  uint16_t     max_attr;                  //!<  Maximum number of index attributes in any of the PFM files
  NV_I32_COORD2 ll[MAX_ABE_PFMS];         //!<  lower left coordinates for get_buffer and put_buffer
  NV_I32_COORD2 ur[MAX_ABE_PFMS];         //!<  upper right coordinates for get_buffer and put_buffer
  float        null_val[MAX_ABE_PFMS];    //!<  Null depth value
  char         attr_format[MAX_ABE_PFMS][NUM_ATTR][20]; //!<  Formats to use for displaying attributes
  int32_t      pfm_handle[MAX_ABE_PFMS];  //!<  PFM file handle
} MISC;


int32_t check_bounds (OPTIONS *options, MISC *misc, int32_t index, uint8_t ck, uint8_t slice);
int32_t check_bounds (OPTIONS *options, MISC *misc, int32_t index, uint8_t mask, uint8_t ck, uint8_t slice);
int32_t check_bounds (OPTIONS *options, MISC *misc, double x, double y, float z, uint32_t v, uint8_t mask, int16_t pfm,
                       uint8_t ck, uint8_t slice);
uint8_t check_line (MISC *misc, int32_t line);
void set_area (nvMapGL *map, OPTIONS *options, MISC *misc, int32_t *x_bounds, int32_t *y_bounds, int32_t num_vertices, int32_t rect_flag);
void keep_area (nvMapGL *map, OPTIONS *options, MISC *misc, int32_t *x_bounds, int32_t *y_bounds, int32_t num_vertices);
void overlayFlag (nvMapGL *map, OPTIONS *options, MISC *misc);
void overlayData (nvMapGL *map, OPTIONS *options, MISC *misc);
void compute_ortho_values (nvMapGL *map, MISC *misc, OPTIONS *options, QScrollBar *sliceBar, uint8_t set_to_min);
void store_undo (MISC *misc, int32_t undo_levels, uint32_t val, uint32_t num, float *cls);
void end_undo_block (MISC *misc);
void undo (MISC *misc);
uint8_t resize_undo (MISC *misc, OPTIONS *options, int32_t undo_levels);


#endif
