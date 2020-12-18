
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



/*  pfmEdit3D class definitions.  */

#ifndef PFM_EDIT_H
#define PFM_EDIT_H

#include "pfmEdit3DDef.hpp"
#include "prefs.hpp"
#include "hotkeyHelp.hpp"
#include "version.hpp"

#include "editFeature.hpp"
#include "ancillaryProg.hpp"
#include "hotkeyPolygon.hpp"
#include "czmilReprocess.hpp"
#include "externalFilter.hpp"
#include "displayLines.hpp"
#include "displayMessage.hpp"
#include "lidarClassDialog.hpp"
#include "lockValue.hpp"
#include "Vector.hpp"
#include "czmilReprocessButtonBox.hpp"


void get_buffer (MISC *misc, OPTIONS *options, uint8_t reload_flag);
int32_t put_buffer (MISC *misc);
int32_t pseudo_dist_from_viewer (MISC *misc, double x, double y);


  /*! \mainpage pfmEdit3D

       <br><br>\section disclaimer Disclaimer

       This is a work of the US Government. In accordance with 17 USC 105, copyright
       protection is not available for any work of the US Government.

       Neither the United States Government nor any employees of the United States
       Government, makes any warranty, express or implied, without even the implied
       warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes
       any liability or responsibility for the accuracy, completeness, or usefulness
       of any information, apparatus, product, or process disclosed, or represents
       that its use would not infringe privately-owned rights. Reference herein to
       any specific commercial products, process, or service by trade name, trademark,
       manufacturer, or otherwise, does not necessarily constitute or imply its
       endorsement, recommendation, or favoring by the United States Government. The
       views and opinions of authors expressed herein do not necessarily state or
       reflect those of the United States Government, and shall not be used for
       advertising or product endorsement purposes.


       <br><br>\section intro Introduction

       The pfmEdit3D program is the 3D editor companion to pfmView which is what most people refer to as the
       Area-Based Editor (ABE) even though it is only the binned surface viewer portion of ABE.  There is
       also a 2D editor, <a href="../../pfmEdit_Documentation/html/index.html"><b>pfmEdit</b></a>, but,
       due to ease of use and speed, the users will usually prefer to use pfmEdit3D, although there are a
       very few functions that are available in <a href="../../pfmEdit_Documentation/html/index.html"><b>pfmEdit</b></a>
       that are not available in pfmEdit3D.


       <br><br>\section sec1 Searching the Doxygen Documentation

       The easiest way to find documentation for a particular C++ method is to go to the
       <a href="functions_func.html"><b>Data Structures>Data Fields>Functions</b></a> tab.  If, for example,
       you are looking for <b><i>editFeature</i></b> you would then click on the <b><i>e</i></b> tab,
       find <b><i>editFeature</i></b>, and then click on the associated structure (in this case editFeature).
       The entire group of public methods for a particular class are documented via their class documentation
       not via their include files.  You can see documentation for them in the main
       <a href="annotated.html"><b>Data Structures</b></a> tab or by selecting the class in the Data Structures
       section of the .hpp file.  For example, editFeature can be accessed via the overall
       <a href="annotated.html"><b>Data Structures</b></a> tab or from the class defined in the Data Structures
       section of the editFeature.hpp file.


       <br><br>\section nvmap The nvMap Class

       The pfmEdit3D program is built around the nvMap class from the nvutility library (libnvutility).  Due to
       this there are many calls to nvMap methods throughout the program.  Documentation for the nvMap class
       can be found in the <a href="../../NVUTILITY_API_Documentation/html/index.html"><b>nvutility documentation</b></a>.
       Specifically, look for the nvMap class in the <a href="../../NVUTILITY_API_Documentation/html/annotated.html"><b>Data
       Structures</b></a> tab.<br><br>


       <br><br>\section structures The OPTIONS and MISC Structures

       The pfmEdit3D program uses two huge structures; OPTIONS and MISC.  The OPTIONS structure contains variables
       that need to be saved when the program exits and restored when it is started.  This is done using QSettings
       in env_in_out.cpp.  The MISC structure contains a bunch of variables that are used throughout the program
       but don't need to be saved.  Both of these structures are initialized in set_defaults.cpp.  I'm sure that
       it's not all that cool to use giant structures to pass things around but at least I'm passing them by
       reference instead of value so it doesn't slow the program down (try passing a big structure by value in a
       tight loop and you'll see what I mean).  The main reason that I'm using these two structures is that
       I didn't want to have to have function calls with fifty million arguments.


       <br><br>\section shared Shared Memory in PFM ABE

       This is a little note about the use of shared memory within the Area-Based Editor (ABE) programs.  If you read
       the Qt documentation (or anyone else's documentation) about the use of shared memory they will say "Dear [insert
       name of omnipotent being of your choice here], whatever you do, always lock shared memory when you use it!".
       The reason they say this is that access to shared memory is not atomic.  That is, reading shared memory and then
       writing to it is not a single operation.  An example of why this might be important - two programs are running,
       the first checks a value in shared memory, sees that it is a zero.  The second program checks the same location
       and sees that it is a zero.  These two programs have different actions they must perform depending on the value
       of that particular location in shared memory.  Now the first program writes a one to that location which was
       supposed to tell the second program to do something but the second program thinks it's a zero.  The second program
       doesn't do what it's supposed to do and it writes a two to that location.  The two will tell the first program 
       to do something.  Obviously this could be a problem.  In real life, this almost never occurs.  Also, if you write
       your program properly you can make sure this doesn't happen.  In ABE we almost never lock shared memory because
       something much worse than two programs getting out of sync can occur.  If we start a program and it locks shared
       memory and then dies (or gets stuck waiting for something), all the other programs will be locked up.  When you
       look through the ABE code you'll see that we very rarely lock shared memory, and then only for very short periods
       of time.  This is by design.<br><br>


  */

