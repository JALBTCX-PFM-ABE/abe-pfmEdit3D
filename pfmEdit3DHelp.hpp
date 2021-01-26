
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



QString exitSaveText = 
  pfmEdit3D::tr ("<img source=\":/icons/exit_save.png\"> Click this button to save changes to the PFM structure and "
                 "then exit from the editor.");
QString exitMaskText = 
  pfmEdit3D::tr ("<img source=\":/icons/exit_mask.png\"> Click this button to save changes to the PFM structure and "
                 "then exit from the editor.  In addition, the area that was edited will be marked as a filter "
                 "masked area in pfmView.");
QString saveNoExitText = 
  pfmEdit3D::tr ("<img source=\":/icons/save_no_exit.png\"> Click this button to save changes but NOT exit from the editor");

QString exitNoSaveText = 
  pfmEdit3D::tr ("<img source=\":/icons/exit_no_save.png\"> Click this button to discard changes and then exit from "
                 "the editor.");

QString resetText = 
  pfmEdit3D::tr ("<img source=\":/icons/reset_view.png\"> Click this button to reset to the original scale and view.");

QString levelText = 
  pfmEdit3D::tr ("<img source=\":/icons/set_view_level.png\"> Click this button to set the view to level.  This will remove "
                 "all tilt from the user's view.  This is handy for removing LiDAR water surfaces.");

QString northUpText = 
  pfmEdit3D::tr ("<img source=\":/icons/north_up.png\"> Click this button to set the view to plan view (looking directly down) and north up.");

QString displayFeatureText = 
  pfmEdit3D::tr ("<img source=\":/icons/display_all_feature.png\"> Click this button to select the feature display mode.  "
               "You can highlight features that have descriptions or remarks containing specific text strings by setting "
               "the feature search string in the preferences dialog <img source=\":/icons/prefs.png\">.");
QString displayChildrenText = 
  pfmEdit3D::tr ("<img source=\":/icons/displayfeaturechildren.png\"> Click this button to display feature sub-records.  "
                 "Feature sub-records are features that have been grouped under a master feature record.  Features "
                 "can be grouped and un-grouped in the edit feature dialog of pfmView.");
QString displayFeatureInfoText = 
  pfmEdit3D::tr ("<img source=\":/icons/displayfeatureinfo.png\"> Click this button to write the description and "
                 "remarks fields of features next to any displayed features.  This button is "
                 "meaningless if <b>Flag Feature Data</b> is set to not display features "
                 "<img source=\":/icons/display_no_feature.png\">.");
QString verifyFeaturesText = 
  pfmEdit3D::tr ("<img source=\":/icons/verify_features.png\"> Click this button to set all valid, displayed features to a confidence "
                 "level of 5.  These will be displayed in a different color in the editor and in pfmView.  The only way to "
                 "<b><i>unverify</i></b> a feature is to edit the feature and manually set the confidence level to a lower value than 5.");


QString displayReferenceText = 
  pfmEdit3D::tr ("<img source=\":/icons/displayreference.png\"> Click this button to flag reference data.  Reference points "
                 "may or may not be invalid but they are not used for computing the surfaces or as selected features.  "
                 "Examples of data that might be flagged as reference data would include fishing nets, floating buoys, "
                 "non-permanent platforms, depth/elevation data from sidescan systems, or other data that are valid but not "
                 "normally used as part of the surfaces.");
QString displayManInvalidText = 
  pfmEdit3D::tr ("<img source=\":/icons/display_manually_invalid.png\"> Click this button to display manually invalidated "
                 "data points along with the valid data points.  If you need to differentiate between valid and invalid you "
                 "can use the data flags options to mark invalid data points.<br><br>"
                 "<b>IMPORTANT NOTE: Data that has been marked as invalid using the filter in the editors or the surface "
                 "viewer are marked as MANUALLY invalid.  Since you are reviewing the filter results this is considered manually "
                 "accepting the filter.  Points that are filtered in pfmLoad or that are marked as invalid by other "
                 "software packages (e.g. Optech's GCS) are marked as FILTER invalid.<br><br>"
                 "OTHER IMPORTANT NOTE: The state of this button will effect how the <i>Restore Rectangle</i> function "
                 "<img source=\":/icons/restore_rect.png\"> and the <i>Restore Polygon</i> function <img source=\":/icons/restore_poly.png\">"
                 "work.  For more information see the help for the <i>Restore</i> buttons.</b>");
QString displayFltInvalidText = 
  pfmEdit3D::tr ("<img source=\":/icons/display_filter_invalid.png\"> Click this button to display filter invalidated "
                 "data points along with the valid data points.  If you need to differentiate between valid and invalid you "
                 "can use the data flags options to mark invalid data points.<br><br>"
                 "<b>IMPORTANT NOTE: Data that has been marked as invalid using the filter in the editors or the surface "
                 "viewer are marked as MANUALLY invalid.  Since you are reviewing the filter results this is considered manually "
                 "accepting the filter.  Points that are filtered in pfmLoad or that are marked as invalid by other "
                 "software packages (e.g. Optech's GCS) are marked as FILTER invalid.<br><br>"
                 "OTHER IMPORTANT NOTE: The state of this button will effect how the <i>Restore Rectangle</i> function "
                 "<img source=\":/icons/restore_rect.png\"> and the <i>Restore Polygon</i> function <img source=\":/icons/restore_poly.png\">"
                 "work.  For more information see the help for the <i>Restore</i> buttons.</b>");
QString displayNullText = 
  pfmEdit3D::tr ("<img source=\":/icons/display_null.png\"> Click this button to display data points that were loaded "
                 "as NULL values.  This is primarily useful for LIDAR data but it will also display data "
                 "points that fell outside the min/max envelope on load (these were set to NULL).  This "
                 "button is only meaningful if display manually invalid <img source=\":/icons/display_manually_invalid.png\"> "
                 "or display filter invalid <img source=\":/icons/display_filter_invalid.png\"> is set to on.");

QString displayAllText = 
  pfmEdit3D::tr ("<img source=\":/icons/displayall.png\"> Click this button to display all lines after selecting "
                 "a single line or a subset of lines to view.");

QString undisplaySingleText = 
  pfmEdit3D::tr ("<img source=\":/icons/undisplaysingle.png\"> Click this button to turn off the display for a single line.  "
                 "When selected the cursor will become the ArrowCursor.  Move the box and arrow cursor to a point in the line "
                 "that you do not want to display and then left click.  The display will redraw after each left click.  This mode "
                 "will remain active until you switch to another mode.  After selecting lines to be hidden you can "
                 "revert back to displaying all data by pressing the <b>View All</b> button "
                 "<img source=\":/icons/displayall.png\">.");

QString displayMultipleText = 
  pfmEdit3D::tr ("<img source=\":/icons/displaylines.png\"> Click this button to select multiple lines to view.  "
                 "When selected the cursor will become the ArrowCursor.  Move the box and arrow cursor to a point "
                 "in a line that you want to display and then left click to select that line to view.  Double click "
                 "to select the last line to view and redraw the display.  If you decide that you don't want to display "
                 "the lines you can middle click to abort.  After selecting multiple lines to view you can revert back "
                 "to displaying all data by pressing the <b>View All</b> button <img source=\":/icons/displayall.png\">.<br><br>"
                 "<b>IMPORTANT NOTE: If you would like to select the lines from a list you can do so by selecting the "
                 "obvious option in the right click popup menu.</b>");

QString highlightPolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/highlight_polygon.png\"> Click this button to select a polygonal area "
                 "inside of which you wish to highlight data points.  When selected the cursor will become the highlight polygon cursor "
                 "<img source=\":/icons/highlight_polygon_cursor.png\">.  Left clicking on a location will cause that point to be "
                 "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                 "highlight the data in the polygon simply left click again.  If, at any time during the operation, "
                 "you wish to discard the polygon and abort the operation simply click the middle mouse "
                 "button or right click and select a menu option.<br><br>"
                 "<b>IMPORTANT NOTE: If you would like to invalidate highlighted data points just press <i>Del</i> (or whatever "
                 "hot key that has been set for <i>DELETE FILTER HIGHLIGHTED</i> in the Preferences dialog).  Conversely, "
                 "if you have invalid data being displayed and you have highlighted some of them you can press <i>Ins</i> (or "
                 "whatever hot key that has been set for <i>CANCEL FILTER HIGHLIGHTED</i> in the Preferences dialog) to validate "
                 "those points.  If you were flagging data (i.e. highlighting by data flag), clicking this button will turn off "
                 "data flagging.</b>");
QString invertHighlightText = 
  pfmEdit3D::tr ("<img source=\":/icons/invert_highlight.png\"> Click this button to invert the highlighted selection.  That is, "
                 "all points that are currently highlighted will be un-highlighted and all points that aren't currently highlighted "
                 "will be highlighted.<br><br>"
                 "<b>IMPORTANT NOTE: If you would like to invalidate highlighted data points just press <i>Del</i> (or whatever "
                 "hot key that has been set for <i>DELETE FILTER HIGHLIGHTED</i> in the Preferences dialog).  Conversely, "
                 "if you have invalid data being displayed and you have highlighted some of them you can press <i>Ins</i> (or "
                 "whatever hot key that has been set for <i>CZNCEL FILTER HIGHLIGHTED</i> in the Preferences dialog) to validate "
                 "those points.  This button will not be enabled if you are flagging data (i.e. highlighting by data flag).</b>");
QString clearPolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/clear_polygon.png\"> Click this button to select a polygonal area inside of which you wish to "
                 "un-highlight data points that have been highlighted or data points that are marked for filtering. This includes data "
                 "points marked for filtering by the area-based filter, the AVT statistical filter, the hockey puck filter, the attribute "
                 "filter, the CZMIL CPF filter, the CHARTS HOF waveform filter, and the change LiDAR classification filter.  When selected "
                 "the cursor will become the clear polygon cursor <img source=\":/icons/clear_polygon_cursor.png\">.  Left clicking on a "
                 "location will cause that point to be the polygon start point.  Moving the cursor will draw a continuous line.  To close "
                 "the polygon and un-highlight the data in the polygon simply left click again.  If, at any time during the operation, "
                 "you wish to discard the polygon and abort the operation simply click the middle mouse "
                 "button or right click and select a menu option.<br><br>"
                 "<b>IMPORTANT NOTE: If you would like to invalidate highlighted data points just press <i>Del</i> (or whatever "
                 "hot key that has been set for <i>DELETE FILTER HIGHLIGHTED</i> in the Preferences dialog).  Conversely, "
                 "if you have invalid data being displayed and you have highlighted some of them you can press <i>Ins</i> (or "
                 "whatever hot key that has been set for <i>CANCEL FILTER HIGHLIGHTED</i> in the Preferences dialog) to validate "
                 "those points.  This button will not be enabled if you are flagging data (i.e. highlighting by data flag).</b>");
QString clearHighlightText = 
  pfmEdit3D::tr ("<img source=\":/icons/clear_highlight.png\"> Click this button to clear all highlighted points from "
                 "the display.<br><br>"
                 "<b>IMPORTANT NOTE: If you were flagging data (i.e. highlighting by data flag) this button will not be enabled.  "
                 "To clear those highlighted points, just turn off data flagging.</b>");

QString contourText = 
  pfmEdit3D::tr ("<img source=\":/icons/contour.png\"> Click this button to toggle drawing of contours on the 2D Tracker and in the 3D "
                 "space.<br><br>"
                 "<b>IMPORTANT NOTE: Contours in the 3D space may be displayed either at the top of the display or in the point cloud.  "
                 "This can be changed in the </b><i>Depth/Elevation</i><b> tab of the </b><i>Preferences</i><b> dialog<br><br>"
                 "<img source=\":/icons/prefs.png\"></b>");


QString unloadText = 
  pfmEdit3D::tr ("<img source=\":/icons/unload.png\"> This toggle button controls whether pfmEdit3D will automatically unload "
                 "edits made to the PFM data to the original input files when you save and exit.");


