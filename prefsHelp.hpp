
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



QString bGrpText = 
  prefs::tr ("Select the format in which you want all geographic positions to be displayed.");

QString orientBoxText = 
  prefs::tr ("Select the orientation for displaying Z values.  Either <b>Depth</b> or <b>Elevation</b>.");

QString depthCheckText = 
  prefs::tr ("Check this box if you want Z values to be displayed as depths (positive Z is down).<br><br>"
             "<b>IMPORTANT NOTE: This orientation value is shared by some of the ABE programs (e.g. pfmView). "
             "If you set it here it will be used by the other programs.</b>");

QString elevationCheckText = 
  prefs::tr ("Check this box if you want Z values to be displayed as elevations (positive Z is up).<br><br>"
             "<b>IMPORTANT NOTE: This orientation value is shared by some of the ABE programs (e.g. pfmView). "
             "If you set it here it will be used by the other programs.</b>");

QString ZFactorText = 
  prefs::tr ("Set the scaling factor for contours <b><i>and</i></b> all displayed depth/elevation values "
             "in pfmView and pfmEdit3D.  The actual values aren't changed just the displayed values.  "
             "This is useful for comparing depths/elevations to GeoTIFFs of scanned charts from NOAA (feet) "
             "or the US Navy (fathoms).");

QString offsetText = 
  prefs::tr ("Set the offset value for contours <b><i>and</i></b> all displayed depth/elevation values "
             "in pfmView and pfmEdit3D.  The actual values aren't changed just the displayed values.  "
             "This is useful for applying estimated ellipsoid offsets prior to the final datum "
             "shift being applied to the data.<br><br>"
             "<b>IMPORTANT NOTE: The offset value is <i>added</i> to the Z value and is applied <i>after</i> "
             "scaling so the units are in scaled units.</b>");

QString egmText = 
  prefs::tr ("Press this button to retrieve the EGM08 model value of the ellipsoid to geoid (approximate mean sea "
             "level) datum offset for the point nearest the center of the minimum bounding rectangle of the displayed "
             "area.");

QString pointSizeText = 
  prefs::tr ("Set the size of each displayed point in pixels.");

QString contourIntText = 
  prefs::tr ("Set the contour interval to be used by pfmView and also pfmEdit3D.  Setting this "
             "to 0.0 will cause the user defined contour levels to be used.  No contours will "
             "be plotted unless the plot contour button <img source=\":/icons/contour.png\"> has been "
             "toggled on.");

QString contourSmText = 
  prefs::tr ("Set the contour smoothing factor by pfmView and also pfmEdit3D.  This value goes "
             "from 0 (no smoothing) to 10 (max smoothing).");

QString locBoxText = 
  prefs::tr ("Select where you would like contours to be displayed.  Checking <b>Top</b> will cause the contours to be displayed at the "
             "top of the 3D display space.  Checking <b>Point cloud</b> will cause the contours to be drawn within the point cloud.");

QString topCheckText = 
  prefs::tr ("Checking this box will cause contours to be displayed at the top of the 3D display space.");

QString cloudCheckText = 
  prefs::tr ("Checking this box will cause contours to be drawn within the point cloud.");

QString WidthText = 
  prefs::tr ("Set the contour line width/thickness in pixels.  Index contours (if set) will be twice this size.");

QString sparseText = 
  prefs::tr ("Set the point limit at which we will plot sparse points for rotation, zoom, and slice.  When we have more than "
             "this number of points loaded the program will only display the min and max points from each PFM bin when "
             "the image is rotated or zoomed.  In addition, when slicing, areas outside of the active slice will only display "
             "the sparse points.  The default is 500,000.");

QString scaleText = 
  prefs::tr ("Check this box to draw the scale on the screen.");

QString czmilWarningsText = 
  prefs::tr ("If this box is checked, warnings issued by czmilReprocess will not be displayed in the parent applications message box. "
             "Normally, you should have this checked.  The only time that you might want to turn the messages back on is if you suspect "
             "that there is a problem with czmilReprocess.  Unchecking this box will slow down the czmilReprocess application and it "
             "will disable the scroll bar status if you receive a message.");

QString autoScaleText = 
  prefs::tr ("If this box is checked, data will be rescaled to fit the window whenever there is a change in the displayed data.  "
             "If this box is un-checked data will not be rescaled unless you turn invalid data display on or off.  Note that "
             "rescaling is limited by the <b>Minimum Z window size</b>.");

