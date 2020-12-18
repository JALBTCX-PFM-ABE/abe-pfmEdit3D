
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


/*!
  Set defaults for all of the program's user defined options.  This called before envin and may be called from 
  prefs.cpp if requested.
*/

void set_defaults (MISC *misc, OPTIONS *options, uint8_t restore)
{
  if (!restore)
    {
      misc->draw_area_width = 1220;
      misc->draw_area_height = 950;
      misc->drawing_canceled = NVFalse;
      misc->feature = NULL;
      memset (&misc->bfd_header, 0, sizeof (BFDATA_HEADER));
      misc->feature_mod = NVFalse;
      misc->resized = NVTrue;
      misc->area_drawn = NVFalse;
      misc->poly_count = 0;
      misc->maxd = 10;
      misc->reference_flag = NVFalse;
      misc->slice = NVFalse;
      misc->hotkey = -1;
      misc->busy = NVFalse;
      misc->visible_feature_count = 0;
      misc->html_help_tag = "#edit_pfm";
      misc->marker_mode = 0;
      misc->frozen_point = -1;
      misc->need_sparse = NVFalse;
      misc->filter_mask = NVFalse;
      misc->filter_kill_count = 0;
      misc->filtered = NVFalse;
      misc->hydro_lidar_present = NVFalse;
      misc->lidar_present = NVFalse;
      misc->hof_present = NVFalse;
      misc->gsf_present = NVFalse;
      misc->las_present = NVFalse;
      misc->czmil_present = NVFalse;
      misc->highlight_count = 0;
      misc->num_lines = 0;


      //  Set up the LiDAR classification numbers, names, and descriptions as defined by the ASPRS LAS 1.4 spec and the 2013 ASPRS paper - 
      //  "LAS Domain Profile Description: Topo-Bathy Lidar", July 17, 2013

      misc->lidar_class_num[0] = -1;   //   -  Dummy value to correspond to "Any" ([0]) in the options->from_lidar_class array
      misc->lidar_class_num[1] = 0;    //   -  Created, never classified
      misc->lidar_class_num[2] = 1;    //   -  Unclassified
      misc->lidar_class_num[3] = 2;    //   -  Ground
      misc->lidar_class_num[4] = 3;    //   -  Low Vegetation
      misc->lidar_class_num[5] = 4;    //   -  Medium Vegetation
      misc->lidar_class_num[6] = 5;    //   -  High Vegetation
      misc->lidar_class_num[7] = 6;    //   -  Building
      misc->lidar_class_num[8] = 7;    //   -  Low Point (noise)
      misc->lidar_class_num[9] = 9;    //   -  Water
      misc->lidar_class_num[10] = 10;  //   -  Rail
      misc->lidar_class_num[11] = 11;  //   -  Road Surface
      misc->lidar_class_num[12] = 12;  //   -  Overlap Points (use overlap bit for LAS 1.4)
      misc->lidar_class_num[13] = 13;  //   -  Wire - Guard (Shield)
      misc->lidar_class_num[14] = 14;  //   -  Wire - Conductor (phase)
      misc->lidar_class_num[15] = 15;  //   -  Transmission Tower
      misc->lidar_class_num[16] = 16;  //   -  Wire-structure Connector (e.g. Insulator)
      misc->lidar_class_num[17] = 17;  //   -  Bridge Deck
      misc->lidar_class_num[18] = 18;  //   -  High Noise
      misc->lidar_class_num[19] = 40;  //   -  Bathymetric point (e.g. seafloor or riverbed; also known as submerged topography)
      misc->lidar_class_num[20] = 41;  //   -  Water surface (sea/river/lake surface from bathymetric or topographic-bathymetric lidar;
                                       //      distinct from point class 9 which is used in topographic only lidar and only designates
                                       //      "water", not "water surface")
      misc->lidar_class_num[21] = 42;  //   -  Derived water surface (synthetic water surface location used in computing refraction at water surface)
      misc->lidar_class_num[22] = 43;  //   -  Submerged object, not otherwise specified (e.g. wreck, rock, submerged piling)
      misc->lidar_class_num[23] = 44;  //   -  International Hydrographic Organization (IHO) S-57 object, not otherwise specified
      misc->lidar_class_num[24] = 45;  //   -  No-bottom-found-at (bathymetric lidar point for which no detectable bottom return was received)

      misc->lidar_class_name[0] = pfmEdit3D::tr ("Any");
      misc->lidar_class_name[1] = pfmEdit3D::tr ("Created, never classified");
      misc->lidar_class_name[2] = pfmEdit3D::tr ("Unclassified");
      misc->lidar_class_name[3] = pfmEdit3D::tr ("Ground");
      misc->lidar_class_name[4] = pfmEdit3D::tr ("Low Vegetation");
      misc->lidar_class_name[5] = pfmEdit3D::tr ("Medium Vegetation");
      misc->lidar_class_name[6] = pfmEdit3D::tr ("High Vegetation");
      misc->lidar_class_name[7] = pfmEdit3D::tr ("Building");
      misc->lidar_class_name[8] = pfmEdit3D::tr ("Low Point (noise)");
      misc->lidar_class_name[9] = pfmEdit3D::tr ("Water");
      misc->lidar_class_name[10] = pfmEdit3D::tr ("Rail");
      misc->lidar_class_name[11] = pfmEdit3D::tr ("Road Surface");
      misc->lidar_class_name[12] = pfmEdit3D::tr ("Overlap Points");
      misc->lidar_class_name[13] = pfmEdit3D::tr ("Wire - Guard");
      misc->lidar_class_name[14] = pfmEdit3D::tr ("Wire - Conductor");
      misc->lidar_class_name[15] = pfmEdit3D::tr ("Transmission Tower");
      misc->lidar_class_name[16] = pfmEdit3D::tr ("Wire-structure Connector");
      misc->lidar_class_name[17] = pfmEdit3D::tr ("Bridge Deck");
      misc->lidar_class_name[18] = pfmEdit3D::tr ("High Noise");
      misc->lidar_class_name[19] = pfmEdit3D::tr ("Bathymetric point");
      misc->lidar_class_name[20] = pfmEdit3D::tr ("Water surface");
      misc->lidar_class_name[21] = pfmEdit3D::tr ("Derived water surface");
      misc->lidar_class_name[22] = pfmEdit3D::tr ("Submerged object, not otherwise specified");
      misc->lidar_class_name[23] = pfmEdit3D::tr ("IHO S-57 object, not otherwise specified");
      misc->lidar_class_name[24] = pfmEdit3D::tr ("No-bottom-found-at");

      misc->lidar_class_desc[0] = pfmEdit3D::tr ("Any LiDAR classification may be changed, this also clears all other <b>From</b> check boxes");
      misc->lidar_class_desc[1] = pfmEdit3D::tr ("Created, never classified");
      misc->lidar_class_desc[2] = pfmEdit3D::tr ("Unclassified");
      misc->lidar_class_desc[3] = pfmEdit3D::tr ("Ground");
      misc->lidar_class_desc[4] = pfmEdit3D::tr ("Low Vegetation");
      misc->lidar_class_desc[5] = pfmEdit3D::tr ("Medium Vegetation");
      misc->lidar_class_desc[6] = pfmEdit3D::tr ("High Vegetation");
      misc->lidar_class_desc[7] = pfmEdit3D::tr ("Building");
      misc->lidar_class_desc[8] = pfmEdit3D::tr ("Low Point (noise)");
      misc->lidar_class_desc[9] = pfmEdit3D::tr ("Water (topographic only LiDAR, not bathy or topo-bathy LiDAR)");
      misc->lidar_class_desc[10] = pfmEdit3D::tr ("Rail");
      misc->lidar_class_desc[11] = pfmEdit3D::tr ("Road Surface");
      misc->lidar_class_desc[12] = pfmEdit3D::tr ("Overlap Points (holdover from LAS 1.2, use overlap bit for LAS 1.4)");
      misc->lidar_class_desc[13] = pfmEdit3D::tr ("Wire - Guard (Shield)");
      misc->lidar_class_desc[14] = pfmEdit3D::tr ("Wire - Conductor (phase)");
      misc->lidar_class_desc[15] = pfmEdit3D::tr ("Transmission Tower");
      misc->lidar_class_desc[16] = pfmEdit3D::tr ("Wire-structure Connector (e.g. Insulator)");
      misc->lidar_class_desc[17] = pfmEdit3D::tr ("Bridge Deck");
      misc->lidar_class_desc[18] = pfmEdit3D::tr ("High Noise");
      misc->lidar_class_desc[19] = pfmEdit3D::tr ("Bathymetric point (e.g. seafloor or riverbed; also known as submerged topography)");
      misc->lidar_class_desc[20] = pfmEdit3D::tr ("Water surface (sea/river/lake surface from bathymetric or topographic-bathymetric lidar; ") + 
                                       pfmEdit3D::tr ("distinct from point class 9 which is used in topographic only lidar and only designates ") +
                                       pfmEdit3D::tr ("<b>water</b>, not <b>water surface</b>");
      misc->lidar_class_desc[21] = pfmEdit3D::tr ("Derived water surface (synthetic water surface location used in computing refraction at water surface)");
      misc->lidar_class_desc[22] = pfmEdit3D::tr ("Submerged object, not otherwise specified (e.g. wreck, rock, submerged piling)");
      misc->lidar_class_desc[23] = pfmEdit3D::tr ("International Hydrographic Organization (IHO) S-57 object, not otherwise specified");
      misc->lidar_class_desc[24] = pfmEdit3D::tr ("No-bottom-found-at (bathymetric lidar point for which no detectable bottom return was received)");


      //  Set up the sine and cosine values for slicing.

      for (int32_t i = 0 ; i < 3600 ; i++)
        {
          misc->sin_array[i] = sinf (((float) i / 10.0) * NV_DEG_TO_RAD);
          misc->cos_array[i] = cosf (((float) i / 10.0) * NV_DEG_TO_RAD);
        }


      //  Tooltip text for the buttons and actions that have editable accelerators

      misc->buttonText[SAVE_EXIT_KEY] = pfmEdit3D::tr ("Save changes and exit");
      misc->buttonText[SAVE_EXIT_MASK_KEY] = pfmEdit3D::tr ("Save changes, exit, and filter mask in pfmView");
      misc->buttonText[NO_SAVE_EXIT_KEY] = pfmEdit3D::tr ("Exit without saving changes");
      misc->buttonText[RESET_KEY] = pfmEdit3D::tr ("Reset to original view");
      misc->buttonText[DELETE_POINT_MODE_KEY] = pfmEdit3D::tr ("Select delete subrecord/record edit mode");
      misc->buttonText[DELETE_RECTANGLE_MODE_KEY] = pfmEdit3D::tr ("Select delete rectangle edit mode");
      misc->buttonText[DELETE_POLYGON_MODE_KEY] = pfmEdit3D::tr ("Select delete polygon edit mode");
      misc->buttonText[AREA_FILTER_KEY] = pfmEdit3D::tr ("Area-based filter");
      misc->buttonText[STAT_FILTER_KEY] = pfmEdit3D::tr ("AVT statistical filter");
      misc->buttonText[HP_FILTER_KEY] = pfmEdit3D::tr ("Hockey Puck filter");
      misc->buttonText[ATTR_FILTER_KEY] = pfmEdit3D::tr ("Attribute filter");
      misc->buttonText[CZMIL_URBAN_FILTER_KEY] = pfmEdit3D::tr ("CZMIL urban noise filter");
      misc->buttonText[RECTANGLE_FILTER_MASK_KEY] = pfmEdit3D::tr ("Select rectangle filter mask mode");
      misc->buttonText[POLYGON_FILTER_MASK_KEY] = pfmEdit3D::tr ("Select polygon filter mask mode");
      misc->buttonText[RUN_HOTKEY_POLYGON_MODE_KEY] = pfmEdit3D::tr ("Select run hotkey program in polygon mode");
      misc->buttonText[EDIT_FEATURE_MODE_KEY] = pfmEdit3D::tr ("Select edit feature mode");
      misc->buttonText[UNDO_KEY] = pfmEdit3D::tr ("Undo last edit operation");
      misc->buttonText[DISPLAY_MULTIPLE_KEY] = pfmEdit3D::tr ("Select display multiple lines mode");
      misc->buttonText[DISPLAY_ALL_KEY] = pfmEdit3D::tr ("Display all lines");
      misc->buttonText[CLEAR_HIGHLIGHT_KEY] = pfmEdit3D::tr ("Clear all highlighted/marked points");
      misc->buttonText[HIGHLIGHT_POLYGON_MODE_KEY] = pfmEdit3D::tr ("Select highlight/mark points in polygon mode");
      misc->buttonText[CLEAR_POLYGON_MODE_KEY] = pfmEdit3D::tr ("Select clear all highlighted/marked points in polygon mode");
      misc->buttonText[TOGGLE_CONTOUR_KEY] = pfmEdit3D::tr ("Toggle contour drawing");
      misc->buttonText[CZMIL_REPROCESS_LAND_KEY] = pfmEdit3D::tr ("CZMIL reprocess as land tool");
      misc->buttonText[CZMIL_REPROCESS_WATER_KEY] = pfmEdit3D::tr ("CZMIL reprocess as water tool");
      misc->buttonText[CZMIL_REPROCESS_SHALLOW_WATER_KEY] = pfmEdit3D::tr ("CZMIL reprocess as shallow water tool");
      misc->buttonText[SET_VIEW_LEVEL_KEY] = pfmEdit3D::tr ("Level the current view (remove any tilt)");
      misc->buttonText[SET_NORTH_UP_KEY] = pfmEdit3D::tr ("Shift to plan view, north up");
      misc->buttonText[CHANGE_LIDAR_CLASS_KEY] = pfmEdit3D::tr ("Select change LiDAR classification mode");


      //  Icons for the buttons and actions that have editable accelerators

      misc->buttonIcon[SAVE_EXIT_KEY] = QIcon (":/icons/exit_save.png");
      misc->buttonIcon[SAVE_EXIT_MASK_KEY] = QIcon (":/icons/exit_mask.png");
      misc->buttonIcon[NO_SAVE_EXIT_KEY] = QIcon (":/icons/exit_no_save.png");
      misc->buttonIcon[RESET_KEY] = QIcon (":/icons/reset_view.png");
      misc->buttonIcon[DELETE_POINT_MODE_KEY] = QIcon (":/icons/delete_point.png");
      misc->buttonIcon[DELETE_RECTANGLE_MODE_KEY] = QIcon (":/icons/delete_rect.png");
      misc->buttonIcon[DELETE_POLYGON_MODE_KEY] = QIcon (":/icons/delete_poly.png");
      misc->buttonIcon[AREA_FILTER_KEY] = QIcon (":/icons/area_filter.png");
      misc->buttonIcon[STAT_FILTER_KEY] = QIcon (":/icons/stat_filter.png");
      misc->buttonIcon[HP_FILTER_KEY] = QIcon (":/icons/hp_filter.png");
      misc->buttonIcon[ATTR_FILTER_KEY] = QIcon (":/icons/attr_filter.png");
      misc->buttonIcon[CZMIL_URBAN_FILTER_KEY] = QIcon (":/icons/czmilUrbanFilter.png");
      misc->buttonIcon[RECTANGLE_FILTER_MASK_KEY] = QIcon (":/icons/filter_mask_rect.png");
      misc->buttonIcon[POLYGON_FILTER_MASK_KEY] = QIcon (":/icons/filter_mask_poly.png");
      misc->buttonIcon[RUN_HOTKEY_POLYGON_MODE_KEY] = QIcon (":/icons/hotkey_poly.png");
      misc->buttonIcon[EDIT_FEATURE_MODE_KEY] = QIcon (":/icons/editfeature.png");
      misc->buttonIcon[UNDO_KEY] = QIcon (":/icons/undo.png");
      misc->buttonIcon[DISPLAY_MULTIPLE_KEY] = QIcon (":/icons/displaylines.png");
      misc->buttonIcon[DISPLAY_ALL_KEY] = QIcon (":/icons/displayall.png");
      misc->buttonIcon[CLEAR_HIGHLIGHT_KEY] = QIcon (":/icons/clear_highlight.png");
      misc->buttonIcon[HIGHLIGHT_POLYGON_MODE_KEY] = QIcon (":/icons/highlight_polygon.png");
      misc->buttonIcon[CLEAR_POLYGON_MODE_KEY] = QIcon (":/icons/clear_polygon.png");
      misc->buttonIcon[TOGGLE_CONTOUR_KEY] = QIcon (":/icons/contour.png");
      misc->buttonIcon[CZMIL_REPROCESS_LAND_KEY] = QIcon(":/icons/czmil_reprocess_land_poly.png");
      misc->buttonIcon[CZMIL_REPROCESS_WATER_KEY] = QIcon(":/icons/czmil_reprocess_water_poly.png");
      misc->buttonIcon[CZMIL_REPROCESS_SHALLOW_WATER_KEY] = QIcon(":/icons/czmil_reprocess_shallow_water_poly.png");
      misc->buttonIcon[SET_VIEW_LEVEL_KEY] = QIcon(":/icons/set_view_level.png");
      misc->buttonIcon[SET_NORTH_UP_KEY] = QIcon(":/icons/north_up.png");
      misc->buttonIcon[CHANGE_LIDAR_CLASS_KEY] = QIcon (":/icons/change_lidar_class.png");


      //  NULL out the buttons and actions so we can tell them apart.

      for (int32_t i = 0 ; i < HOTKEYS ; i++)
        {
          misc->button[i] = NULL;
          misc->action[i] = NULL;
        }


      misc->abe_share->key = 0;
      misc->abe_share->modcode = 0;
      misc->displayed_area.min_y = -91.0;


      //  Set up the hard-wired hot keys.

      misc->hard_key[COLOR_BY_DEPTH] = "Ctrl+d";
      misc->hard_key[COLOR_BY_LINE] = "Ctrl+l";
      misc->hard_key[COLOR_BY_HORIZONTAL_ERROR] = "Meta+h";
      misc->hard_key[COLOR_BY_VERTICAL_ERROR] = "Meta+v";
      misc->hard_key[COLOR_BY_ATTR_00] = "Alt+0";
      misc->hard_key[COLOR_BY_ATTR_01] = "Alt+1";
      misc->hard_key[COLOR_BY_ATTR_02] = "Alt+2";
      misc->hard_key[COLOR_BY_ATTR_03] = "Alt+3";
      misc->hard_key[COLOR_BY_ATTR_04] = "Alt+4";
      misc->hard_key[COLOR_BY_ATTR_05] = "Alt+5";
      misc->hard_key[COLOR_BY_ATTR_06] = "Alt+6";
      misc->hard_key[COLOR_BY_ATTR_07] = "Alt+7";
      misc->hard_key[COLOR_BY_ATTR_08] = "Alt+8";
      misc->hard_key[COLOR_BY_ATTR_09] = "Alt+9";

      for (int32_t i = 0 ; i < POST_ATTR ; i++)
        {
          misc->attrStatNum[i] = -1;
          misc->attrBoxFlag[i] = 0;
        }

      misc->qsettings_app = "pfmEdit3D";

      misc->add_feature_index = -1;
      misc->nearest_feature_point = -1;

      misc->bfd_open = NVFalse;

      misc->undo_count = 0;
      misc->time_attr = -1;
      misc->process_id = getpid ();
      misc->czmilReprocessFlag = NVFalse;
      misc->filter_undo.count = 0;


      //  Get the proper version based on the ABE_CZMIL environment variable.

      strcpy (misc->program_version, VERSION);

      if (getenv ("ABE_CZMIL") != NULL)
        {
          if (strstr (getenv ("ABE_CZMIL"), "CZMIL"))
            {
              QString qv = QString (VERSION);
              QString str = qv.section (' ', 4, 6);

              str.prepend ("CME Software - 3D Editor ");

              strcpy (misc->program_version, str.toLatin1 ());
            }
          else
            {
              strcpy (misc->program_version, VERSION);
            }
        }


      //  This is a special case for an option.  We don't want to reset the undo levels if we called for a restore of defaults from the prefs dialog.

      options->undo_levels = 1000;
    }


  /*

    Ancillary program command line substitutions:


    [MOSAIC_FILE] - associated mosaic file name
    [TARGET_FILE] - associated feature (target) file name
    [PFM_FILE] - PFM list or handle file name
    [BIN_FILE] - PFM bin directory name
    [INDEX_FILE] - PFM index directory name
    [INPUT_FILE] - input data file associated with the current point
    [SHARED_MEMORY_ID] - ABE shared memory ID (some programs like chartsPic require this)
    [LINE] - line name associated with the current point
    [Z_VALUE] - Z value of the current point
    [X_VALUE] - X value (usually longitude) associated with the current point
    [Y_VALUE] - Y value (usually latitude) associated with the current point
    [MIN_Y] - minimum Y value in the currently displayed area
    [MIN_X] - minimum X value in the currently displayed area
    [MAX_Y] - maximum Y value in the currently displayed area
    [MAX_X] - maximum X value in the currently displayed area
    [FILE_NUMBER] - PFM input file number associated with the current point
    [LINE_NUMBER] - PFM input line number associated with the current point
    [VALIDITY] - PFM validity word for the current point
    [RECORD] - input file record number associated with the current point
    [SUBRECORD] - input file subrecord (usually beam) number associated with the current point
    [DATA_TYPE] - PFM data type of the current point

    [CL] - run this program as a command line program (creates a dialog for output)
    [SHARED_MEMORY_KEY] - Add the shared memory ID so the outboard program can track it


    Note: the commands used to be options (i.e. user modifiable but it became too complicated).
    If you change these you must update the documentation in hotkeysHelp.cpp.

    Note: Polygon eligible commands cannot have an associated button.  Associated buttons are set in pfmEdit3D.cpp.

    If you want to add or subtract programs it needs to be done here, in pfmEdit3DDef.hpp, and add or subtract buttons
    in pfmEdit3D.cpp.

  */


  options->kill_and_respawn = NVFalse;

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      options->kill_switch[i] = KILL_SWITCH_OFFSET + i;

      for (int32_t j = 0 ; j < PFM_DATA_TYPES ; j++) options->data_type[i][j] = NVFalse;

      switch (i)
        {
        case EXAMGSF:
          options->prog[i] = "examGSF --file [INPUT_FILE] --record [RECORD]";
          options->name[i] = "examGSF";
          options->description[i] = pfmEdit3D::tr ("GSF non-graphical data viewer.  Displays data for nearest point.");
          options->data_type[i][PFM_GSF_DATA] = NVTrue;
          options->hotkey[i] = "e";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 0;
          break;

        case GSFMONITOR:
          options->prog[i] = "gsfMonitor [SHARED_MEMORY_KEY]";
          options->name[i] = "gsfMonitor";
          options->description[i] = pfmEdit3D::tr ("GSF non-graphical data viewer.  Displays ping data for nearest point.");
          options->data_type[i][PFM_GSF_DATA] = NVTrue;
          options->hotkey[i] = "g";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 1;
          break;

        case WAVEFORMMONITOR:
          options->prog[i] = "waveformMonitor [SHARED_MEMORY_KEY]";
          options->name[i] = "waveformMonitor";
          options->description[i] = pfmEdit3D::tr ("CZMIL/HOF waveform monitor.  Displays waveform for nearest point.");
          options->data_type[i][PFM_SHOALS_1K_DATA] = NVTrue;
          options->data_type[i][PFM_CHARTS_HOF_DATA] = NVTrue;
          options->data_type[i][PFM_CZMIL_DATA] = NVTrue;
          options->hotkey[i] = "w";
          options->action[i] = "1,2,3,4";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 1;
          break;


          //  For historical reasons we are using "x" to kick this off.  It used to be the "eXtended" information
          //  for the waveformMonitor program.

        case LIDARMONITOR:
          options->prog[i] = "lidarMonitor [SHARED_MEMORY_KEY]";
          options->name[i] = "lidarMonitor";
          options->description[i] = pfmEdit3D::tr ("CZMIL, HOF, TOF, or LAS textual data viewer.  Displays entire record for nearest point.");
          options->data_type[i][PFM_SHOALS_1K_DATA] = NVTrue;
          options->data_type[i][PFM_CHARTS_HOF_DATA] = NVTrue;
          options->data_type[i][PFM_SHOALS_TOF_DATA] = NVTrue;
          options->data_type[i][PFM_CZMIL_DATA] = NVTrue;
          options->data_type[i][PFM_LAS_DATA] = NVTrue;
          options->hotkey[i] = "x";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 1;
          break;

        case CHARTSPIC:
          options->prog[i] = "chartsPic [SHARED_MEMORY_KEY]";
          options->name[i] = "chartsPic";
          options->description[i] = pfmEdit3D::tr ("CZMIL/HOF/TOF down-looking image viewer.  Displays nearest down-looking photo.");
          options->data_type[i][PFM_SHOALS_1K_DATA] = NVTrue;
          options->data_type[i][PFM_CHARTS_HOF_DATA] = NVTrue;
          options->data_type[i][PFM_SHOALS_TOF_DATA] = NVTrue;
          options->data_type[i][PFM_CZMIL_DATA] = NVTrue;
          options->hotkey[i] = "i";
          options->action[i] = "r,t,h";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 1;
          break;

        case MOSAICVIEW:
          options->prog[i] = "mosaicView [SHARED_MEMORY_KEY] [MOSAIC_FILE]";
          options->name[i] = "mosaicView";
          options->description[i] = pfmEdit3D::tr ("GeoTIFF viewer for photo mosaics or other GeoTIFF files (scanned maps, etc.).");
          options->data_type[i][PFM_UNDEFINED_DATA] = NVTrue;
          options->hotkey[i] = "m";
          options->action[i] = "z,+,-";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 0;
          break;

        case WAVEWATERFALL_SHALLOW:
          options->prog[i] = "waveWaterfall [SHARED_MEMORY_KEY] -a";
          options->name[i] = "waveWaterfall (Shallow)";
          options->description[i] = 
            pfmEdit3D::tr ("CZMIL/HOF waveform waterfall display (shallow waveform).  Displays shallow waveforms for geographically nearest 9 records.");
          options->data_type[i][PFM_SHOALS_1K_DATA] = NVTrue;
          options->data_type[i][PFM_CHARTS_HOF_DATA] = NVTrue;
          options->data_type[i][PFM_CZMIL_DATA] = NVTrue;
          options->hotkey[i] = "a";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 1;
          break;

        case WAVEWATERFALL_DEEP:
          options->prog[i] = "waveWaterfall [SHARED_MEMORY_KEY] -p";
          options->name[i] = "waveWaterfall (Deep)";
          options->description[i] = 
            pfmEdit3D::tr ("CZMIL/HOF waveform waterfall display (deep waveform).  Displays Deep waveforms for geographically nearest 9 records.");
          options->data_type[i][PFM_SHOALS_1K_DATA] = NVTrue;
          options->data_type[i][PFM_CHARTS_HOF_DATA] = NVTrue;
          options->data_type[i][PFM_CZMIL_DATA] = NVTrue;
          options->hotkey[i] = "p";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 1;
          break;

        case CZMILWAVEMONITOR:
          options->prog[i] = "CZMILwaveMonitor [SHARED_MEMORY_KEY]";
          options->name[i] = "CZMILwaveMonitor";
          options->description[i] = pfmEdit3D::tr ("CZMIL waveform monitor.  Displays waveforms for nearest point(s).");
          options->data_type[i][PFM_CZMIL_DATA] = NVTrue;
          options->hotkey[i] = "Alt+w";
          options->action[i] = "n,0,1,2,3,4,5,6,7,8,9,c";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 1;
          break;

        case LASWAVEMONITOR:
          options->prog[i] = "LASwaveMonitor [SHARED_MEMORY_KEY]";
          options->name[i] = "LASwaveMonitor";
          options->description[i] = pfmEdit3D::tr ("LAS waveform monitor.  Displays waveforms for nearest point(s).");
          options->data_type[i][PFM_LAS_DATA] = NVTrue;
          options->hotkey[i] = "Alt+m";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 1;
          break;

        case HOFRETURNKILL:
          options->prog[i] = "hofReturnKill [SHARED_MEMORY_KEY]";
          options->name[i] = "hofReturnKill (kill all HOF secondary returns)";
          options->description[i] = pfmEdit3D::tr ("Kill all HOF secondary returns.");
          options->data_type[i][PFM_CHARTS_HOF_DATA] = NVTrue;
          options->hotkey[i] = "!";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVTrue;
          options->hk_poly_filter[i] = 1;
          options->state[i] = 0;
          break;

        case HOFRETURNKILL_SWA:
          options->prog[i] = "hofReturnKill -s [SHARED_MEMORY_KEY]";
          options->name[i] = "hofReturnKill (invalidate shallow water algorithm data)";
          options->description[i] = pfmEdit3D::tr ("Kill HOF Shallow Water Algorithm or Shoreline Depth Swap data.");
          options->data_type[i][PFM_CHARTS_HOF_DATA] = NVTrue;
          options->hotkey[i] = "$";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVTrue;
          options->hk_poly_filter[i] = 1;
          options->state[i] = 0;
          break;


          //  IMPORTANT NOTE: The following functions have an associated button in one of the button groups.  If you need to add an
          //  external program that requires a button DO NOT add it to the HOTKEYS group in pfmEdit3DDef.hpp.  Just do the same thing
          //  that was done for the following functions.

        case HOFWAVEFILTER:
          options->prog[i] = "hofWaveFilter [SHARED_MEMORY_KEY]";
          options->name[i] = "hofWaveFilter (CHARTS HOF waveform filter)";
          options->description[i] = pfmEdit3D::tr ("Checks for low slope and checks adjacent waveforms for isolated points.");
          options->data_type[i][PFM_CHARTS_HOF_DATA] = NVTrue;
          options->hotkey[i] = "Ctrl+w";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 0;
          break;


          //  This is not used but I've kept it for reference (in case I want to add another external filter)...

        case CPF_DEEP_FILTER:
          options->prog[i] = "cpfDeepFilter [SHARED_MEMORY_KEY]";
          options->name[i] = "cpfDeepFilter";
          options->description[i] = pfmEdit3D::tr ("CZMIL CPF deep return filter.");
          options->data_type[i][PFM_CZMIL_DATA] = NVTrue;
          options->hotkey[i] = "Ctrl+Alt+f";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 0;
          break;


          //  IMPORTANT NOTE: The following six functions are not ancillary programs.  If you have to add new ancillary programs,
          //  add them above these just for esthetics.

        case INVALIDATE_FEATURES:
          options->prog[i] = "INVALIDATE_FEATURES";
          options->name[i] = "INVALIDATE FEATURES";
          options->description[i] = pfmEdit3D::tr ("Invalidate all features included in the hotkey polygon.  This is an internal function.");
          options->data_type[i][PFM_UNDEFINED_DATA] = NVTrue;
          options->hotkey[i] = "v";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVTrue;
          options->hk_poly_filter[i] = 1;
          options->state[i] = 0;
          break;


        case DELETE_FILTER_HIGHLIGHTED:
          options->prog[i] = "DELETE FILTER HIGHLIGHTED";
          options->name[i] = "DELETE FILTER HIGHLIGHTED";
          options->description[i] = pfmEdit3D::tr ("Delete points marked by the filter or highlighted.  This is an internal function.");
          options->data_type[i][PFM_UNDEFINED_DATA] = NVTrue;
          options->hotkey[i] = "Del";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 0;
          break;


        case CANCEL_FILTER_HIGHLIGHTED:
          options->prog[i] = "CLEAR FILTER HIGHLIGHTED";
          options->name[i] = "CLEAR FILTER HIGHLIGHTED";
          options->description[i] =
            pfmEdit3D::tr ("Clear points marked by the filter/highlighted or restore highlighted invalid points.  This is an internal function.");
          options->data_type[i][PFM_UNDEFINED_DATA] = NVTrue;
          options->hotkey[i] = "Ins";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 0;
          break;


        case DELETE_SINGLE_POINT:
          options->prog[i] = "DELETE SINGLE POINT";
          options->name[i] = "DELETE SINGLE POINT";
          options->description[i] = pfmEdit3D::tr ("Delete single point nearest cursor in DELETE_POINT mode.  This is an internal function.");
          options->data_type[i][PFM_UNDEFINED_DATA] = NVTrue;
          options->hotkey[i] = "d";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 0;
          break;


          //  IMPORTANT NOTE: The following two functions affect external ancillary programs.  They are used for freezing the marker
          //  or freezing all of the multi-markers.

        case FREEZE:
          options->prog[i] = "FREEZE";
          options->name[i] = "FREEZE MAIN BOX CURSOR";
          options->description[i] = pfmEdit3D::tr ("Toggle freezing of the box cursor.  This is an internal function that affects ancillary programs.");
          options->data_type[i][PFM_UNDEFINED_DATA] = NVTrue;
          options->hotkey[i] = "f";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 0;
          break;

        case FREEZE_ALL:
          options->prog[i] = "FREEZE ALL";
          options->name[i] = "FREEZE ALL BOX CURSORS";
          options->description[i] =
            pfmEdit3D::tr ("Toggle freezing of all of the box cursors.  This is an internal function that affects ancillary programs.");
          options->data_type[i][PFM_UNDEFINED_DATA] = NVTrue;
          options->hotkey[i] = "Alt+f";
          options->action[i] = "";
          options->progButton[i] = NULL;
          options->hk_poly_eligible[i] = NVFalse;
          options->hk_poly_filter[i] = 0;
          options->state[i] = 0;
          break;
        }
    }


  options->highlight_tip = NVTrue;
  options->min_window_size = 4.0;
  options->iho_min_window = 0;
  options->display_man_invalid = NVFalse;
  options->display_flt_invalid = NVFalse;
  options->display_contours = NVFalse;
  options->display_null = NVFalse;
  options->display_feature = 0;
  options->display_children = NVFalse;
  options->display_reference = NVFalse;
  options->display_feature_info = NVFalse;
  options->function = DELETE_POLYGON;
  options->slice_percent = 5;
  options->slice_alpha = 24;
  options->slice_gray = NVTrue;
  options->contour_width = 2;
  options->smoothing_factor = 0;
  options->contour_location = 0;
  options->point_size = 3;
  options->contour_color = QColor (255, 255, 255, 255);
  options->edit_color = QColor (255, 255, 255, 255);
  options->marker_color = QColor (255, 255, 255, 255);
  options->ref_color[0] = QColor (255, 255, 255, 255);
  options->tracker_color = QColor (255, 255, 0, 255);
  options->background_color = QColor (0, 0, 0, 255);
  options->scale_color = QColor (255, 255, 255, 255);
  options->feature_color = QColor (255, 255, 255, 128);
  options->feature_info_color = QColor (255, 255, 255, 255);
  options->feature_highlight_color = QColor (255, 0, 0, 255);
  options->verified_feature_color = QColor (0, 255, 0, 128);
  options->scaled_offset_z_color = QColor (255, 0, 0);
  options->font = QApplication::font ();
  options->feature_size = 0.01;
  options->unload_prog = "pfm_unload";
  options->auto_unload = NVFalse;
  options->last_rock_feature_desc = 4;
  options->last_offshore_feature_desc = 17;
  options->last_light_feature_desc = 9;
  options->last_lidar_feature_desc = 0;
  options->last_feature_description = "";
  options->last_feature_remarks = "";
  options->overlap_percent = 5;
  options->feature_dir = ".";
  options->main_button_icon_size = 24;
  options->rotation_increment = 5.0;
  options->color_index = 0;
  options->flag_index = 0;
  options->sparse_limit = 250000;
  options->exaggeration = 3.0;
  options->zoom_percent = 10;
  options->screenshot_delay = 2;
  options->draw_scale = NVTrue;
  options->zx_rotation = 0.0;
  options->y_rotation = 0.0;
  options->filter_type = HP_FILTER;
  options->area_filter_std = 2.4;
  options->stat_filter_std = 2.4;
  options->stat_filter_radius = 2.0;
  options->stat_filter_count = 10;
  options->stat_filter_passes = 1;
  options->hp_filter_radius = 1.0;
  options->hp_filter_height = 1.0;
  options->hp_filter_neighbors = 10;
  options->hp_filter_ml_neighbors = 3;
  options->hp_filter_passes = 0;
  options->czmil_urban_radius[0] = 5.0;
  options->czmil_urban_radius[1] = 3.0;
  options->czmil_urban_height[0] = 10.0;
  options->czmil_urban_height[1] = 6.0;
  options->czmil_urban_d_index[0] = 25;
  options->czmil_urban_d_index[1] = 125;
  options->filter_min_z = -12000.0;
  options->filter_max_z = 12000.0;
  options->filter_apply_z_range = NVFalse;
  options->deep_filter_only = NVFalse;
  options->feature_radius = 20.0;
  options->auto_scale = NVTrue;
  options->hofWaveFilter_search_radius = 2.0;
  options->hofWaveFilter_search_width = 8;
  options->hofWaveFilter_rise_threshold = 5;
  options->hofWaveFilter_pmt_ac_zero_offset_required = 10;
  options->hofWaveFilter_apd_ac_zero_offset_required = 5;
  for (int32_t i = 0 ; i < 9 ; i++) options->czmil_probability[i] = 0.9999;
  options->czmil_cpf_filter_inval = NVFalse;
  options->czmil_cpf_filter_amp_min = 600;
  options->hide_persistent = NVFalse;
  options->hide_visible = NVFalse;
  for (int32_t i = 0 ; i < HIDE_SLOTS ; i++) options->hide_flag[i] = NVFalse;
  options->mask_active = MASK_NONE;
  options->msg_window_x = 0;
  options->msg_window_y = 0;
  options->msg_width = 800;
  options->msg_height = 600;
  for (int32_t i = 0 ; i < NUM_ATTR ; i++) options->attrFilterText[i] = "";
  options->horzFilterText = "";
  options->vertFilterText = "";
  options->auto_filter_mask = NVFalse;
  options->track_tab = NVTrue;
  options->no_color_scale_masked = NVFalse;
  options->invalid_edit_color = NVFalse;
  options->num_hp_threads = 4;
  options->z_orientation = 1.0;
  options->pref_tab = 0;
  options->lat_or_east = 0;
  options->czmil_suppress_warnings = NVTrue;
  options->czmil_reprocess_mode = NVTrue;
  for (int32_t i = 0 ; i < 2 ; i++) options->czmil_reprocess_flags[i] = 1;
  options->lidar_class = 3;
  options->from_lidar_class[0] = NVTrue;
  for (int32_t i = 1 ; i < LIDAR_CLASSES ; i++) options->from_lidar_class[i] = NVFalse;
  options->default_feature_type = BFDATA_HYDROGRAPHIC;

  for (int32_t i = 0 ; i < POST_ATTR ; i++) options->attrStatName[i] = "";

  options->time_attr_filt = "";
  for (int32_t i = 0 ; i < GSF_ATTRIBUTES ; i++) options->gsf_attr_filt[i] = "";
  for (int32_t i = 0 ; i < HOF_ATTRIBUTES ; i++) options->hof_attr_filt[i] = "";
  for (int32_t i = 0 ; i < TOF_ATTRIBUTES ; i++) options->tof_attr_filt[i] = "";
  for (int32_t i = 0; i < CZMIL_ATTRIBUTES; i++) options->czmil_attr_filt[i] = "";
  for (int32_t i = 0 ; i < LAS_ATTRIBUTES ; i++) options->las_attr_filt[i] = "";
  for (int32_t i = 0; i < BAG_ATTRIBUTES; i++) options->bag_attr_filt[i] = "";


  //  Default to red-blue, unlocked, 0.0 for all color bands.

  for (int32_t i = 0 ; i < NUM_SCALE_LEVELS ; i++)
    {
      options->min_hsv_color[i] = 0;
      options->max_hsv_color[i] = 240;
      options->min_hsv_locked[i] = NVFalse;
      options->max_hsv_locked[i] = NVFalse;
      options->min_hsv_value[i] = 0.0;
      options->max_hsv_value[i] = 0.0;
    }


  //  Now change the color by depth default to red-magenta.

  options->max_hsv_color[0] = 315;


  // 
  //  Set the multi-waveform cursor colors.  MAX_STACK_POINTS is 9 so we manually define 9 colors.
  //  If MAX_STACK_POINTS gets changed you should add/subtract.
  //

  // white

  misc->abe_share->mwShare.multiColors[0].r = 255;
  misc->abe_share->mwShare.multiColors[0].g = 255;
  misc->abe_share->mwShare.multiColors[0].b = 255;
  misc->abe_share->mwShare.multiColors[0].a = 255;


  // red 

  misc->abe_share->mwShare.multiColors[1].r = 255;
  misc->abe_share->mwShare.multiColors[1].g = 0;
  misc->abe_share->mwShare.multiColors[1].b = 0;
  misc->abe_share->mwShare.multiColors[1].a = 255;


  // yellow 

  misc->abe_share->mwShare.multiColors[2].r = 255;
  misc->abe_share->mwShare.multiColors[2].g = 255;
  misc->abe_share->mwShare.multiColors[2].b = 0;
  misc->abe_share->mwShare.multiColors[2].a = 255;


  // green

  misc->abe_share->mwShare.multiColors[3].r = 0;
  misc->abe_share->mwShare.multiColors[3].g = 255;
  misc->abe_share->mwShare.multiColors[3].b = 0;
  misc->abe_share->mwShare.multiColors[3].a = 255;


  // cyan

  misc->abe_share->mwShare.multiColors[4].r = 0;
  misc->abe_share->mwShare.multiColors[4].g = 255;
  misc->abe_share->mwShare.multiColors[4].b = 255;
  misc->abe_share->mwShare.multiColors[4].a = 255;


  // blue

  misc->abe_share->mwShare.multiColors[5].r = 0;
  misc->abe_share->mwShare.multiColors[5].g = 0;
  misc->abe_share->mwShare.multiColors[5].b = 255;
  misc->abe_share->mwShare.multiColors[5].a = 255;


  // orange

  misc->abe_share->mwShare.multiColors[6].r = 153;
  misc->abe_share->mwShare.multiColors[6].g = 128;
  misc->abe_share->mwShare.multiColors[6].b = 0;
  misc->abe_share->mwShare.multiColors[6].a = 255;


  // magenta

  misc->abe_share->mwShare.multiColors[7].r = 128;
  misc->abe_share->mwShare.multiColors[7].g = 0;
  misc->abe_share->mwShare.multiColors[7].b = 255;
  misc->abe_share->mwShare.multiColors[7].a = 255;


  // dark green

  misc->abe_share->mwShare.multiColors[8].r = 0;
  misc->abe_share->mwShare.multiColors[8].g = 153;
  misc->abe_share->mwShare.multiColors[8].b = 0;
  misc->abe_share->mwShare.multiColors[8].a = 255;


  for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++)
    {
      options->waveColor[i].setRgb (misc->abe_share->mwShare.multiColors[i].r, misc->abe_share->mwShare.multiColors[i].g,
                                    misc->abe_share->mwShare.multiColors[i].b);
      options->waveColor[i].setAlpha (misc->abe_share->mwShare.multiColors[i].a);
    }


  options->buttonAccel[SAVE_EXIT_KEY] = "Ctrl+s";
  options->buttonAccel[SAVE_EXIT_MASK_KEY] = "Ctrl+f";
  options->buttonAccel[NO_SAVE_EXIT_KEY] = "Ctrl+q";
  options->buttonAccel[RESET_KEY] = "Ctrl+r";
  options->buttonAccel[DELETE_POINT_MODE_KEY] = "F3";
  options->buttonAccel[DELETE_RECTANGLE_MODE_KEY] = "F4";
  options->buttonAccel[DELETE_POLYGON_MODE_KEY] = "F5";
  options->buttonAccel[AREA_FILTER_KEY] = "Alt+a";
  options->buttonAccel[STAT_FILTER_KEY] = "Alt+s";
  options->buttonAccel[HP_FILTER_KEY] = "F6";
  options->buttonAccel[ATTR_FILTER_KEY] = "Ctrl+a";
  options->buttonAccel[CZMIL_URBAN_FILTER_KEY] = "Ctrl+u";
  options->buttonAccel[RECTANGLE_FILTER_MASK_KEY] = "F7";
  options->buttonAccel[POLYGON_FILTER_MASK_KEY] = "F8";
  options->buttonAccel[RUN_HOTKEY_POLYGON_MODE_KEY] = "F9";
  options->buttonAccel[EDIT_FEATURE_MODE_KEY] = "F10";
  options->buttonAccel[UNDO_KEY] = "Ctrl+z";
  options->buttonAccel[DISPLAY_MULTIPLE_KEY] = "F11";
  options->buttonAccel[DISPLAY_ALL_KEY] = "F12";
  options->buttonAccel[CLEAR_HIGHLIGHT_KEY] = "Ctrl+h";
  options->buttonAccel[HIGHLIGHT_POLYGON_MODE_KEY] = "Ctrl+p";
  options->buttonAccel[CLEAR_POLYGON_MODE_KEY] = "Ctrl+c";
  options->buttonAccel[TOGGLE_CONTOUR_KEY] = "Alt+c";
  options->buttonAccel[CZMIL_REPROCESS_LAND_KEY] = "Alt+l";
  options->buttonAccel[CZMIL_REPROCESS_WATER_KEY] = "Alt+h";
  options->buttonAccel[CZMIL_REPROCESS_SHALLOW_WATER_KEY] = "Alt+u";
  options->buttonAccel[SET_VIEW_LEVEL_KEY] = "Ctrl+v";
  options->buttonAccel[SET_NORTH_UP_KEY] = "Ctrl+n";
  options->buttonAccel[CHANGE_LIDAR_CLASS_KEY] = "Ctrl+g";
}