QString prefsText = 
  pfmEdit3D::tr ("<img source=\":/icons/prefs.png\"> Click this button to change program preferences.  This "
                 "includes colors, contour interval, minimum Z window size, position display format, and all hot keys.");


QString messageText = 
  pfmEdit3D::tr ("This menu option will display a dialog containing any nonfatal error messages from this program and/or error "
                 "messages from ancillary programs that were started by this program.");

QString stopText = 
  pfmEdit3D::tr ("<img source=\":/icons/stop.png\"> Click this button to cancel drawing of the data.  A much easier "
                 "way to do this though is to click any mouse button in the display area or press any "
                 "key on the keyboard.  The stop button is really just there so that the interface looks "
                 "similar to the viewer and also to provide a place for help on how to stop the drawing.");


QString deletePointText = 
  pfmEdit3D::tr ("<img source=\":/icons/delete_point.png\"> Click this button to select invalidate subrecord/record/file "
                 "mode.  In this mode you can place the cursor on a subrecord (beam, shot, point) and "
                 "invalidate the subrecord by pressing the <Shift> key and then left clicking (or via the right click menu), "
                 "invalidate the record (ping, shot) by pressing the middle mouse button, or mark the file for invalidation in pfmView "
                 "using the right click menu.  Information about the current "
                 "point will be displayed in the status bars at the bottom of the window.<br><br>"
                 "You can also mark a point by using the right-click popup menu.  Note that the <b>Highlight point</b> option may be "
                 "disabled if you have filter kill points up or are flagging data using any of the flag buttons "
                 "<img source=\":/icons/user_flag.png\"><br><br>"
                 "Hot keys and action keys are available when in this mode.  Please check the <b>Preferences<b> dialog "
                 "<img source=\":/icons/prefs.png\">, specifically the <b>Ancillary Programs</b> dialog, to see what action "
                 "keys are available for each ancillary program.  To find out what the keys do in the ancillary program "
                 "you must use the help in the ancillary program.  You can also run ancillary programs by selecting the "
                 "program from the right click popup menu.  " 
                 "<p><b><i>IMPORTANT NOTE: Only in this mode, ADD_FEATURE mode <img source=\":/icons/addfeature.png\">, or "
                 "EDIT_FEATURE mode <img source=\":/icons/editfeature.png\"> can you use hot keys to launch ancillary "
                 "programs.</i></b></p>");
QString deleteRectText = 
  pfmEdit3D::tr ("<img source=\":/icons/delete_rect.png\"> Click this button to select a rectangular area "
                 "to invalidate.  When selected the cursor will become the invalidate rectangle cursor "
                 "<img source=\":/icons/delete_rect_cursor.png\">.  Left clicking on a location will cause that point to be "
                 "the rectangle anchor point.  Moving the cursor will cause a rectangle to appear.  To finalize the "
                 "rectangle and invalidate the data left click again.  If, at any time during the operation, you wish to "
                 "discard the rectangle and abort the operation simply click the middle mouse button or right click and "
                 "select one of the menu options.");
QString deletePolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/delete_poly.png\"> Click this button to select a polygonal area "
                 "to invalidate.  When selected the cursor will become the invalidate polygon cursor "
                 "<img source=\":/icons/delete_poly_cursor.png\">.  Left clicking on a location will cause that point to be "
                 "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                 "invalidate the data in the polygon simply left click again.  If, at any time during the operation, "
                 "you wish to discard the polygon and abort the operation simply click the middle mouse "
                 "button or right click and select a menu option.");
QString keepPolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/keep_poly.png\"> Click this button to select a polygonal area "
                 "and invalidate all data <b>outside</b> of the polygon.  When selected the cursor will "
                 "become the invalidate outside polygon cursor <img source=\":/icons/keep_poly_cursor.png\">.  Left "
                 "clicking on a location will cause that point to be the polygon start point.  To close the polygon and "
                 "invalidate the data outside of the polygon simply left click again.  If, at any time during the "
                 "operation, you wish to discard the polygon and abort the operation simply click the middle mouse "
                 "button or right click and select a menu option.");
QString restoreRectText = 
  pfmEdit3D::tr ("<img source=\":/icons/restore_rect.png\"> Click this button to select a rectangular area "
                 "in which to restore invalidated data.  When selected the cursor will become the restore rectangle "
                 "cursor <img source=\":/icons/restore_rect_cursor.png\">.  Left clicking on a location will cause that "
                 "point to be the rectangle anchor point.  Moving the cursor will cause a rectangle to appear.  To finalize "
                 "the rectangle and restore the invalid data simply left click again.  If, at any time during the "
                 "operation, you wish to discard the rectangle and abort the operation simply click the middle mouse button "
                 "or right click and select one of the menu options.<br><br>"
                 "<b>IMPORTANT NOTE: The state of the <i>Display Manually Invalidated</i> button "
                 "<img source=\":/icons/display_manually_invalid.png\"> and the <i>Display Filter Invalidated</i> button "
                 "<img source=\":/icons/display_filter_invalid.png\"> will effect "
                 "how this function works.  If either of these buttons is selected then only invalid data that "
                 "is actually being displayed will be restored.  So, if you want to restore only manually invalid data, just set the "
                 "<i>Display Manually Invalid</i> button <img source=\":/icons/display_manually_invalid.png\"> prior to restoring points.</b>");
QString restorePolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/restore_poly.png\"> Click this button to select a polygonal area "
                 "in which to restore invalidated data.  When selected the cursor will become the restore polygon cursor "
                 "<img source=\":/icons/restore_poly_cursor.png\">.  Left clicking on a location will cause that point to "
                 "be the polygon start point.  To close the polygon and restore invalid data in the polygon simply left "
                 "click again.  If, at any time during the operation, you wish to discard the polygon and abort the "
                 "operation simply click the middle mouse button or right click and select a menu option.<br><br>"
                 "<b>IMPORTANT NOTE: The state of the <i>Display Manually Invalidated</i> button "
                 "<img source=\":/icons/display_manually_invalid.png\"> and the <i>Display Filter Invalidated</i> button "
                 "<img source=\":/icons/display_filter_invalid.png\"> will effect "
                 "how this function works.  If either of these buttons is selected then only invalid data that "
                 "is actually being displayed will be restored.  So, if you want to restore only manually invalid data, just set the "
                 "<i>Display Manually Invalid</i> button <img source=\":/icons/display_manually_invalid.png\"> prior to restoring points.</b>");

QString referencePolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/reference_poly.png\"> Click this button to select a polygonal area "
                 "in which to set all points to reference points.  Reference points may or may not be invalid but they are "
                 "not used for computing the surfaces or as selected features.  Examples of data that might be flagged as "
                 "reference data would include fishing nets, floating buoys, non-permanent platforms, depth/elevation data from "
                 "sidescan systems, or other data that are valid but not normally used as part of the surfaces.  "
                 "When selected the cursor will become the set reference polygon cursor "
                 "<img source=\":/icons/reference_poly_cursor.png\">.  Left clicking on a location will cause that point to "
                 "be the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                 "set the data in the polygon to reference simply left click again.  If, at any time during the operation, "
                 "you wish to discard the polygon and abort the operation simply click the middle mouse "
                 "button or right click and select a menu option.<br><br>"
                 "<b>IMPORTANT NOTE: There is an extra right click popup menu option for this mode.  It allows you to invalidate "
                 "all non-masked (visible) or non-transparent reference points.  There is a stupid, Micro$oft-like <i>'Do "
                 "you really want to do this'</i> type of message when you use that option.</b>");
QString unreferencePolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/unreference_poly.png\"> Click this button to select a polygonal area "
                 "in which to set all reference points to non-reference points.  Reference points may or may not be "
                 "invalid but they are not used for computing the surfaces or as selected features.  Examples of data "
                 "that might be flagged as reference data would include fishing nets, floating buoys, non-permanent "
                 "platforms, depth/elevation data from sidescan systems, or other data that are valid but not normally used as part "
                 "of the surfaces.  When selected the cursor will become the unset reference polygon cursor "
                 "<img source=\":/icons/unreference_poly_cursor.png\">.  Left clicking on a location will cause that point "
                 "to be the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                 "set the reference data in the polygon to non-reference data simply left click again.  If, at any time "
                 "during the operation, you wish to discard the polygon and abort the operation simply click the middle "
                 "mouse button or right click and select a menu option.<br><br>"
                 "<b>IMPORTANT NOTE: There is an extra right click popup menu option for this mode.  It allows you to invalidate "
                 "all non-masked (visible) or non-transparent reference points.  There is a stupid, Micro$oft-like <i>'Do "
                 "you really want to do this'</i> type of message when you use that option.<b>");
QString hotkeyPolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/hotkey_poly.png\"> Click this button to select a polygonal area "
                 "in which to run an ancillary program that is started with a hotkey.  When selected the cursor will "
                 "become the hotkey polygon cursor <img source=\":/icons/hotkey_poly_cursor.png\">.  Left clicking on a "
                 "location will cause that point to be the polygon start point.  Moving the cursor will draw a continuous "
                 "line.  To close the polygon in preparation for pressing a hotkey simply left click again.  After defining "
                 "the polygon just press the hotkey that is associated with the ancillary program that you wish to run or select "
                 "the program from the right click popup menu.  You can also invalidate groups of features using this function.<br>"
                 "To see the available ancillary programs look under preferences <img source=\":/icons/prefs.png\">.  If, "
                 "at any time during the operation, you wish to discard the polygon and abort the operation simply click "
                 "the middle mouse button or right click and select a menu option.");

QString addFeatureText = 
  pfmEdit3D::tr ("<img source=\":/icons/addfeature.png\"> Click this button to add a feature to an existing "
                 "BFD feature file.  If a feature file does not exist you will be given the option of creating "
                 "one.  When selected the cursor will become the add feature cursor "
                 "<img source=\":/icons/add_feature_cursor.png\">.  "
                 "The box cursor will snap to the nearest point.  You may place a feature by left clicking on the desired "
                 "point.  When you left click a feature edit dialog will appear for you to modify.  If, at any time "
                 "during the operation, you wish to abort the operation simply click the middle mouse button or right "
                 "click and select one of the menu options.<br><br>"
                 "Hot keys and action keys are available when in this mode.  Please check the <b>Preferences<b> dialog "
                 "<img source=\":/icons/prefs.png\">, specifically the <b>Ancillary Programs</b> dialog, to see what action "
                 "keys are available for each ancillary program.  To find out what the keys do in the ancillary program "
                 "you must use the help in the ancillary program." 
                 "<p><b><i>IMPORTANT NOTE: Only in this mode, DELETE_POINT mode <img source=\":/icons/delete_point.png\">, "
                 "or EDIT_FEATURE mode <img source=\":/icons/editfeature.png\"> can you use hot keys to launch ancillary "
                 "programs.</i></b></p>");
QString editFeatureText = 
  pfmEdit3D::tr ("<img source=\":/icons/editfeature.png\"> Click this button to edit an existing feature.  "
                 "Note that you may only edit invalid features if you have invalid data viewing "
                 "<img source=\":/icons/display_manually_invalid.png\"> toggled on.  When selected the cursor will become the "
                 "edit feature cursor <img source=\":/icons/edit_feature_cursor.png\">.  The box cursor will snap to the "
                 "nearest feature.  "
                 "You may edit that feature by left clicking while the box cursor is attached to the desired feature.  "
                 "When you left click a feature edit dialog will appear for you to modify.  If, at any time during the "
                 "operation, you wish to abort the operation simply click the middle mouse button or right click and "
                 "select one of the menu options.<br><br>"
                 "Hot keys and action keys are available when in this mode.  Please check the <b>Preferences<b> dialog "
                 "<img source=\":/icons/prefs.png\">, specifically the <b>Ancillary Programs</b> dialog, to see what action "
                 "keys are available for each ancillary program.  To find out what the keys do in the ancillary program "
                 "you must use the help in the ancillary program.  Note that the 6th option for <b>ChartsPic</b> will "
                 "<b>ONLY</b> work in this mode.  The 6th option is used to get <b>chartsPic</b> to save a downlooking "
                 "image associated with a CHARTS HOF or TOF file.  The 6th option is normally <b><i>t</i></b> but may "
                 "have been changed by the user."
                 "<p><b><i>IMPORTANT NOTE: Only in this mode, DELETE_POINT mode <img source=\":/icons/delete_point.png\">, "
                 "or ADD_FEATURE mode <img source=\":/icons/addfeature.png\"> can you use hot keys to launch ancillary "
                 "programs.</i></b></p>");
