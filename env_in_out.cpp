
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



#include "pfmEdit3D.hpp"


double settings_version = 6.69;


//  - Note to self : I'm not saving the contour intervals because they always get passed in from pfmView

/*!
  These functions store and retrieve the program settings (environment) from a .ini file.  On both Linux and Windows
  the file will be called "misc->qsettings_app".ini (the name of the application with a .ini extension) and will be
  stored in a directory called ABE.config.  On Linux, the ABE.config directory will be stored in your $HOME directory.
  On Windows, it will be stored in your $USERPROFILE folder.  If you make a change to the way a variable is used and
  you want to force the defaults to be restored just change the settings_version to a newer number (I've been using
  the program version number from version.hpp - which you should be updating EVERY time you make a change to the
  program!).  You don't need to change the settings_version though unless you want to force the program to go back
  to the defaults (which can annoy your users).  So, the settings_version won't always match the program version.
*/

uint8_t envin (OPTIONS *options, MISC *misc, QMainWindow *mainWindow)
{
  double saved_version = 0.0;
  QString buttonHotKey[HOTKEYS];
  QString progHotKey[NUMPROGS];


  //  Get the INI file name

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/" + QString (misc->qsettings_app) + ".ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/" + QString (misc->qsettings_app) + ".ini";
#endif

  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup (misc->qsettings_app);


  saved_version = settings.value (QString ("settings version"), saved_version).toDouble ();


  //  We're trying to save the hot keys since these are a PITA for the user to change back if they've made major changes 
  //  to them.  We want to use whatever the user had saved even if we change the settings version.  We will still have to
  //  check for hot key conflicts though since there might be new keys with new default values.  To do this we're going
  //  to save the default hot keys (set in set_defaults.cpp) and then do a comparison.  If there are conflicts we'll replace 
  //  the user's setting with the default and then issue a warning.


  //  Due to massive changes at version 6.40 I'm having to force a complete replacement of ALL of the settings.  I actually
  //  managed to consistently crash Windows 7 if I tried to use the old hotkey settings ;-)

  if (saved_version < 6.40) return (NVFalse);


  uint8_t hotkey_conflict = NVFalse;

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      //  Save the default so we can check it later.

      buttonHotKey[i] = options->buttonAccel[i];


      QString string = misc->buttonText[i] + QString (" hot key");
      options->buttonAccel[i] = settings.value (string, options->buttonAccel[i]).toString ();
    }

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      //  Save the default so we can check it later.

      progHotKey[i] = options->hotkey[i];


      QString name = options->name[i] + QString (" Hotkey");
      options->hotkey[i] = settings.value (name, options->hotkey[i]).toString ();


      //  Specific check for CZMILwaveMonitor actions prior to version 5.09 and chartsPic actions prior to version 6.69.

      if ((saved_version >= 5.09 || options->name[i] != "CZMILwaveMonitor") && (saved_version >= 6.69 || options->name[i] != "chartsPic"))
        {
          name = options->name[i] + QString (" Action Key");
          options->action[i] = settings.value (name, options->action[i]).toString ();
        }


      name = options->name[i] + QString (" State");
      options->state[i] = settings.value (name, options->state[i]).toInt ();
    }


  //  Make sure we have no hotkey duplications.  First, the buttons against the buttons.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      for (int32_t j = i + 1 ; j < HOTKEYS ; j++)
        {
          if (options->buttonAccel[i].toUpper () == options->buttonAccel[j].toUpper ())
            {
              hotkey_conflict = NVTrue;
              options->buttonAccel[i] = buttonHotKey[i];
              options->buttonAccel[j] = buttonHotKey[j];
            }
        }
    }


  //  Next, the programs against the programs.

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      for (int32_t j = i + 1 ; j < NUMPROGS ; j++)
        {
          if (options->hotkey[i].toUpper () == options->hotkey[j].toUpper ())
            {
              hotkey_conflict = NVTrue;
              options->hotkey[i] = progHotKey[i];
              options->hotkey[j] = progHotKey[j];
            }
        }
    }


  //  Finally, the buttons against the programs.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      for (int32_t j = 0 ; j < NUMPROGS ; j++)
        {
          if (options->buttonAccel[i].toUpper () == options->hotkey[j].toUpper ())
            {
              hotkey_conflict = NVTrue;
              options->buttonAccel[i] = buttonHotKey[i];
              options->hotkey[j] = progHotKey[j];
            }
        }
    }


  if (hotkey_conflict)
    {
      QMessageBox::warning (0, QString ("pfmEdit3D settings"),
                            QString ("Hot key conflicts have been detected when reading program settings!<br><br>") +
                            QString ("Some of your hotkeys have been reset to default values!"));
    }


  //  If the settings version has changed we need to leave the values at the new defaults since they may have changed.

  if (settings_version != saved_version) return (NVFalse);


  options->color_index = settings.value (QString ("color index"), options->color_index).toInt ();

  options->flag_index = settings.value (QString ("flag index"), options->flag_index).toInt ();

  options->display_contours = settings.value (QString ("contour flag"), options->display_contours).toBool ();

  options->zoom_percent = settings.value (QString ("zoom percentage"), options->zoom_percent).toInt ();

  options->exaggeration = settings.value (QString ("Z exaggeration"), options->exaggeration).toFloat ();

  options->zx_rotation = settings.value (QString ("ZX rotation"), options->zx_rotation).toFloat ();

  options->y_rotation = settings.value (QString ("Y rotation"), options->y_rotation).toFloat ();

  options->smoothing_factor = settings.value (QString ("smoothing factor"), options->smoothing_factor).toInt ();

  options->contour_location = settings.value (QString ("contour location flag"), options->contour_location).toBool ();

  options->point_size = settings.value (QString ("point size"), options->point_size).toInt ();

  options->main_button_icon_size = settings.value (QString ("main button icon size"), options->main_button_icon_size).toInt ();

  options->contour_width = settings.value (QString ("contour width"), options->contour_width).toInt ();

  options->sparse_limit = settings.value (QString ("sparse point limit"), options->sparse_limit).toInt ();

  options->slice_percent = settings.value (QString ("slice percentage"), options->slice_percent).toInt ();

  options->slice_alpha = settings.value (QString ("slice alpha"), options->slice_alpha).toInt ();

  options->slice_gray = settings.value (QString ("gray slice flag"), options->slice_gray).toBool ();

  options->invalid_edit_color = settings.value (QString ("edit color invalid flag"), options->invalid_edit_color).toBool ();

  options->min_window_size = settings.value (QString ("minimum Z window size"), options->min_window_size).toFloat ();

  options->iho_min_window = settings.value (QString ("IHO min window"), options->iho_min_window).toInt ();

  options->function = settings.value (QString ("function"), options->function).toInt ();

  int32_t red = settings.value (QString ("contour color/red"), options->contour_color.red ()).toInt ();
  int32_t green = settings.value (QString ("contour color/green"), options->contour_color.green ()).toInt ();
  int32_t blue = settings.value (QString ("contour color/blue"), options->contour_color.blue ()).toInt ();
  int32_t alpha = settings.value (QString ("contour color/alpha"), options->contour_color.alpha ()).toInt ();
  options->contour_color.setRgb (red, green, blue, alpha);

  red = settings.value (QString ("edit color/red"), options->edit_color.red ()).toInt ();
  green = settings.value (QString ("edit color/green"), options->edit_color.green ()).toInt ();
  blue = settings.value (QString ("edit color/blue"), options->edit_color.blue ()).toInt ();
  alpha = settings.value (QString ("edit color/alpha"), options->edit_color.alpha ()).toInt ();
  options->edit_color.setRgb (red, green, blue, alpha);

  red = settings.value (QString ("marker color/red"), options->marker_color.red ()).toInt ();
  green = settings.value (QString ("marker color/green"), options->marker_color.green ()).toInt ();
  blue = settings.value (QString ("marker color/blue"), options->marker_color.blue ()).toInt ();
  alpha = settings.value (QString ("marker color/alpha"), options->marker_color.alpha ()).toInt ();
  options->marker_color.setRgb (red, green, blue, alpha);

  red = settings.value (QString ("tracker color/red"), options->tracker_color.red ()).toInt ();
  green = settings.value (QString ("tracker color/green"), options->tracker_color.green ()).toInt ();
  blue = settings.value (QString ("tracker color/blue"), options->tracker_color.blue ()).toInt ();
  alpha = settings.value (QString ("tracker color/alpha"), options->tracker_color.alpha ()).toInt ();
  options->tracker_color.setRgb (red, green, blue, alpha);

  red = settings.value (QString ("background color/red"), options->background_color.red ()).toInt ();
  green = settings.value (QString ("background color/green"), options->background_color.green ()).toInt ();
  blue = settings.value (QString ("background color/blue"), options->background_color.blue ()).toInt ();
  alpha = settings.value (QString ("background color/alpha"), options->background_color.alpha ()).toInt ();
  options->background_color.setRgb (red, green, blue, alpha);

  red = settings.value (QString ("reference color/red"), options->ref_color[0].red ()).toInt ();
  green = settings.value (QString ("reference color/green"), options->ref_color[0].green ()).toInt ();
  blue = settings.value (QString ("reference color/blue"), options->ref_color[0].blue ()).toInt ();
  alpha = settings.value (QString ("reference color/alpha"), options->ref_color[0].alpha ()).toInt ();
  options->ref_color[0].setRgb (red, green, blue, alpha);

  red = settings.value (QString ("feature color/red"), options->feature_color.red ()).toInt ();
  green = settings.value (QString ("feature color/green"), options->feature_color.green ()).toInt ();
  blue = settings.value (QString ("feature color/blue"), options->feature_color.blue ()).toInt ();
  alpha = settings.value (QString ("feature color/alpha"), options->feature_color.alpha ()).toInt ();
  options->feature_color.setRgb (red, green, blue, alpha);

  red = settings.value (QString ("feature info color/red"), options->feature_info_color.red ()).toInt ();
  green = settings.value (QString ("feature info color/green"), options->feature_info_color.green ()).toInt ();
  blue = settings.value (QString ("feature info color/blue"), options->feature_info_color.blue ()).toInt ();
  alpha = settings.value (QString ("feature info color/alpha"), options->feature_info_color.alpha ()).toInt ();
  options->feature_info_color.setRgb (red, green, blue, alpha);

  red = settings.value (QString ("feature poly color/red"), options->feature_poly_color.red ()).toInt ();
  green = settings.value (QString ("feature poly color/green"), options->feature_poly_color.green ()).toInt ();
  blue = settings.value (QString ("feature poly color/blue"), options->feature_poly_color.blue ()).toInt ();
  alpha = settings.value (QString ("feature poly color/alpha"), options->feature_poly_color.alpha ()).toInt ();
  options->feature_poly_color.setRgb (red, green, blue, alpha);

  red = settings.value (QString ("highlighted feature color/red"), options->feature_highlight_color.red ()).toInt ();
  green = settings.value (QString ("highlighted feature color/green"), options->feature_highlight_color.green ()).toInt ();
  blue = settings.value (QString ("highlighted feature color/blue"), options->feature_highlight_color.blue ()).toInt ();
  alpha = settings.value (QString ("highlighted feature color/alpha"), options->feature_highlight_color.alpha ()).toInt ();
  options->feature_highlight_color.setRgb (red, green, blue, alpha);

  red = settings.value (QString ("verified feature color/red"), options->verified_feature_color.red ()).toInt ();
  green = settings.value (QString ("verified feature color/green"), options->verified_feature_color.green ()).toInt ();
  blue = settings.value (QString ("verified feature color/blue"), options->verified_feature_color.blue ()).toInt ();
  alpha = settings.value (QString ("verified feature color/alpha"), options->verified_feature_color.alpha ()).toInt ();
  options->verified_feature_color.setRgb (red, green, blue, alpha);


  red = settings.value (QString ("scale color/red"), options->scale_color.red ()).toInt ();
  green = settings.value (QString ("scale color/green"), options->scale_color.green ()).toInt ();
  blue = settings.value (QString ("scale color/blue"), options->scale_color.blue ()).toInt ();
  alpha = settings.value (QString ("scale color/alpha"), options->scale_color.alpha ()).toInt ();
  options->scale_color.setRgb (red, green, blue, alpha);


  red = settings.value (QString ("scaled offset z color/red"), options->scaled_offset_z_color.red ()).toInt ();
  green = settings.value (QString ("scaled offset z color/green"), options->scaled_offset_z_color.green ()).toInt ();
  blue = settings.value (QString ("scaled offset z color/blue"), options->scaled_offset_z_color.blue ()).toInt ();
  alpha = settings.value (QString ("scaled offset z color/alpha"), options->scaled_offset_z_color.alpha ()).toInt ();
  options->scaled_offset_z_color.setRgb (red, green, blue, alpha);


  for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++)
    {
      QString string = QString ("Wave color %1").arg (i, 2, 10, QChar ('0'));
      red = settings.value (string + "/red", options->waveColor[i].red ()).toInt ();
      green = settings.value (string + "/green", options->waveColor[i].green ()).toInt ();
      blue = settings.value (string + "/blue", options->waveColor[i].blue ()).toInt ();
      alpha = settings.value (string + "/alpha", options->waveColor[i].alpha ()).toInt ();
      options->waveColor[i].setRgb (red, green, blue, alpha);

      misc->abe_share->mwShare.multiColors[i].r = red;
      misc->abe_share->mwShare.multiColors[i].g = green;
      misc->abe_share->mwShare.multiColors[i].b = blue;
      misc->abe_share->mwShare.multiColors[i].a = alpha;
    }


  for (int32_t i = 0 ; i < NUM_SCALE_LEVELS ; i++)
    {
      QString string = QString ("minimum hsv color value %1").arg (i);
      options->min_hsv_color[i] = settings.value (string, options->min_hsv_color[i]).toInt ();
      string = QString ("maximum hsv color value %1").arg (i);
      options->max_hsv_color[i] = settings.value (string, options->max_hsv_color[i]).toInt ();
      string = QString ("minimum hsv locked flag %1").arg (i);
      options->min_hsv_locked[i] = settings.value (string, options->min_hsv_locked[i]).toBool ();
      string = QString ("maximum hsv locked flag %1").arg (i);
      options->max_hsv_locked[i] = settings.value (string, options->max_hsv_locked[i]).toBool ();
      string = QString ("minimum hsv locked value %1").arg (i);
      options->min_hsv_value[i] = settings.value (string, options->min_hsv_value[i]).toFloat ();
      string = QString ("maximum hsv locked value %1").arg (i);
      options->max_hsv_value[i] = settings.value (string, options->max_hsv_value[i]).toFloat ();
    }


  options->feature_size = (float) settings.value (QString ("feature size"), options->feature_size).toDouble ();

  options->draw_scale = settings.value (QString ("draw scale flag"), options->draw_scale).toBool ();

  options->auto_scale = settings.value (QString ("auto scale flag"), options->auto_scale).toBool ();

  options->hofWaveFilter_search_radius = settings.value (QString ("hofWaveFilter search radius"), options->hofWaveFilter_search_radius).toDouble ();
  options->hofWaveFilter_search_width = settings.value (QString ("hofWaveFilter search width"), options->hofWaveFilter_search_width).toInt ();
  options->hofWaveFilter_rise_threshold = settings.value (QString ("hofWaveFilter rise threshold"), options->hofWaveFilter_rise_threshold).toInt ();
  options->hofWaveFilter_pmt_ac_zero_offset_required = settings.value (QString ("hofWaveFilter PMT AC zero offset required"),
                                                                       options->hofWaveFilter_pmt_ac_zero_offset_required).toInt ();
  options->hofWaveFilter_apd_ac_zero_offset_required = settings.value (QString ("hofWaveFilter APD AC zero offset required"),
                                                                       options->hofWaveFilter_apd_ac_zero_offset_required).toInt ();

  options->czmil_cpf_filter_inval = settings.value (QString ("cpfDeepFilter invalidate flag"), options->czmil_cpf_filter_inval).toBool ();
  options->czmil_cpf_filter_amp_min = settings.value (QString ("cpfDeepFilter waveform amplitude minimum"),
                                                      options->czmil_cpf_filter_amp_min).toInt ();

  options->display_feature = settings.value (QString ("view feature positions"), options->display_feature).toInt ();

  options->display_children = settings.value (QString ("view sub-feature positions"), options->display_children).toBool ();

  options->display_feature_info = settings.value (QString ("view feature info"), options->display_feature_info).toBool ();

  options->display_feature_poly = settings.value (QString ("view feature polygon"), options->display_feature_poly).toBool ();

  options->screenshot_delay = settings.value (QString ("screenshot delay"), options->screenshot_delay).toInt ();

  options->display_reference = settings.value (QString ("view reference data positions"),
                                               options->display_reference).toBool ();

  options->unload_prog = settings.value (QString ("unload program name"), options->unload_prog).toString ();
  options->auto_unload = settings.value (QString ("auto unload flag"), options->auto_unload).toBool ();


  options->last_rock_feature_desc = settings.value (QString ("last rock feature descriptor index"), options->last_rock_feature_desc).toInt ();

  options->last_offshore_feature_desc = settings.value (QString ("last offshore feature descriptor index"),
                                                        options->last_offshore_feature_desc).toInt ();

  options->last_light_feature_desc = settings.value (QString ("last light feature descriptor index"), options->last_light_feature_desc).toInt ();

  options->last_lidar_feature_desc = settings.value (QString ("last lidar feature descriptor index"), options->last_lidar_feature_desc).toInt ();

  options->last_feature_description = settings.value (QString ("last feature description"), options->last_feature_description).toString ();

  settings.setValue (QString ("overlap percent"), options->overlap_percent);

  options->last_feature_remarks = settings.value (QString ("last feature remarks"), options->last_feature_remarks).toString ();

  options->overlap_percent = settings.value (QString ("overlap percent"), options->overlap_percent).toInt ();

  options->filter_type = settings.value (QString ("filter type"), options->filter_type).toInt ();

  options->area_filter_std = settings.value (QString ("filter standard deviation"), options->area_filter_std).toDouble ();

  options->stat_filter_std = settings.value (QString ("AVT filter standard deviation"), options->stat_filter_std).toDouble ();

  options->stat_filter_radius = settings.value (QString ("AVT statistical filter radius"), options->stat_filter_radius).toDouble ();

  options->stat_filter_count = settings.value (QString ("AVT statistical filter count"), options->stat_filter_count).toInt ();

  options->stat_filter_passes = settings.value (QString ("AVT filter passes"), options->stat_filter_passes).toInt ();

  options->deep_filter_only = settings.value (QString ("deep filter only flag"), options->deep_filter_only).toBool ();

  options->feature_radius = settings.value (QString ("feature filter exclusion radius"), options->feature_radius).toFloat ();

  options->num_hp_threads = settings.value (QString ("HP filter number of threads"), options->num_hp_threads).toInt ();

  options->hp_filter_radius = settings.value (QString ("HP filter radius"), options->hp_filter_radius).toDouble ();

  options->hp_filter_height = settings.value (QString ("HP filter height"), options->hp_filter_height).toDouble ();

  options->hp_filter_neighbors = settings.value (QString ("HP filter neighbors"), options->hp_filter_neighbors).toInt ();

  options->hp_filter_ml_neighbors = settings.value (QString ("HP filter multi-line neighbors"), options->hp_filter_ml_neighbors).toInt ();

  options->hp_filter_passes = settings.value (QString ("HP filter passes"), options->hp_filter_passes).toInt ();

  options->filter_apply_z_range = settings.value (QString ("filter apply z range flag"), options->filter_apply_z_range).toBool ();

  options->filter_min_z = settings.value (QString ("filter minimum z"), options->filter_min_z).toFloat ();

  options->filter_max_z = settings.value (QString ("filter maximum z"), options->filter_max_z).toFloat ();

  options->undo_levels = settings.value (QString ("undo levels"), options->undo_levels).toInt ();

  options->pref_tab = settings.value (QString ("preference tab"), options->pref_tab).toInt ();

  options->lat_or_east = settings.value (QString ("latitude or easting"), options->lat_or_east).toBool ();

  options->feature_dir = settings.value (QString ("feature directory"), options->feature_dir).toString ();


  options->kill_and_respawn = settings.value (QString ("kill and respawn flag"), options->kill_and_respawn).toBool ();

  options->highlight_tip = settings.value (QString ("highlighted point tool tip flag"), options->highlight_tip).toBool ();


  options->auto_filter_mask = settings.value (QString ("auto filter mask flag"), options->auto_filter_mask).toBool ();

  options->track_tab = settings.value (QString ("track tab flag"), options->track_tab).toBool ();

  options->no_color_scale_masked = settings.value (QString ("no color scale masked data flag"), options->no_color_scale_masked).toBool ();

  options->czmil_suppress_warnings = settings.value (QString ("suppress czmilReprocess warnings"), options->czmil_suppress_warnings).toBool ();

  options->czmil_reprocess_mode = settings.value (QString ("czmilReprocess mode"), options->czmil_reprocess_mode).toBool ();

  for (int32_t i = 0 ; i < 2 ; i++) options->czmil_reprocess_flags[i] = settings.value (QString ("czmilReprocess flag[%1]").arg (i),
                                                                                        options->czmil_reprocess_flags[i]).toInt ();

  for (int32_t i = 0 ; i < 9 ; i++) options->czmil_probability[i] = settings.value (QString ("CZMIL channel[%1] probability threshold").arg (i),
                                                                                    options->czmil_probability[i]).toFloat ();

  for (int32_t i = 0 ; i < LIDAR_CLASSES ; i++) options->from_lidar_class[i] = settings.value (QString ("from lidar classifications[%1]").arg (i),
                                                                                               options->from_lidar_class[i]).toInt ();
  options->lidar_class = settings.value (QString ("lidar classification"), options->lidar_class).toInt ();

  options->hide_persistent = settings.value (QString ("hide persistent flag"), options->hide_persistent).toBool ();
  options->hide_visible = settings.value (QString ("hide visible flag"), options->hide_visible).toBool ();
  options->mask_active = settings.value (QString ("mask active flags"), options->mask_active).toInt ();
  options->hide_num = settings.value (QString ("number of hide entries"), options->hide_num).toInt();
  for (int32_t i = 0 ; i < options->hide_num ; i++)
    {
      options->hide_flag[i] = settings.value (QString ("hide[%1] flag").arg (i), options->hide_flag[i]).toBool ();
      options->hide_name[i] = settings.value (QString ("hide name[%1]").arg (i), options->hide_name[i]).toString ();
    }


  options->lineFilterText = settings.value ("Line number filter range string", options->lineFilterText).toString ();
  options->horzFilterText = settings.value ("Horizontal error filter range string", options->horzFilterText).toString ();
  options->vertFilterText = settings.value ("Vertical error filter range string", options->vertFilterText).toString ();


  //  Retrieve the possible attribute filter range strings.

  options->time_attr_filt = settings.value (options->time_attribute.name + QString (" filter ranges"), options->time_attr_filt).toString ();
  for (int32_t i = 0 ; i < GSF_ATTRIBUTES ; i++)
    options->gsf_attr_filt[i] = settings.value (options->gsf_attribute[i].name + QString (" filter ranges"), options->gsf_attr_filt[i]).toString ();
  for (int32_t i = 0 ; i < HOF_ATTRIBUTES ; i++)
    options->hof_attr_filt[i] = settings.value (options->hof_attribute[i].name + QString (" filter ranges"), options->hof_attr_filt[i]).toString ();
  for (int32_t i = 0 ; i < TOF_ATTRIBUTES ; i++)
    options->tof_attr_filt[i] = settings.value (options->tof_attribute[i].name + QString (" filter ranges"), options->tof_attr_filt[i]).toString ();
  for (int32_t i = 0; i < CZMIL_ATTRIBUTES; i++)
    options->czmil_attr_filt[i] = settings.value (options->czmil_attribute[i].name + QString (" filter ranges"),
                                                  options->czmil_attr_filt[i]).toString ();
  for (int32_t i = 0 ; i < LAS_ATTRIBUTES ; i++)
    options->las_attr_filt[i] = settings.value (options->las_attribute[i].name + QString (" filter ranges"), options->las_attr_filt[i]).toString ();
  for (int32_t i = 0; i < BAG_ATTRIBUTES; i++)
    options->bag_attr_filt[i] = settings.value (options->bag_attribute[i].name + QString (" filter ranges"), options->bag_attr_filt[i]).toString ();


  //  Save the Status tab attribute names.

  for (int32_t i = 0 ; i < POST_ATTR ; i++)
    {
      options->attrStatName[i] = settings.value (QString ("status tab attribute name %1").arg (i), options->attrStatName[i]).toString ();
    }


  //  This is pretty confusing.  What we're doing is finding out which attribute in the current PFM matches which possible attribute
  //  so that we can retrieve the attribute filter string associated with that attribute (not the attribute number of that attribute in this
  //  particular PFM).  That way, when we open another PFM, if the attributes are in a different order we can assign the correct 
  //  filter ranges to the correct attribute.

  for (int32_t i = 0 ; i < NUM_ATTR ; i++)
    {
      uint8_t done = NVFalse;

      if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->time_attribute.name)
        {
          options->attrFilterText[i] = options->time_attr_filt;
          done = NVTrue;
        }

      if (!done)
        {
          for (int32_t j = 0 ; j < GSF_ATTRIBUTES ; j++)
            {
              if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->gsf_attribute[j].name)
                {
                  options->attrFilterText[i] = options->gsf_attr_filt[j];
                  done = NVTrue;
                  break;
                }
            }

          if (!done)
            {
              for (int32_t j = 0 ; j < HOF_ATTRIBUTES ; j++)
                {
                  if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->hof_attribute[j].name)
                    {
                      options->attrFilterText[i] = options->hof_attr_filt[j];
                      done = NVTrue;
                      break;
                    }
                }

              if (!done)
                {
                  for (int32_t j = 0 ; j < TOF_ATTRIBUTES ; j++)
                    {
                      if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->tof_attribute[j].name)
                        {
                          options->attrFilterText[i] = options->tof_attr_filt[j];
                          done = NVTrue;
                          break;
                        }
                    }

                  if (!done)
                    {
                      for (int32_t j = 0 ; j < CZMIL_ATTRIBUTES ; j++)
                        {
                          if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->czmil_attribute[j].name)
                            {
                              options->attrFilterText[i] = options->czmil_attr_filt[j];
                              done = NVTrue;
                              break;
                            }
                        }

                      if (!done)
                        {
                          for (int32_t j = 0 ; j < LAS_ATTRIBUTES ; j++)
                            {
                              if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->las_attribute[j].name)
                                {
                                  options->attrFilterText[i] = options->las_attr_filt[j];
                                  done = NVTrue;
                                  break;
                                }
                            }

                          if (!done)
                            {
                              for (int32_t j = 0 ; j < BAG_ATTRIBUTES ; j++)
                                {
                                  if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->bag_attribute[j].name)
                                    {
                                      options->attrFilterText[i] = options->bag_attr_filt[j];
                                      break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


  options->msg_window_x = settings.value (QString ("message window x position"), options->msg_window_x).toInt ();
  options->msg_window_y = settings.value (QString ("message window y position"), options->msg_window_y).toInt ();
  options->msg_width = settings.value (QString ("message window width"), options->msg_width).toInt ();
  options->msg_height = settings.value (QString ("message window height"), options->msg_height).toInt ();


  //  This is the killed flag that can be set when pfmView exits.  If pfmView was shut down since the last time
  //  this program was running we may have ancillary program states set to "running" (2) even though pfmView
  //  killed them.

  int32_t pfmView_killed = 0;
  pfmView_killed = settings.value (QString ("pfmView killed"), pfmView_killed).toInt();


  //  Now we set the ancillary program states if pfmView was killed (possibly with ancillary programs running).

  if (pfmView_killed)
    {
      for (int32_t i = 0 ; i < NUMPROGS ; i++)
        {
          //  If it was anything other than 0 we want to make sure it's set to 1.

          if (options->state[i]) options->state[i] = 1;
        }
    }


  mainWindow->restoreState (settings.value (QString ("main window state")).toByteArray (), NINT (settings_version * 100.0L));

  mainWindow->restoreGeometry (settings.value (QString ("main window geometry")).toByteArray ());


  settings.endGroup ();


  //  We need to get the Z orientation value from the globalABE settings file.  This value is used in some of the ABE programs
  //  to determine whether Z is displayed as depth (the default) or elevation.  We also want to get the font from the global
  //  settings since it will be used in (hopefully) all of the ABE map GUI applications.  Also, the CZMIL urban noise filter
  //  settings are shared between pfmEdit3D and czmilUrbanFilter.

#ifdef NVWIN3X
  QString ini_file2 = QString (getenv ("USERPROFILE")) + "/ABE.config/" + "globalABE.ini";
#else
  QString ini_file2 = QString (getenv ("HOME")) + "/ABE.config/" + "globalABE.ini";
#endif

  QSettings settings2 (ini_file2, QSettings::IniFormat);
  settings2.beginGroup ("globalABE");


  options->z_orientation = settings2.value (QString ("ABE Z orientation factor"), options->z_orientation).toFloat ();

  QString defaultFont = options->font.toString ();
  QString fontString = settings2.value (QString ("ABE map GUI font"), defaultFont).toString ();
  options->font.fromString (fontString);


  options->czmil_urban_radius[0] = settings2.value (QString ("CZMIL urban vicinity radius"), options->czmil_urban_radius[0]).toDouble ();
  options->czmil_urban_radius[1] = settings2.value (QString ("CZMIL urban filter radius"), options->czmil_urban_radius[1]).toDouble ();
  options->czmil_urban_height[0] = settings2.value (QString ("CZMIL urban vicinity height"), options->czmil_urban_height[0]).toDouble ();
  options->czmil_urban_height[1] = settings2.value (QString ("CZMIL urban filter height"), options->czmil_urban_height[1]).toDouble ();
  options->czmil_urban_d_index[0] = settings2.value (QString ("CZMIL urban filter lower D_index"), options->czmil_urban_d_index[0]).toInt ();
  options->czmil_urban_d_index[1] = settings2.value (QString ("CZMIL urban filter upper D_index"), options->czmil_urban_d_index[1]).toInt ();


  //  Default feature type is used in all ABE programs that create or edit features.  The reason I want to inform the users here as well as in
  //  pfmView is that pfmEdit3D can be shelled from pfmEditShell (HydroFusion does that).

  uint8_t ft = 99;
  ft = (uint8_t) settings2.value (QString ("default feature type"), ft).toInt ();


  //  If the user has never set the default feature type we need to inform them of it and let them choose one.

  if (ft >= BFDATA_FEATURE_TYPES)
    {
      QMessageBox msgBox;

      msgBox.setText (pfmEdit3D::tr ("A new field has been added to the Binary Feature Data (BFD) format.  This field defines the feature type.  "
                                     "There are two possible feature types, <b>Hydrographic</b> and <b>Informational</b>.  The type of feature used "
                                     "will affect the way Area-Based Editor (ABE) programs mark point data.  For <b>Hydrographic</b> features, ABE will "
                                     "try to mark the nearest associated data point with the PFM_SELECTED_FEATURE flag.  When "
                                     "GSF and/or CZMIL CPF files are the input files for the PFM, these flags will be unloaded to the input files and "
                                     "translated to the corresponding GSF or CPF flags.  If you intend to mostly use features for information purposes "
                                     "(e.g. to mark areas of interest or problem areas) then you should select <b>Informational</b> as the default feature "
                                     "type.  That way the underlying data will not be marked.<br><br>The default feature type can be changed at any time "
                                     "in the feature editing dialog.  Changes made to the feature type in the feature editing dialog will be saved as the "
                                     "new default feature type for all ABE programs.  Also note that feature type was not available for pre-3.0 BFD files. "
                                     "If you are using a preexisting pre-3.0 BFD file you can still force the program to use <b>Informational</b> "
                                     "feature types but they won't be saved to the file.  However, it will prevent the ABE programs from setting the "
                                     "flags on data points.<br><br>"
                                     "Please select the default feature type."));

      QAbstractButton *pButtonYes = msgBox.addButton (pfmEdit3D::tr ("Hydrographic"), QMessageBox::YesRole);
      msgBox.addButton (pfmEdit3D::tr ("Informational"), QMessageBox::NoRole);
 
      msgBox.exec ();
 
      if (msgBox.clickedButton () == pButtonYes)
        {
          options->default_feature_type = BFDATA_HYDROGRAPHIC;
        }
      else
        {
          options->default_feature_type = BFDATA_INFORMATIONAL;
        }
    }
  else
    {
      options->default_feature_type = ft;
    }


  settings2.endGroup ();

  return (NVTrue);
}


void envout (OPTIONS *options, MISC *misc, QMainWindow *mainWindow)
{
  //  Get the INI file name

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/" + QString (misc->qsettings_app) + ".ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/" + QString (misc->qsettings_app) + ".ini";
#endif

  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup (misc->qsettings_app);

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      QString string = misc->buttonText[i] + QString (" hot key");
      settings.setValue (string, options->buttonAccel[i]);
    }

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      QString name = options->name[i] + QString (" Hotkey");
      settings.setValue (name, options->hotkey[i]);

      name = options->name[i] + QString (" Action Key");
      settings.setValue (name, options->action[i]);

      name = options->name[i] + QString (" State");
      settings.setValue (name, options->state[i]);
    }

  settings.setValue (QString ("settings version"), settings_version);

  settings.setValue (QString ("color index"), options->color_index);

  settings.setValue (QString ("flag index"), options->flag_index);

  settings.setValue (QString ("contour flag"), options->display_contours);

  settings.setValue (QString ("zoom percentage"), options->zoom_percent);

  settings.setValue (QString ("Z exaggeration"), options->exaggeration);

  settings.setValue (QString ("ZX rotation"), options->zx_rotation);

  settings.setValue (QString ("Y rotation"), options->y_rotation);

  settings.setValue (QString ("contour width"), options->contour_width);

  settings.setValue (QString ("smoothing factor"), options->smoothing_factor);

  settings.setValue (QString ("contour location flag"), options->contour_location);

  settings.setValue (QString ("main button icon size"), options->main_button_icon_size);

  settings.setValue (QString ("point size"), options->point_size);

  settings.setValue (QString ("sparse point limit"), options->sparse_limit);

  settings.setValue (QString ("slice percentage"), options->slice_percent);

  settings.setValue (QString ("slice alpha"), options->slice_alpha);

  settings.setValue (QString ("gray slice flag"), options->slice_gray);

  settings.setValue (QString ("edit color invalid flag"), options->invalid_edit_color);

  settings.setValue (QString ("minimum Z window size"), options->min_window_size);

  settings.setValue (QString ("IHO min window"), options->iho_min_window);

  settings.setValue (QString ("function"), options->function);

  settings.setValue (QString ("contour color/red"), options->contour_color.red ());
  settings.setValue (QString ("contour color/green"), options->contour_color.green ());
  settings.setValue (QString ("contour color/blue"), options->contour_color.blue ());
  settings.setValue (QString ("contour color/alpha"), options->contour_color.alpha ());

  settings.setValue (QString ("edit color/red"), options->edit_color.red ());
  settings.setValue (QString ("edit color/green"), options->edit_color.green ());
  settings.setValue (QString ("edit color/blue"), options->edit_color.blue ());
  settings.setValue (QString ("edit color/alpha"), options->edit_color.alpha ());

  settings.setValue (QString ("marker color/red"), options->marker_color.red ());
  settings.setValue (QString ("marker color/green"), options->marker_color.green ());
  settings.setValue (QString ("marker color/blue"), options->marker_color.blue ());
  settings.setValue (QString ("marker color/alpha"), options->marker_color.alpha ());

  settings.setValue (QString ("tracker color/red"), options->tracker_color.red ());
  settings.setValue (QString ("tracker color/green"), options->tracker_color.green ());
  settings.setValue (QString ("tracker color/blue"), options->tracker_color.blue ());
  settings.setValue (QString ("tracker color/alpha"), options->tracker_color.alpha ());

  settings.setValue (QString ("background color/red"), options->background_color.red ());
  settings.setValue (QString ("background color/green"), options->background_color.green ());
  settings.setValue (QString ("background color/blue"), options->background_color.blue ());
  settings.setValue (QString ("background color/alpha"), options->background_color.alpha ());

  settings.setValue (QString ("reference color/red"), options->ref_color[0].red ());
  settings.setValue (QString ("reference color/green"), options->ref_color[0].green ());
  settings.setValue (QString ("reference color/blue"), options->ref_color[0].blue ());
  settings.setValue (QString ("reference color/alpha"), options->ref_color[0].alpha ());

  settings.setValue (QString ("feature color/red"), options->feature_color.red ());
  settings.setValue (QString ("feature color/green"), options->feature_color.green ());
  settings.setValue (QString ("feature color/blue"), options->feature_color.blue ());
  settings.setValue (QString ("feature color/alpha"), options->feature_color.alpha ());

  settings.setValue (QString ("feature info color/red"), options->feature_info_color.red ());
  settings.setValue (QString ("feature info color/green"), options->feature_info_color.green ());
  settings.setValue (QString ("feature info color/blue"), options->feature_info_color.blue ());
  settings.setValue (QString ("feature info color/alpha"), options->feature_info_color.alpha ());

  settings.setValue (QString ("feature poly color/red"), options->feature_poly_color.red ());
  settings.setValue (QString ("feature poly color/green"), options->feature_poly_color.green ());
  settings.setValue (QString ("feature poly color/blue"), options->feature_poly_color.blue ());
  settings.setValue (QString ("feature poly color/alpha"), options->feature_poly_color.alpha ());

  settings.setValue (QString ("highlighted feature color/red"), options->feature_highlight_color.red ());
  settings.setValue (QString ("highlighted feature color/green"), options->feature_highlight_color.green ());
  settings.setValue (QString ("highlighted feature color/blue"), options->feature_highlight_color.blue ());
  settings.setValue (QString ("highlighted feature color/alpha"), options->feature_highlight_color.alpha ());

  settings.setValue (QString ("verified feature color/red"), options->verified_feature_color.red ());
  settings.setValue (QString ("verified feature color/green"), options->verified_feature_color.green ());
  settings.setValue (QString ("verified feature color/blue"), options->verified_feature_color.blue ());
  settings.setValue (QString ("verified feature color/alpha"), options->verified_feature_color.alpha ());

  settings.setValue (QString ("scale color/red"), options->scale_color.red ());
  settings.setValue (QString ("scale color/green"), options->scale_color.green ());
  settings.setValue (QString ("scale color/blue"), options->scale_color.blue ());
  settings.setValue (QString ("scale color/alpha"), options->scale_color.alpha ());

  settings.setValue (QString ("scaled offset z color/red"), options->scaled_offset_z_color.red ());
  settings.setValue (QString ("scaled offset z color/green"), options->scaled_offset_z_color.green ());
  settings.setValue (QString ("scaled offset z color/blue"), options->scaled_offset_z_color.blue ());
  settings.setValue (QString ("scaled offset z color/alpha"), options->scaled_offset_z_color.alpha ());

  for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++)
    {
      QString string = QString ("Wave color %1").arg (i, 2, 10, QChar ('0'));
      settings.setValue (string + "/red", options->waveColor[i].red ());
      settings.setValue (string + "/green", options->waveColor[i].green ());
      settings.setValue (string + "/blue", options->waveColor[i].blue ());
      settings.setValue (string + "/alpha", options->waveColor[i].alpha ());
    }


  for (int32_t i = 0 ; i < NUM_SCALE_LEVELS ; i++)
    {
      QString string = QString ("minimum hsv color value %1").arg (i);
      settings.setValue (string, options->min_hsv_color[i]);
      string = QString ("maximum hsv color value %1").arg (i);
      settings.setValue (string, options->max_hsv_color[i]);
      string = QString ("minimum hsv locked flag %1").arg (i);
      settings.setValue (string, options->min_hsv_locked[i]);
      string = QString ("maximum hsv locked flag %1").arg (i);
      settings.setValue (string, options->max_hsv_locked[i]);
      string = QString ("minimum hsv locked value %1").arg (i);
      settings.setValue (string, options->min_hsv_value[i]);
      string = QString ("maximum hsv locked value %1").arg (i);
      settings.setValue (string, options->max_hsv_value[i]);
    }


  settings.setValue (QString ("feature size"), options->feature_size);

  settings.setValue (QString ("draw scale flag"), options->draw_scale);

  settings.setValue (QString ("auto scale flag"), options->auto_scale);

  settings.setValue (QString ("hofWaveFilter search radius"), options->hofWaveFilter_search_radius);
  settings.setValue (QString ("hofWaveFilter search width"), options->hofWaveFilter_search_width);
  settings.setValue (QString ("hofWaveFilter rise threshold"), options->hofWaveFilter_rise_threshold);
  settings.setValue (QString ("hofWaveFilter PMT AC zero offset required"), options->hofWaveFilter_pmt_ac_zero_offset_required);
  settings.setValue (QString ("hofWaveFilter APD AC zero offset required"), options->hofWaveFilter_apd_ac_zero_offset_required);

  settings.setValue (QString ("cpfDeepFilter invalidate flag"), options->czmil_cpf_filter_inval);
  settings.setValue (QString ("cpfDeepFilter waveform amplitude minimum"), options->czmil_cpf_filter_amp_min);

  settings.setValue (QString ("view feature positions"), options->display_feature);
  settings.setValue (QString ("view sub-feature positions"), options->display_children);
  settings.setValue (QString ("view feature info"), options->display_feature_info);
  settings.setValue (QString ("view feature polygon"), options->display_feature_poly);
  settings.setValue (QString ("screenshot delay"), options->screenshot_delay);
  settings.setValue (QString ("view reference data positions"), options->display_reference);

  settings.setValue (QString ("unload program name"), options->unload_prog);
  settings.setValue (QString ("auto unload flag"), options->auto_unload);

  settings.setValue (QString ("last rock feature descriptor index"), options->last_rock_feature_desc);

  settings.setValue (QString ("last offshore feature descriptor index"), options->last_offshore_feature_desc);

  settings.setValue (QString ("last light feature descriptor index"), options->last_light_feature_desc);

  settings.setValue (QString ("last lidar feature descriptor index"), options->last_lidar_feature_desc);

  settings.setValue (QString ("last feature description"), options->last_feature_description);

  settings.setValue (QString ("last feature remarks"), options->last_feature_remarks);

  settings.setValue (QString ("filter type"), options->filter_type);

  settings.setValue (QString ("filter standard deviation"), options->area_filter_std);

  settings.setValue (QString ("AVT filter standard deviation"), options->stat_filter_std);

  settings.setValue (QString ("AVT statistical filter radius"), options->stat_filter_radius);

  settings.setValue (QString ("AVT statistical filter count"), options->stat_filter_count);

  settings.setValue (QString ("AVT filter passes"), options->stat_filter_passes);

  settings.setValue (QString ("deep filter only flag"), options->deep_filter_only);

  settings.setValue (QString ("feature filter exclusion radius"), options->feature_radius);

  settings.setValue (QString ("HP filter number of threads"), options->num_hp_threads);

  settings.setValue (QString ("HP filter radius"), options->hp_filter_radius);

  settings.setValue (QString ("HP filter height"), options->hp_filter_height);

  settings.setValue (QString ("HP filter neighbors"), options->hp_filter_neighbors);

  settings.setValue (QString ("HP filter multi-line neighbors"), options->hp_filter_ml_neighbors);

  settings.setValue (QString ("HP filter passes"), options->hp_filter_passes);

  settings.setValue (QString ("filter apply z range flag"), options->filter_apply_z_range);

  settings.setValue (QString ("filter minimum z"), options->filter_min_z);

  settings.setValue (QString ("filter maximum z"), options->filter_max_z);

  settings.setValue (QString ("undo levels"), options->undo_levels);

  settings.setValue (QString ("preference tab"), options->pref_tab);

  settings.setValue (QString ("latitude or easting"), options->lat_or_east);


  settings.setValue (QString ("feature directory"), options->feature_dir);


  settings.setValue (QString ("kill and respawn flag"), options->kill_and_respawn);

  settings.setValue (QString ("highlighted point tool tip flag"), options->highlight_tip);

  settings.setValue (QString ("auto filter mask flag"), options->auto_filter_mask);

  settings.setValue (QString ("track tab flag"), options->track_tab);

  settings.setValue (QString ("no color scale masked data flag"), options->no_color_scale_masked);

  settings.setValue (QString ("suppress czmilReprocess warnings"), options->czmil_suppress_warnings);

  settings.setValue (QString ("czmilReprocess mode"), options->czmil_reprocess_mode);

  for (int32_t i = 0 ; i < 2 ; i++) settings.setValue (QString ("czmilReprocess flag[%1]").arg (i), options->czmil_reprocess_flags[i]);

  for (int32_t i = 0 ; i < 9 ; i++) settings.setValue (QString ("CZMIL channel[%1] probability threshold").arg (i), options->czmil_probability[i]);

  for (int32_t i = 0 ; i < LIDAR_CLASSES ; i++) settings.setValue (QString ("from lidar classifications[%1]").arg (i), options->from_lidar_class[i]);
  settings.setValue (QString ("lidar classification"), options->lidar_class);

  settings.setValue (QString ("hide persistent flag"), options->hide_persistent);
  settings.setValue (QString ("hide visible flag"), options->hide_visible);
  settings.setValue (QString ("mask active flags"), options->mask_active);
  settings.setValue (QString ("number of hide entries"), options->hide_num);
  for (int32_t i = 0 ; i < HIDE_SLOTS ; i++)
    {
      settings.setValue (QString ("hide[%1] flag").arg (i), options->hide_flag[i]);
      settings.setValue (QString ("hide name[%1]").arg (i), options->hide_name[i]);
    }


  settings.setValue ("Line number filter range string", options->lineFilterText);
  settings.setValue ("Horizontal error filter range string", options->horzFilterText);
  settings.setValue ("Vertical error filter range string", options->vertFilterText);


  //  Restore the Status tab attribute names.

  for (int32_t i = 0 ; i < POST_ATTR ; i++)
    {
      settings.setValue (QString ("status tab attribute name %1").arg (i), options->attrStatName[i]);
    }


  //  This is pretty confusing.  What we're doing is finding out which attribute in the current PFM matches which possible attribute
  //  so that we can save the attribute filter string associated with that attribute (not the attribute number of that attribute in this
  //  particular PFM).  That way, when we open another PFM, if the attributes are in a different order we can assign the correct 
  //  filter ranges to the correct attribute.

  for (int32_t i = 0 ; i < NUM_ATTR ; i++)
    {
      uint8_t done = NVFalse;

      if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->time_attribute.name)
        {
          options->time_attr_filt = options->attrFilterText[i];
          done = NVTrue;
        }

      if (!done)
        {
          for (int32_t j = 0 ; j < GSF_ATTRIBUTES ; j++)
            {
              if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->gsf_attribute[j].name)
                {
                  options->gsf_attr_filt[j] = options->attrFilterText[i];
                  done = NVTrue;
                  break;
                }
            }

          if (!done)
            {
              for (int32_t j = 0 ; j < HOF_ATTRIBUTES ; j++)
                {
                  if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->hof_attribute[j].name)
                    {
                      options->hof_attr_filt[j] = options->attrFilterText[i];
                      done = NVTrue;
                      break;
                    }
                }

              if (!done)
                {
                  for (int32_t j = 0 ; j < TOF_ATTRIBUTES ; j++)
                    {
                      if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->tof_attribute[j].name)
                        {
                          options->tof_attr_filt[j] = options->attrFilterText[i];
                          done = NVTrue;
                          break;
                        }
                    }

                  if (!done)
                    {
                      for (int32_t j = 0 ; j < CZMIL_ATTRIBUTES ; j++)
                        {
                          if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->czmil_attribute[j].name)
                            {
                              options->czmil_attr_filt[j] = options->attrFilterText[i];
                              done = NVTrue;
                              break;
                            }
                        }

                      if (!done)
                        {
                          for (int32_t j = 0 ; j < LAS_ATTRIBUTES ; j++)
                            {
                              if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->las_attribute[j].name)
                                {
                                  options->las_attr_filt[j] = options->attrFilterText[i];
                                  done = NVTrue;
                                  break;
                                }
                            }

                          if (!done)
                            {
                              for (int32_t j = 0 ; j < BAG_ATTRIBUTES ; j++)
                                {
                                  if (QString (misc->abe_share->open_args[0].head.ndx_attr_name[i]) == options->bag_attribute[j].name)
                                    {
                                      options->bag_attr_filt[j] = options->attrFilterText[i];
                                      break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


  //  Now we actually save the possible attribute filter range strings.

  settings.setValue (options->time_attribute.name + QString (" filter ranges"), options->time_attr_filt);
  for (int32_t i = 0 ; i < GSF_ATTRIBUTES ; i++) settings.setValue (options->gsf_attribute[i].name + QString (" filter ranges"),
                                                                    options->gsf_attr_filt[i]);
  for (int32_t i = 0 ; i < HOF_ATTRIBUTES ; i++) settings.setValue (options->hof_attribute[i].name + QString (" filter ranges"),
                                                                    options->hof_attr_filt[i]);
  for (int32_t i = 0 ; i < TOF_ATTRIBUTES ; i++) settings.setValue (options->tof_attribute[i].name + QString (" filter ranges"),
                                                                    options->tof_attr_filt[i]);
  for (int32_t i = 0; i < CZMIL_ATTRIBUTES; i++) settings.setValue (options->czmil_attribute[i].name + QString (" filter ranges"),
                                                                    options->czmil_attr_filt[i]);
  for (int32_t i = 0 ; i < LAS_ATTRIBUTES ; i++) settings.setValue (options->las_attribute[i].name + QString (" filter ranges"),
                                                                    options->las_attr_filt[i]);
  for (int32_t i = 0; i < BAG_ATTRIBUTES; i++) settings.setValue (options->bag_attribute[i].name + QString (" filter ranges"),
                                                                  options->bag_attr_filt[i]);


  settings.setValue (QString ("message window x position"), options->msg_window_x);
  settings.setValue (QString ("message window y position"), options->msg_window_y);
  settings.setValue (QString ("message window width"), options->msg_width);
  settings.setValue (QString ("message window height"), options->msg_height);


  //  This is the killed flag that can be set when pfmView exits.  We always set this to 0 since, if this program is running,
  //  pfmView must be running.

  settings.setValue (QString ("pfmView killed"), 0);


  settings.setValue (QString ("main window state"), mainWindow->saveState (NINT (settings_version * 100.0L)));

  settings.setValue (QString ("main window geometry"), mainWindow->saveGeometry ());


  settings.endGroup ();


  //  We need to set the Z orientation value in the globalABE settings file.  This value is used in some of the ABE programs
  //  to determine whether Z is displayed as depth (the default) or elevation.  Also, CZMIL urban noise filter settings are
  //  shared between pfmEdit3D and czmilUrbanFilter.

#ifdef NVWIN3X
  QString ini_file2 = QString (getenv ("USERPROFILE")) + "/ABE.config/" + "globalABE.ini";
#else
  QString ini_file2 = QString (getenv ("HOME")) + "/ABE.config/" + "globalABE.ini";
#endif

  QSettings settings2 (ini_file2, QSettings::IniFormat);
  settings2.beginGroup ("globalABE");


  settings2.setValue (QString ("CZMIL urban vicinity radius"), options->czmil_urban_radius[0]);
  settings2.setValue (QString ("CZMIL urban filter radius"), options->czmil_urban_radius[1]);
  settings2.setValue (QString ("CZMIL urban vicinity height"), options->czmil_urban_height[0]);
  settings2.setValue (QString ("CZMIL urban filter height"), options->czmil_urban_height[1]);
  settings2.setValue (QString ("CZMIL urban filter lower D_index"), options->czmil_urban_d_index[0]);
  settings2.setValue (QString ("CZMIL urban filter upper D_index"), options->czmil_urban_d_index[1]);


  settings2.setValue (QString ("ABE Z orientation factor"), options->z_orientation);


  //  Default feature type is used in all ABE programs that create or edit features.

  settings2.setValue (QString ("default feature type"), options->default_feature_type);


  settings2.endGroup ();
}