class pfmEdit3D:public QMainWindow
{
  Q_OBJECT 


 public:

  pfmEdit3D (int *argc = 0, char **argv = 0, QWidget *parent = 0);
  ~pfmEdit3D ();

  void redrawMap (uint8_t redraw2D);
  void editFeatureNum (int32_t feature_number);


 protected:

  OPTIONS         options;

  MISC            misc;

  prefs           *prefs_dialog;

  editFeature     *editFeature_dialog;

  displayLines    *displayLines_dialog;

  displayMessage  *displayMessage_dialog;

  lidarClassDialog *lidar_class_dialog;

  QMenu           *hideMenu;

  QPainterPath    tracker_2D;

  QTimer          *trackCursor, *blinkTimer;

  ancillaryProg   *ancillaryProgram[NUMPROGS];

  hotkeyPolygon   *hkp;

  czmilReprocess  *czmil_reprocess;

  externalFilter  *hofExf, *cpfExf;

  QLabel          *latLabel, *lonLabel, *zoneLabel, *recLabel, *filLabel, *pfmLabel, *linLabel, *attrValue[PRE_ATTR + POST_ATTR], *linNum,
                  *hLabel, *vLabel, *attrFilterName, *attrFilterMin, *attrFilterMax, *transLabel, *sizeLabel, *meterLabel, *radiusLabel,
                  *countLabel,  *minZLabel, *maxZLabel, *avtMinZLabel, *avtMaxZLabel, *hpMinZLabel, *hpMaxZLabel, *attrMinZLabel,
                  *attrMaxZLabel, *avtPassLabel, *hpPassLabel, *lidarClassChangeFrom, *lidarClassChangeTo, *lidarClassMinZLabel,
                  *lidarClassMaxZLabel, *layerName[MAX_ABE_PFMS], *attrBoxValue[NUM_ATTR], *lonName;

  clickLabel      *attrName[PRE_ATTR + POST_ATTR], *latName, *attrBoxName[NUM_ATTR];

  QScrollBar      *sliceBar, *transBar, *sizeBar, *exagBar;

  nvMapGL         *map;

  nvMap           *trackMap, *histogram;

  QProgressDialog *unload_prog;

  QProcess        *unloadProc;

  czmilReprocessButtonBox *czmilReprocessButtons;

  NVMAPGL_DEF     mapdef;

  NVMAP_DEF       track_mapdef, histogram_mapdef;

  QToolBar        *toolBar[NUM_TOOLBARS];

  QButtonGroup    *exitGrp;

  QActionGroup    *flagGrp, *hideGrp;

  QAction         *bHelp, *popup[NUMPOPUPS], *popupHelp, *saveExitAction, *maskExitAction, *flag[PFM_USER_FLAGS + PRE_USER],
                  *hideAct[PFM_USER_FLAGS + PFM_DATA_TYPES], *displayFeature[4], *messageAction;