QString moveFeatureText = 
  pfmEdit3D::tr ("<img source=\":/icons/movefeature.png\"> Click this button to move an existing feature to "
                 "another location.  When selected the cursor will become the move feature cursor "
                 "<img source=\":/icons/move_feature_cursor.png\">.  The box cursor will snap to the nearest feature.  "
                 "Acquire that feature by left clicking while the box cursor is attached to the desired feature.  After left "
                 "clicking the box cursor will then snap to the nearest point instead of the nearest feature.  To attach the "
                 "feature to that point simply left click again.  If, at any time during the operation, you wish to abort "
                 "the operation simply click the middle mouse button or right clicking and selecting an option from the "
                 "menu.");
QString deleteFeatureText = 
  pfmEdit3D::tr ("<img source=\":/icons/deletefeature.png\"> Click this button to invalidate an existing "
                 "feature.  This function will set the feature confidence value to 0.  When selected the cursor will become "
                 "the invalidate feature cursor <img source=\":/icons/delete_feature_cursor.png\">.  The box cursor will snap to "
                 "the nearest feature.  You may invalidate that feature by left clicking while the box cursor is attached to the "
                 "desired feature.  If, at any time during the operation, you wish to abort the operation simply click the "
                 "middle mouse button or right clicking and selectin an option from the menu.");


//  If you modify mapText (re GUI control), change the text in hotkeyHelp.cpp as well.

QString mapText = 
  pfmEdit3D::tr ("The pfmEdit3D program is used to display and edit data points that have been stored "
                 "in a PFM structure.  This program is launched from the pfmView program and is "
                 "not run from the command line.  The various editing and viewing options are "
                 "initiated from the tool bar buttons.<br><br>"
                 "The data points will be colored by the selected data field in the information window on the left.  The selected "
                 "field is indicated by its background fading from red on the right to the normal background color on the left. "
                 "To change the <b><i>color by</i></b> option just click on one of the fields or use the associated hot key.  The hot key "
                 "can be found by hovering over the field and reading the tool tip (hint: <i>Meta</i> is the "
                 "<i>Microsoft/Ubuntu/Penguin</i> key).<br><br>"
                 "Context sensitive help is available for almost all objects and areas in the window by selecting the What's This button "
                 "<img source=\":/icons/contextHelp.png\"> and then clicking on the item of interest.<br><br>"
                 "Point of view is controlled by holding down the <b>Ctrl</b> key and clicking and dragging with the "
                 "left mouse button.  It can also be controlled by holding down the <b>Ctrl</b> key and using the "
                 "left, right, up, and down arrow keys.  Zoom is controlled by holding down the <b>Ctrl</b> key and "
                 "using the mouse wheel or holding down the <b>Ctrl</b> key, pressing the right mouse button, and moving "
                 "the cursor up or down.  To reposition the center of the view just place the cursor at the desired "
                 "location, hold down the <b>Ctrl</b> key, and click the middle mouse button.  Z exaggeration can be "
                 "changed by pressing <b>Ctrl-Page Up</b> or <b>Ctrl-Page Down</b> or by editing "
                 "the exaggeration value in the Preferences dialog <img source=\":/icons/prefs.png\"><br><br><br>"
                 "Slicing of the data is initiated by using the mouse wheel, dragging the slider "
                 "in the scroll bar on the right of the display, by clicking the up or down arrow keys (without "
                 "pressing the <b>Ctrl</b> key), pressing the <b>Shift</b> key and the left mouse button and sliding the cursor, "
                 "or pressing the up and down arrow buttons in the slice scroll bar.<br><br>"
                 "<b>IMPORTANT NOTE: When in DELETE_POINT mode <img source=\":/icons/delete_point.png\">, "
                 "ADD_FEATURE mode <img source=\":/icons/addfeature.png\">, or EDIT_FEATURE mode "
                 "<img source=\":/icons/editfeature.png\"> hot keys and action keys will be active.  Use the Hotkeys Help "
                 "entry in the Help menu to see what all of the hotkeys and action keys are.  These keys may be changed "
                 "in the Preferences dialog if needed.");

QString trackMapText = 
  pfmEdit3D::tr ("This is the 2D tracking map.  It will show you where your cursor is in relation to the 3D cursor position.");

QString histText = 
  pfmEdit3D::tr ("This is the histogram of the data.  It will show distribution based on the <b><i>color by</i></b> attribute value.  "
                 "The X scale is minimum attribute value to maximum attribute value.  To view the X and Y value just hover the "
                 "cursor over the histogram plot and the values will be displayed in the tool tip.  There are twice as many bars "
                 "in the histogram as there are scale boxes on the left of the point information display area.");

QString exagBarText = 
  pfmEdit3D::tr ("This scroll bar controls/monitors the vertical exaggeration of the data.  Pressing the up and down "
                 "arrow keys will change the exaggeration by 1.0.  Clicking the mouse while the cursor is in the trough "
                 "will change the value by 5.0.  Dragging the slider will display the value in the <b>Exag:</b> field in "
                 "the status area on the left but the exaggeration will not change until the slider is released.  The "
                 "maximum value is 100.0 and the minimum value is 1.0.  If the exaggeration has been set to less than 1.0 "
                 "due to large vertical data range the scroll bar will be inactive.  The exaggeration can also be changed "
                 "by pressing the <b>Ctrl</b> key and then simultaneously pressing the <b>PageUp</b> or <b>PageDown</b> button.");

QString sliceBarText = 
  pfmEdit3D::tr ("This scroll bar controls/monitors the slice as it moves through the data.  Pressing the up and down "
                 "arrow keys will move the slice through the data.  The slice size will be the percent of the viewable data "
                 "that is set with the slice size scroll bar.  When you press the up arrow the view will move "
                 "into the data one slice.  You can achieve the same effect by turning the mouse scroll wheel.  If you want more "
                 "control over the slice movement you can press the <b>Shift</b> key, press the left mouse button, and then slide the "
                 "cursor up or down, or you can click on the slider in the slice scroll bar and move it up or down.  When displaying "
                 "a slice, any edits will only effect data that is in full color (non-translucent).  Slicing is extremely handy in "
                 "dealing with very bumpy surfaces to allow you to clean up hidden flyers.  You can turn off slicing by right clicking "
                 "and selecting <b>Turn off slicing</b>, by double clicking anywhere in the window, or by pressing the <b>Esc</b> key.");

QString sizeText = 
  pfmEdit3D::tr ("Set the slice size using this scroll bar.  To understand how slicing works try "
                 "to visualize the surface as it would be in plan view but with the bottom (leading) edge defined "
                 "by the angle of view.  The slice size will be this percentage (default is 5 percent or 1/20th) of "
                 "the displayed data.  When you press one of the up or down arrow keys or click one of the slice bar "
                 "arrow buttons the slice will move by this amount.  The range for this scroll bar is 1 to 50.");


QString transText = 
  pfmEdit3D::tr ("Set the transparency value to be used for data that is not in the current slice.  If "
                 "the slider is moved to the bottom, data outside the slice will be invisible.  If it is moved to the top "
                 "the data will be almost completely opaque.  The range for this scroll bar is 0 (transparent) to 64 "
                 "(semi-transparent).");

QString colorScaleBoxText = 
  pfmEdit3D::tr ("The color scale is an index for the color ranges.  The scale can be turned off in the preferences dialog.<br><br>"
                 "<b>IMPORTANT NOTE: When the color scheme is color by line the color scale is not applicable and will be set to "
                 "all gray with no values.</b>");


QString pfmEdit3DAboutText = 
  pfmEdit3D::tr ("<center>pfmEdit3D<br><br>"
                 "Author : Jan C. Depner (area.based.editor@gmail.com)<br>"
                 "Date : 31 January 2005<br><br>"
                 "The History Of PFM<br><br></center>"
                 "PFM, or Pure File Magic, was conceived on a recording trip to Nashville in early 1996.  "
                 "The design was refined over the next year or so by the usual band of suspects.  The "
                 "purpose of this little piece of work was to allow hydrographers to geographically view "
                 "minimum, maximum, and average binned surfaces, of whatever bin size they chose, and "
                 "then allow them to edit the original depth/elevation data.  After editing the depth/elevation data, the "
                 "bins would be recomputed and the binned surface redisplayed.  The idea being that the "
                 "hydrographer could view the min or max binned surface to find flyers and then just "
                 "edit those areas that required it.  In addition to the manual viewing and hand editing, "
                 "the PFM format is helpful for automatic filtering in places where data from different "
                 "files overlaps.  Also, there is a hook to display an associated GeoTIFF imagery file.<br><br>"
                 "pfmEdit3D is a very simple Qt binned surface viewer.  After all of the editing is finished, "
                 "the status information can be loaded back into the original raw input data files.");

QString miscLabelText = 
  pfmEdit3D::tr ("This area displays the currently active mode or information about the operation in progress.");

QString statusBarText = 
  pfmEdit3D::tr ("The status bar is used to display the progress of the current operation.");

QString progStatusText = 
  pfmEdit3D::tr ("The status bar is used to display the progress of the current operation.");

QString flagText = 
  pfmEdit3D::tr ("Click this button to access a pulldown menu allowing you to select data flagging options.  The "
                 "flagging options are:<br><br>"
                 "<ul>"
                 "<li><img source=\":/icons/user_flag.png\"> - Turn off data flagging</li>"
                 "<li><img source=\":/icons/suspect.png\"> - mark suspect data</li>"
                 "<li><img source=\":/icons/selected.png\"> - mark selected soundings (for CZMIL, LAS, CHARTS HOF, or CHARTS TOF data this "
                 "flag marks classification attribute changes)</li>"
                 "<li><img source=\":/icons/feature.png\"> - mark selected feature soundings</li>"
                 "<li><img source=\":/icons/designated.png\"> - mark hydrographer designated soundings</li>"
                 "<li><img source=\":/icons/flag_invalid.png\"> - mark invalid data (only visible if <b>Display invalid</b> is selected)</li>"
                 "<li><img source=\":/icons/user_flag01.png\"> - mark PFM_USER_01 flagged data</li>"
                 "<li><img source=\":/icons/user_flag02.png\"> - mark PFM_USER_02 flagged data</li>"
                 "<li><img source=\":/icons/user_flag03.png\"> - mark PFM_USER_03 flagged data</li>"
                 "<li><img source=\":/icons/user_flag04.png\"> - mark PFM_USER_04 flagged data</li>"
                 "<li><img source=\":/icons/user_flag05.png\"> - mark PFM_USER_05 flagged data</li>"
                 "</ul><br><br>"
                 "The PFM_USER_NN flags may be disabled if the PFM file being viewed does not have valid "
                 "user flags.<br><br>"
                 "<b>IMPORTANT NOTE: If you would like to invalidate data points highlighted using one of these options just "
                 "press <i>Del</i> (or whatever hot key that has been set for <i>DELETE FILTER HIGHLIGHTED</i> in the "
                 "Preferences dialog).  Conversely, if you have data points highlighted by one of these options you can "
                 "turn off the associated flag (or validate the point if you are marking invalid points) by pressing <i>Ins</i> (or "
                 "whatever hot key that has been set for <i>CANCEL FILTER HIGHLIGHTED</i> in the Preferences "
                 "<img source=\":/icons/prefs.png\"> dialog).</b>");

QString hideText = 
  pfmEdit3D::tr ("Click this button to switch the status area to the <b>Hidden Data</b> tab.  This button is superfluous since you can easily "
                 "switch to the <b>Hidden Data</b> tab by clicking on the tab itself.  This button was left here because it used to be a "
                 "pulldown menu with a tear-off.  This button will force the user to discover the <b>Hidden Data</b> tab");