QString minZText = 
  prefs::tr ("Set the minimum Z extents.  This will be in the units that the data is in (almost always meters).  The "
	     "purpose is to keep data with little Z variability from filling the screen with <b><i>fuzz</i></b> by limiting "
	     "the self scaling of the Z axis to this minimum value.<br><br>"
	     "<b>Important note: If the combo box to the left of this is set to anything other than <i>Set Manually -></i> "
	     "then the max of this value and the computed IHO TVU will be used as the minimum Z window size.</b>");

QString ihoOrderText = 
  prefs::tr ("Set the minimum Z extents manually or based on IHO order.  If this is set to anything other than "
	     "<b>Set Manually -></b> the minimum Z extents will be set based on the S44 IHO order TVU computations.  "
	     "These are:<br><br>"
	     "<ul>"
	     "<li>Special Order   -    0.25m + 0.0075 * depth/elevation</li>"
	     "<li>Order 1a/1b     -    0.50m + 0.0130 * depth/elevation</li>"
	     "<li>Order 2         -    1.00m + 0.0230 * depth/elevation</li>"
	     "</ul>"
	     "<br><br>The depth/elevation value used in the above equations will be the minimum depth/elevation displayed.<br><br>"
	     "<b>Important note: The maximum of the computed TVU and the manually set minimum Z window size will be used "
	     "so that, in very shallow water, we don't shut the window down too far.  If you always want to use the computed TVU "
	     "set the minimum value to 0.1 or some other very small number.</b>");

QString iconSizeText = 
  prefs::tr ("Set the size of the main window buttons.  The options are 16, 20, 24, 28, or 32 pixels.<br><br>"
             "<b>IMPORTANT NOTE: The icons size will change when you click the <i>Apply</i> button but "
             "the locations will not be entirely correct until you have exited and restarted the program.</b>");

QString overlapText = 
  prefs::tr ("Set the window overlap percentage that will be used when you use the Ctrl-arrow keys to move the "
	     "edit area.");

QString rotText = 
  prefs::tr ("Set the view rotation increment in degrees.  This value is the amount the alternate views will rotate "
             "when the left or right arrow keys are pressed.");

QString fSizeText = 
  prefs::tr ("Set the feature marker size.  The default is 0.006.");

QString zoomPercentText = 
  prefs::tr ("Set the percentage value for zooming the data view in or out.");

QString prefsUndoText = 
  prefs::tr ("Set the maximum number of undo levels.  This is the number of edit operations that can be <b><i>undone</i></b>. "
             "The value ranges from a reasonable 100 to a ridiculous 1,000,000.  The higher this value is set, the more memory "
             "you use to save the undo information.");


QString killText = 
  prefs::tr ("Check this box if you would like ancillary programs to exit when you close the pfmEdit3D window and then "
             "respawn when you start it again.  This only works for the following LIDAR programs:<br><br>"
             "<ul>"
             "<li>waveformMonitor</li>"
             "<li>chartsPic</li>"
             "<li>chartsMonitor</li>"
             "<li>waveWaterfall (Shallow)</li>"
             "<li>waveWaterfall (Deep)</li>"
             "<li>CZMILwaveMonitor</li>"
             "</ul><br><br>"
             "<b>IMPORTANT NOTE: The programs should respawn in the same locations, however, if there is overlap, the "
             "order of the windows may not be preserved.  Also, if you are running Compiz (desktop effects) on Linux "
             "using dual monitors, the program locations may not restore correctly.</b>");

QString tipText = 
  prefs::tr ("Check this box if you would like to see the delete/clear highlighted points tool tip when highlighted or "
             "filter kill points are present on the screen.  If you already know the hot keys for these actions and the "
             "tool tip is annoying the hell out of you, uncheck this box.");

QString hspText = 
  prefs::tr ("Check this box if you would like the hide by type options that you have selected to be put into effect the next "
             "time you start the editor.  Please remember that doing this may cause you to open the editor onto an "
             "apparently empty window.  The hide clear button <img source=\":/icons/clear_mask_type.png\"> will be flashing "
             "when you start the editor to let you know that some (or possibly all) of your data is hidden.");

QString hmvText = 
  prefs::tr ("Check this box if you would like the <b>Hidden Data</b> tab to be visible on startup.  This will only happen if the "
             "<b>Make hide by attribute persistent</b> option has been selected and there is hidden data.");

QString dFilterText = 
  prefs::tr ("Set this check box to indicate that data will only be filtered in the downward direction when running the "
             "filter.");