  QToolButton     *bExitSave, *bExitMask, *bExitNoSave, *bReset, *bLevel, *bNorthUp, *bDisplayFeature, *bDisplayChildren, *bDisplayFeatureInfo,
                  *bVerifyFeatures, *bDisplayReference, *bMeasure, *bDisplayManInvalid, *bDisplayFltInvalid, *bDisplayNull, *bDisplayAll,
                  *bDisplayMultiple, *bUndisplaySingle, *bUnload, *bPrefs, *bStop, *bDeletePoint, *bDeleteRect, *bDeletePoly, *bKeepPoly,
                  *bRestoreRect, *bRestorePoly, *bUndo, *bReferencePoly, *bUnreferencePoly, *bAddFeature, *bEditFeature, *bDeleteFeature,
                  *bMoveFeature, *bHotkeyPoly, *bFlag, *bHighlightPoly, *bClearPoly, *bInvertHighlight, *bClearHighlight, *bContour, *bHide,
                  *bAreaFilter, *bStatFilter, *bHPFilter, *bAttrFilter, *bFilterRectMask, *bFilterPolyMask, *bClearMasks,
                  *bMaskInsideRect, *bMaskOutsideRect, *bMaskInsidePoly, *bMaskOutsidePoly, *bMaskClear, *bMaskReset, *bFlagReset,
                  *bLidarMonitor, *bHOFWaveFilter, *bCPFFilter, *bChartsPic, *bWaveformMonitor, *bCZMILwaveMonitor, *bWaveWaterfallShallow,
                  *bWaveWaterfallDeep, *bGSFMonitor, *bDistanceThresh, *bCZMILReprocessLand, *bCZMILReprocessWater,
                  *bCZMILReprocessShallowWater, *bCZMILUrbanFilter, *bLASwaveMonitor, *bChangeLidarClass;

  QPushButton     *filterDelete, *filterCancel, *avtFilterDelete, *avtFilterCancel, *hpFilterDelete, *hpFilterCancel, *attrFilterDelete,
                  *attrFilterHide, *attrFilterInvert, *attrFilterCancel, *czmilUrbanFilterDelete, *czmilUrbanFilterCancel,
                  *czmilUrbanFilterHide, *hofWaveFilterDelete, *hofWaveFilterCancel, *cpfFilterAccept, *cpfFilterDiscard;

  QIcon           flagIcon[PFM_USER_FLAGS + PRE_USER], displayFeatureIcon[4];

  QCursor         editFeatureCursor, deleteFeatureCursor, addFeatureCursor, moveFeatureCursor, rotateCursor, zoomCursor, maskCursor,
                  deleteRectCursor, deletePolyCursor, restoreRectCursor, restorePolyCursor, hotkeyPolyCursor, measureCursor, keepPolyCursor,
                  unreferencePolyCursor, referencePolyCursor, filterMaskRectCursor, filterMaskPolyCursor, highlightPolyCursor, clearPolyCursor,
                  czmilReprocessCursor, czmilReprocessPolyCursor, czmilReprocessRectCursor, sliceCursor, cpfFilterCursor,
                  changeLidarClassCursor;

  QTabWidget      *notebook, *statbook;

  QGroupBox       *colorScaleBox, *filterBox, *statFilterBox, *hpFilterBox, *attrFilterBox, *cufFilterBox, *hofFilterBox, *cpfFilterBox,
                  *lidarClassChangeBox;

  QLineEdit       *stdValue, *avtValue, *minValue, *maxValue, *srValue, *swValue, *rtValue, *pmtValue, *apdValue, *attrRangeText;
                  
  QDoubleSpinBox  *hpRadiusValue, *hpHeightValue, *radiusValue, *minZValue, *maxZValue, *avtMinZValue, *avtMaxZValue, *hpMinZValue,
                  *hpMaxZValue, *attrMinZValue, *attrMaxZValue, *lidarClassMinZValue, *lidarClassMaxZValue, *cufVRadiusValue,
                  *cufFRadiusValue, *cufVHeightValue, *cufFHeightValue;

  QSpinBox        *countValue, *hpNeighborsValue, *hpMultiNeighborsValue, *avtPassValue, *hpPassValue, *valSpin, *cufLDIndexValue,
                  *cufUDIndexValue;

  QCheckBox       *rangeCheck, *avtRangeCheck, *hpRangeCheck, *attrRangeCheck, *invCheck, *valCheck, *lidarClassRangeCheck,
                  *layerCheck[MAX_ABE_PFMS];