QString hideMenuText = 
  pfmEdit3D::tr ("This menu allows the user to hide or show data based on PFM user flags and/or PFM data types.  The flags and data types are "
                 "not fixed (with the exception of the <b> Hide highlighted data</b> checkbox) but are based on the data that has been read "
                 "in to the pfmEdit3D application.  There are two invert buttons, <b>Invert flags</b> and <b>Invert types</b>.  These allow "
                 "the user to invert the checked boxes.  This is essentially equivalent to having a <b>Show</b> menu.<br><br>"
                 "There two clear buttons for this menu.  The first is the <b>Clear hide/show flag settings</b> "
                 "<img source=\":/icons/clear_mask_flag.png\"> button.  When this is blinking (indicating hidden data) clicking it will "
                 "re-display all points that have been hidden based on a specific PFM user flag or highlighting.  The second is the "
                 "<b>Clear hide/show type settings</b> <img source=\":/icons/clear_mask_type.png\"> button.  This will redisplay all points "
                 "that have been hidden based on data type (and/or channel in the case of CZMIL data).<br><br>"
                 "<b>IMPORTANT NOTE: The <i>Hidden Data</i> tab will be visible on startup if you have selected <i>Make hide by "
                 "attribute persistent</i> and <i>Make Hidden Data tab visible</i> in the Preferences <img source=\":/icons/prefs.png\"> "
                 "dialog.</b>");

QString czmilReprocessButtonsText = 
  pfmEdit3D::tr ("<img source=\":/icons/czmil_reprocess_buttons_11.png\"> Click this button in the brown or blue areas to select/deselect the "
                 "types of CZMIL data to be reprocessed when using the CZMIL reprocessing functions, for example <br><br>"
                 "<img source=\":/icons/czmil_reprocess_water_poly.png\"><br><br>"
                 "Brown represents land processed shots (i.e. processing mode less than CZMIL_OPTECH_CLASS_HYBRID), blue represents water "
                 "processed shots (i.e. processing mode greater than CZMIL_OPTECH_CLASS_HYBRID, including "
                 "CZMIL_OPTECH_CLASS_SHALLOW_WATER).<br><br>"
                 "When you deselect a type, the button icon will change.  For example, if you deselect water shot reprocessing the button "
                 "will look like this:<br><br>"
                 "<img source=\":/icons/czmil_reprocess_buttons_10.png\"><br><br>"
                 "If you deselect land shot reprocessing the button will look like this:<br><br>"
                 "<img source=\":/icons/czmil_reprocess_buttons_01.png\"><br><br>"
                 "<b>IMPORTANT NOTES: You will not be allowed to deselect both land and water (that makes no sense at all).<br><br>"
                 "No CZMIL_OPTECH_CLASS_HYBRID processed shots will ever be reprocessed.<br><br>"
                 "At present, you do not have to deselect the brown (land) button if you are reprocessing shots as land.  The application "
                 "will not send land processed shots to be reprocessed as land. The same holds true for water and shallow water processed "
                 "shots. So, at present, it is kind of useless to select only land shots to be reprocessed as land.  It should be "
                 "intuitively obvious to the most casual observer that these two buttons are really only applicable to reprocessing as "
                 "shallow water.  There is a possibility that, in the future, the user may be able to change some or all of the "
                 "reprocessing parameters so it may be possible to reprocess land as land (with different parameters than the original).");

QString czmilReprocessLandText = 
  pfmEdit3D::tr ("<img source=\":/icons/czmil_reprocess_land_poly.png\"> Click this button to select an area "
                 "inside of which you wish to mark points for reprocessing as land shots in HydroFusion.  When selected the cursor "
                 "will become the CZMIL reprocess polygon cursor <img source=\":/icons/czmil_reprocess_poly_cursor.png\"> or the CZMIL "
                 "reprocess rectangle cursor <img source=\":/icons/czmil_reprocess_rect_cursor.png\"> depending on the current mode.<br><br>"
                 "To switch between the two modes, press this button, then move the cursor to the main map, then press the right mouse "
                 "button, and select the relevant <b>Switch to...</b> option.  When you do this, the button icon and the cursor will change "
                 "to reflect your selection.<br><br>"
                 "<ul>"
                 "<li>Rectangle reprocess mode: After clicking the button the cursor will change to the CZMIL reprocess rectangle cursor "
                 "<img source=\":/icons/czmil_reprocess_rect_cursor.png\">.  Click the left mouse button to define a starting point "
                 "for a rectangle.  Move the mouse to define the reprocessing bounds.  Left click again to begin the reprocessing "
                 "operation.  To abort the operation click the middle mouse button.</li><br><br>"
                 "<li>Polygon reprocess mode: After clicking the button the cursor will change to the CZMIL reprocess polygon cursor "
                 "<img source=\":/icons/czmil_reprocess_poly_cursor.png\">.  Click the left mouse button to define a starting point "
                 "for a polygon.  Move the mouse cursor to define the polygon, then left clickagain to close the polygon and begin the "
                 "reprocessing operation. To abort the operation click the middle mouse button.</li>"
                 "</ul><br><br>"
                 "<b>IMPORTANT NOTES: This button will only be enabled if you have only one PFM opened and you have loaded the processing "
                 "mode as an attribute.  Please be aware that CZMIL reprocessing may not work completely "
                 "with CPF files earlier than version 2.  Any error messages will be displayed in the error message dialog and the "
                 "reprocessing will not be finished until the error message dialog is closed.  For this reason, if the error message dialog "
                 "is displayed when reprocessing is started it will be automatically closed.<br><br>"
                 "The type of data to be processed is indicated (and can be changed) by the CZMIL Reprocess Types button "
                 "<img source=\":/icons/czmil_reprocess_buttons_11.png\"></b>");

QString czmilReprocessWaterText = 
  pfmEdit3D::tr ("<img source=\":/icons/czmil_reprocess_water_poly.png\"> Click this button to select an area "
                 "inside of which you wish to mark points for reprocessing as water shots in HydroFusion.  When selected the cursor "
                 "will become the CZMIL reprocess polygon cursor <img source=\":/icons/czmil_reprocess_poly_cursor.png\"> or the CZMIL "
                 "reprocess rectangle cursor <img source=\":/icons/czmil_reprocess_rect_cursor.png\"> depending on the current mode.<br><br>"
                 "To switch between the two modes, press this button, then move the cursor to the main map, then press the right mouse "
                 "button, and select the relevant <b>Switch to...</b> option.  When you do this, the button icon and the cursor will change "
                 "to reflect your selection.<br><br>"
                 "<ul>"
                 "<li>Rectangle reprocess mode: After clicking the button the cursor will change to the CZMIL reprocess rectangle cursor "
                 "<img source=\":/icons/czmil_reprocess_rect_cursor.png\">.  Click the left mouse button to define a starting point "
                 "for a rectangle.  Move the mouse to define the reprocessing bounds.  Left click again to begin the reprocessing "
                 "operation.  To abort the operation click the middle mouse button.</li><br><br>"
                 "<li>Polygon reprocess mode: After clicking the button the cursor will change to the CZMIL reprocess polygon cursor "
                 "<img source=\":/icons/czmil_reprocess_poly_cursor.png\">.  Click the left mouse button to define a starting point "
                 "for a polygon.  Move the mouse cursor to define the polygon, then left click again to close the polygon and begin the "
                 "reprocessing operation. To abort the operation click the middle mouse button.</li>"
                 "</ul><br><br>"
                 "<b>IMPORTANT NOTES: This button will only be enabled if you have only one PFM opened and you have loaded the processing "
                 "mode as an attribute.  Please be aware that CZMIL reprocessing may not work completely "
                 "with CPF files earlier than version 2.  Any error messages will be displayed in the error message dialog and the "
                 "reprocessing will not be finished until the error message dialog is closed.  For this reason, if the error message dialog "
                 "is displayed when reprocessing is started it will be automatically closed.<br><br>"
                 "The type of data to be processed is indicated (and can be changed) by the CZMIL Reprocess Types button "
                 "<img source=\":/icons/czmil_reprocess_buttons_11.png\"></b>");

QString czmilReprocessShallowWaterText = 
  pfmEdit3D::tr ("<img source=\":/icons/czmil_reprocess_shallow_water_poly.png\"> Click this button to select an area "
                 "inside of which you wish to mark points for reprocessing as shallow water shots in HydroFusion.  When selected the cursor "
                 "will become the CZMIL reprocess polygon cursor <img source=\":/icons/czmil_reprocess_poly_cursor.png\"> or the CZMIL "
                 "reprocess rectangle cursor <img source=\":/icons/czmil_reprocess_rect_cursor.png\"> depending on the current mode.<br><br>"
                 "To switch between the two modes, press this button, then move the cursor to the main map, then press the right mouse "
                 "button, and select the relevant <b>Switch to...</b> option.  When you do this, the button icon and the cursor will change "
                 "to reflect your selection.<br><br>"
                 "<ul>"
                 "<li>Rectangle reprocess mode: After clicking the button the cursor will change to the CZMIL reprocess rectangle cursor "
                 "<img source=\":/icons/czmil_reprocess_rect_cursor.png\">.  Click the left mouse button to define a starting point "
                 "for a rectangle.  Move the mouse to define the reprocessing bounds.  Left click again to begin the reprocessing "
                 "operation.  To abort the operation click the middle mouse button.</li><br><br>"
                 "<li>Polygon reprocess mode: After clicking the button the cursor will change to the CZMIL reprocess polygon cursor "
                 "<img source=\":/icons/czmil_reprocess_poly_cursor.png\">.  Click the left mouse button to define a starting point "
                 "for a polygon.  Move the mouse cursor to define the polygon, then left clickagain to close the polygon and begin the "
                 "reprocessing operation. To abort the operation click the middle mouse button.</li>"
                 "</ul><br><br>"
                 "<b>VERY IMPORTANT NOTE: When reprocessing water shots as shallow water, <i>ONLY</i> water surface shots with a single, "
                 "valid water surface return and <i>NO</i> subsequent valid returns will be reprocessed!<br><br><b>"
                 "<b>IMPORTANT NOTES: This button will only be enabled if you have only one PFM opened and you have loaded the processing "
                 "mode as an attribute.  Please be aware that CZMIL reprocessing may not work completely "
                 "with CPF files earlier than version 2.  Any error messages will be displayed in the error message dialog and the "
                 "reprocessing will not be finished until the error message dialog is closed.  For this reason, if the error message dialog "
                 "is displayed when reprocessing is started it will be automatically closed.<br><br>"
                 "The type of data to be processed is indicated (and can be changed) by the CZMIL Reprocess Types button "
                 "<img source=\":/icons/czmil_reprocess_buttons_11.png\"></b>");

QString cpfFilterText = 
  pfmEdit3D::tr ("<img source=\":/icons/cpf_filter.png\"> Click this button to select a polygonal area "
                 "inside of which you wish to filter CZMIL CPF data points using the filter options defined in the <b>CPF Filter</b> tab of "
                 "the 2D Tracker/Histogram/Filter area on the left side of the window.  When selected the cursor will become the CPF filter "
                 "cursor \"<img source=\":/icons/cpf_filter_cursor.png\">.  Left clicking on a location will cause that point to be the "
                 "polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and mark the points for "
                 "reprocessing simply left click again.  If, at any time during the operation, you wish to discard the polygon and abort "
                 "the operation simply click the middle mouse button or right click and select a menu option.  This tab is persistent. "
                 "That is, you may select, filter, and accept as many times as you like.  To exit from the filter process and close the "
                 "tab just click the <b>Close</b> button.<br><br>"
                 "You can use the <b>Clear highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark points "
                 "that you do not wish to filter prior to clicking the <b>Accept</b> button.<br><br>"
                 "The CPF filter will do one of two things, either invalidate deep returns where there is no shallow central channel "
                 "surface return, or validate the last filter invalidated, deep channel, land processed return where the deep channel waveform "
                 "amplitude value at the interest point is greater than or equal to the <b>Minimum amplitude</b>.<br><br>"
                 "<b>IMPORTANT NOTE: The filter options can be changed at any time but they will not be applied until a new polygon has been "
                 "defined in the point cloud.</b>");