QString featureRadiusText = 
  prefs::tr ("Set the distance around a feature within which filtering will not be allowed.  Setting this to 0 turns "
	     "off <b>ALL</b> feature protection during filtering.  This includes the feature polygon exclusion.");

QString hpThreadsText = 
  prefs::tr ("This is the number of threads to be used to run the Hockey Puck or AVT filters.  Most modern systems have at least 4 cores "
             "available so the default of 4 is probably fine.  If you have a system with 16 or more cores you can go ahead and "
             "set this to 16.  The more threads you use the faster the filter runs.");

QString aFilterText = 
  prefs::tr ("Set this check box to indicate that you want pfmView to automatically filter mask this area on exit if you have run any filter "
             "while in the editor");

QString contourLevelsText = 
  prefs::tr ("Set individual contour levels.  Pressing this button will set the standard contour "
             "interval to 0.0 and bring up a table dialog to allow the user to insert contour "
             "levels (one per line, ascending).  The default levels are IHO standard contour "
             "levels.");

QString contoursTableText = 
  prefs::tr ("Use this table to set individual contour levels.  Levels should be set in ascending "
             "order.");


QString hotKeysText = 
  prefs::tr ("Click this button to change the accelerator key sequences associated with some of "
             "the function buttons.");

QString dataTypeTableText = 
  prefs::tr ("Select the data types that the command will work on.  If you want the command to work "
             "regardless of the data type of the current point (for instance, in the case of the "
             "mosaic file) choose Undefined.");

QString closeDataTypesText = 
  prefs::tr ("Click this button to close the data types dialog.");

QString hotKeyTableText = 
  prefs::tr ("Accelerator key sequences (hot keys) for some of the buttons in the main window may be changed "
             "in this dialog.  Up to four comma separated key values with modifiers may be used.  The format is "
             "illustrated by the following examples:<br><br>"
             "<ul>"
             "<li>q</li>"
             "<li>Shift+q</li>"
             "<li>Ctrl+q</li>"
             "<li>Meta+q</li>"
             "<li>Ctrl+Alt+q</li>");

QString closeHotKeyText = 
  prefs::tr ("Click this button to close the hot key dialog.");


QString closeContoursText = 
  prefs::tr ("This button closes the contour level dialog.");

QString colorText = 
  prefs::tr ("Set color options in this tab.");

QString noMaskedCheckText = 
  prefs::tr ("Check this box to ignore hidden (masked) data when computing the Z scale and the color range.  Inother words, the Z scale and the "
             "color range will be computed from only those points that are visible after hiding has occurred.  Normally, this is not checked and "
             "hidden data are used to compute the scale and color ranges.<br><br>"
             "<b>IMPORTANT NOTE: When this is checked and the histogram is displayed, the histogram will be recomputed when data are gidden.  This "
             "may seem counterintuitive but the histogram is based on the computed color range.  When this is not checked, the histogram won't "
             "change when you hide data.</b>");

QString graySliceCheckText = 
  prefs::tr ("Check this box to set data outside of the slice to gray, semi-transparent instead of colored (based on the color-by option). "
             "Transparency is still set by the slice transparency slider on the right side of the window.  Sometimes it is easier to see the "
             "data in the slice when using gray for the other data.");

QString whiteInvalidCheckText = 
  prefs::tr ("Check this box to show invalid data in the <b>Edit</b> color.<br><br>"
             "<b>Please be aware that NULL data and reference data are always shown in the </b>Edit<b> color.  Those settings are not optional so, "
             "if you are viewing NULL and/or reference data and invalid data at the same time, there may be some confusion if you have set this "
             "check box.</b>");

QString conText = 
  prefs::tr ("Set depth/elevation display options in this tab");

QString kboxText = 
  prefs::tr ("Set the hot keys in this tab.");

QString filtText = 
  prefs::tr ("Set the filter options in this tab");

QString miscText = 
  prefs::tr ("This tab contains settings for a number of miscellaneous options including the number of undo levels, "
             "the feature size, and the rotation increment.");

QString contourColorText = 
  prefs::tr ("Click this button to change the color that is used to plot contours.  "
             "After selecting a color the map will be redrawn.");

QString trackerColorText = 
  prefs::tr ("Click this button to change the color that is used to plot the <i>other ABE window</i> tracker.  "
             "After selecting a color the map will be redrawn.");

QString editColorText = 
  prefs::tr ("Click this button to change the color that is used to draw rectangles and polygons on the screen "
             "when performing edit operations.  After selecting a color the map will be redrawn.");

QString markerColorText = 
  prefs::tr ("Click this button to change the color that is used to draw markers (for highlighting or masking).  "
             "After selecting a color the map will be redrawn.");