  QSlider         *stdSlider, *avtSlider, *srSlider, *swSlider, *rtSlider, *pmtSlider, *apdSlider;

  QMenu           *popupMenu;

  QPalette        slicePalette, transPalette, sizePalette, exagPalette, label_palette[2], blankPalette, colorPalette[2], colorBoxPalette[2],
                  attrPalette[PRE_ATTR + NUM_ATTR];;

  NV_I32_COORD2   marker[16];

  float           *save_z, attr_filter_scale;

  uint8_t         polygon_flip, finishing, slicing, double_click, unload_done, filter_active, attr_filter_active, hof_filter_active, 
                  force_auto_unload, popup_active, cr_available, call_mode;

  int32_t         unload_pfm, popup_prog[NUMPOPUPS], menu_cursor_x, menu_cursor_y, mv_tracker, rb_rectangle,
                  rb_polygon, rb_measure, mv_measure, mv_measure_anchor, mv_2D_tracker, multi_marker[MAX_STACK_POINTS], active_window_id,
                  lock_point, prev_poly_x, prev_poly_y, prev_icon_size, start_ctrl_x, start_ctrl_y, rotate_index, move_feature, local_num_lines,
                  local_line_num[PFM_MAX_FILES], rb_dist, moveWindow, hist_bar_count[NUM_HIST_BARS], start_shift_y, height_shift_y,
                  flags_offset;

  double          menu_cursor_lat, menu_cursor_lon, menu_cursor_z, range_x, range_y, range_z;

  NV_F64_COORD2	  distLineStart, distLineEnd;

  NV_F64_COORD3   line_anchor;

  QString         hotkey_message, zoneName, labelColorString[2];

  QColor          labelColor[2];

  NV_F64_XYMBR    orig_bounds, hist_bar_mbr[NUM_HIST_BARS];


  void closeEvent (QCloseEvent *event);
  void setFunctionCursor (int32_t function);
  void setWidgetStates (uint8_t enable);
  void autoUnload ();
  void geo_xyz_label (double lat, double lon, double z);
  void readStandardError (QProcess *proc);

  void discardMovableObjects ();

  void midMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat, double z);
  void leftMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat, double z);
  void rightMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat, double z);
  void runPopupProg (int32_t prog);
  void clean_exit (int32_t ret);

  void setStatusAttributes ();