QString invCheckText = 
  pfmEdit3D::tr ("Check this box to set the CPF filter to invalidate deep returns where there is no shallow central channel surface "
                 "return.<br><br>"
                 "<b>IMPORTANT NOTE: The filter function will not be run (or re-run) until you select and complete a polygon in the point "
                 "cloud surrounding the points that you wish to filter.</b>");

QString valCheckText = 
  pfmEdit3D::tr ("Check this box to set the CPF filter to validate the last filter invalidated, deep channel, land processed return where "
                 "the deep channel waveform amplitude value at the interest point is greater than or equal to the <b>Minimum "
                 "amplitude</b>.<br><br>"
                 "<b>IMPORTANT NOTE: The filter function will not be run (or re-run) until you select and complete a polygon in the point "
                 "cloud surrounding the points that you wish to filter.  Also note that you must have manually invalid data displayed "
                 "<img source=\":/icons/display_manually_invalid.png\"> so that you can select the data that you want to validate.</b>");

QString valSpinText = 
  pfmEdit3D::tr ("This is the deep channel minimum waveform amplitude value to be used for the CPF filter when validating deep returns. "
                 "The deep channel waveform amplitude value at the interest point must be greater than the <b>Minimum amplitude</b> value "
                 "for it to be validated.<br><br>"
                 "<b>IMPORTANT NOTE: The filter function will not be run (or re-run) until you select and complete a polygon in the point "
                 "cloud surrounding the points that you wish to filter.  Also note that you must have manually invalid data displayed "
                 "<img source=\":/icons/display_manually_invalid.png\"> so that you can select the data that you want to validate.</b>");

QString lidarMonitorText = 
  pfmEdit3D::tr ("<img source=\":/icons/lidar_monitor.png\"> Click this button to run (or kill) the lidarMonitor program.  The "
                 "lidarMonitor program will display HOF, TOF, or CZMIL record data for the current point nearest the "
                 "cursor in a text format.<br><br>"
                 "<b>IMPORTANT NOTE: The hotkey for this button can be changed by changing the associated ancillary program hotkey "
                 "in the Preferences <img source=\":/icons/prefs.png\"> dialog.</b>");

QString chartsPicText = 
  pfmEdit3D::tr ("<img source=\":/icons/charts_pic.png\"> Click this button to run (or kill) the LIDAR chartsPic program.<br><br>"
                 "<b>IMPORTANT NOTE: The hotkey for this button can be changed by changing the associated ancillary program hotkey "
                 "in the Preferences <img source=\":/icons/prefs.png\"> dialog.</b>");

QString waveformMonitorText = 
  pfmEdit3D::tr ("<img source=\":/icons/waveform_monitor.png\"> Click this button to run (or kill) the LIDAR waveformMonitor program.<br><br>"
                 "<b>IMPORTANT NOTE: The hotkey for this button can be changed by changing the associated ancillary program hotkey "
                 "in the Preferences <img source=\":/icons/prefs.png\"> dialog.</b>");

QString CZMILwaveMonitorText = 
  pfmEdit3D::tr ("<img source=\":/icons/wave_monitor.png\"> Click this button to run (or kill) the LIDAR CZMILwaveMonitor program.<br><br>"
                 "<b>IMPORTANT NOTE: The hotkey for this button can be changed by changing the associated ancillary program hotkey "
                 "in the Preferences <img source=\":/icons/prefs.png\"> dialog.</b>");

QString LASwaveMonitorText = 
  pfmEdit3D::tr ("<img source=\":/icons/LASwaveMonitor.png\"> Click this button to run (or kill) the LASwaveMonitor program.<br><br>"
                 "<b>IMPORTANT NOTE: The hotkey for this button can be changed by changing the associated ancillary program hotkey "
                 "in the Preferences <img source=\":/icons/prefs.png\"> dialog.</b>");

QString waveWaterfallShallowText = 
  pfmEdit3D::tr ("<img source=\":/icons/wave_waterfall_shallow.png\"> Click this button to run (or kill) the LIDAR waveWaterfall program "
                 "in shallow mode.<br><br>"
                 "<b>IMPORTANT NOTE: The hotkey for this button can be changed by changing the associated ancillary program hotkey "
                 "in the Preferences <img source=\":/icons/prefs.png\"> dialog.</b>");

QString waveWaterfallDeepText = 
  pfmEdit3D::tr ("<img source=\":/icons/wave_waterfall_deep.png\"> Click this button to run (or kill) the LIDAR waveWaterfall program "
                 "in deep mode.<br><br>"
                 "<b>IMPORTANT NOTE: The hotkey for this button can be changed by changing the associated ancillary program hotkey "
                 "in the Preferences <img source=\":/icons/prefs.png\"> dialog.</b>");

QString changeLidarClassText = 
  pfmEdit3D::tr ("<img source=\":/icons/change_lidar_class.png\"> Click this button to switch to <b>Change LiDAR Classification</b> mode.  "
                 "When selected the cursor will become the change LiDAR classification cursor "
                 "<img source=\":/icons/change_lidar_class_cursor.png\">.  Left clicking on a location will cause that point to be "
                 "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                 "change the LiDAR classification or the data in the polygon simply left click again.  If, at any time during the operation, "
                 "you wish to discard the polygon and abort the operation simply click the middle mouse button or right click and select a "
                 "menu option. To change the current LiDAR classification value, right click and select <b>Change selected classification "
                 "type</b> from the menu.<br><br>"
                 "You can use the <b>Clear highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark points "
                 "that you do not wish to filter prior to clicking the <b>Accept</b> button.<br><br>");

QString lidarClassChangeBoxText = 
  pfmEdit3D::tr ("This tab contains information on the current state of the LiDAR Change Classification operation.  It includes the "
                 "<b>From</b> and <b>To</b> classification type fields and an optional Z range limiter.  Click <b>Accept</b> to accept and "
                 "make the LiDAR classification changes to the marked points or click <b>Discard</b> to clear the selection.  This tab is "
                 "persistent.  That is, you may select, filter, and accept as many times as you like.  To exit from the filter process and "
                 "close the tab just click the <b>Close</b> button.<br><br>"
                 "You can use the <b>Clear highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark points "
                 "that you do not wish to filter prior to clicking the <b>Accept</b> button.<br><br>"
                 "<b>IMPORTANT NOTE: The filter Z range limit flag and limits are used for the Hockey Puck filter, the statistical filter, the "
                 "attribute filter, and the LiDAR Change Classification filter.  Changing the checkbox or the Z range limits in any of the "
                 "filters will change it for all of the filters.</b>");

QString gsfMonitorText = 
  pfmEdit3D::tr ("<img source=\":/icons/gsf_monitor.png\"> Click this button to run (or kill) the gsfMonitor program.<br><br>"
                 "<b>IMPORTANT NOTE: The hotkey for this button can be changed by changing the associated ancillary program hotkey "
                 "in the Preferences <img source=\":/icons/prefs.png\"> dialog.</b>");

QString layerPrefsText = 
  pfmEdit3D::tr ("Selecting this menu item will cause the layer preferences dialog to appear.  In this dialog you can "
                 "turn display of the layers (i.e. PFM files) on or off.");

QString toolbarText = 
  pfmEdit3D::tr ("There are seven available tool bars in the pfmEdit3D program.  They are<br><br>"
                 "<ul>"
                 "<li>View tool bar - contains buttons to change the view</li>"
                 "<li>Feature tool bar - contains buttons to view and modify the features</li>"
                 "<li>Reference tool bar - contains buttons to view and change reference status (also view suspect)</li>"
                 "<li>Selected tool bar - contains buttons to view and change selected features</li>"
                 "<li>Utilities tool bar - contains buttons to modify the preferences and get context sensitive help</li>"
                 "<li>Edit tool bar - contains buttons to set editing modes</li>"
                 "<li>Mask tool bar - contains buttons to hide data inside or outside of rectangles or polygons</li>"
                 "</ul>"
                 "The tool bars may be turned on or off and relocated to any location on the screen.  You may click and "
                 "drag the tool bars using the handle to the left of (or above) the tool bar.  Right clicking in the tool "
                 "bar or in the menu bar will pop up a menu allowing you to hide or show the individual tool bars.  The "
                 "location, orientation, and visibility of the tool bars will be saved on exit.");

QString maskInsideRectText = 
  pfmEdit3D::tr ("<img source=\":/icons/mask_inside_rect.png\"> Click this button to select a rectangular area "
                 "inside of which you want to hide the data.  When selected the cursor will become the mask cursor "
                 "<img source=\":/icons/mask_cursor.png\">.  Left clicking on a location will cause that point to be "
                 "the rectangle anchor point.  Moving the cursor will cause a rectangle to appear.  To finalize the "
                 "rectangle and hide the data, left click again.  If, at any time during the operation, you wish to "
                 "discard the rectangle and abort the operation simply click the middle mouse button or right click and "
                 "select one of the menu options.  To make all hidden data visible, press the (flashing) clear hidden areas button "
                 "<img source=\":/icons/clear_mask_shape.png\">");
QString maskOutsideRectText = 
  pfmEdit3D::tr ("<img source=\":/icons/mask_outside_rect.png\"> Click this button to select a rectangular area "
                 "outside of which you want to hide the data.  When selected the cursor will become the mask cursor "
                 "<img source=\":/icons/mask_cursor.png\">.  Left clicking on a location will cause that point to be "
                 "the rectangle anchor point.  Moving the cursor will cause a rectangle to appear.  To finalize the "
                 "rectangle and hide the data, left click again.  If, at any time during the operation, you wish to "
                 "discard the rectangle and abort the operation simply click the middle mouse button or right click and "
                 "select one of the menu options.  To make all hidden data visible press the (flashing) clear hidden areas button "
                 "<img source=\":/icons/clear_mask_shape.png\">");
QString maskInsidePolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/mask_inside_poly.png\"> Click this button to select a polygonal area "
                 "inside of which you want to hide the data.  When selected the cursor will become the mask cursor "
                 "<img source=\":/icons/mask_cursor.png\">.  Left clicking on a location will cause that point to be "
                 "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                 "hide the data simply left click again.  If, at any time during the operation, "
                 "you wish to discard the polygon and abort the operation simply click the middle mouse "
                 "button or right click and select a menu option.  To make all hidden data visible press the (flashing) clear hidden areas "
                 "button <img source=\":/icons/clear_mask_shape.png\">");
QString maskOutsidePolyText = 
  pfmEdit3D::tr ("<img source=\":/icons/mask_outside_poly.png\"> Click this button to select a polygonal area "
                 "outside of which you want to hide the data.  When selected the cursor will become the mask cursor "
                 "<img source=\":/icons/mask_cursor.png\">.  Left clicking on a location will cause that point to be "
                 "the polygon start point.  Moving the cursor will draw a continuous line.  To close the polygon and "
                 "hide the data simply left click again.  If, at any time during the operation, "
                 "you wish to discard the polygon and abort the operation simply click the middle mouse "
                 "button or right click and select a menu option.  To make all hidden data visble press the (flashing) clear hidden areas "
                 "button <img source=\":/icons/clear_mask_shape.png\">");
QString maskClearText = 
  pfmEdit3D::tr ("<img source=\":/icons/clear_mask_shape.png\"> If this button is flashing it means that some data has been hidden by area "
                 "(rectangle or polygon).  You can click this button to make all data that has been hidden by area visible.");

QString maskResetText = 
  pfmEdit3D::tr ("<img source=\":/icons/clear_mask_type.png\"> If this button is flashing it means that some data has been hidden by data type "
                 "or filter.  You can click this button to make all data that has been hidden by data type or filter visible.");

QString flagResetText = 
  pfmEdit3D::tr ("<img source=\":/icons/clear_mask_flag.png\"> If this button is flashing it means that some data has been hidden by PFM "
                 "user flag or highlighting.  You can click this button to make all data that has been hidden by data type or highlighting.");