QString backgroundColorText = 
  prefs::tr ("Click this button to change the color that is used as background.  After "
             "selecting a color the map will be redrawn.");

QString refColorText = 
  prefs::tr ("Click this button to change the color that is used to represent reference or NULL data.  After "
             "selecting a color the map will be redrawn.");

QString featureColorText = 
  prefs::tr ("Click this button to change the color that is used to plot features.  "
             "After selecting a color the map will be redrawn.");

QString featureInfoColorText = 
  prefs::tr ("Click this button to change the color that is used to write the feature information "
             "next to the featuresplot features.  After selecting a color the map will be redrawn.");

QString highFeatureColorText = 
  prefs::tr ("Click this button to change the color that is used to plot highlighted features.  The features that will be highlighted "
             "are determined by the text search, text search invert, and text search type that are set in pfmView.  "
             "After selecting a color the map will be redrawn.");

QString verFeatureColorText = 
  prefs::tr ("Click this button to change the color that is used to plot verified features (confidence = 5).  "
             "After selecting a color the map will be redrawn.");

QString scaleColorText = 
  prefs::tr ("Click this button to change the color that is used for the scale.  After "
             "selecting a color the map will be redrawn.");

QString scaledOffsetZColorText = 
  prefs::tr ("Click this button to change the color that is used to draw Z values in the information box when the Z value is being "
             "scaled and/or offset (for example meters to feet and/or EGM2008 Z offset aplied).  This willalso change the color used to "
             "plot the numbers on the box scale if it is being displayed.");

QString waveColorText = 
  prefs::tr ("Click any of the numbered cursor buttons to change the color of the multiple cursors that "
             "are associated with the CZMILwaveMonitor and waveWaterfall programs.  The <b>0</b> color will be the "
             "color of the single cursor/marker.  The waveform colors in CZMILwaveMonitor and waveWaterfall will "
             "use these colors as well.");

QString restoreDefaultsText = 
  prefs::tr ("Click this button to restore all of the preferences to the original default values.  "
             "This includes <b><i>all</i></b> of the system preferences, not just those in the "
             "preferences dialog.");

QString closePrefsText = 
  prefs::tr ("Click this button to discard all preference changes and close the dialog.  The main map and "
             "coverage map will not be redrawn.");

QString applyPrefsText = 
  prefs::tr ("Click this button to apply all changes and close the dialog.  The main map and coverage map will "
             "be redrawn.");

QString ancillaryProgramsText = 
  prefs::tr ("Click this button to change ancillary programs, hot keys, action keys, and data types associated "
             "with each program.");

QString programText = 
  prefs::tr ("This is the external program that will be run when you press the associated hot key.  The hot keys and "
	     "action keys may be changed but they must be unique.  For information on the programs look at "
	     "<b>Hot Keys</b> in the main program's <b>Help</b> menu.");

QString hotkeyText = 
  prefs::tr ("The hot key is the single key that will initiate the associated program.  These must be unique.  Keys with modifiers may be used.  The format is "
             "illustrated by the following examples:<br><br>"
             "<ul>"
             "<li>q</li>"
             "<li>Shift+q</li>"
             "<li>Ctrl+q</li>"
             "<li>Meta+q</li>"
             "<li>Ctrl+Alt+q</li>"
             "</ul>");

QString actionkeyText = 
  prefs::tr ("The action key is a single key or a group of comma (,) separated single keys that will be passed to the "
             "associated program as command line arguments of the form --actionkeyNN=key.  Action keys are used via "
             "shared memory to cause some form of action to be taken in the ancillary program and pfmEdit.  Programs "
             "that use action keys are specially built to access PFM shared memory.  The following are examples of action keys:<br><br>"
             "<ul>"
             "<li>q</li>"
             "<li>Shift+q</li>"
             "<li>Ctrl+q</li>"
             "<li>Meta+q</li>"
             "<li>Ctrl+Alt+q</li>"
             "<li>q,s,1,2,3,4</li>"
             "<li>Ctrl+q,Alt+s</li>"
             "</ul><br><br>"
             "<b>IMPORTANT NOTE: If you change the action keys for an ancillary program that is "
             "already running you must kill the ancillary program and restart it for the new action keys "
             "to take effect.</b>");

QString closeAncillaryProgramText = 
  prefs::tr ("Click this button to close the ancillary programs dialog.");

QString czmilText = 
  prefs::tr ("Set the options for handling CZMIL data in this tab.");
