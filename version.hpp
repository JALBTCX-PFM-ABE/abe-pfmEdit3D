
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




#ifndef VERSION

#define     VERSION     "PFM Software - pfmEdit3D V6.98 - 01/25/21"

#endif

/*! <pre>

    Version 1.00
    Jan C. Depner
    11/18/08

    First working version.


    Version 1.01
    Jan C. Depner
    11/21/08

    Added masking and a measuring tool.


    Version 1.02
    Jan C. Depner
    11/24/08

    Changed default action keys for waveMonitor (n is now a toggle for nearest neighbor and single waveform
    display modes).  Fix bug in slotMouseMove when in MEASURE mode.


    Version 1.03
    Jan C. Depner
    11/25/08

    Replaced charts_list with chartsMonitor and removed the "extended" (x) action key from the waveMonitor defaults.


    Version 1.04
    Jan C. Depner
    11/25/08

    Lock the file and line names when we freeze the cursor.


    Version 1.05
    Jan C. Depner
    11/28/08

    Fixed zoom, Z exaggeration, setting map center.  Add ability to mask by PFM_USER flag.  Made ancillary program action
    keys work as toggles.  That is, the first time you hit the key it starts the program.  The next time it kills it.
    This is only for GUI programs that save state.


    Version 1.06
    Jan C. Depner
    12/01/08

    Added ability to mask by data type if more than one data type is being displayed.


    Version 1.07
    Jan C. Depner
    12/04/08

    Now sets PFM_SELECTED_FEATURE on points that are associated with a target.  Added ability to mark 
    PFM_SELECTED_FEATURE and PFM_DESIGNATED_SOUNDING data.


    Version 1.08
    Jan C. Depner
    12/12/08

    Allow hot keys in MEASURE mode.


    Version 1.09
    Jan C. Depner
    12/15/08

    Added screenshot grabber for adding and editing targets.  Also, only unfreeze if we delete the point
    or we explicitly unfreeze by pressing f or F.  Do not unfreeze on mode change.


    Version 1.10
    Jan C. Depner
    12/16/08

    Added right click menu option when in SET_REFERENCE or UNSET_REFERENCE mode to allow you to delete
    all visible, non-transparent reference points.


    Version 1.11
    Jan C. Depner
    01/09/09

    Various and sundry changes to try to speed up display in Windoze (SUX).


    Version 1.20
    Jan C. Depner
    01/23/09

    Changed to XOR for movable objects because redrawing the point cloud every time made the response too
    slow when you were viewing a lot of points.  I saved the original, non-XOR version in nvMapGL.cpp.noXOR.


    Version 1.21
    Jan C. Depner
    01/27/09

    Removed timer from polygon drawing.


    Version 1.30
    Jan C. Depner
    02/11/09

    Added coordination between pfmView, pfmEdit3D, and pfm3D.  When pfmEdit3D is started pfm3D will unload
    it's display lists and memory unless you move the cursor back into pfm3D in which case it will reload
    until you leave again.  Lists and memory will be reloaded upon exit from pfmEdit3D.  Fixed problems with
    rubberband polygons.


    Version 1.31
    Jan C. Depner
    02/12/09

    Finally found the problem with display speed while rotating (on Windoze).  It actually had an impact on Linux
    but the disk read speed was so much better that you couldn't tell ;-)  Also, added draw scale option to prefs.
    It's not really a scale, just a box at the moment.


    Version 1.32
    Jan C. Depner
    02/18/09

    Numerous hacks to try to fix the Windoze version of nvMapGL.cpp.    Fixed the extended help feature to use the
    PFM_ABE environment variable to find the help file.  Also, hard-wired the browser names in Windoze and Linux.


    Version 1.33
    Jan C. Depner
    02/22/09

    Hopefully handling the movable objects (XORed) correctly now.  This should make the Windoze version
    a bit nicer.


    Version 1.34
    Jan C. Depner
    02/24/09

    Finally figured it out - I have to handle all of the GL_BACK and GL_FRONT buffer swapping manually.  If
    I let it auto swap it gets out of control.


    Version 1.35
    Jan C. Depner
    03/13/09

    Minor change to allow waveWaterfall and chartsPic to handle WLF data.


    Version 1.36
    Jan C. Depner
    03/20/09

    Brought back the old waveMonitor as waveformMonitor at the request of the ACE.


    Version 1.37
    Jan C. Depner
    03/25/09

    Fixed the flicker at the end of a rotate or zoom operation.  Save the last Y and ZX rotations to restore
    on startup.


    Version 1.40
    Jan C. Depner
    03/31/09

    Replaced support of NAVO standard target (XML) files with Binary Feature Data file support.


    Version 1.41
    Jan C. Depner
    04/13/09

    Use NINT instead of typecasting to NV_INT32 when saving Qt window state.  Integer truncation was inconsistent on Windows.


    Version 1.42
    Jan C. Depner
    04/14/09

    Added selectable feature marker size option.


    Version 1.43
    Jan C. Depner
    04/15/09

    Minor bug in editFeature.cpp.


    Version 1.44
    Jan C. Depner
    04/20/09

    Fixed display of NULL data.


    Version 1.45
    Jan C. Depner
    04/23/09

    Changed the acknowledgments help to include Qt and a couple of others.


    Version 1.46
    Jan C. Depner
    04/27/09

    Replaced QColorDialog::getRgba with QColorDialog::getColor.


    Version 1.47
    Jan C. Depner
    05/04/09

    Use paintEvent for "expose" events in nvMapGL.cpp.  This wasn't a problem under compiz but shows up
    under normal window managers.


    Version 1.48
    Jan C. Depner
    05/21/09

    Set all QFileDialogs to use List mode instead of Detail mode.


    Version 1.49
    Jan C. Depner
    06/11/09

    Added support for PFM_CHARTS_HOF_DATA.


    Version 1.50
    Jan C. Depner
    06/24/09

    Changed the BFD names to avoid collision with GNU Binary File Descriptor library.


    Version 1.51
    Jan C. Depner
    07/15/09

    Was computing the 3D bounds incorrectly.


    Version 1.52
    Jan C. Depner
    07/28/09

    Changed use of _sleep to Sleep on MinGW (Windows).


    Version 1.53
    Jan C. Depner
    07/29/09

    Added tool tips to tool bars.
    Changed %lf in printf statements to %f.  The MinGW folks broke this even though it works on every
    compiler known to man (including theirs up to the last version).  No compiler warnings or errors
    are issued.  Many thanks to the MinGW folks for fixing what wasn't broken.


    Version 1.54
    Jan C. Depner
    08/20/09

    Moved flagging from "color by" menu to its own menu now that we can overlay lines in 2D.  Also, mark
    all invalid points with an X when invalid data is being displayed.  Added hofReturnKill program with
    -s option to kill Shallow Water Algorithm and/or Shoreline Depth Swapped data.


    Version 1.55
    Jan C. Depner
    08/25/09

    Added filter masks, filter button, and area-based filtering to the editor.


    Version 1.56
    Jan C. Depner
    08/27/09

    Added message when filter doesn't find any points to delete.


    Version 1.60
    Jan C. Depner
    09/10/09

    Added LIDAR tool bar.  Added push buttons to start LIDAR monitoring programs (like waveMonitor) to the
    LIDAR tool bar.


    Version 1.61
    Jan C. Depner
    09/11/09

    Added "no kill" (-n) option to command line.  This is used if pfmEdit3D is started from pfmEdit
    instead of from pfmView.  We wouldn't want pfmEdit3D to kill or respawn the programs that pfmEdit
    is in control of.  Fixed getColor calls so that cancel works.


    Version 1.62
    Jan C. Depner
    09/16/09

    Made new blinkTimer for mask/unmask buttons.  Added check for kill of ancillary programs that are attached
    to buttons.


    Version 1.63
    Jan C. Depner
    09/22/09

    Allow EDIT_FEATURE to be a saved edit function so that the editor may come up in that mode on start.


    Version 1.64
    Jan C. Depner
    09/23/09

    Added ability to "verify" (set confidence to 5) all displayed valid features.


    Version 1.65
    Jan C. Depner
    10/22/09

    Cleaned up hot key handling.


    Version 2.00
    Jan C. Depner
    11/02/09

    Added 2D tracker and attributeViewer.


    Version 2.01
    Jan C. Depner
    11/12/09

    Added delete records (pings) in rectangle or polygon.  Only available from the right mouse menu.


    Version 2.02
    Jan C. Depner
    12/01/09

    Fixed mask reset bug.


    Version 2.03
    Jan C. Depner
    12/02/09

    Replaced get_egm96 with get_egm08.


    Version 2.04
    Jan C. Depner
    12/09/09

    Fixed VirtualBox close bug.


    Version 2.05
    Jan C. Depner
    12/11/09

    Added "Edit return status:" to the printf calls that return info to pfmView.


    Version 2.10
    Jan C. Depner
    12/17/09

    Added 100 to 1,000,000 undo levels - woo hoo!


    Version 2.11
    Jan C. Depner
    12/28/09

    Fixed bug in slotMouseMove.  I wasn't checking to see if a rubberband construct existed prior to trying
    to drag it.  Also, added filter standard deviation button.


    Version 2.12
    Jan C. Depner
    12/29/09

    Stopped the filter from filtering points when there is not enough surrounding valid data.


    Version 2.13
    Jan C. Depner
    01/05/10

    Made filter confirmation optional.  Replaced all calls to the "inside" function with calls to
    "inside_polygon2".  This is supposed to be faster although I couldn't see any major improvement.


    Version 2.14
    Jan C. Depner
    01/06/10

    Fixed the filter so that it uses the points in memory (and their validity from whatever editing has been
    done) instead of reading the data from the PFM.  I just stole the filter code from pfmView and in that
    case it was OK to read from the PFM since changes made there are automatically updated in the PFM.
    In the editor the changes don't happen until you exit, so here we have to use the data in memory
    and make pseudo bins in memory to do the proper thing.


    Version 2.15
    Jan C. Depner
    01/14/10

    Fixed filter undo problem.  Also, fixed problem with single value in all depths (as when an area has been
    land masked).


    Version 2.16
    Jan C. Depner
    01/26/10

    Replaced geoSwath call with gsfMonitor.  The geoSwath program was sort of useless in this context anyway.


    Version 2.20
    Jan C. Depner
    01/28/10

    Added right click menu for all non-monitor type ancillary programs when in DELETE_POINT mode or
    HOTKEY_POLYGON mode.


    Version 2.21
    Jan C. Depner
    02/17/10

    Wasn't checking against rectangle in get_buffer.  If we had a large bin size this caused all kinds of grief - the area you got
    in the editor was much larger than what you asked for.


    Version 2.22
    Jan C. Depner
    02/25/10

    Added support for running in read only mode (from pfmWDBView via pfmView).


    Version 2.23
    Jan C. Depner
    03/02/10

    Moved the filter message box to the bottom of the screen so it wouldn't get in the way.


    Version 2.24
    Jan C. Depner
    03/09/10

    Added special translators for time and datum attributes from PFMWDB.


    Version 2.25
    Jan C. Depner
    03/25/10

    Fixed a color range problem.  Fixed redraw of 2D track map when not needed.  Fixed overlay of flags after selection.


    Version 2.26
    Jan C. Depner, Micah Tinkler
    04/02/10

    Added ability to filter mask after running the filter while the filter confirmation box is still up.  This allows you to
    look at the points the filter has picked and mask some out prior to accepting the filter (it reruns the filter and 
    regenerates the filter confirmation dialog with the new filter count).  Filter confirmation is no longer optional.
    Standard deviation can be set in the filter confirmation dialog.


    Version 2.27
    Jan C. Depner, Micah Tinkler
    04/09/10

    When the filter message dialog is displayed DELETE_POINT mode snaps to filter kill points only.  Surrounding points
    used for waveWaterfall and other ancillary programs still come from all valid points.  Replaced hot keys for 
    KEEP_POLYGON, RESTORE_RECTANGLE, and RESTORE_POLYGON with hotkeys for FILTER, RECT_FILTER_MASK, and POLY_FILTER_MASK.
    Also, made the Enter key trigger the filter message dialog Accept action if the filter message dialog is present.


    Version 2.30
    Jan C. Depner
    04/14/10

    Added optional color scale to the right of the main window.


    Version 3.00
    Jan C. Depner
    04/20/10

    Completely rearranged the GUI.  Moved the 2D tracker, status info, and color scale to the left side.  Moved the slicer to the
    right side.


    Version 3.01
    Jan C. Depner
    04/21/10

    Fixed the ever elusive "deep flyer not showing up" problem.  Added ability to use Shift/mouse wheel to modify the filterMessage_dialog
    standard deviation value (had to play timer games to make it work right).


    Version 3.02
    Jan C. Depner
    04/22/10

    Added ability to mark files for deletion using the right click menu in DELETE_POINT mode.  Files are queued for deletion in pfmView using
    the Edit menu.  Also, cursor now tracks in main window when moved in 2D tracker window.


    Version 3.03
    Jan C. Depner
    05/03/10

    Moved the filter message dialog into a notebook page that is behind the 2D Tracker.  It only comes up after you have run the
    filter.


    Version 3.04
    Jan C. Depner
    05/04/10

    Fixed posfix and fixpos calls to use numeric constants instead of strings for type.


    Version 3.05
    Jan C. Depner
    06/03/10

    Added ability to display either manually invalid, filter invalid, or both instead of just displaying any invalid data.
    This helps a lot when processing LIDAR data since GCS invalidated points are marked as PFM_FILTER_INVAL while points removed
    in the editor or viewer by the filter (or manually) are marked as PFM_MANUALLY_INVAL.


    Version 3.06
    Jan C. Depner
    06/04/10

    Can now invalidate features in hotkey polygon mode.


    Version 3.07
    Jan C. Depner
    06/15/10

    Fixed resize bug when first entering window in DELETE_POINT mode.


    Version 3.08
    Jan C. Depner
    06/16/10

    Now changes all status information when the cursor is in the 2D tracker.


    Version 3.09
    Jan C. Depner
    06/25/10

    Added support for displaying PFM_HAWKEYE_DATA in the chartsMonitor ancillary program.


    Version 3.10
    Jan C. Depner
    06/28/10

    Changed chartsMonitor to lidarMonitor due to multiple lidar support.


    Version 3.11
    Jan C. Depner
    07/06/10

    HSV min and max colors are now passed in on the command line so that they can be set by pfmView or CZMIL.  Also,
    HSV min and max locked values can be optionally passed in on the command line.


    Version 3.12
    Jan C. Depner
    09/02/10

    Added ability to use the text search string, text search invert, and text search type from pfmView.


    Version 3.13
    Jan C. Depner
    09/08/10

    Changes to handle Qt 4.6.3.


    Version 3.14
    Jan C. Depner
    09/17/10

    Fixed bug displaying feature info.


    Version 3.15
    Gary Morris (USM)/Jan C. Depner
    09/20/10

    Merge of USM CZMIL code.


    Version 3.16
    Jan C. Depner
    10/08/10

    Removed select/unselect soundings options since we don't do that in PFM_ABE anymore.
    You can still flag them though.


    Version 3.17
    Jan C. Depner
    10/20/10

    Fixed tracker display to check for the PFM being displayed when multiple PFMs are up.


    Version 3.18
    Jan C. Depner
    10/26/10

    Fixed sign on Z value for modes other than DELETE_POINT.


    Version 4.00
    Jan C. Depner
    11/05/10

    Finally fixed auto Z scaling in nvMapGL (nvutility library).  Also added minimum Z extents to preferences.


    Version 4.01
    Jan C. Depner
    12/03/10

    Added rmsMonitor to ancillary programs.  This program reads smrmsg files associated with HOF or TOF and displays the
    RMS values for the record.


    Version 4.02
    Jan C. Depner
    12/06/10

    Cleaned up buttonText, buttonAccel, buttonIcon, and button array handling.  Made selection of multiple lines  work
    in the display instead of having a list of the lines.


    Version 4.03
    Jan C. Depner
    12/09/10

    Fixed the slice and size bar buttons (finally).


    Version 4.10
    Gary Morris (USM), Jan C. Depner
    12/13/10

    Added Gary's changes to support the attributeViewer AV_DISTANCE_TOOL.  Also, we can finally mark data points in a somewhat
    proper way.  They don't disappear when you rotate or zoom.


    Version 4.11
    Gary Morris (USM), Jan C. Depner
    12/15/10

    Can now highlight and un-highlight in polygons.  Also, pressing the Enter key when data is highlighted will
    invalidate the highlighted points.


    Version 4.12
    Jan C. Depner
    12/22/10

    Optional contours in the 2D tracker map.  Maintain aspect ratio when displaying data.


    Version 4.13
    Jan C. Depner
    12/30/10

    Ctrl + arrow keys now does a move of the area being edited the same way that the 2D editor does it.


    Version 4.14
    Jan C. Depner
    01/03/11

    Removed displaySingle button and replaced with undisplaySingle (i.e. hide single).  Changed operation of 
    displayMultiple to select last line with double click instead of just ending displayMultiple mode.


    Version 4.15
    Jan C. Depner
    01/04/11

    Added new color array for color by line mode.  It now uses twice the number of colors.  The color wheel
    values go from 200 to 0/360 to 300 twice.  The first half of the color array uses value set to 255 (full color).
    The second half uses value set to 64 (darker).  This gives us a better color spread in order to differentiate
    between lines.


    Version 4.16
    Jan C. Depner
    01/05/11

    Added hotkey check in the preferences dialog to prevent conflicts.  I really should have done this a long time
    ago but it was way down on the priority list ;-)


    Version 4.17
    Jan C. Depner
    01/06/11

    Correct problem with the way I was instantiating the main widget.  This caused an intermittent error on Windows7/XP.


    Version 4.18
    Jan C. Depner
    01/07/11

    Added the cursor distance, azimuth, and delta Z to the measure tool output (in the status bar).


    Version 4.19
    Jan C. Depner
    01/14/11

    Added an exaggeration scrollbar to the left side of the window.


    Version 4.20
    Jan C. Depner
    01/16/11

    Positioned non-modal popup dialogs in the center of the screen.


    Version 4.21
    Jan C. Depner
    02/10/11

    Removed the chartsSwap program from the list of ancillary programs.  We don't do that anymore.


    Version 4.22
    Jan C. Depner
    02/16/11

    Added hawkeyeMonitor to ancillary programs.


    Version 4.23
    Gary Morris (USM), Jan C. Depner
    02/18/11

    Merged Gary's changes to support updates to the attributeViewer.


    Version 4.24
    Jan C. Depner
    03/09/11

    Strip /PFMWDB:: off of input files in the ancillary programs stuff if we're working on the PFM World Data Base (PFMWDB).
    Just in case we might want to move input files to the PFMWDB directory and work on them.


    Version 4.28
    Jan C. Depner
    03/28/11

    Allow user to define the key for invalidating filter highlighted or manually highlighted data.  Try to save the user's
    hot key definitions even across setting version changes.


    Version 4.29
    Jan C. Depner
    03/30/11

    Added option (in Preferences) to turn auto-scaling on or off.  Removed NOOP functions (i.e. we now allow editing when displaying
    invalid data since you'll see it in the 2D tracker).  We originally turned off editing because it confused people when data 
    didn't disappear.


    Version 4.30
    Jan C. Depner
    04/06/11

    Brought back the displayLines list as a right click option when in SET_MULTIPLE mode.  Request from Paul Marin.


    Version 4.31
    Jan C. Depner
    04/07/11

    The min and max depth HSV values, colors, and locks are now handled in ABE shared memory and are no longer read in as
    arguments from pfmView.


    Version 4.32
    Jan C. Depner
    04/11/11

    Properly populate the file name when creating a new feature file.  It was leaving it blank on Windows.


    Version 4.40
    Jan C. Depner
    04/14/11

    Added the externalFilter function to support the (newly added) hofWaveFilter ancillary program.  Removed tofWaterKill.


    Version 4.50
    Jan C. Depner
    04/22/11

    The point cloud memory (POINT_CLOUD structure) is now allocated in shared memory.  This leaves a little slop around the 
    edges of the area to be edited and we have to pre-scan the bins to get the count but it will save a ton of memory and
    a bunch of I/O when running ancillary programs that need a large amount of data passed to them (mostly hofWaveFilter).
    There doesn't seem to be much of a hit in terms of speed when we do this.


    Version 4.51
    Gary Morris (USM)
    04/27/11

    Fixes to color handling in attributeViewerFunctions.cpp.


    Version 4.60
    Jan C. Depner
    04/28/11

    Flagging data points now makes the points "highlighted".  This allows you to delete flagged points.  You can also now
    restore highlighted invalid data points.


    Version 4.61
    Jan C. Depner
    05/05/11

    We no longer rescale or recolor range our data due to masking.  This actually makes sense when you think about it since the
    masked data is not invalid.  Thanx and a tip of the hat to McClain Walker (3001).  Filters (including hofWaveFilter) now
    honor the mask flag.  Happy Cinco de Mayo!!!


    Version 4.62
    Jan C. Depner
    05/09/11

    Fixed problem with pre-existing shared point cloud memory.  The memory may not have been detached if a previous
    editor process crashed.  Now it attaches the pre-existing memory, detaches it, and then creates a new point cloud 
    shared memory segment.  Fixed a serious FUBAR when looking for the nearest filter highlighted point.  Had to add
    a new function to nvMapGL.cpp in the nvutility library.  The function used in pfmEdit3D is getMarker3DCoords.
    Added invert highlight function and button.


    Version 4.63
    Jan C. Depner
    05/11/11

    Don't display features that are outside of the slice.


    Version 4.64
    Jan C. Depner
    05/13/11

    Added ability to lock min/max colors and values when coloring by attribute.


    Version 4.65
    Jan C. Depner
    05/18/11

    Fixed hide and show problems (due to incorrectly determining unique data types) when more than one PFM 
    is being edited.  Moved mask responsibility from hofWaveFilter back here.  Desensitize hide and show
    "highlighted" checkboxes in hide and show menus when no points are highlighted.  Fixed return from hofReturnKill,
    it was missing the "line" value in the sscanf of the shared file.


    Version 4.66
    Jan C. Depner
    05/20/11

    Added pmt and apd ac zero offset thresholds to the hofWaveFilter parameters.


    Version 4.67
    Jan C. Depner
    05/26/11

    Added highlight record (ping, shot) right click popup menu option.


    Version 4.68
    Jan C. Depner
    06/13/11

    Fix bug with attribute filter max and min values.


    Version 4.69
    Jan C. Depner
    06/27/11

    Fix bug when editing multiple PFMs.  Added menu to allow displaying of all, unverified, verified, or no features
    (as opposed to just on/off).


    Version 4.70
    Jan C. Depner
    07/08/11

    Fixed slice bug that was causing the slice to be one slice too far along when we started slicing.


    Version 4.71
    Jan C. Depner
    07/15/11

    Fixed bug changing minimum color for color-by options.


    Version 4.72
    Jan C. Depner
    07/19/11

    Added QMessageBox output for memory allocation error in get_buffer.c.


    Version 4.73
    Jan C. Depner
    07/22/11

    Fixed ancillary program button state bug.  If pfmEdit3D was started with ancillary programs up
    (state set to 2) it was resetting the state to 1 because the QProcess wasn't associated with
    the current instantiation of pfmEdit3D.  Now it checks for the state being 2 (already running)
    as well.


    Version 4.74
    Jan C. Depner
    07/29/11

    Added input of "pfmView killed" qsetting so that pfmEdit3D will know that pfmView was shut down
    since the last time it was running.  This allows us to properly set the "monitor" type ancillary
    program "running" states since pfmView issues a kill command to all of the associated ancillary
    programs when it exits.


    Version 4.75
    Jan C. Depner
    08/03/11

    Added CZMIL support in get_feature_event_time.cpp.


    Version 4.76
    Gary Morris (USM)
    08/11/11
	
    Added an AV Distance Threshold toolButton tied to the Attribute Viewer running.
    Implemented a new handshake between Attribute Viewer and Editor to select user-defined attribute color codes.
    Moved the selection of drawing a line for the AV Distance Threshold tool out of the track map and to the 3D map.
    Allowed for talking to multiple type attribute viewers.
	

    Version 4.77
    Jan Depner
    08/16/11
	
    Fix data type lookup table in get_buffer.cpp when there's more than 1 PFM open.
	

    Version 4.78
    Jan Depner
    08/31/11
	
    Changed waveMonitor to CZMILwaveMonitor.
	

    Version 4.79
    Jan Depner
    10/24/11
	
    Added fix for invalid highlights not showing up when you delete data while displaying
    invalid with invalid data flagged.


    Version 4.80
    Jan C. Depner
    11/30/11

    Converted .png icons to .png icons.


    Version 4.81
    Jan C. Depner
    12/29/11

    Fixed screenshot.cpp to grab both screens on Windows.  It will also scale the windows if
    they aren't the same height.


    Version 4.82
    Jan C. Depner
    03/06/12

    Added a tooltip to the main window whenever you have highlighted or filter kill points.
    I kept forgetting what key to press to kill the points.  DOH!


    Version 4.90
    Jan C. Depner
    03/19/12

    Added a local Hockey Puck filter to the editor.  This was added after playing with the Point Cloud
    Library RadiusOutlierRemoval filter which worked well but was spherical in nature.  The Hockey Puck
    filter allows us to use different X/Y distances versus Z distances.  It also allows us to use the 
    horizontal and vertical uncertainty and (possibly in the future) line numbers to make our filter
    smarter.  Oh, and did I mention, the Hockey Puck filter is about twice as fast ;-)


    Version 4.91
    Jan C. Depner
    06/20/12

    Removed support for CZMIL until the formats are finished.


    Version 4.92
    Jan C. Depner
    07/17/12

    Added support for preliminary CZMIL data formats.  Changed APD and PMT tags to shallow and deep respectively.


    Version 4.93
    Jan C. Depner
    08/05/12

    - If a point is marked as PFM_SUSPECT, it gets unmarked if the point is marked invalid.
    - Pressing the Insert button while flagging data (suspect, selected, user, etc.) now unsets the associated flag.
      if it's marked as invalid (and Display Invalid is selected) it will still have the invalid flags cleared.


    Version 4.94
    Jan C. Depner
    09/18/12

    Fixed a bug in the detection of multi-line data in the filter.  I wasn't checking for multi-line inside
    the hockey puck.  I just checked for multi-line inside the footprint.  Needed to do both.


    Version 4.95
    Jan C. Depner (PFM Software)
    06/05/13

    - Display Null button is now no longer dependent on Display Manually Invalid or Display Filter Invalid buttons being set.
    - Fixed displaying of NULLS for CZMIL data.
    - Added Show and Hide options for CZMIL channels if only one PFM is loaded and only CZMIL data is loaded in that PFM.
    - Split up the LiDAR tool bar into the generic LiDAR toolbar and the CZMIL, CHARTS, GSF, and Hawkeye tool bars.
    - Added reprocess land, water, and user buttons to the CZMIL tool bar.


    Version 4.96
    Jan C. Depner (PFM Software)
    06/18/13

    - Did what I could to speed up the CZMIL reclassification code (moved recompute out of inner loop).
    - Added confirmation dialog to the CZMIL reclassification buttons.


    Version 4.97
    Jan C. Depner (PFM Software)
    06/19/13

    - Added restore_all to the Preferences->Miscellaneous dialog so that people working on CZMIL data can make the 
      restore functions just restore manually invalid data instead of both manually and filter invalidated data.


    Version 4.98
    Jan C. Depner (PFM Software)
    06/20/13

    - Made the CZMIL reprocessing options blazingly fast by moving the "search and destroy" part to an external 
      batch program (for the moment called pfm_czmil_prep).
    - Added undo to the CZMIL reprocessing options.
    - Due to the above, removed the confirmation dialogs from the CZMIL reprocessing options.


    Version 4.99
    Jan C. Depner (PFM Software)
    06/27/13

    - Changed the Accept/Reject filter buttons to Delete/Cancel (requested by ACE).
    - Changed the reprocessing output file to carry the channel instead of the subrecord.


    Version 5.00
    Jan C. Depner (PFM Software)
    08/14/13

    - Added CZMIL data type to the chartsPic list of supported data types.


    Version 5.01
    Jan C. Depner (PFM Software)
    08/29/13

    - Added cpfPreFilter (the CZMIL pre-pulse return filter) to the list of associated ancillary programs.


    Version 5.02
    Jan C. Depner (PFM Software)
    09/04/13

    - Added a button to set the view to level.  That is, remove tilt from the user's view.  This is handy for removing
      LiDAR water surfaces.


    Version 5.03
    Jan C. Depner (PFM Software)
    09/06/13

    - Fix my self inflicted CZMIL pre-pulse filter hotkey conflict.  DOH!
    - Fixed the Hawkeye monitor button so it is only active if Hawkeye data is present.


    Version 5.04
    Jan C. Depner (PFM Software)
    12/09/13

    Switched to using .ini file in $HOME (Linux) or $USERPROFILE (Windows) in the ABE.config directory.  Now
    the applications qsettings will not end up in unknown places like ~/.config/navo.navy.mil/blah_blah_blah on
    Linux or, in the registry (shudder) on Windows.


    Version 5.05
    Jan C. Depner (PFM Software)
    01/02/14

    Got rid of the restore_all flag and instead use the Display Manually Invalid and Display Filter Invalid button
    states (and their related flags, optionsdisplay_man_invalid and options.display_flt_invalid) to determine
    what invalid data to restore when using the Restore Rectangle or Restore Polygon functions.


    Version 5.06
    Jan C. Depner (PFM Software)
    01/10/14

    Added CZMILDEEPFILTER external program to filter deep returns where there is no shallow central channel surface return.


    Version 5.07
    Stacy Johnson
    01/11/2014

    Added upto three digit precision to HP filter radius and height


    Version 5.08
    Jan C. Depner (PFM Software)
    01/17/14

    Added 0, 6, 7, 8, and 9 to the action keys for CZMILwaveMonitor.


    Version 5.09
    Jan C. Depner (PFM Software)
    01/21/14

    - Increased internal action key searches from 10 to 20 to accomodate the switches for CZMILwaveMonitor.
    - Changed CZMIL reprocess User to reprocess shallow water.


    Version 5.10
    Jan C. Depner (PFM Software)
    01/23/14

    - CZMIL show/hide options now work with multiple PFMs.
    - Added 'c' key to CZMILwaveMonitor action keys to toggle "current channel" mode.


    Version 5.11
    Jan C. Depner (PFM Software)
    02/22/14

    - Added hotkeys for color by : horizontal uncertainty, vertical uncertainty, and attributes.


    Version 5.12
    Jan C. Depner (PFM Software)
    02/26/14

    Cleaned up "Set but not used" variables that show up using the 4.8.2 version of gcc.


    Version 5.13
    Jan C. Depner (PFM Software)
    03/17/14

    Removed WLF support.  Top o' the mornin' to ye!


    Version 5.14
    Jan C. Depner (PFM Software)
    03/19/14

    - Changed the pointer to the ABE user guide HTML file from PFM/Area_Based_Editor.html to PFM_ABE_User_Guide.html.
    - Straightened up the Open Source acknowledgments.


    Version 5.15
    Jan C. Depner (PFM Software)
    03/21/14

    Removed rmsMonitor button (not used anymore).


    Version 5.16
    Jan C. Depner (PFM Software)
    05/06/14

    During testing of PFMv6 it was discovered that the increase in the number of input files plus the increase
    in the number of open PFM files caused a problem in get_buffer.cpp with a static array of NV_INT16 that
    was dimensioned to [MAX_PFM_FILES][PFM_MAX_FILES].  MAX_PFM_FILES is the maximum allowed number of open
    PFM files and PFM_MAX_FILES is the maximum number of input files in a single PFM. This was increased to
    150 and 8192 respectively which gave us a static array of 2,457,600 bytes.  For some reason Windows has
    a problem with very large static arrays.  Since the array was pretty darn large I just calloc'ed the
    array instead of declaring it.  An additional advantage of this is that I got to free the array when I
    was done with it.


    Version 5.17
    Jan C. Depner (PFM Software)
    05/07/14

    Fixed a couple of string literal bugs.


    Version 5.18
    Jan C. Depner (PFM Software)
    05/27/14

    - Added the new LGPL licensed GSF library to the acknowledgments.


    Version 5.19
    Jan C. Depner (PFM Software)
    06/19/14

    - Removed PFMWDB support.  No one was using it.  It seemed like a good idea but I guess not.


    Version 5.20
    Jan C. Depner (PFM Software)
    06/25/14

    - Replaced all printf statements with fprintf (stdout, and then added fflush (stdout) after them.


    Version 5.21
    Jan C. Depner (PFM Software)
    06/30/14

    - Replaced all of the old, borrowed icons with new, public domain icons.  Mostly from the Tango set
      but a few from flavour-extended and 32pxmania.


    Version 5.22
    Jan C. Depner (PFM Software)
    07/05/14

    - Had to change the argument order in pj_init_plus for the UTM projection.  Newer versions of 
      proj4 appear to be very sensitive to this.


    Version 5.23
    Jan C. Depner (PFM Software)
    07/13/14

    - Added LAS data type to lidarMonitor ancillary program data types.


    Version 5.24
    Jan C. Depner (PFM Software)
    07/23/14

    - Switched from using the old NV_INT64 and NV_U_INT32 type definitions to the C99 standard stdint.h and
      inttypes.h sized data types (e.g. int64_t and uint32_t).


    Version 5.26
    Jan C. Depner (PFM Software)
    07/29/14

    - Fixed errors discovered by cppcheck.


    Version 5.27
    Jan C. Depner (PFM Software)
    11/24/14

    - Added CZMIL reprocessing module to work with the CZMIL reprocess buttons.


    Version 5.28
    Jan C. Depner (PFM Software)
    12/09/14

    - Added minimum bounding rectangle checks to set_area in an attempt to speed up point inside polygon checking.


    Version 5.29
    Jan C. Depner (PFM Software)
    12/18/14

    - Added CZMIL V1 reprocessing warning box.  If the user tries to reprocess CZMIL V1 files and there are shots
      that can't be reprocessed due to too few returns in the original shot, this warning will be displayed.


    Version 6.00
    Jan C. Depner (PFM Software)
    12/25/14

    - After running czmilReprocess pfmEdit3D now sends a regen signal to pfmView or pfmEditShell to tell it to restart the ditor with
      the same bounds as before.
    - Replaced all (but one) realloced arrays with std::vector to address speed issues on Windows.


    Version 6.01
    Jan C. Depner (PFM Software)
    12/28/14

    - Taking advantage of a change to get2DCoords in nvMapGL.cpp in the nvutility library.  Basically, it no longer makes calls to
      get the viewport, mvmatrix, and projmatrix except the first time it's called for the loop of points.  This really shows up
      in Windows but it actually makes the Linux version faster as well.


    Version 6.02
    Jan C. Depner (PFM Software)
    12/30/14

    - Fixed bug in slotHideMenu.


    Version 6.03
    Jan C. Depner (PFM Software)
    12/31/14

    - Added option to make hide/show persistent across edit sessions.


    Version 6.04
    Jan C. Depner (PFM Software)
    01/28/15

    - Made the filters (statistical, hockey puck (TM), and attribute) *NOT* invalidate hidden data.  Thanx and
      a tip of the hat to Brandon Ellis (NGC).


    Version 6.05
    Jan C. Depner (PFM Software)
    02/11/15

    - Got rid of the compile time CZMIL naming BS.  It now checks the ABE_CZMIL environment variable at run time.


    Version 6.06
    Jan C. Depner (PFM Software)
    02/12/15

    - czmilReprocess.cpp now checks for the existence of every possible czmil_lidar_params.dat file and aborts if
      it's not found.  This keeps the czmilReprocess program from deleting points and then erroring out.


    Version 6.07
    Jan C. Depner (PFM Software)
    02/13/15

    - To give better feedback to shelling programs in the case of errors I've added the program name to all
      output to stderr.


    Version 6.08
    Jan C. Depner (PFM Software)
    02/15/15

    - More error message cleanup.  All non-fatal error messages and fatal error messages from child processes (like
      lidarMonitor) will now be displayed in a message dialog that will automatically pop up when a message is sent to
      stderr.


    Version 6.09
    Jan C. Depner (PFM Software)
    02/19/15

    - Added the "no invalid" mode as a command line option.  This mode will not load invalid data to be edited.


    Version 6.10
    Jan C. Depner (PFM Software)
    02/22/15

    - Added "Save" button for the error messages dialog.


    Version 6.11
    Jan C. Depner (PFM Software)
    03/06/15

    - Somehow, along the way, I managed to break the Hockey Puck (TM) filter by setting it to always check the "slice".
      How I managed to do that is beyond me.  Anyway, it's fixed now.


    Version 6.12
    Jan C. Depner (PFM Software)
    03/07/15

    - Replaced the last two realloc calls with std::vector.


    Version 6.13
    Jan C. Depner (PFM Software)
    03/14/15

    - Since AHAB Hawkeye has switched to LAS format I have removed support for the old Hawkeye I binary format.


    Version 6.14
    Jan C. Depner (PFM Software)
    03/18/15

    - Damn!  I forgot to decrement the number of tool bars when I got rid of the Hawkeye tool bar.  Caused a crash,
      now fixed.


    Version 6.15
    Jan C. Depner (PFM Software)
    03/19/15

    - Added Northing and Easting back to the cursor position display.


    Version 6.16
    Jan C. Depner (PFM Software)
    03/20/15

    - LAS data (not LAZ) can now use the lidarMonitor program.


    Version 6.17
    Jan C. Depner (PFM Software)
    03/22/15

    - Added the LASwaveMonitor program as an ancillary data monitor.


    Version 6.18
    Jan C. Depner (PFM Software), Jim Hammack (New Wave Systems)
    04/03/15

    - Computes zone prior to defining UTM projection so that we don't need PROJ_LIB set (I think).


    Version 6.19
    Jan C. Depner (PFM Software)
    06/26/15

    - Offset the CZMIL channel number by one when displaying in the attribute fields so that it matches the 
      channel strings from the CZMIL API.


    Version 6.20
    Jan C. Depner (PFM Software)
    06/27/15

    - Fixed PROJ4 init problem.


    Version 6.21
    Jan C. Depner (PFM Software)
    06/30/15

    - Widened 2D tracker box so that I could use the full attribute names as labels.  That was one of my
      own pet peeves ;-)  Oh, BTW, happy new leap second!


    Version 6.22
    Jan C. Depner (PFM Software)
    07/02/15

    - Changed attribute filter to use strings of comma separated ranges instead of the min and max sliders.
      It's much easier this way!
    - Made buttons for both the Hockey Puck and statistical filters so that the user doesn't have to select one or the other in
      the preferences tab.
    - Made automatic filter masking when exiting pfmEdit3D optional (set in Preferences).
    - Added "Hide" button to attribute filter notebook tab.


    Version 6.23
    Jan C. Depner (PFM Software)
    07/05/15

    - Allow filter masking in slice mode.
    - Make filter masking work for attribute filter.  Don't know why I didn't do that before.


    Version 6.24
    Jan C. Depner (PFM Software)
    07/06/15

    - Added a SIMPLE histogram plot as the second tab after the 2D tracker.  If you'd rather see a histogram of
      the data just manually switch to the histogram tab and it will come up automatically until you switch back.
      If you want to see what the values in the histogram are just hover the mouse over it and the X and Y values
      will be displayed in the tool tip.
    - Thanx and a tip of the hat to C. Wayne Wright for the histogram idea.
    - Fixed "color by" icon and text bug.
    - Added option to ignore masked (hidden) data when rescaling colors.  Normally all valid data is considered for
      color scaling whether it's masked or not.


    Version 6.25
    Jan C. Depner (PFM Software)
    07/11/15

    - Fixed manual filter masking on exit (whch I broke two versions ago).
    - Added "Apply" button to the Hockey Puck filter tab so that you don't have to figure out how to apply changes.


    Version 6.26
    Jan C. Depner (PFM Software)
    07/12/15

    - Made the Hockey Puck filter multithreaded.  It now runs 1, 4, or 16 threads depending on the setting in the Preferences dialog.
      The default is 4 threads.  Fixed manual filter masking on exit (whch I broke two versions ago).


    Version 6.27
    Jan C. Depner (PFM Software)
    07/14/15

    - Fixed the scale min/max value and color locking for attributes (as opposed to depth).
    - Clear filter selections when filter parameters are changed and re-applied.
    - Made histogram reflect scale min/max value and color locks.
    - Made alternate background color for information labels match standard background.
    - Fixed the Preferences dialog so that it would properly scale the Z offset if the Z scaling factor was changed.
    - Made line numbers work like any other attribute (colors, filters, etc.)


    Version 6.30
    Jan C. Depner (PFM Software)
    07/18/15

    - Made hide by area (polygon/rectangle inside/outside) separate from hide by attribute/flag/filter/highlighted.  I did
      this by making the "mask" field of the point cloud data a bit mask instead of a single true/false flag.  Now there are
      two "hide" reset buttons so that you can hide data by attribute, make it persistent, but not clear it if you want to
      hide by area and clear those settings.  One side feature of this was that we no longer need the "show" button or menu
      since we can now combine hide by area with hide by attribute without having to play silly buggers with the showing
      and hiding.  Now the hide menu allows you to hide or un-hide.
    - Hard-wired the information area fields background and text colors to white, gray, and black so that changes in the 
      system window colors won't do weird things to them.
    - Properly saves the filter ranges for all attributes so that the proper ranges will be loaded if a PFM with the 
      attributes in a different order is opened.


    Version 6.31
    Jan C. Depner (PFM Software)
    07/21/15

    - Removed the CZMIL attributeViewer from the ancillary programs.  Nobody was using it and it carried a bunch of kruft
      along with it.
    - Removed the option to change the points from little squares to spheres or diamonds or some other rubbish.  I'm not
      interested in making the dots pretty, I'm interested in speed!  Making the points some form of object slowed the
      program to a crawl in the interest of esthetics.  I call BS on that.


    Version 6.32
    Jan C. Depner (PFM Software)
    07/22/15

    - Moved attributes.cpp and attributes.hpp to the utility library since they're used in multiple programs.
      Now I only have to modify them in one place.


    Version 6.33
    Jan C. Depner (PFM Software)
    08/02/15

    - Got rid of redundant color array for transparent data.
    - Added option to set data outside of the slice to gray, semi-transparent instead of colored, semi-transparent.


    Version 6.34
    Jan C. Depner (PFM Software)
    08/05/15

    - Removed "color by" buttons and moved that functionality to the attribute name labels (e.g. Depth, Point count, StdDev,
      or a PFM NDX attribute name.  Whichever field is currently being used for "color by" will have a background fading from red
      on the right to the normal background on the left.


    Version 6.35
    Jan C. Depner (PFM Software)
    08/07/15

    - Made gray outside of slice the default.
    - Added option for coloring invalid data in the "Edit" color.
    - Made scaleBox use the attribute format when coloring by attribute.


    Version 6.36
    Jan C. Depner (PFM Software)
    08/13/15

    - Two freaking heisenbugs - it took me two days to find the stinking things!


    Version 6.40
    Jan C. Depner (PFM Software)
    08/14/15

    - Removed old UNISIPS code.
    - Added option to Preferences to display Z values to the user as either elevations or depths.
    - Happy VJ day!


    Version 6.41
    Jan C. Depner (PFM Software)
    08/18/15

    - Fixed bug in set_area.cpp.  It was deleting data that was hidden.


    Version 6.42
    Jan C. Depner (PFM Software)
    08/19/15

    - Hard-wired hotkey Ctrl+d to color by depth/elevation and Ctrl+l to color by line number.


    Version 6.43
    Jan C. Depner (PFM Software)
    08/21/15

    - Added "shift slice" mode.  If you press the Shift key, press the left mouse button, and then slide the cursor upward it will slice
      almost exactly like grabbing and sliding the Slice scrollbar slider.


    Version 6.44
    Jan C. Depner (PFM Software)
    09/02/15

    - Fixed color by line mode so that the colors aren't so close together.  There may be some confusion if displaying multiple
      PFMs but there's not much that can be done about that.
    - Made the "Hide by type" and the "Hide by flag" masks different so that the user can hide and clear by either separately.  This allows you
      to hide/un-hide based of e.g. CZMIL land flag and hide specific CZMIL channels.
    - Made the "Hide by type/flag" menu a tear-off so that the user could change hide options without having to pull down the menu for each change of
      options.


    Version 6.45
    Jan C. Depner (PFM Software)
    09/11/15

    - Added "invert" buttons to the hide menu for "flags" and "types".
    - Added hard-wired hot keys for "color-by" options.
    - Let's roll!  Never forget!


    Version 6.46
    Jan C. Depner (PFM Software)
    09/29/15

    - Another shot at trying to get the line colors separated.  This appears to work better.  If you have more than about ten lines
      it's still hard to differentiate some of them.


    Version 6.47
    Jan C. Depner (PFM Software)
    10/01/15

    - Finally got the line colors fixed.  I had to add a field to the POINT_CLOUD structure in the utility library's 
      ABE.h file to get it to work properly.


    Version 6.48
    Jan C. Depner (PFM Software)
    10/14/15

    - Fixed bug that only happened in Windows when trying to display NULL time attribute value.


    Version 6.50
    Jan C. Depner (PFM Software)
    10/28/15

    - Moved the "Hide" menu to a tab behind the "status" area on the left of the window.  This was the only reasonable way to make
      the "Hide" menu always available and always in the same place.
    - Added the "Make Hidden Data tab visible" option so that the user can have the Hidden Data tab come up at startup if the options
      are set in the Preferences dialog and there is hidden data.


    Version 6.51
    Jan C. Depner (PFM Software)
    11/04/15

    - Added Z range limits to the Hockey Puck, statistical, and attribute filters.
    - Made better What's This help for the Hockey Puck filter button.  It has a diagram of the Hockey Puck with annotated parameters.
    - Made the Preferences dialog open on the last used tab used instead of just defaulting to the first tab.


    Version 6.52
    Jan C. Depner (PFM Software)
    11/06/15

    - Modified scale factors for CZMIL interest points to match change to CZMIL API (from 10.0 to 100.0).


    Version 6.53
    Jan C. Depner (PFM Software)
    12/03/15

    - Added multi-threaded AVT (Antonyan Vardan Transform) statistical filter option (a real statistical filter).
    - Fixed the multi-threaded hockey puck filter - it wasn't filtering completely to the top or right sides of the area,
    - Made AVT and Hockey Puck filters honor the feature radius exclusion.
    - Added passes for Hockey Puck and AVT filters.
    - Removed filter parameters from the Preferences dialog (those that are settable from the various filter tabs).


    Version 6.54
    Jan C. Depner (PFM Software)
    12/28/15

    - Added option to turn off czmilReprocess warning messages.


    Version 6.55
    Jan C. Depner (PFM Software)
    01/12/16

    - Added CPF filter button, associated tab in the Histogram area, and the cpfDeepFilter program run from the CPF filter button.
    - Moved displayMessage from pfmEdit3D.cpp to an external QDialog class, displayMessage.cpp.


    Version 6.56
    Jan C. Depner (PFM Software)
    02/11/16

    - Fixed the explanation of what the CPF filter does when validating deep returns.  I had to change the program to do what
      Nick wanted it to do instead of what he asked for it to do ;-)


    Version 6.57
    Jan C. Depner (PFM Software)
    02/14/16

    - Added support for CZMIL CWF interest point amplitude attribute.
    - Happy Valentine's day!


    Version 6.60
    Jan C. Depner (PFM Software)
    04/24/16

    - Added ability to set LiDAR (CZMIL and/or LAS) classification values.
    - You can now use the "Clear highlighted/marked points in polygon" mode to unselect filter marked points.  This includes data points
      marked for filtering by the area-based filter, the AVT statistical filter, the hockey puck filter, the attribute filter, the CZMIL
      CPF filter, the CHARTS HOF waveform filter, and the change LiDAR classification filter.  Using this instead of the filter mask
      modes is a lot faster and easier to see.  The filter mask modes still work but you should probably only use them prior to filtering.


    Version 6.61
    Jan C. Depner (PFM Software)
    04/29/16

    - Added CHARTS HOF and CHARTS TOF to the types of LiDAR for which classification values may be changed.


    Version 6.62
    Jan C. Depner (PFM Software)
    07/12/16

    - Added 3 new CZMIL attributes that were added in version 3.0 of the CZMIL API.  These are CZMIL D_index,
      CZMIL D_index_cube, and CZMIL User data.
    - Removed experimental CWF interest point amplitude attribute.  D_index will work better and it comes
      right out of the CPF file so there isn't a huge amount of overhead.


    Version 6.63
    Jan C. Depner (PFM Software)
    08/03/16

    - Turned contours back on in 3D view (when requested).  They can be dispayed either at the top of the 3D
      space (default) or within the point cloud.


    Version 6.64
    Jan C. Depner (PFM Software)
    08/08/16

    - Now gets font from globalABE.ini as set in Preferences by pfmView.
    - Fixed problem with preferences not opening on the last used tab when a value had been changed previously.
    - To avoid possible confusion, removed translation setup from QSettings in env_in_out.cpp.


    Version 6.65
    Jan C. Depner (PFM Software)
    08/15/16

    - Changed "-" to ">" in the attribute filter strings so that we could use negative values as part of a range.


    Version 6.66
    Jan C. Depner (PFM Software)
    08/18/16

    - Now properly marks and unmarks points nearest to a feature (based on confidence).


    Version 6.67
    Jan C. Depner (PFM Software)
    08/20/16

    - Added support for Binary Feature Data (BFD) 3.0 feature type field.  Now, features will be marked or unmarked
      in the data based on whether the feature in question is Hydrographic (marked) or Informational (unmarked).
      This is in addition to unmarking for confidence set to 0.


    Version 6.68
    Jan C. Depner (PFM Software)
    08/21/16

    - Now uses PFM_DESIGNATED_SOUNDING for child features.


    Version 6.69
    Jan C. Depner (PFM Software)
    12/19/16

    - Added orthorectified image key to chartsPic action keys.


    Version 6.70
    Jan C. Depner (PFM Software)
    04/07/17

    - Removed check for features.roi for when preparing to reprocess CZMIL data.  It's no longer required for
      the CZMIL reprocessing API.


    Version 6.71
    Jan C. Depner (PFM Software)
    04/26/17

    - Switched to using the TEMP directory (from QDir::tempPath) for temporary shared_file.
    - Added better error messages if creating the shared_file causes an error (like being unable to write in
      your own home folder).


    Version 6.72
    Jan C. Depner (PFM Software)
    05/31/17

    - Fixed bug for HOFWaveFilter external program (undefined slot).
    - Disabled filter masks if any filter is already in use.  The capability to use the "Clear highlighted/marked
      points in polygon" button to clear filter arked points was added in version 6.60.
    - Cleaned up context help for the filter and filter mask buttons.


    Version 6.73
    Jan C. Depner (PFM Software)
    06/04/17

    - Fixed a possible problem with opening and reading HOF and TOF files.  It probably isn't a problem.


    Version 6.74
    Jan C. Depner (PFM Software)
    06/06/17

    - Changed hofReturnKill to kill secondary returns instead of doing low slope waveform filtering.


    Version 6.75
    Jan C. Depner (PFM Software)
    06/08/17

    - Fixed labeling of CZMIL user flags.


    Version 6.76
    Jan C. Depner (PFM Software)
    09/20/17

    - A bunch of changes to support doing translations in the future.  There is a generic
      pfmEdit3D_xx.ts file that can be run through Qt's "linguist" to translate to another language.
    - Replaced all the hard-coded browser starting code with a call to QDesktopServices.  DOH!


    Version 6.77
    Jan C. Depner (PFM Software)
    04/18/18

    - Added changes to the CZMIL reprocessing code to limit the CZMIL processing modes to just land
      (CZMIL_OPTECH_CLASS_LAND), water (CZMIL_OPTECH_CLASS_WATER), and shallow water
      (CZMIL_OPTECH_CLASS_SHALLOW_WATER).
    - Added rectangle select to CZMIL reprocessing.
    - Added CZMIL processing mode button to allow user to limit the data to be reprocessed.
    - Fixed the LiDAR classification change so that it now actually works!
    - Now saves the LiDAR classification "from" classes between calls.
    - If the user changes modes while in LiDAR classification or CPF filter modes, the dialog will
      now go back to the histogram or 2D tracker.
    - Replaced the Layers menu button and the manageLayers dialog with a notebook page.  It now
      looks more like pfmView and will be less confusing to the user (not to mention the simpler
      code).
    - Added no_reference flag to go along with the no_invalid flag.


    Version 6.78
    Jan C. Depner (PFM Software)
    05/16/18

    - Added IR back to CZMIL reprocessing logic.


    Version 6.79
    Jan C. Depner (PFM Software)
    06/03/18

    - Added Invert button to attribute filter.


    Version 6.80
    Jan C. Depner (PFM Software)
    07/17/18

    - Added LAS 1.4 attributes to the LAS attributes


    Version 6.90
    Jan C. Depner (PFM Software)
    07/03/19

    - Support for PFMv7.  This is mostly for the 20 attributes and 10 user flags.
      Actually, most of these changes were done in May/June but I forgot to update
      this file.  Also, there was a bug in how the attributes were handled across
      instances of the editor and I just fixed that.


    Version 6.91
    Jan C. Depner (PFM Software)
    07/10/19

    - Added support for using PFM_USER_06 for Hockey Puck filtered CZMIL data.


    Version 6.92
    Jan C. Depner (PFM Software)
    07/21/19

    - Added CZMIL Urban noise flags attribute.


    Version 6.93
    Jan C. Depner (PFM Software)
    07/22/19

    - Fixed the histogram tooltip for integer attributes.
    - Corrected typo in attribute filter box.


    Version 6.94
    Jan C. Depner (PFM Software)
    08/18/19

    - Added CZMIL Urban Noise Filter (also known as the Nick Johnson filter ... it actually
      works).
    - Added code to display CZMIL Urban noise flags as binary and to display a custom tool tip for it
      ('cause it's kinda confusin').


    Version 6.95
    Jan C. Depner (PFM Software)
    08/20/19

    - Updated CZMIL Urban Noise Filter to the latest algorithm tweaks from Nick Johnson.


    Version 6.96
    Jan C. Depner (PFM Software)
    08/22/19

    - Changed the filters (HP, AVT, area, attr, CZMIL Urban) so that they don't run automatically when you press
      the button.  Changed the 'Apply' button to 'Run' and updated the documentation (i.e. Help).


    Version 6.97
    Jan C. Depner (PFM Software)
    08/28/19

    - Fixed display of flagged data when turning display of invalid/null data on/off.
 
  Version 6.98
  Russ R. Johnson (OptimalGEO)
  01/25/21
  
  - Added Save without Exit (SHOM Request #11 from 05/29/20 request document) 

</pre>*/