QString measureText = 
  pfmEdit3D::tr ("<img source=\":/icons/measure.png\"> Click this button to measure distances and depth/elevation differences.  When this "
                 "button is clicked the cursor will become the measure cursor <img source=\":/icons/measure_cursor.png\">.  Position the "
                 "box/crosshair on a point and then move the cursor.    The distance in meters, azimuth in degrees, and the difference "
                 "in Z in meters will be displayed in the status bar.  When you are finished click the left mouse button.<br><br>"
                 "<b>IMPORTANT NOTE: There are two sets of distance, azimuth, and deltaZ values.  They are labeled Nearest Point and "
                 "Cursor.  The nearest point is the one that the box/crosshair snaps to.  The cursor position is a bit more "
                 "complicated.  The cursor position is computed by assuming that the cursor is actually in a plane that is parallel to the "
                 "screen face and at the depth/elevation of the anchor point (the starting point of the line, not the nearest point).  This "
                 "gives the user the ability to measure approximate distances without having to use the position of the nearest point to "
                 "compute an absolute distance.<br><br>"
                 "<font color=\"#ff0000\">TIP: To minimize Z distortion when trying to get approximate distances press the Reset button "
                 "<img source=\":/icons/reset_view.png\"> and then use the left and right arrow keys to rotate the view.</font></b>");

QString filterText = 
  pfmEdit3D::tr ("<img source=\":/icons/filter.png\"> Click this button to access a menu of filters to be run on the displayed data.");

QString areaFilterText = 
  pfmEdit3D::tr ("<img source=\":/icons/area_filter.png\"> Press this button to access the tab used to run the area-based, pseudo-statistical "
                 "filter. The area-based, pseudo-statistical filter uses the already defined PFM bins and the pre-computed bin standard "
                 "deviations to run a fast approximation of a statistical filter.  The standard deviation slider just sets an approximate "
                 "value to be used in the filter.  The filter parameters can be modified in the area-based filter tab that will appear when "
                 "this button is pressed. This filter process will be performed on every visible point in the point cloud. This is a "
                 "one-pass filter, that is, it will run only one pass and will not reevaluate the data based on points invalidated during "
                 "the pass.  If you want to kill more points, just re-run it. To start the filter process, press the <b>Run</b> "
                 "button.<br><br>"
                 "<b>IMPORTANT NOTE: The filter operation will mark all of the points that are to be invalidated.  The user must then "
                 "select <i>Delete</i>, or <i>Cancel</i> to either invalidate the marked points or to cancel the operation.<br><br>"
                 "<b><font color=\"#ff0000\">TIP: After running the filter, while the points are marked, You can use the <b>Clear "
                 "highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark any points that you do not wish "
                 "to filter prior to clicking the <b>Delete</b> button.  You can also adjust the filter standard deviation slider in the "
                 "area-based filter tab which will re-run the filter.  Pressing the <i>Del</i> key (or whatever hot key that has been set "
                 "for <i>DELETE FILTER HIGHLIGHTED</i> in the Preferences dialog <img source=\":/icons/prefs.png\">) while the points are "
                 "marked and the tab is visible will automatically invalidate the marked, un-masked points.</font></b><br><br>"
                 "Thanx, and a tip of the hat to Micah Tinkler for the <i>filtering in the editor</i> idea and especially "
                 "for the suggestion to be able to mask <i><b>after</b></i> the filter has been run.");

QString statFilterText = 
  pfmEdit3D::tr ("<img source=\":/icons/stat_filter.png\"> Press this button to access the tab used to run the AVT (Antonyan Vardan Transform) "
                 "statistical filter on the displayed data points.  The filter parameters can be modified in the statistical filter tab that "
                 "will appear when this button is pressed.  This filter process will be performed on every visible point in the point cloud. "
                 "It is performed iteratively until the number of passes is reached or, if the number of passes is set to 0, until there are "
                 "no more points being marked invalid. To start the filter process, press the <b>Run</b> button.<br><br>"
                 "<b>IMPORTANT NOTE: The filter operation will mark all of the points that are to be invalidated.  The user must then "
                 "select <i>Delete</i>, or <i>Cancel</i> to either invalidate the marked points or to cancel the operation.<br><br>"
                 "<b><font color=\"#ff0000\">TIP: After running the filter, while the points are marked, You can use the <b>Clear "
                 "highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark any points that you do not wish "
                 "to filter prior to clicking the <b>Delete</b> button.  You can also adjust the filter standard deviation slider in the "
                 "statistical filter tab which will re-run the filter.  You can also adjust the radius, minimum count, and number of passes "
                 "in the filter tab.  After modifying one or more of these values, pressing the <b>Run</b> button will re-run the filter. "
                 "Pressing the <i>Del</i> key (or whatever hot key that has been set for <i>DELETE FILTER HIGHLIGHTED</i> in the Preferences "
                 "dialog <img source=\":/icons/prefs.png\">) while the points are marked and the tab is visible will automatically "
                 "invalidate the marked, un-masked points.</font></b><br><br>"
                 "Thanx, and a tip of the hat to Micah Tinkler for the <i>filtering in the editor</i> idea and especially "
                 "for the suggestion to be able to mask <i><b>after</b></i> the filter has been run.");

QString HPFilterText = 
  pfmEdit3D::tr ("<img source=\":/icons/hp_filter.png\"> Press this button to access the tab used to run the <b>Hockey Puck</b> filter on "
                 "the displayed data points.<br><br>"
                 "The filter parameters can be modified in the Hockey Puck filter tab that will appear when this button is pressed.  The "
                 "following diagram shows the parameters in relation to the Hockey Puck shape and to points from multiple lines (indicated "
                 "by different point colors). To start the filter process, press the <b>Run</b> button.<br>"
                 "<center><img source=\":/icons/HP_diagram.png\"></center><br>"
                 "The Hockey Puck filter is a proximity filter that works by checking every point in the point cloud against nearby points "
                 "to determine if there are <b>Neighbors</b>, or <b>Multi-line Neighbors</b> (if more than one line is present) within "
                 "the specified horizontal <b>Radius</b> and vertical <b>Height</b> (i.e. the hockey puck).  If there aren't enough points "
                 "of either kind within the hockey puck then the point will be marked for invalidation.<br><br>"
                 "In the above image, <b>Neighbors</b> are defined as points within the Hockey Puck shape other than the <b>Filter point</b>. "
                 "<b>Multi-line Neighbors</b> are points within the Hockey Puck shape that are not from the same line as the <b>Filter "
                 "point</b>. Looking at the above image, it should be obvious that, if the <b>Neighbors</b> value is set to more than 3 and "
                 "the <b>Multi-line Neighbors</b> value is set to 0 (disable multi-line check) or more than 1, the <b>Filter point</b> will "
                 "be invalidated.  Conversely, if the <b>Neighbors</b> value is set to 3 or less or the multi-line neigbors value is set to "
                 "1 then the <b>Filter point</b> will be left untouched.  The <b>Multi-line Neighbors</b> check, if not disabled, will "
                 "override the <b>Neighbors</b> check. The rationale for this being that the presence of one or more points from a line "
                 "other than the <b>Filter point</b> line carries more weight than points from the same line as the <b>Filter point</b>. "
                 "This filter process will be performed on every visible point in the point cloud.  It is performed iteratively until the "
                 "number of passes is reached or, if the number of passes is set to 0, until there are no more points being marked "
                 "invalid.<br>" 
                 "<b>IMPORTANT NOTE: The filter operation will mark all of the points that are to be invalidated.  The user must then "
                 "select <b>Delete</b>, or <b>Cancel</b> to either invalidate the marked points or to cancel the operation.<br><br>"
                 "<b><font color=\"#ff0000\">TIP: After running the filter, while the points are marked, You can use the <b>Clear "
                 "highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark any points that you do not wish "
                 "to filter prior to clicking the <b>Delete</b> button.  You can also adjust the <b>Hockey Puck</b> radius, height, number "
                 "of neighbors, and number of multi-line neighbors in the filter tab.  After modifying one or more of these values, "
                 "pressing the <b>Run</b> button will re-run the filter.  Pressing the <i>Del</i> key (or whatever hot key that has been "
                 "set for <i>DELETE FILTER HIGHLIGHTED</i> in the Preferences dialog <img source=\":/icons/prefs.png\">) while the points "
                 "are marked and the tab is visible will automatically invalidate the marked, un-masked points.</font></b><br>"
                 "Thanx, and a tip of the hat to Micah Tinkler for the <i>filtering in the editor</i> idea and especially "
                 "for the suggestion to be able to mask <i><b>after</b></i> the filter has been run.");

QString attrFilterText = 
  pfmEdit3D::tr ("<img source=\":/icons/attr_filter.png\"> Press this button to access the tab used to filter the data points based on the "
                 "attribute that the data is currently being colored by (i.e. the highlighted field in the <b>Status</b> tab).  The "
                 "<b>Filter ranges</b> field will accept a combination of values separated by commas (,) and/or right arrows (>).  For "
                 "example, the text <b>2.0,7.2>26.8,30.0</b> will cause all data having a value (for the selected attribute) of 2.0, or in "
                 "the range 7.2 to 26.8 inclusive, or having a value of 30.0 to be flagged for invalidation.  You may change the text "
                 "ranges and press <b>Run</b> to re-run the filter. To start the filter process, press the <b>Run</b> button.<br><br>"
                 "<b>IMPORTANT NOTE: The filter operation will mark all of the points that are to be filtered and display "
                 "them prior to actually invalidating the filtered points.  Optionally, instead of invalidating the points, you can hide "
                 "them by clicking the <i>Hide</i> button.<br><br>"
                 "<b><font color=\"#ff0000\">TIP: After running the filter, while the points are marked, You can use the <b>Clear "
                 "highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark any points that you do not wish "
                 "to filter prior to clicking the <b>Delete</b> button.  Pressing the <i>Del</i> key (or whatever hot key that has been set "
                 "for <i>DELETE FILTER HIGHLIGHTED</i> in the Preferences dialog <img source=\":/icons/prefs.png\">) will invalidate the "
                 "marked, un-masked points.</font></b><br><br>"
                 "Thanx, and a tip of the hat to Micah Tinkler for the <i>filtering in the editor</i> idea and especially "
                 "for the suggestion to be able to mask <i><b>after</b></i> the filter has been run.");

QString attrFilterRangeText = 
  pfmEdit3D::tr ("The <b>Filter ranges</b> field will accept a combination of values separated by commas (,) and/or right arrows (>). "
                 "For example, the text <b>2.0,7.2>26.8,30.0</b> will cause all data having a value (for the selected attribute) of 2.0, "
                 "or in the range 7.2 to 26.8 inclusive, or having a value of 30.0 to be flagged for invalidation.  You may change the text "
                 "ranges and press <b>Run</b> to re-run the filter.<br><br>"
                 "<b>IMPORTANT NOTE: The filter operation will mark all of the points that are to be filtered and display them prior to "
                 "actually invalidating the filtered points.  Optionally, instead of invalidating the points, you can hide them by clicking "
                 "the <i>Hide</i> button.<br><br>"
                 "<b><font color=\"#ff0000\">TIP: After running the filter, while the points are marked, You can use the <b>Clear "
                 "highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark any points that you do not wish "
                 "to filter prior to clicking the <b>Delete</b> button.  Pressing the <i>Del</i> key (or whatever hot key that has been set "
                 "for <i>DELETE FILTER HIGHLIGHTED</i> in the Preferences dialog) will invalidate the marked, un-masked "
                 "points.</font></b><br><br>"
                 "Thanx, and a tip of the hat to Micah Tinkler for the <i>filtering in the editor</i> idea and especially "
                 "for the suggestion to be able to mask <i><b>after</b></i> the filter has been run.");