protected slots:

  void slotPopupMenu (QAction *ac);
  void slotPopupHelp ();

  void slotMouseDoubleClick (QMouseEvent *e, double lon, double lat, double z);
  void slotMousePress (QMouseEvent *e, double lon, double lat, double z);
  void slotMouseRelease (QMouseEvent *e, double lon, double lat, double z);
  void slotWheel (QWheelEvent *e, double lon, double lat, double z);
  void slotMouseMove (QMouseEvent *e, double lon, double lat, double z, NVMAPGL_DEF l_mapdef);
  void slotResize (QResizeEvent *e);

  void slotTrackMousePress (QMouseEvent *e, double lon, double lat);
  void slotTrackMouseRelease (QMouseEvent *e, double lon, double lat);
  void slotTrackMouseMove (QMouseEvent *e, double lon, double lat);
  void slotTrackResize (QResizeEvent *e);
  void slotTrackPreRedraw (NVMAP_DEF mapdef);
  void slotTrackPostRedraw (NVMAP_DEF mapdef);

  void slotNotebookChanged (int index);
  void slotStatbookChanged (int index);

  void slotHistogramMouseMove (QMouseEvent *e, double x, double y);
  void slotHistogramResize (QResizeEvent *e);
  void slotHistogramPreRedraw (NVMAP_DEF mapdef);
  void slotHistogramPostRedraw (NVMAP_DEF mapdef);

  void slotTrackCursor ();
  void slotBlinkTimer ();

  void slotExitSave ();
  void slotExitMask ();
  void slotExitNoSave ();
  void slotExit (int id);

  void slotMaskClear ();
  void slotMaskReset ();
  void slotFlagReset ();
  void slotReset ();
  void slotLevel ();
  void slotNorthUp ();

  void slotSliceTriggered (int action);
  void slotSizeTriggered (int action);
  void slotTransTriggered (int action);

  void slotExagTriggered (int action);
  void slotExagReleased ();
  void slotExaggerationChanged (float exaggeration, float apparent_exaggeration);

  void slotFeatureMenu (QAction *action);
  void slotDisplayChildren ();
  void slotDisplayFeatureInfo ();
  void slotVerifyFeatures ();
  void slotDisplayReference ();
  void slotDisplayManInvalid ();
  void slotDisplayFltInvalid ();
  void slotDisplayNull ();

  void slotDisplayAll ();
  void slotUndisplaySingle ();
  void slotDisplayMultiple ();

  void slotInvertHighlight ();
  void slotClearHighlight ();

  void slotColorByClicked (QMouseEvent *e, int id);
  void slotFlagMenu (QAction *action);

  void slotProg (int id);

  void slotHideMenuShow ();
  void slotHideMenu (QAction *action);
  void slotInvertHideFlags ();
  void slotInvertHideTypes ();

  void slotAttrBoxClicked (QMouseEvent *e, int id);

  void slotRedrawMap ();

  void slotHotkeyPolygonDone ();
  void slotHofExternalFilterDone ();
  void slotCPFExternalFilterDone ();

  void slotDisplayLinesDataChanged ();

  void slotLockValueDone (uint8_t accepted);
  void slotMinScalePressed (QMouseEvent *e);
  void slotMaxScalePressed (QMouseEvent *e);

  void slotLatitudeClicked (QMouseEvent *e, int id);

  void slotMode (int id);

  void slotUnload ();

  void slotStop ();

  void slotContour ();

  void slotLayerClicked (int id);

  void slotUnloadError (QProcess::ProcessError error);
  void slotUnloadDone (int exitCode, QProcess::ExitStatus exitStatus);
  void slotUnloadReadyReadStandardError ();

  void slotEditFeatureDataChanged ();


  void extendedHelp ();
  void slotHotkeyHelp ();
  void slotToolbarHelp ();

  void slotAreaFilter ();
  void slotStatFilter ();
  void slotHPFilter ();
  void slotCZMILUrbanFilter ();
  void slotFilter ();
  void slotFilterApply ();
  void slotClearFilterMasks ();
  void slotFilterStdChanged (int value);
  void slotFilterStdReturn ();
  void slotHpFilterApply ();
  void slotCzmilUrbanFilterApply ();
  void slotFilterDelete ();
  void slotFilterHide ();
  void slotFilterCancel ();
  void slotAvtFilterStdChanged (int value);
  void slotAvtFilterStdReturn ();
  void slotAvtFilterApply ();

  void slotRadValueChanged (double value);
  void slotCountValueChanged (int value);

  void slotAvtPassValueChanged (int value);
  void slotHpPassValueChanged (int value);
  void slotRangeCheckStateChanged (int state);
  void slotMinZValueChanged (double value);
  void slotMaxZValueChanged (double value);

  void slotAttrFilter ();
  void slotAttrFilterApply ();
  void slotAttrFilterInvert ();

  void slotCzmilReprocessButtonsPressed (QMouseEvent *e);

  void slotHOFWaveFilter ();
  void slotHOFWaveFilterSrChanged (int value);
  void slotHOFWaveFilterSrReturn ();
  void slotHOFWaveFilterSwChanged (int value);
  void slotHOFWaveFilterSwReturn ();
  void slotHOFWaveFilterRtChanged (int value);
  void slotHOFWaveFilterRtReturn ();
  void slotHOFWaveFilterPMTChanged (int value);
  void slotHOFWaveFilterPMTReturn ();
  void slotHOFWaveFilterAPDChanged (int value);
  void slotHOFWaveFilterAPDReturn ();

  void slotCPFGrp (int id);
  void slotCPFFilterAccept ();
  void slotCPFFilterDiscard ();

  void slotLidarClassChangePrefs ();
  void slotLidarClassChangeDataChanged ();
  void slotLidarClassChangeAccept ();
  void slotLidarClassChangeDiscard ();

  void slotTabClose ();

  void slotUndo ();

  void slotPrefs ();
  void slotPrefDataChanged ();
  void slotPrefHotKeyChanged (int32_t i);

  void slotReadStandardError (QProcess *proc);
  void slotMessage ();
  void slotMessageDialogClosed ();

  void about ();
  void slotAcknowledgments ();
  void aboutQt ();


 private:

  void keyPressEvent (QKeyEvent *e);
  void keyReleaseEvent (QKeyEvent *e);
  void DrawMultiCursors (void);
  void runProg (int id);
  uint8_t killProg (int id);

};

#endif