QString attrFilterApplyText = 
  pfmEdit3D::tr ("Click this button to apply any changes that were made to the attribute filter ranges and/or the Z range limits.  This will "
                 "rerun the attribute filter and mark the points for invalidation.<br><br>"
                 "<b>IMPORTANT NOTE: The filter operation will mark all of the points that are to be filtered and display them prior to "
                 "actually invalidating the filtered points.  Optionally, instead of invalidating the points, you can hide them by clicking "
                 "the <i>Hide</i> button.<br><br>"
                 "<b><font color=\"#ff0000\">TIP: After running the filter, while the points are marked, You can use the <b>Clear "
                 "highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark any points that you do not wish "
                 "to filter prior to clicking the <b>Delete</b> button.  Pressing the <i>Del</i> key (or whatever hot key that has been "
                 "set for <i>DELETE FILTER HIGHLIGHTED</i> in the Preferences dialog) will invalidate the marked, un-masked "
                 "points.</font></b><br><br>"
                 "Thanx, and a tip of the hat to Micah Tinkler for the <i>filtering in the editor</i> idea and especially "
                 "for the suggestion to be able to mask <i><b>after</b></i> the filter has been run.");

QString attrFilterHideText = 
  pfmEdit3D::tr ("Click this button to hide, instead of invalidating, points selected by the filter.  When clicked, the points selected by "
                 "the filter will be masked and the <b>Clear hide settings</b> button <img source=\":/icons/clear_mask_attr.png\"> will "
                 "begin blinking if it is not already.");

QString attrFilterInvertText = 
  pfmEdit3D::tr ("Click this button to invert the selection of points by the filter.  When clicked, the points selected by the filter "
                 "will be unselected and the points not selected will be flagged.");

QString CZMILUrbanFilterText = 
  pfmEdit3D::tr ("<img source=\":/icons/czmilUrbanFilter.png\">Press this button to access the tab used to run the <b>CZMIL Urban Noise "
                 "Filter</b> (also known as the Nick Johnson Filter) on the displayed In order to run this filter both the <b>CZMIL Urban "
                 "noise flags</b> and the <b>CZMIL D_index</b> attributes must have been loaded into the PFM. To start the filter process, "
                 "press the <b>Run</b> button. The algorithms for loading, marking, searching, and filtering (which are, by necessity, "
                 "linked) are as follows:<br>"
                 "<ul>"
                 "<li>All returns from shots that are processed as topo (Optech processing mode = 1) will be marked as "
                 "CZMIL_URBAN_TOPO (bit 1 set, returns from hybrid or bathy processed shots will be marked as 0).</li>"
                 "<li>All returns from shots that are processed as topo that have 5 or more valid returns on that channel will "
                 "be marked as CZMIL_URBAN_SOFT_HIT (bit 2 set).</li>"
                 "<li>All returns from shots that are processed as topo that have 2  or fewer valid returns will be marked as "
                 "CZMIL_URBAN_HARD_HIT (bit 3 set).  Note that CZMIL_URBAN_SOFT_HIT and CZMIL_URBAN_HARD_HIT are mutually "
                 "exclusive.</li>"
                 "<li>Returns on channels with 3 or fewer returns from shots that are processed as topo that have 3 or fewer "
                 "valid returns on a majority (4 or more of 7, IR and deep channels are ignored) of the channels will be marked "
                 "as CZMIL_URBAN_MAJORITY (bit 4 set).  Pay close attention here - returns on channels with 4 or more valid "
                 "returns are <font color=\"#ff0000\">NOT</font> marked as CZMIL_URBAN_MAJORITY.</li>"
                 "<li>The filter, either in czmilUrbanFilter or in pfmEdit3D, will perform a hockey puck (user specified radius "
                 "and height) vicinity search based on the criteria set in the program.  Any valid CZMIL_URBAN_HARD_HIT return "
                 "from a topo processed shot that is within the specified radius and height of a valid CZMIL_URBAN_MAJORITY "
                 "return will be marked as CZMIL_URBAN_VICINITY.  In czmilUrbanFilter this is written to the CZMIL Urban noise "
                 "flags attribute.  In pfmEdit3D it is not written to the attribute since the search is always run (due to "
                 "possible changes in validity).</li>"
                 "<li>The filter, either in czmilUrbanFilter or pfmEdit3D, will perform a hockey puck search for valid "
                 "CZMIL_URBAN_TOPO returns within the user specified radius and height of any valid CZMIL_URBAN_MAJORITY or "
                 "CZMIL_URBAN_VICINITY return with a D_index value that meets or exceeds the user specified upper D_index cutoff. "
                 "Prior to the search, CZMIL_URBAN_TOPO points that exceed the user specified lower D_index cutoff are excluded "
                 "(no point in looking at them since they won't be invalidated).  Also excluded are CZMIL_URBAN_TOPO returns that "
                 "are also marked as CZMIL_URBAN_SOFT_HIT.  Once it gets through all that testing, any remaining valid "
                 "CZMIL_URBAN_TOPO points with a D_index value less than the user specified lower D_index cutoff will be marked as "
                 "invalid.  In czmilUrbanFilter they will be marked as PFM_FILTER_INVAL with PFM_USER_FLAG_06 set (unless the user "
                 "specified setting to PFM_SUSPECT) and in pfmEdit3D they will be set to PFM_MANUALLY_INVAL.</li>"
                 "</ul><br>"
                 "The binary flags that will be displayed in pfmEdit3D (as a series of 1s and 0s) will be in this order, left to "
                 "right:<br>"
                 "CZMIL_URBAN_VICINITY, CZMIL_URBAN_MAJORITY, CZMIL_URBAN_HARD_HIT, CZMIL_URBAN_SOFT_HIT, CZMIL_URBAN_TOPO<br><br>"
                 "<b>IMPORTANT NOTE: Changing the filter parameters in the CZMIL Urban Noise Filter tab will automatically change the "
                 "defaults in the standalone czmilUrbanFilter program.  This is done so that you can test the filter here and then run "
                 "it on the entire PFM or some portion of the PFM.</b>");

QString cufFilterBoxText = 
  pfmEdit3D::tr ("This is the CZMIL Urban Noise Filter tab.<br><br>"
                 "This filter runs in two passes.  First, it searches the data for any topo points that are in "
                 "the vicinity of urban feature points and marks them as being in the vicinity of an urban feature.  "
                 "Urban feature points are marked by pfmLoad as CZMIL_URBAN_MAJORITY if a majority of the 7 channels "
                 "of a topo shot had less than 4 valid returns.<br><br>"
                 "The second pass uses the Hockey Puck filter to mark points as invalid or suspect if their D_index "
                 "value is less than the lower D_index cutoff and they are within the radius/height of urban features "
                 "(CZMIL_URBAN_MAJORITY or CZMIL_URBAN_VICINITY) whose D_index value is greater than or equal to the upper D_index "
                 "value.<br><br>"
                 "Data that does not meet the Hockey Puck filter criteria will be marked as invalid.<br><br>"
                 "<b>IMPORTANT NOTE: Changing the filter parameters in the CZMIL Urban Noise Filter tab will automatically change the "
                 "defaults in the standalone czmilUrbanFilter program.  This is done so that you can test the filter here and then run "
                 "it on the entire PFM or some portion of the PFM.</b>");

QString cufVRadiusValueText = 
  pfmEdit3D::tr ("This is the CZMIL Urban Noise filter vicinity search radius value.<br><br>"
                 "To see an explanation of how the CZMIL Urban Noise Filter works, press the context sensitive help button and then "
                 "click on the CZMIL Urban Noise Filter button.<br><br>"
                 "<b>IMPORTANT NOTE: Changing this value will automatically change the default value for this parameter in the "
                 "standalone czmilUrbanFilter program.  This is done so that you can test the filter here and then run "
                 "it on the entire PFM or some portion of the PFM.</b>");

QString cufFRadiusValueText = 
  pfmEdit3D::tr ("This is the CZMIL Urban Noise filter Hockey Puck filter radius value.<br><br>"
                 "To see an explanation of how the CZMIL Urban Noise Filter works, press the context sensitive help button and then "
                 "click on the CZMIL Urban Noise Filter button.<br><br>"
                 "<b>IMPORTANT NOTE: Changing this value will automatically change the default value for this parameter in the "
                 "standalone czmilUrbanFilter program.  This is done so that you can test the filter here and then run "
                 "it on the entire PFM or some portion of the PFM.</b>");

QString cufVHeightValueText = 
  pfmEdit3D::tr ("This is the CZMIL Urban Noise filter vicinity search height value.<br><br>"
                 "To see an explanation of how the CZMIL Urban Noise Filter works, press the context sensitive help button and then "
                 "click on the CZMIL Urban Noise Filter button.<br><br>"
                 "<b>IMPORTANT NOTE: Changing this value will automatically change the default value for this parameter in the "
                 "standalone czmilUrbanFilter program.  This is done so that you can test the filter here and then run "
                 "it on the entire PFM or some portion of the PFM.</b>");

QString cufFHeightValueText = 
  pfmEdit3D::tr ("This is the CZMIL Urban Noise filter Hockey Puck filter height value.<br><br>"
                 "To see an explanation of how the CZMIL Urban Noise Filter works, press the context sensitive help button and then "
                 "click on the CZMIL Urban Noise Filter button.<br><br>"
                 "<b>IMPORTANT NOTE: Changing this value will automatically change the default value for this parameter in the "
                 "standalone czmilUrbanFilter program.  This is done so that you can test the filter here and then run "
                 "it on the entire PFM or some portion of the PFM.</b>");

QString cufLDIndexText = 
  pfmEdit3D::tr ("This is the CZMIL Urban Noise filter lower D_index cutoff value.<br><br>"
                 "To see an explanation of how the CZMIL Urban Noise Filter works, press the context sensitive help button and then "
                 "click on the CZMIL Urban Noise Filter button.<br><br>"
                 "<b>IMPORTANT NOTE: Changing this value will automatically change the default value for this parameter in the "
                 "standalone czmilUrbanFilter program.  This is done so that you can test the filter here and then run "
                 "it on the entire PFM or some portion of the PFM.</b>");

QString cufUDIndexText = 
  pfmEdit3D::tr ("This is the CZMIL Urban Noise filter upper D_index cutoff value.<br><br>"
                 "To see an explanation of how the CZMIL Urban Noise Filter works, press the context sensitive help button and then "
                 "click on the CZMIL Urban Noise Filter button.<br><br>"
                 "<b>IMPORTANT NOTE: Changing this value will automatically change the default value for this parameter in the "
                 "standalone czmilUrbanFilter program.  This is done so that you can test the filter here and then run "
                 "it on the entire PFM or some portion of the PFM.</b>");

QString cufApplyText = 
  pfmEdit3D::tr ("Clicking this button will re-run the CZMIL Urban Noise Filter using the values in the associated parameter fields.<br><br>"
                 "To see an explanation of how the CZMIL Urban Noise Filter works, press the context sensitive help button and then "
                 "click on the CZMIL Urban Noise Filter button.<br><br>");

QString HOFWaveFilterText = 
  pfmEdit3D::tr ("<img source=\":/icons/hof_filter.png\"> Click this button to filter CHARTS HOF data points using a proximity based "
                 "waveform filter.  The filter consists of three passes.  The first pass invalidates selected points that have a low fore "
                 "or back slope.  The second pass uses the search radius combined with the horizontal uncertainty and vertical uncertainty "
                 "to eliminate from filtering any valid points that have a valid point from another line within the Hockey Puck of "
                 "Confidence.  The final pass looks for rising waveforms in both PMT and APD (if applicable) in the waveforms of "
                 "valid or invalid adjacent points (within the radius and Z limits).  The search width defines how far before and after "
                 "the selected bin to search for a rise.  The rise threshold defines how many consecutive rise points are considered "
                 "significant.<br><br>"
                 "You can use the <b>Clear highlighted/marked points mode</b> <img source=\":/icons/clear_polygon.png\"> to unmark points "
                 "that you do not wish to filter prior to clicking the <b>Delete</b> button.<br><br>"
                 "<b>IMPORTANT NOTE: This button will turn off slicing, un-hide any hidden data, and clear all masks (except filter masks) "
                 "prior to running the filter.</b>");

QString filterRectMaskText = 
  pfmEdit3D::tr ("<img source=\":/icons/filter_mask_rect.png\"> Click this button to define temporary areas to protect (mask) from "
                 "subsequent filter operations <img source=\":/icons/filter.png\">.<br><br>"
                 "After clicking the button the cursor will change to the rectangle mask cursor "
                 "<img source=\":/icons/filter_mask_rect_cursor.png\">.  "
                 "Click the left mouse button to define a starting point for the rectangle.  Moving the mouse will draw a rectangle.  "
                 "Left click to end rectangle definition and mask the area.  To abort the operation click the middle mouse button.<br><br>"
                 "<b>IMPORTANT NOTE: The filter mask areas are only temporary.  If you change the area of the PFM that you "
                 "are viewing these will be discarded.</b>");
QString filterPolyMaskText = 
  pfmEdit3D::tr ("<img source=\":/icons/filter_mask_poly.png\"> Click this button to define temporary areas to protect (mask) from "
                 "subsequent filter operations <img source=\":/icons/filter.png\">.<br><br>"
                 "After clicking the button the cursor will change to the polygon mask cursor "
                 "<img source=\":/icons/filter_mask_poly_cursor.png\">.  "
                 "Click the left mouse button to define a starting point for a polygon.  Move the mouse to define the polygon to "
                 "be masked.  Left click again to define the last vertex of the polygon.  "
                 "To abort the operation click the middle mouse button.<br><br>"
                 "<b>IMPORTANT NOTE: The filter mask areas are only temporary.  If you change the area of the PFM that you "
                 "are viewing these will be discarded.</b>");
QString clearMasksText = 
  pfmEdit3D::tr ("<img source=\":/icons/clear_filter_masks.png\"> Click this button to clear all currently defined filter masks.");

QString undoText = 
  pfmEdit3D::tr ("<img source=\":/icons/undo.png\"> Click this button to undo the last edit operation.  When there are no edit "
               "operations to be undone this button will not be enabled.");

QString filterBoxText = 
  pfmEdit3D::tr ("This tab becomes active clicking the area-based filter <img source=\":/icons/area_filter.png\"> button.");

QString avtFilterBoxText = 
  pfmEdit3D::tr ("This tab becomes active after clicking the AVT statistical filter <img source=\":/icons/stat_filter.png\"> button.");

QString hpFilterBoxText = 
  pfmEdit3D::tr ("This tab becomes active after clicking the Hockey Puck filter <img source=\":/icons/hp_filter.png\"> button.");

QString attrFilterBoxText = 
  pfmEdit3D::tr ("This tab becomes active after clicking the attribute filter <img source=\":/icons/attr_filter.png\"> button.");

QString stdSliderText = 
  pfmEdit3D::tr ("Move this slider to the right to increase the amount of filtering (i.e. decrease the standard deviation value) or "
                 "move it left to decrease the amount of filtering (i.e. increase the standard deviation value).");

QString hpRadiusValueText = 
  pfmEdit3D::tr ("Change the X/Y <b>Radius</b> (in meters) to be used for the Hockey Puck filter.  Increasing this value will decrease the "
                 "amount of filtering.  After changing the value and clicking the <b>Run</b> button the filter is re-run on the visible "
                 "data and the number of points to be rejected is modified.<br><br>"
                 "<b>IMPORTANT NOTE: The horizontal uncertainty of each of the two points being compared at any given time is added to the "
                 "horizontal radius so that any possible overlap of the positional data can be used to validate a point.</b>");

QString hpHeightValueText = 
  pfmEdit3D::tr ("Change the vertical <b>Height</b> (in meters) to be used for the Hockey Puck filter.  Increasing this value will decrease "
                 "the amount of filtering.  After changing the value and clicking the <b>Run</b> button the filter is re-run on the visible "
                 "data and the number of points to be rejected is modified.");

QString hpNeighborsValueText = 
  pfmEdit3D::tr ("Change the number of <b>Neighbors</b> required within the horizontal <b>Radius</b> and vertical <b>Height</b> (i.e. the "
                 "hockey puck) of the Hockey Puck filter.  Increasing this value will increase the amount of filtering.  Setting this value "
                 "to 0 will force the filter to only consider points in areas with multiple lines (safer).  After changing the value and "
                 "clicking the <b>Run</b> button the filter is re-run on the visible data and the number of points to be rejected is "
                 "modified.");

QString hpMultiNeighborsValueText = 
  pfmEdit3D::tr ("Change the number of <b>Multi-line Neighbors</b> (i.e. neighbors when more than one line is present) required within the "
                 "horizontal <b>Radius</b> and vertical <b>Height</b> (i.e. the hockey puck) of the Hockey Puck filter.  Increasing this "
                 "value will increase the amount of filtering.  After changing the value and clicking the <b>Run</b> button the filter is "
                 "re-run on the visible data and the number of points to be rejected is modified.");

QString rangeCheckText = 
  pfmEdit3D::tr ("Check this box to limit the filter to the selected range.  The range can be defined in depth or elevation depending on "
                 "the users <b>Z Orientation Preference</b> setting.<br><br>"
                 "<b>IMPORTANT NOTE: The filter Z range limit flag and limits are used for the Hockey Puck filter, the statistical filter, "
                 "the attribute filter, and the LiDAR Change Classification filter.  Changing the checkbox or the Z range limits in any of "
                 "the filters will change it for all of the filters.</b>");

QString avtCheckText = 
  pfmEdit3D::tr ("Check this box to use an actual AVT (Antonyan Vardan Transform) statistical filter instead of the pseudo-statistical, "
                 "area-based filter.  If this box is not checked, the <b>Radius</b> and <b>Count</b> values will be ignored.<br><br>"
                 "<b>IMPORTANT NOTE: This filter will be <i>MUCH</i> slower than the pseudo-statistical filter but the results may be "
                 "better.  The AVT filter may be speeded up by increasing the number of filter threads in the Preferences dialog "
                 "<img source=\":/icons/prefs.png\">.</b>");

QString radiusValueText = 
  pfmEdit3D::tr ("This is the radius, in meters, of the circle around each point to be filtered (AVT filter).  The standard deviation "
                 "will be computed using all valid points (hidden or not) within this circle.");

QString countValueText = 
  pfmEdit3D::tr ("This is the minimum number of points allowed in the standard deviation computation circle (AVT filter).  If there are "
                 "less than this number of points in the circle, the standard deviation will not be computed and the point in the center "
                 "of the circle will not be invalidated regardless of its distance from the average.");

QString passValueText = 
  pfmEdit3D::tr ("This is the number of passes that the filter will make on the data.  If this is set to 0, the filter will run until no more "
                 "points are invalidated.");

QString minZValueText = 
  pfmEdit3D::tr ("This is the minimum Z value that will be used to limit the filter.  The value can be defined as a depth or an elevation "
                 "depending on the users <b>Z Orientation Preference</b> setting.<br><br>"
                 "<b>IMPORTANT NOTE: The filter Z range limit flag and limits are used for the Hockey Puck filter, the statistical filter, "
                 "the attribute filter, and the LiDAR Change Classification filter.  Changing the checkbox or the Z range limits in any of "
                 "the filters will change it for all of the filters.</b>");

QString maxZValueText = 
  pfmEdit3D::tr ("This is the maximum Z value that will be used to limit the filter.  The value can be defined as a depth or an elevation "
                 "depending on the users <b>Z Orientation Preference</b> setting.<br><br>"
                 "<b>IMPORTANT NOTE: The filter Z range limit flag and limits are used for the Hockey Puck filter, the statistical filter, "
                 "the attribute filter, and the LiDAR Change Classification filter.  Changing the checkbox or the Z range limits in any of "
                 "the filters will change it for all of the filters.</b>");

QString filterApplyText = 
  pfmEdit3D::tr ("Clicking this button will re-run the statistical filter using the values in the associated parameter fields.");

QString hpFilterApplyText = 
  pfmEdit3D::tr ("Clicking this button will re-run the Hockey Puck filter using the values in the associated parameter fields.<br><br>"
                 "To see an explanation of the Hockey Puck filter, press the context sensitive Help button and then click on the Hockey "
                 "Puck filter button <img source=\":/icons/hp_filter.png\">.");

QString srSliderText = 
  pfmEdit3D::tr ("Move this slider left/right to decrease/increase the search radius used by the hofWaveFilter program when searching for "
                 "nearby points.  Decreasing this value makes the filter more aggressive.<br><br>"
                 "You can also type the value you want into the text box and press <i>Return</i>.");
QString swSliderText = 
  pfmEdit3D::tr ("Move this slider left/right to decrease/increase the search width used by the hofWaveFilter program when searching for "
                 "rising values on nearby waveforms.  Decreasing this value makes the filter more aggressive.<br><br>"
                 "You can also type the value you want into the text box and press <i>Return</i>.");
QString rtSliderText = 
  pfmEdit3D::tr ("Move this slider left/right to decrease/increase the rise threshold used by the hofWaveFilter program when searching for "
                 "rising values on nearby waveforms.  Increasing this value makes the filter more aggressive.<br><br>"
                 "You can also type the value you want into the text box and press <i>Return</i>.");
QString pmtSliderText = 
  pfmEdit3D::tr ("Move this slider left/right to decrease/increase the PMT AC zero offset threshold.  If a point in the HOF waveform "
                 "is not at least this value above the PMT AC zero offset it will be marked as invalid.  Increasing this value makes "
                 "the filter more aggressive.  Setting this value to 0 disables the AC offset check in the filter.<br><br>"
                 "You can also type the value you want into the text box and press <i>Return</i>.");
QString apdSliderText = 
  pfmEdit3D::tr ("Move this slider left/right to decrease/increase the APD AC zero offset threshold.  If a point in the HOF waveform "
                 "is not at least this value above the APD AC zero offset it will be marked as invalid.  Increasing this value makes "
                 "the filter more aggressive.  Setting this value to 0 disables the AC offset check in the filter.<br><br>"
                 "You can also type the value you want into the text box and press <i>Return</i>.");

QString zNameText = 
  pfmEdit3D::tr ("This is the Z value for the point that is nearest to the cursor.  If the text of the value is in red "
                 "then either an offset has been set and/or a scale has been set in the Depth/Elevation tab of the Preferences dialog "
                 "<img source=\":/icons/prefs.png\">.  This is useful for applying temporary ellipsoid to geoid offsets to "
                 "allow the user to get a better idea of where the land/water boundary is.  Setting the offset value back "
                 "to zero and the scale value back to 1.00 will cause the value to be printed in black.");

QString latNameText = 
  pfmEdit3D::tr ("This is the latitude of the point that is nearest to the cursor.  The format of the latitude may "
                 "be changed in the Position Format tab of Preferences dialog "
                 "<img source=\":/icons/prefs.png\">.");

QString lonNameText = 
  pfmEdit3D::tr ("This is the longitude of the point that is nearest to the cursor.  The format of the longitude may "
                 "be changed in the Position Format tab of the Preferences dialog "
                 "<img source=\":/icons/prefs.png\">.");

QString CZMILUrbanNoiseFlagsText = 
  pfmEdit3D::tr ("The CZMIL Urban noise flags are set on load and by the czmilUrbanFilter program.  This value is displayed "
                 "in binary form so that the user can see exactly what flags are set.  The flags are defined as follows (right "
                 "to left):<br>"
                 "<ul>"
                 "<li>bit 0 - If set, indicates returns from topo processed shots (CZMIL_URBAN_TOPO)</li>"
                 "<li>bit 1 - Indicates returns on channels with 5 or more valid returns (CZMIL_URBAN_SOFT_HIT)</li>"
                 "<li>bit 2 - Indicates returns on channels with only 1 or 2 valid returns (CZMIL_URBAN_HARD_HIT)</li>"
                 "<li>bit 3 - Indicates returns from a shot where the majority of the shallow channels had 3 or fewer valid returns "
                 "(CZMIL_URBAN_MAJORITY)</li>"
                 "<li>bit 4 - Indicates CZMIL_URBAN_HARD_HIT returns within a user specified distance of CZMIL_URBAN_MAJORITY returns "
                 "(CZMIL_URBAN_VICINITY)</li>"
                 "</ul>"
                 "Note that for any value other than 0, bit 0 will always be set.  This indicates that the shot was topo processed.");
