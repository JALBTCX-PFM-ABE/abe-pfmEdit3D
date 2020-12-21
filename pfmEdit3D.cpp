
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



//  pfmEdit3D class.

#include "pfmEdit3D.hpp"
#include "pfmEdit3DHelp.hpp"
#include "verticalDatums.hpp"
#include "acknowledgments.hpp"


void envout (OPTIONS *options, MISC *misc, QMainWindow *mainWindow);
void setFlags (MISC *misc, OPTIONS *options);


/***************************************************************************/
/*!

   - Module :        pfmEdit3D

   - Programmer :    Jan C. Depner

   - Date :          11/18/08

   - Purpose :       3D version of Area-Based Editor point cloud editor.

****************************************************************************/

pfmEdit3D::pfmEdit3D (int *argc, char **argv, QWidget *parent):
  QMainWindow (parent, 0)
{
  uint8_t envin (OPTIONS *options, MISC *misc, QMainWindow *mainWindow);
  void set_defaults (MISC *misc, OPTIONS *options, uint8_t restore);


  extern char *optarg;
  int32_t option_index = 0;
  int32_t shmid = 0;


  options.z_factor = 1.0;
  options.z_offset = 0.0;
  force_auto_unload = NVFalse;
  misc.no_invalid = NVFalse;
  misc.no_reference = NVFalse;

  while (NVTrue) 
    {
      static struct option long_options[] = {{"shared_memory_key", required_argument, 0, 0},
                                             {"force_auto_unload", no_argument, 0, 0},
                                             {0, no_argument, 0, 0}};

      char c = (char) getopt_long (*argc, argv, "nr", long_options, &option_index);
      if (c == -1) break;

      switch (c) 
        {
        case 0:

          switch (option_index)
            {
            case 0:
              sscanf (optarg, "%d", &shmid);
              break;

            case 1:
              force_auto_unload = NVTrue;
              break;
            }
          break;


          //  Don't load invalid data flag.

        case 'n':
          misc.no_invalid = NVTrue;
          break;


          //  Don't load reference data flag.

        case 'r':
          misc.no_reference = NVTrue;
          break;
        }
    }


  //  We must have the shared memory ID.

  if (!shmid)
    {
      QMessageBox::critical (this, "pfmEdit3D", tr ("pfmEdit3D can only be run from pfmView using shared memory."), QMessageBox::Close);
      exit (-1);
    }


  /******************************************* IMPORTANT NOTE ABOUT SHARED MEMORY **************************************** \

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
      memory and then dies, all the other programs will be locked up.  When you look through the ABE code you'll see
      that we very rarely lock shared memory, and then only for very short periods of time.  This is by design.

  \******************************************* IMPORTANT NOTE ABOUT SHARED MEMORY ****************************************/


  //  Get the shared memory area.  If it doesn't exist, quit.  It should have already been created by pfmView.  The key
  //  is the process ID of the parent plus "_abe".  The parent process ID is a required command line argument (--shared_memory_id).

  QString skey;
  skey.sprintf ("%d_abe", shmid);


  misc.abeShare = new QSharedMemory (skey);

  if (misc.abeShare->attach (QSharedMemory::ReadWrite)) misc.abe_share = (ABE_SHARE *) misc.abeShare->data ();


  options.position_form = misc.abe_share->position_form;
  options.z_factor = misc.abe_share->z_factor;
  options.z_offset = misc.abe_share->z_offset;


  //  Have to set the focus policy or keypress events don't work properly at first in Focus Follows Mouse mode

  setFocusPolicy (Qt::WheelFocus);
  setFocus (Qt::ActiveWindowFocusReason);
  

  //  Set a few defaults for startup

  strcpy (misc.progname, argv[0]);
  polygon_flip = NVFalse;
  prev_poly_x = -1;
  prev_poly_y = -1;
  popup_active = NVFalse;
  double_click = NVFalse;
  move_feature = 0;
  prefs_dialog = NULL;
  mv_tracker = -1;
  mv_2D_tracker = -1;
  mv_measure_anchor = -1;
  mv_measure = -1;
  rb_polygon = -1;
  rb_measure = -1;
  rb_dist = -1;
  rb_rectangle = -1;
  moveWindow = 0;
  start_ctrl_x = -1;
  start_ctrl_y = -1;
  start_shift_y = -1;
  height_shift_y = 0;
  rotate_index = 0;
  lock_point = 0;
  slicing = NVFalse;
  filter_active = NVFalse;
  attr_filter_active = NVFalse;
  hof_filter_active = NVFalse;
  misc.nearest_point = 0;
  local_num_lines = 0;
  displayMessage_dialog = NULL;
  lidar_class_dialog = NULL;
  misc.messages = new QStringList ();
  misc.num_messages = 0;
  call_mode = NVTrue;
  flags_offset = PFM_USER_FLAGS + 1;


  //  Set all of the possible attributes (according to ABE).

  setTimeAttributes (&options.time_attribute);
  setGSFAttributes (options.gsf_attribute);
  setHOFAttributes (options.hof_attribute);
  setTOFAttributes (options.tof_attribute);
  setCZMILAttributes (options.czmil_attribute, options.czmil_flag_name);
  setLASAttributes (options.las_attribute);
  setBAGAttributes (options.bag_attribute);


  //  Check to see if we have the czmilReprocess program.

  char crname[1218];
#ifdef NVWIN3X
  strcpy (crname, "czmilReprocess.exe");
#else
  strcpy (crname, "czmilReprocess");
#endif
  cr_available = NVTrue;
  if (find_startup_name (crname) == NULL) cr_available = NVFalse;


  //  Need to set the distLineStart and distLineEnd variables to indicate no prior selection

  distLineStart.x = distLineStart.y = -1.0f;
  distLineEnd.x = distLineEnd.y = -1.0f;


  // 
  //                      Key flags are intialized here.
  //                      multiMode represents a waveform display mode in the CZMILwaveMonitor program 
  //                      (0: nearest-neighbor, 1: single waveform).  multiNum refers to the number
  //                      of multiple waveforms at the current time to display.  This variable will
  //                      equal to MAX_STACK_POINTS if we are in nearest-neighbor mode.
  //

  if (!misc.abe_share->mwShare.waveMonitorRunning) misc.abe_share->mwShare.multiMode = 1;
  misc.abe_share->mwShare.multiNum = 0;

  for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++)
    {
      multi_marker[i] = -1;
      misc.abe_share->mwShare.multiPresent[i] = -1;
    }


  save_z = NULL;


  //  Set up the marker data list

  marker[0].x = -MARKER_W;
  marker[0].y = -MARKER_H;
  marker[1].x = MARKER_W;
  marker[1].y = -MARKER_H;
  marker[2].x = MARKER_W;
  marker[2].y = -MARKER_H;
  marker[3].x = MARKER_W;
  marker[3].y = MARKER_H;
  marker[4].x = MARKER_W;
  marker[4].y = MARKER_H;
  marker[5].x = -MARKER_W;
  marker[5].y = MARKER_H;
  marker[6].x = -MARKER_W;
  marker[6].y = MARKER_H;
  marker[7].x = -MARKER_W;
  marker[7].y = -MARKER_H;
  marker[8].x = -MARKER_W;
  marker[8].y = 0;
  marker[9].x = -(MARKER_W / 5);
  marker[9].y = 0;
  marker[10].x = (MARKER_W / 5);
  marker[10].y = 0;
  marker[11].x = MARKER_W;
  marker[11].y = 0;
  marker[12].x = 0;
  marker[12].y = -MARKER_H;
  marker[13].x = 0;
  marker[13].y = -(MARKER_W / 5);
  marker[14].x = 0;
  marker[14].y = (MARKER_W / 5);
  marker[15].x = 0;
  marker[15].y = MARKER_H;


  //  Make the 2D "tracker" cursor painter path.

  tracker_2D = QPainterPath ();

  tracker_2D.moveTo (0, 0);
  tracker_2D.lineTo (30, 0);
  tracker_2D.lineTo (30, 20);
  tracker_2D.lineTo (0, 20);
  tracker_2D.lineTo (0, 0);

  tracker_2D.moveTo (0, 10);
  tracker_2D.lineTo (12, 10);

  tracker_2D.moveTo (30, 10);
  tracker_2D.lineTo (18, 10);

  tracker_2D.moveTo (15, 0);
  tracker_2D.lineTo (15, 6);

  tracker_2D.moveTo (15, 20);
  tracker_2D.lineTo (15, 14);


  //  Set the main icon

  setWindowIcon (QIcon (":/icons/pfm_abe.png"));


  //  Define all of the cursors

  editFeatureCursor = QCursor (QPixmap (":/icons/edit_feature_cursor.png"), 11, 26);
  deleteFeatureCursor = QCursor (QPixmap (":/icons/delete_feature_cursor.png"), 15, 15);
  addFeatureCursor = QCursor (QPixmap (":/icons/add_feature_cursor.png"), 15, 15);
  moveFeatureCursor = QCursor (QPixmap (":/icons/move_feature_cursor.png"), 15, 15);
  deleteRectCursor = QCursor (QPixmap (":/icons/delete_rect_cursor.png"), 3, 0);
  deletePolyCursor = QCursor (QPixmap (":/icons/delete_poly_cursor.png"), 3, 0);
  restoreRectCursor = QCursor (QPixmap (":/icons/restore_rect_cursor.png"), 1, 1);
  restorePolyCursor = QCursor (QPixmap (":/icons/restore_poly_cursor.png"), 1, 1);
  hotkeyPolyCursor = QCursor (QPixmap (":/icons/hotkey_poly_cursor.png"), 0, 19);
  keepPolyCursor = QCursor (QPixmap (":/icons/keep_poly_cursor.png"), 5, 0);
  unreferencePolyCursor = QCursor (QPixmap (":/icons/unreference_poly_cursor.png"), 1, 1);
  referencePolyCursor = QCursor (QPixmap (":/icons/reference_poly_cursor.png"), 1, 1);
  rotateCursor = QCursor (QPixmap (":/icons/rotate_cursor.png"), 17, 17);
  zoomCursor = QCursor (QPixmap (":/icons/zoom_cursor.png"), 11, 11);
  maskCursor = QCursor (QPixmap (":/icons/mask_cursor.png"), 1, 1);
  measureCursor = QCursor (QPixmap (":/icons/measure_cursor.png"), 1, 1);
  filterMaskRectCursor = QCursor (QPixmap (":/icons/filter_mask_rect_cursor.png"), 1, 1);
  filterMaskPolyCursor = QCursor (QPixmap (":/icons/filter_mask_poly_cursor.png"), 1, 1);
  highlightPolyCursor = QCursor (QPixmap (":/icons/highlight_polygon_cursor.png"), 1, 1);
  clearPolyCursor = QCursor (QPixmap (":/icons/clear_polygon_cursor.png"), 1, 1);
  czmilReprocessPolyCursor = QCursor (QPixmap (":/icons/czmil_reprocess_poly_cursor.png"), 1, 1);
  czmilReprocessRectCursor = QCursor (QPixmap (":/icons/czmil_reprocess_rect_cursor.png"), 1, 1);
  cpfFilterCursor = QCursor (QPixmap (":/icons/cpf_filter_cursor.png"), 1, 1);
  sliceCursor = QCursor (QPixmap (":/icons/slice_cursor.png"), 1, 24);
  changeLidarClassCursor = QCursor (QPixmap (":/icons/change_lidar_class_cursor.png"), 3, 0);


  for (int32_t i = 0 ; i < NUMPROGS ; i++) ancillaryProgram[i] = NULL;


  if (misc.abe_share->polygon_count)
    {
      misc.abe_share->edit_area.min_y = 999999999.0;
      misc.abe_share->edit_area.max_y = -999999999.0;
      misc.abe_share->edit_area.min_x = 999999999.0;
      misc.abe_share->edit_area.max_x = -999999999.0;

      for (int32_t i = 0 ; i < misc.abe_share->polygon_count ; i++)
        {
          misc.abe_share->edit_area.min_y = qMin (misc.abe_share->edit_area.min_y, misc.abe_share->polygon_y[i]);
          misc.abe_share->edit_area.min_x = qMin (misc.abe_share->edit_area.min_x, misc.abe_share->polygon_x[i]);
          misc.abe_share->edit_area.max_y = qMax (misc.abe_share->edit_area.max_y, misc.abe_share->polygon_y[i]);
          misc.abe_share->edit_area.max_x = qMax (misc.abe_share->edit_area.max_x, misc.abe_share->polygon_x[i]);
        }
    }


  //  Open the PFM files and compute the MBR of all of the PFM areas.

  misc.total_mbr.min_y = 999.0;
  misc.total_mbr.min_x = 999.0;
  misc.total_mbr.max_y = -999.0;
  misc.total_mbr.max_x = -999.0;

  for (int32_t pfm = 0 ; pfm < misc.abe_share->pfm_count ; pfm++)
    {
      if ((misc.pfm_handle[pfm] = open_existing_pfm_file (&misc.abe_share->open_args[pfm])) < 0)
        {
          QMessageBox::warning (this, "pfmEdit3D", tr ("The file \n%1\n is not a PFM handle or list file or there was an error reading the file.\n\n"
                                                       "The error message returned was:\n\n").arg
                                (QDir::toNativeSeparators (QString (misc.abe_share->open_args[pfm].list_path))).arg (QString (pfm_error_str (pfm_error))));
        }

      if (misc.abe_share->open_args[pfm].head.proj_data.projection)
        {
          QMessageBox::warning (this, "pfmEdit3D", tr ("Sorry, pfmEdit3D does not handle projected data."));
          close_pfm_file (misc.pfm_handle[pfm]);
          fprintf (stdout, "Edit return status:0,0\n");
          fflush (stdout);
          clean_exit (-1);
        }


      misc.total_mbr.min_y = qMin (misc.total_mbr.min_y, misc.abe_share->open_args[pfm].head.mbr.min_y);
      misc.total_mbr.min_x = qMin (misc.total_mbr.min_x, misc.abe_share->open_args[pfm].head.mbr.min_x);
      misc.total_mbr.max_y = qMax (misc.total_mbr.max_y, misc.abe_share->open_args[pfm].head.mbr.max_y);
      misc.total_mbr.max_x = qMax (misc.total_mbr.max_x, misc.abe_share->open_args[pfm].head.mbr.max_x);


      //  Compute the average bin sizes.

      misc.avg_x_bin_size_degrees += misc.abe_share->open_args[pfm].head.x_bin_size_degrees;
      misc.avg_y_bin_size_degrees += misc.abe_share->open_args[pfm].head.y_bin_size_degrees;
    }


  //  Check for CZMIL and/or LAS attributes.

  misc.czmil_chan_attr = -1;
  misc.czmil_proc_attr = -1;
  misc.lidar_class_attr = -1;
  misc.czmil_urban_attr = -1;
  misc.czmil_d_index_attr = -1;
  for (int32_t i = 0 ; i < NUM_ATTR ; i++)
    {
      if (!strcmp (misc.abe_share->open_args[0].head.ndx_attr_name[i], "CZMIL Channel number")) misc.czmil_chan_attr = i;
      if (!strcmp (misc.abe_share->open_args[0].head.ndx_attr_name[i], "CZMIL Processing mode")) misc.czmil_proc_attr = i;
      if (!strcmp (misc.abe_share->open_args[0].head.ndx_attr_name[i], "CZMIL D_index")) misc.czmil_d_index_attr = i;
      if (!strcmp (misc.abe_share->open_args[0].head.ndx_attr_name[i], "CZMIL Urban noise flags")) misc.czmil_urban_attr = i;

      if (!strcmp (misc.abe_share->open_args[0].head.ndx_attr_name[i], "CZMIL Classification") ||
          !strcmp (misc.abe_share->open_args[0].head.ndx_attr_name[i], "LAS Classification") ||
          !strcmp (misc.abe_share->open_args[0].head.ndx_attr_name[i], "HOF Classification status") ||
          !strcmp (misc.abe_share->open_args[0].head.ndx_attr_name[i], "TOF Classification status"))
        misc.lidar_class_attr = i;
    }


  misc.avg_x_bin_size_degrees /= (double) misc.abe_share->pfm_count;
  misc.avg_y_bin_size_degrees /= (double) misc.abe_share->pfm_count;


  //  For the average bin size in meters we're really only interested in the Y bin size because we're going to use it for geo_distance.

  if (misc.abe_share->otf_width)
    {
      double az;
      invgp (NV_A0, NV_B0, misc.total_mbr.min_y, misc.total_mbr.min_x, misc.total_mbr.min_y + misc.abe_share->otf_y_bin_size, misc.total_mbr.min_x,
             &misc.avg_bin_size_meters, &az);
    }
  else
    {
      double az;
      invgp (NV_A0, NV_B0, misc.total_mbr.min_y, misc.total_mbr.min_x, misc.total_mbr.min_y + misc.avg_y_bin_size_degrees, misc.total_mbr.min_x,
             &misc.avg_bin_size_meters, &az);
    }

  init_geo_distance (misc.avg_bin_size_meters, misc.total_mbr.min_x, misc.total_mbr.min_y, misc.total_mbr.max_x, misc.total_mbr.max_y);


  //  We need to initialize the UTM/Geodetic conversion software (from PROJ.4).  Get the zone from the center of the total MBR of all
  //  of the PFM files.

  double cen_x = misc.total_mbr.min_x + (misc.total_mbr.max_x - misc.total_mbr.min_x) / 2.0;
  double cen_y = misc.total_mbr.min_y + (misc.total_mbr.max_y - misc.total_mbr.min_y) / 2.0;

  int32_t zone = (int32_t) (31.0 + cen_x / 6.0);
  if (zone >= 61) zone = 60;        
  if (zone <= 0) zone = 1;

  char blk[20] = {'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'};
  int32_t blknum = int32_t ((cen_y + 80.0) / 8.0);

  zoneName.sprintf ("%d%1c", zone, blk[blknum]);

  char str[128];
  if (cen_y < 0.0)
    {
      sprintf (str, "+proj=utm +zone=%d +lon_0=%.9f +ellps=WGS84 +datum=WGS84 +south", zone, cen_x);
    }
  else
    {
      sprintf (str, "+proj=utm +zone=%d +lon_0=%.9f +ellps=WGS84 +datum=WGS84", zone, cen_x);
    }

  if (!(misc.pj_utm = pj_init_plus (str)))
    {
      QMessageBox::critical (this, "pfmEdit3D", tr ("Error initializing UTM projection\n"));
    }

  if (!(misc.pj_latlon = pj_init_plus ("+proj=latlon +ellps=WGS84 +datum=WGS84")))
    {
      QMessageBox::critical (this, "pfmEdit3D", tr ("Error initializing latlon projection\n"));
    }


  //  Set all of the defaults

  set_defaults (&misc, &options, NVFalse);


  //  We have to get the icon size out of sequence (the rest of the options are read in env_in_out.cpp)
  //  so that we'll have the proper sized icons for the toolbars.  Otherwise, they won't be placed correctly.

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/" + QString (misc.qsettings_app) + ".ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/" + QString (misc.qsettings_app) + ".ini";
#endif


  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup (misc.qsettings_app);
  options.main_button_icon_size = settings.value ("main button icon size", options.main_button_icon_size).toInt ();
  settings.endGroup ();


  //  Create View toolbar

  toolBar[0] = new QToolBar (tr ("View tool bar"));
  toolBar[0]->setToolTip (tr ("View tool bar"));
  addToolBar (toolBar[0]);
  toolBar[0]->setObjectName (tr ("View tool bar"));

  QButtonGroup *exitGrp = new QButtonGroup (this);
  connect (exitGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotExit (int)));

  bExitSave = new QToolButton (this);
  misc.button[SAVE_EXIT_KEY] = bExitSave;
  bExitSave->setIcon (misc.buttonIcon[SAVE_EXIT_KEY]);
  exitGrp->addButton (bExitSave, 0);
  bExitSave->setWhatsThis (exitSaveText);
  toolBar[0]->addWidget (bExitSave);


  //  Get the normal background color

  blankPalette.setColor (QPalette::Window, bExitSave->palette ().color (QWidget::backgroundRole ()));
  blankPalette.setColor (QPalette::WindowText, bExitSave->palette ().color (QWidget::backgroundRole ()));


  bExitMask = new QToolButton (this);
  misc.button[SAVE_EXIT_MASK_KEY] = bExitMask;
  bExitMask->setIcon (misc.buttonIcon[SAVE_EXIT_MASK_KEY]);
  exitGrp->addButton (bExitMask, 1);
  bExitMask->setWhatsThis (exitMaskText);
  toolBar[0]->addWidget (bExitMask);

  bExitNoSave = new QToolButton (this);
  misc.button[NO_SAVE_EXIT_KEY] = bExitNoSave;
  bExitNoSave->setIcon (misc.buttonIcon[NO_SAVE_EXIT_KEY]);
  exitGrp->addButton (bExitNoSave, 2);
  bExitNoSave->setWhatsThis (exitNoSaveText);
  toolBar[0]->addWidget (bExitNoSave);

  bReset = new QToolButton (this);
  misc.button[RESET_KEY] = bReset;
  bReset->setIcon (misc.buttonIcon[RESET_KEY]);
  bReset->setWhatsThis (resetText);
  connect (bReset, SIGNAL (clicked ()), this, SLOT (slotReset ()));
  toolBar[0]->addWidget (bReset);

  bLevel = new QToolButton (this);
  misc.button[SET_VIEW_LEVEL_KEY] = bLevel;
  bLevel->setIcon (misc.buttonIcon[SET_VIEW_LEVEL_KEY]);
  bLevel->setWhatsThis (levelText);
  connect (bLevel, SIGNAL (clicked ()), this, SLOT (slotLevel ()));
  toolBar[0]->addWidget (bLevel);


  bNorthUp = new QToolButton (this);
  misc.button[SET_NORTH_UP_KEY] = bNorthUp;
  bNorthUp->setIcon (misc.buttonIcon[SET_NORTH_UP_KEY]);
  bNorthUp->setWhatsThis (northUpText);
  connect (bNorthUp, SIGNAL (clicked ()), this, SLOT (slotNorthUp ()));
  toolBar[0]->addWidget (bNorthUp);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  bDisplayManInvalid = new QToolButton (this);
  bDisplayManInvalid->setIcon (QIcon (":/icons/display_manually_invalid.png"));
  bDisplayManInvalid->setToolTip (tr ("Display manually invalidated points in addition to valid points"));
  bDisplayManInvalid->setWhatsThis (displayManInvalidText);
  bDisplayManInvalid->setCheckable (true);
  toolBar[0]->addWidget (bDisplayManInvalid);

  bDisplayFltInvalid = new QToolButton (this);
  bDisplayFltInvalid->setIcon (QIcon (":/icons/display_filter_invalid.png"));
  bDisplayFltInvalid->setToolTip (tr ("Display filter invalidated points in addition to valid points"));
  bDisplayFltInvalid->setWhatsThis (displayFltInvalidText);
  bDisplayFltInvalid->setCheckable (true);
  toolBar[0]->addWidget (bDisplayFltInvalid);

  bDisplayNull = new QToolButton (this);
  bDisplayNull->setIcon (QIcon (":/icons/display_null.png"));
  bDisplayNull->setToolTip (tr ("Display NULL value points in addition to valid points"));
  bDisplayNull->setWhatsThis (displayNullText);
  bDisplayNull->setCheckable (true);
  toolBar[0]->addWidget (bDisplayNull);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  bDisplayAll = new QToolButton (this);
  misc.button[DISPLAY_ALL_KEY] = bDisplayAll;
  bDisplayAll->setIcon (misc.buttonIcon[DISPLAY_ALL_KEY]);
  bDisplayAll->setWhatsThis (displayAllText);
  bDisplayAll->setEnabled (false);
  connect (bDisplayAll, SIGNAL (clicked ()), this, SLOT (slotDisplayAll ()));
  toolBar[0]->addWidget (bDisplayAll);


  bUndisplaySingle = new QToolButton (this);
  bUndisplaySingle->setIcon (QIcon (":/icons/undisplaysingle.png"));
  bUndisplaySingle->setToolTip (tr ("Hide data points from a single line"));
  bUndisplaySingle->setWhatsThis (undisplaySingleText);
  connect (bUndisplaySingle, SIGNAL (clicked ()), this, SLOT (slotUndisplaySingle ()));
  toolBar[0]->addWidget (bUndisplaySingle);

  bDisplayMultiple = new QToolButton (this);
  misc.button[DISPLAY_MULTIPLE_KEY] = bDisplayMultiple;
  bDisplayMultiple->setIcon (misc.buttonIcon[DISPLAY_MULTIPLE_KEY]);
  bDisplayMultiple->setToolTip (tr ("Select multiple files mode"));
  bDisplayMultiple->setWhatsThis (displayMultipleText);
  connect (bDisplayMultiple, SIGNAL (clicked ()), this, SLOT (slotDisplayMultiple ()));
  toolBar[0]->addWidget (bDisplayMultiple);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  //  If you add to the 5 buttons below you must change the value of PRE_USER in pfmEdit3DDef.hpp.  These are the
  //  pre-user flag buttons.  NOTE: flag_invalid should always be the last before the USER flags (i.e. PRE_USER - 1).

  flagIcon[0] = QIcon (":/icons/user_flag.png");
  flagIcon[1] = QIcon (":/icons/suspect.png");
  flagIcon[2] = QIcon (":/icons/selected.png");
  flagIcon[3] = QIcon (":/icons/feature.png");
  flagIcon[4] = QIcon (":/icons/designated.png");
  flagIcon[5] = QIcon (":/icons/flag_invalid.png");


  flagIcon[PRE_USER + 0] = QIcon (":/icons/user_flag01.png");
  flagIcon[PRE_USER + 1] = QIcon (":/icons/user_flag02.png");
  flagIcon[PRE_USER + 2] = QIcon (":/icons/user_flag03.png");
  flagIcon[PRE_USER + 3] = QIcon (":/icons/user_flag04.png");
  flagIcon[PRE_USER + 4] = QIcon (":/icons/user_flag05.png");
  flagIcon[PRE_USER + 5] = QIcon (":/icons/user_flag06.png");
  flagIcon[PRE_USER + 6] = QIcon (":/icons/user_flag07.png");
  flagIcon[PRE_USER + 7] = QIcon (":/icons/user_flag08.png");
  flagIcon[PRE_USER + 8] = QIcon (":/icons/user_flag09.png");
  flagIcon[PRE_USER + 9] = QIcon (":/icons/user_flag10.png");


  QMenu *flagMenu = new QMenu (this);

  flagGrp = new QActionGroup (this);
  connect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
  flagGrp->setExclusive (true);

  flag[0] = flagMenu->addAction (tr ("Turn off data flags"));
  flagGrp->addAction (flag[0]);
  flag[0]->setIcon (flagIcon[0]);
  flag[0]->setCheckable (true);

  flag[1] = flagMenu->addAction (tr ("Mark suspect data"));
  flagGrp->addAction (flag[1]);
  flag[1]->setIcon (flagIcon[1]);
  flag[1]->setCheckable (true);

  flag[2] = flagMenu->addAction (tr ("Mark selected soundings"));
  flagGrp->addAction (flag[2]);
  flag[2]->setIcon (flagIcon[2]);
  flag[2]->setCheckable (true);

  flag[3] = flagMenu->addAction (tr ("Mark selected features"));
  flagGrp->addAction (flag[3]);
  flag[3]->setIcon (flagIcon[3]);
  flag[3]->setCheckable (true);

  flag[4] = flagMenu->addAction (tr ("Mark designated soundings"));
  flagGrp->addAction (flag[4]);
  flag[4]->setIcon (flagIcon[4]);
  flag[4]->setCheckable (true);

  flag[5] = flagMenu->addAction (tr ("Mark invalid data"));
  flagGrp->addAction (flag[5]);
  flag[5]->setIcon (flagIcon[5]);
  flag[5]->setCheckable (true);

  for (int32_t i = 0 ; i < PFM_USER_FLAGS ; i++)
    {
      QString string = tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[i]);
      flag[i + PRE_USER] = flagMenu->addAction (string);
      flagGrp->addAction (flag[i + PRE_USER]);
      flag[i + PRE_USER]->setIcon (flagIcon[i + PRE_USER]);
      flag[i + PRE_USER]->setCheckable (true);
      if (string.contains ("PFM_USER_")) flag[i + PRE_USER]->setVisible (false);
    }

  bFlag = new QToolButton (this);
  bFlag->setIcon (QIcon (":/icons/user_flag.png"));
  bFlag->setToolTip (tr ("Toggle data flags on/off"));
  bFlag->setWhatsThis (flagText);
  bFlag->setPopupMode (QToolButton::InstantPopup);
  bFlag->setMenu (flagMenu);
  toolBar[0]->addWidget (bFlag);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  QButtonGroup *modeGrp = new QButtonGroup (this);
  connect (modeGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotMode (int)));
  modeGrp->setExclusive (true);

  bHighlightPoly = new QToolButton (this);
  misc.button[HIGHLIGHT_POLYGON_MODE_KEY] = bHighlightPoly;
  bHighlightPoly->setIcon (misc.buttonIcon[HIGHLIGHT_POLYGON_MODE_KEY]);
  modeGrp->addButton (bHighlightPoly, HIGHLIGHT_POLYGON);
  bHighlightPoly->setToolTip (tr ("Highlight points in polygon mode"));
  bHighlightPoly->setWhatsThis (highlightPolyText);
  bHighlightPoly->setCheckable (true);
  toolBar[0]->addWidget (bHighlightPoly);

  bInvertHighlight = new QToolButton (this);
  bInvertHighlight->setIcon (QIcon (":/icons/invert_highlight.png"));
  bInvertHighlight->setToolTip (tr ("Invert highlighted point selection"));
  bInvertHighlight->setWhatsThis (invertHighlightText);
  connect (bInvertHighlight, SIGNAL (clicked ()), this, SLOT (slotInvertHighlight ()));
  toolBar[0]->addWidget (bInvertHighlight);

  bClearPoly = new QToolButton (this);
  misc.button[CLEAR_POLYGON_MODE_KEY] = bClearPoly;
  bClearPoly->setIcon (misc.buttonIcon[CLEAR_POLYGON_MODE_KEY]);
  modeGrp->addButton (bClearPoly, CLEAR_POLYGON);
  bClearPoly->setToolTip (tr ("Clear highlights in polygon mode"));
  bClearPoly->setWhatsThis (clearPolyText);
  bClearPoly->setCheckable (true);
  toolBar[0]->addWidget (bClearPoly);

  bClearHighlight = new QToolButton (this);
  misc.button[CLEAR_HIGHLIGHT_KEY] = bClearHighlight;
  bClearHighlight->setIcon (misc.buttonIcon[CLEAR_HIGHLIGHT_KEY]);
  bClearHighlight->setWhatsThis (clearHighlightText);
  connect (bClearHighlight, SIGNAL (clicked ()), this, SLOT (slotClearHighlight ()));
  toolBar[0]->addWidget (bClearHighlight);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  bContour = new QToolButton (this);
  misc.button[TOGGLE_CONTOUR_KEY] = bContour;
  bContour->setIcon (misc.buttonIcon[TOGGLE_CONTOUR_KEY]);
  bContour->setWhatsThis (contourText);
  bContour->setCheckable (true);
  toolBar[0]->addWidget (bContour);


  //  Create Feature toolbar

  toolBar[1] = new QToolBar (tr ("Feature tool bar"));
  toolBar[1]->setToolTip (tr ("Feature tool bar"));
  addToolBar (toolBar[1]);
  toolBar[1]->setObjectName (tr ("Feature tool bar"));

  QMenu *featureMenu = new QMenu (this);

  QActionGroup *featureGrp = new QActionGroup (this);
  connect (featureGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFeatureMenu (QAction *)));

  displayFeatureIcon[0] = QIcon (":/icons/display_no_feature.png");
  displayFeatureIcon[1] = QIcon (":/icons/display_all_feature.png");
  displayFeatureIcon[2] = QIcon (":/icons/display_unverified_feature.png");
  displayFeatureIcon[3] = QIcon (":/icons/display_verified_feature.png");
  displayFeature[0] = featureMenu->addAction (tr ("Don't display features"));
  displayFeature[1] = featureMenu->addAction (tr ("Display all features"));
  displayFeature[2] = featureMenu->addAction (tr ("Display unverified features"));
  displayFeature[3] = featureMenu->addAction (tr ("Display verified features"));
  for (int32_t i = 0 ; i < 4 ; i++)
    {
      displayFeature[i]->setIcon (displayFeatureIcon[i]);
      featureGrp->addAction (displayFeature[i]);
    }

  bDisplayFeature = new QToolButton (this);
  bDisplayFeature->setIcon (displayFeatureIcon[options.display_feature]);
  bDisplayFeature->setToolTip (tr ("Select type of feature data to display"));
  bDisplayFeature->setWhatsThis (displayFeatureText);
  bDisplayFeature->setPopupMode (QToolButton::InstantPopup);
  bDisplayFeature->setMenu (featureMenu);
  toolBar[1]->addWidget (bDisplayFeature);

  bDisplayChildren = new QToolButton (this);
  bDisplayChildren->setIcon (QIcon (":/icons/displayfeaturechildren.png"));
  bDisplayChildren->setToolTip (tr ("Display feature group members"));
  bDisplayChildren->setWhatsThis (displayChildrenText);
  bDisplayChildren->setCheckable (true);
  toolBar[1]->addWidget (bDisplayChildren);

  bDisplayFeatureInfo = new QToolButton (this);
  bDisplayFeatureInfo->setIcon (QIcon (":/icons/displayfeatureinfo.png"));
  bDisplayFeatureInfo->setToolTip (tr ("Display feature information"));
  bDisplayFeatureInfo->setWhatsThis (displayFeatureInfoText);
  bDisplayFeatureInfo->setCheckable (true);
  toolBar[1]->addWidget (bDisplayFeatureInfo);

  bAddFeature = new QToolButton (this);
  bAddFeature->setIcon (QIcon (":/icons/addfeature.png"));
  modeGrp->addButton (bAddFeature, ADD_FEATURE);
  bAddFeature->setToolTip (tr ("Add feature mode"));
  bAddFeature->setWhatsThis (addFeatureText);
  bAddFeature->setCheckable (true);
  toolBar[1]->addWidget (bAddFeature);

  bEditFeature = new QToolButton (this);
  misc.button[EDIT_FEATURE_MODE_KEY] = bEditFeature;
  bEditFeature->setIcon (misc.buttonIcon[EDIT_FEATURE_MODE_KEY]);
  modeGrp->addButton (bEditFeature, EDIT_FEATURE);
  bEditFeature->setToolTip (tr ("Edit feature mode"));
  bEditFeature->setWhatsThis (editFeatureText);
  bEditFeature->setCheckable (true);
  toolBar[1]->addWidget (bEditFeature);

  bMoveFeature = new QToolButton (this);
  bMoveFeature->setIcon (QIcon (":/icons/movefeature.png"));
  modeGrp->addButton (bMoveFeature, MOVE_FEATURE);
  bMoveFeature->setToolTip (tr ("Move feature mode"));
  bMoveFeature->setWhatsThis (moveFeatureText);
  bMoveFeature->setCheckable (true);
  toolBar[1]->addWidget (bMoveFeature);

  bDeleteFeature = new QToolButton (this);
  bDeleteFeature->setIcon (QIcon (":/icons/deletefeature.png"));
  modeGrp->addButton (bDeleteFeature, DELETE_FEATURE);
  bDeleteFeature->setToolTip (tr ("Invalidate feature mode"));
  bDeleteFeature->setWhatsThis (deleteFeatureText);
  bDeleteFeature->setCheckable (true);
  toolBar[1]->addWidget (bDeleteFeature);


  bVerifyFeatures = new QToolButton (this);
  bVerifyFeatures->setIcon (QIcon (":/icons/verify_features.png"));
  bVerifyFeatures->setToolTip (tr ("Verify all valid features"));
  bVerifyFeatures->setWhatsThis (verifyFeaturesText);
  connect (bVerifyFeatures, SIGNAL (clicked ()), this, SLOT (slotVerifyFeatures ()));
  toolBar[1]->addWidget (bVerifyFeatures);


  //  Create Reference toolbar

  toolBar[2] = new QToolBar (tr ("Reference tool bar"));
  toolBar[2]->setToolTip (tr ("Reference tool bar"));
  addToolBar (toolBar[2]);
  toolBar[2]->setObjectName (tr ("Reference tool bar"));

  bDisplayReference = new QToolButton (this);
  bDisplayReference->setIcon (QIcon (":/icons/displayreference.png"));
  bDisplayReference->setToolTip (tr ("Mark reference data"));
  bDisplayReference->setWhatsThis (displayReferenceText);
  bDisplayReference->setCheckable (true);
  toolBar[2]->addWidget (bDisplayReference);


  toolBar[2]->addSeparator ();
  toolBar[2]->addSeparator ();


  bReferencePoly = new QToolButton (this);
  bReferencePoly->setIcon (QIcon (":/icons/reference_poly.png"));
  modeGrp->addButton (bReferencePoly, SET_REFERENCE);
  bReferencePoly->setToolTip (tr ("Polygon set reference mode"));
  bReferencePoly->setWhatsThis (referencePolyText);
  bReferencePoly->setCheckable (true);
  toolBar[2]->addWidget (bReferencePoly);

  bUnreferencePoly = new QToolButton (this);
  bUnreferencePoly->setIcon (QIcon (":/icons/unreference_poly.png"));
  modeGrp->addButton (bUnreferencePoly, UNSET_REFERENCE);
  bUnreferencePoly->setToolTip (tr ("Polygon unset reference mode"));
  bUnreferencePoly->setWhatsThis (unreferencePolyText);
  bUnreferencePoly->setCheckable (true);
  toolBar[2]->addWidget (bUnreferencePoly);


  //  Create Utilities toolbar

  toolBar[3] = new QToolBar (tr ("Utilities tool bar"));
  toolBar[3]->setToolTip (tr ("Utilities tool bar"));
  addToolBar (toolBar[3]);
  toolBar[3]->setObjectName (tr ("Utilities tool bar"));


  bUnload = new QToolButton (this);
  bUnload->setIcon (QIcon (":/icons/unload.png"));
  bUnload->setToolTip (tr ("Automatically unload changed data on save/exit"));
  bUnload->setWhatsThis (unloadText);
  bUnload->setCheckable (true);
  toolBar[3]->addWidget (bUnload);


  toolBar[3]->addSeparator ();
  toolBar[3]->addSeparator ();


  bPrefs = new QToolButton (this);
  bPrefs->setIcon (QIcon (":/icons/prefs.png"));
  bPrefs->setToolTip (tr ("Change application preferences"));
  bPrefs->setWhatsThis (prefsText);
  connect (bPrefs, SIGNAL (clicked ()), this, SLOT (slotPrefs ()));
  toolBar[3]->addWidget (bPrefs);


  toolBar[3]->addSeparator ();
  toolBar[3]->addSeparator ();


  bHelp = QWhatsThis::createAction (this);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  toolBar[3]->addAction (bHelp);


  //  Create Edit toolbar

  toolBar[4] = new QToolBar (tr ("Edit tool bar"));
  toolBar[4]->setToolTip (tr ("Edit tool bar"));
  addToolBar (toolBar[4]);
  toolBar[4]->setObjectName (tr ("Edit tool bar"));

  bStop = new QToolButton (this);
  bStop->setIcon (QIcon (":/icons/stop.png"));
  bStop->setToolTip (tr ("Stop drawing"));
  bStop->setWhatsThis (stopText);
  bStop->setEnabled (false);
  connect (bStop, SIGNAL (clicked ()), this, SLOT (slotStop ()));
  toolBar[4]->addWidget (bStop);


  toolBar[4]->addSeparator ();
  toolBar[4]->addSeparator ();


  bDeletePoint = new QToolButton (this);
  misc.button[DELETE_POINT_MODE_KEY] = bDeletePoint;
  bDeletePoint->setIcon (misc.buttonIcon[DELETE_POINT_MODE_KEY]);
  modeGrp->addButton (bDeletePoint, DELETE_POINT);
  bDeletePoint->setToolTip (tr ("Delete subrecord/record mode"));
  bDeletePoint->setWhatsThis (deletePointText);
  bDeletePoint->setCheckable (true);
  toolBar[4]->addWidget (bDeletePoint);


  toolBar[4]->addSeparator ();
  toolBar[4]->addSeparator ();


  bDeleteRect = new QToolButton (this);
  misc.button[DELETE_RECTANGLE_MODE_KEY] = bDeleteRect;
  bDeleteRect->setIcon (misc.buttonIcon[DELETE_RECTANGLE_MODE_KEY]);
  modeGrp->addButton (bDeleteRect, DELETE_RECTANGLE);
  bDeleteRect->setToolTip (tr ("Rectangular delete mode"));
  bDeleteRect->setWhatsThis (deleteRectText);
  bDeleteRect->setCheckable (true);
  toolBar[4]->addWidget (bDeleteRect);

  bDeletePoly = new QToolButton (this);
  misc.button[DELETE_POLYGON_MODE_KEY] = bDeletePoly;
  bDeletePoly->setIcon (misc.buttonIcon[DELETE_POLYGON_MODE_KEY]);
  modeGrp->addButton (bDeletePoly, DELETE_POLYGON);
  bDeletePoly->setToolTip (tr ("Polygon delete mode"));
  bDeletePoly->setWhatsThis (deletePolyText);
  bDeletePoly->setCheckable (true);
  toolBar[4]->addWidget (bDeletePoly);


  toolBar[4]->addSeparator ();
  toolBar[4]->addSeparator ();


  bKeepPoly = new QToolButton (this);
  bKeepPoly->setIcon (QIcon (":/icons/keep_poly.png"));
  modeGrp->addButton (bKeepPoly, KEEP_POLYGON);
  bKeepPoly->setToolTip (tr ("Delete outside polygon mode"));
  bKeepPoly->setWhatsThis (keepPolyText);
  bKeepPoly->setCheckable (true);
  toolBar[4]->addWidget (bKeepPoly);


  toolBar[4]->addSeparator ();
  toolBar[4]->addSeparator ();


  bRestoreRect = new QToolButton (this);
  bRestoreRect->setIcon (QIcon (":/icons/restore_rect.png"));
  modeGrp->addButton (bRestoreRect, RESTORE_RECTANGLE);
  bRestoreRect->setToolTip (tr ("Rectangle restore mode"));
  bRestoreRect->setWhatsThis (restoreRectText);
  bRestoreRect->setCheckable (true);
  toolBar[4]->addWidget (bRestoreRect);

  bRestorePoly = new QToolButton (this);
  misc.button[10] = bRestorePoly;
  bRestorePoly->setIcon (QIcon (":/icons/restore_poly.png"));
  modeGrp->addButton (bRestorePoly, RESTORE_POLYGON);
  bRestorePoly->setToolTip (tr ("Polygon restore mode"));
  bRestorePoly->setWhatsThis (restorePolyText);
  bRestorePoly->setCheckable (true);
  toolBar[4]->addWidget (bRestorePoly);


  bUndo = new QToolButton (this);
  misc.button[UNDO_KEY] = bUndo;
  bUndo->setIcon (misc.buttonIcon[UNDO_KEY]);
  bUndo->setToolTip (tr ("Undo last edit operation"));
  bUndo->setWhatsThis (undoText);
  connect (bUndo, SIGNAL (clicked ()), this, SLOT (slotUndo ()));
  toolBar[4]->addWidget (bUndo);


  toolBar[4]->addSeparator ();
  toolBar[4]->addSeparator ();


  bHotkeyPoly = new QToolButton (this);
  misc.button[RUN_HOTKEY_POLYGON_MODE_KEY] = bHotkeyPoly;
  bHotkeyPoly->setIcon (misc.buttonIcon[RUN_HOTKEY_POLYGON_MODE_KEY]);
  modeGrp->addButton (bHotkeyPoly, HOTKEY_POLYGON);
  bHotkeyPoly->setToolTip (tr ("Polygon hot key mode"));
  bHotkeyPoly->setWhatsThis (hotkeyPolyText);
  bHotkeyPoly->setCheckable (true);
  toolBar[4]->addWidget (bHotkeyPoly);


  toolBar[4]->addSeparator ();
  toolBar[4]->addSeparator ();

  bAreaFilter = new QToolButton (this);
  misc.button[AREA_FILTER_KEY] = bAreaFilter;
  bAreaFilter->setIcon (misc.buttonIcon[AREA_FILTER_KEY]);
  bAreaFilter->setToolTip (tr ("Run the area-based filter on the displayed data"));
  bAreaFilter->setWhatsThis (areaFilterText);
  connect (bAreaFilter, SIGNAL (clicked ()), this, SLOT (slotAreaFilter ()));
  toolBar[4]->addWidget (bAreaFilter);

  bStatFilter = new QToolButton (this);
  misc.button[STAT_FILTER_KEY] = bStatFilter;
  bStatFilter->setIcon (misc.buttonIcon[STAT_FILTER_KEY]);
  bAreaFilter->setToolTip (tr ("Run the AVT statistical filter on the displayed data"));
  bStatFilter->setWhatsThis (statFilterText);
  connect (bStatFilter, SIGNAL (clicked ()), this, SLOT (slotStatFilter ()));
  toolBar[4]->addWidget (bStatFilter);

  bHPFilter = new QToolButton (this);
  misc.button[HP_FILTER_KEY] = bHPFilter;
  bHPFilter->setIcon (misc.buttonIcon[HP_FILTER_KEY]);
  bHPFilter->setToolTip (tr ("Run the Hockey Puck filter on the displayed data"));
  bHPFilter->setWhatsThis (HPFilterText);
  connect (bHPFilter, SIGNAL (clicked ()), this, SLOT (slotHPFilter ()));
  toolBar[4]->addWidget (bHPFilter);

  bAttrFilter = new QToolButton (this);
  misc.button[ATTR_FILTER_KEY] = bAttrFilter;
  bAttrFilter->setIcon (misc.buttonIcon[ATTR_FILTER_KEY]);
  bAttrFilter->setToolTip (tr ("Run the attribute filter on the displayed data"));
  bAttrFilter->setWhatsThis (attrFilterText);
  connect (bAttrFilter, SIGNAL (clicked ()), this, SLOT (slotAttrFilter ()));
  toolBar[4]->addWidget (bAttrFilter);

  bFilterRectMask = new QToolButton (this);
  misc.button[RECTANGLE_FILTER_MASK_KEY] = bFilterRectMask;
  bFilterRectMask->setIcon (misc.buttonIcon[RECTANGLE_FILTER_MASK_KEY]);
  bFilterRectMask->setWhatsThis (filterRectMaskText);
  modeGrp->addButton (bFilterRectMask, RECT_FILTER_MASK);
  bFilterRectMask->setToolTip (tr ("Rectangle filter mask mode"));
  bFilterRectMask->setCheckable (true);
  toolBar[4]->addWidget (bFilterRectMask);

  bFilterPolyMask = new QToolButton (this);
  misc.button[POLYGON_FILTER_MASK_KEY] = bFilterPolyMask;
  bFilterPolyMask->setIcon (misc.buttonIcon[POLYGON_FILTER_MASK_KEY]);
  bFilterPolyMask->setWhatsThis (filterPolyMaskText);
  modeGrp->addButton (bFilterPolyMask, POLY_FILTER_MASK);
  bFilterPolyMask->setToolTip (tr ("Polygon filter mask mode"));
  bFilterPolyMask->setCheckable (true);
  toolBar[4]->addWidget (bFilterPolyMask);

  bClearMasks = new QToolButton (this);
  bClearMasks->setIcon (QIcon (":/icons/clear_filter_masks.png"));
  bClearMasks->setToolTip (tr ("Clear all filter masks"));
  bClearMasks->setWhatsThis (clearMasksText);
  connect (bClearMasks, SIGNAL (clicked ()), this, SLOT (slotClearFilterMasks ()));
  toolBar[4]->addWidget (bClearMasks);

  toolBar[4]->addSeparator ();
  toolBar[4]->addSeparator ();


  bMeasure = new QToolButton (this);
  bMeasure->setIcon (QIcon (":/icons/measure.png"));
  modeGrp->addButton (bMeasure, MEASURE);
  bMeasure->setToolTip (tr ("Measuring mode"));
  bMeasure->setWhatsThis (measureText);
  bMeasure->setCheckable (true);
  toolBar[4]->addWidget (bMeasure);


  //  Create Mask toolbar

  toolBar[5] = new QToolBar (tr ("Mask tool bar"));
  toolBar[5]->setToolTip (tr ("Mask tool bar"));
  addToolBar (toolBar[5]);
  toolBar[5]->setObjectName (tr ("Mask tool bar"));

  bMaskOutsideRect = new QToolButton (this);
  bMaskOutsideRect->setIcon (QIcon (":/icons/mask_outside_rect.png"));
  bMaskOutsideRect->setToolTip (tr ("Hide data outside rectangle mode"));
  bMaskOutsideRect->setWhatsThis (maskOutsideRectText);
  bMaskOutsideRect->setCheckable (true);
  modeGrp->addButton (bMaskOutsideRect, MASK_OUTSIDE_RECTANGLE);
  toolBar[5]->addWidget (bMaskOutsideRect);

  bMaskOutsidePoly = new QToolButton (this);
  bMaskOutsidePoly->setIcon (QIcon (":/icons/mask_outside_poly.png"));
  bMaskOutsidePoly->setToolTip (tr ("Hide data outside polygon mode"));
  bMaskOutsidePoly->setWhatsThis (maskOutsidePolyText);
  bMaskOutsidePoly->setCheckable (true);
  modeGrp->addButton (bMaskOutsidePoly, MASK_OUTSIDE_POLYGON);
  toolBar[5]->addWidget (bMaskOutsidePoly);

  bMaskInsideRect = new QToolButton (this);
  bMaskInsideRect->setIcon (QIcon (":/icons/mask_inside_rect.png"));
  bMaskInsideRect->setToolTip (tr ("Hide data inside rectangle mode"));
  bMaskInsideRect->setWhatsThis (maskInsideRectText);
  bMaskInsideRect->setCheckable (true);
  modeGrp->addButton (bMaskInsideRect, MASK_INSIDE_RECTANGLE);
  toolBar[5]->addWidget (bMaskInsideRect);

  bMaskInsidePoly = new QToolButton (this);
  bMaskInsidePoly->setIcon (QIcon (":/icons/mask_inside_poly.png"));
  bMaskInsidePoly->setToolTip (tr ("Hide data inside polygon mode"));
  bMaskInsidePoly->setWhatsThis (maskInsidePolyText);
  bMaskInsidePoly->setCheckable (true);
  modeGrp->addButton (bMaskInsidePoly, MASK_INSIDE_POLYGON);
  toolBar[5]->addWidget (bMaskInsidePoly);

  bMaskClear = new QToolButton (this);
  bMaskClear->setIcon (QIcon (":/icons/clear_mask_shape.png"));
  bMaskClear->setToolTip (tr ("Clear hidden rectangles/polygons"));
  bMaskClear->setWhatsThis (maskClearText);
  connect (bMaskClear, SIGNAL (clicked ()), this, SLOT (slotMaskClear ()));
  toolBar[5]->addWidget (bMaskClear);


  toolBar[5]->addSeparator ();
  toolBar[5]->addSeparator ();


  //  Create LIDAR toolbar
  //  If you want to add or subtract programs it needs to be done here, in pfmEditDef.hpp, and in set_defaults.cpp

  toolBar[6] = new QToolBar (tr ("LiDAR tool bar"));
  toolBar[6]->setToolTip (tr ("LiDAR tool bar"));
  addToolBar (toolBar[6]);
  toolBar[6]->setObjectName (tr ("LiDAR tool bar"));


  QButtonGroup *progGrp = new QButtonGroup (this);
  connect (progGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotProg (int)));
  progGrp->setExclusive (false);


  bLidarMonitor = new QToolButton (this);
  bLidarMonitor->setIcon (QIcon (":/icons/lidar_monitor.png"));
  bLidarMonitor->setWhatsThis (lidarMonitorText);
  bLidarMonitor->setCheckable (true);
  QString tip = options.description[LIDARMONITOR] + "  [" + options.hotkey[LIDARMONITOR] + "]";
  bLidarMonitor->setToolTip (tip);
  bLidarMonitor->setShortcut (QKeySequence (options.hotkey[LIDARMONITOR]));
  options.progButton[LIDARMONITOR] = bLidarMonitor;
  progGrp->addButton (bLidarMonitor, LIDARMONITOR);
  toolBar[6]->addWidget (bLidarMonitor);


  bChartsPic = new QToolButton (this);
  bChartsPic->setIcon (QIcon (":/icons/charts_pic.png"));
  bChartsPic->setWhatsThis (chartsPicText);
  bChartsPic->setCheckable (true);
  tip =  options.description[CHARTSPIC] + "  [" + options.hotkey[CHARTSPIC] + "]";
  bChartsPic->setToolTip (tip);
  bChartsPic->setShortcut (QKeySequence (options.hotkey[CHARTSPIC]));
  options.progButton[CHARTSPIC] = bChartsPic;
  progGrp->addButton (bChartsPic, CHARTSPIC);
  toolBar[6]->addWidget (bChartsPic);

  bWaveformMonitor = new QToolButton (this);
  bWaveformMonitor->setIcon (QIcon (":/icons/waveform_monitor.png"));
  bWaveformMonitor->setWhatsThis (waveformMonitorText);
  bWaveformMonitor->setCheckable (true);
  tip =  options.description[WAVEFORMMONITOR] + "  [" + options.hotkey[WAVEFORMMONITOR] + "]";
  bWaveformMonitor->setToolTip (tip);
  bWaveformMonitor->setShortcut (QKeySequence (options.hotkey[WAVEFORMMONITOR]));
  options.progButton[WAVEFORMMONITOR] = bWaveformMonitor;
  progGrp->addButton (bWaveformMonitor, WAVEFORMMONITOR);
  toolBar[6]->addWidget (bWaveformMonitor);

  bWaveWaterfallShallow = new QToolButton (this);
  bWaveWaterfallShallow->setIcon (QIcon (":/icons/wave_waterfall_shallow.png"));
  bWaveWaterfallShallow->setWhatsThis (waveWaterfallShallowText);
  bWaveWaterfallShallow->setCheckable (true);
  tip =  options.description[WAVEWATERFALL_SHALLOW] + "  [" + options.hotkey[WAVEWATERFALL_SHALLOW] + "]";
  bWaveWaterfallShallow->setToolTip (tip);
  bWaveWaterfallShallow->setShortcut (QKeySequence (options.hotkey[WAVEWATERFALL_SHALLOW]));
  options.progButton[WAVEWATERFALL_SHALLOW] = bWaveWaterfallShallow;
  progGrp->addButton (bWaveWaterfallShallow, WAVEWATERFALL_SHALLOW);
  toolBar[6]->addWidget (bWaveWaterfallShallow);

  bWaveWaterfallDeep = new QToolButton (this);
  bWaveWaterfallDeep->setIcon (QIcon (":/icons/wave_waterfall_deep.png"));
  bWaveWaterfallDeep->setWhatsThis (waveWaterfallDeepText);
  bWaveWaterfallDeep->setCheckable (true);
  tip =  options.description[WAVEWATERFALL_DEEP] + "  [" + options.hotkey[WAVEWATERFALL_DEEP] + "]";
  bWaveWaterfallDeep->setToolTip (tip);
  bWaveWaterfallDeep->setShortcut (QKeySequence (options.hotkey[WAVEWATERFALL_DEEP]));
  options.progButton[WAVEWATERFALL_DEEP] = bWaveWaterfallDeep;
  progGrp->addButton (bWaveWaterfallDeep, WAVEWATERFALL_DEEP);
  toolBar[6]->addWidget (bWaveWaterfallDeep);

  bChangeLidarClass = new QToolButton (this);
  misc.button[CHANGE_LIDAR_CLASS_KEY] = bChangeLidarClass;
  bChangeLidarClass->setIcon (misc.buttonIcon[CHANGE_LIDAR_CLASS_KEY]);
  modeGrp->addButton (bChangeLidarClass, CHANGE_LIDAR_CLASS);
  bChangeLidarClass->setToolTip (tr ("Change LiDAR classification mode")); 
  bChangeLidarClass->setWhatsThis (changeLidarClassText);
  bChangeLidarClass->setCheckable (true);
  toolBar[6]->addWidget (bChangeLidarClass);



  //  Create GSF toolbar

  toolBar[7] = new QToolBar (tr ("GSF tool bar"));
  toolBar[7]->setToolTip (tr ("GSF tool bar"));
  addToolBar (toolBar[7]);
  toolBar[7]->setObjectName (tr ("GSF tool bar"));

  bGSFMonitor = new QToolButton (this);
  bGSFMonitor->setIcon (QIcon (":/icons/gsf_monitor.png"));
  bGSFMonitor->setWhatsThis (gsfMonitorText);
  bGSFMonitor->setCheckable (true);
  tip =  options.description[GSFMONITOR] + "  [" + options.hotkey[GSFMONITOR] + "]";
  bGSFMonitor->setToolTip (tip);
  bGSFMonitor->setShortcut (options.hotkey[GSFMONITOR]);
  options.progButton[GSFMONITOR] = bGSFMonitor;
  progGrp->addButton (bGSFMonitor, GSFMONITOR);
  toolBar[7]->addWidget (bGSFMonitor);


  //  Create CZMIL toolbar

  toolBar[8] = new QToolBar (tr ("CZMIL tool bar"));
  toolBar[8]->setToolTip (tr ("CZMIL tool bar"));
  addToolBar (toolBar[8]);
  toolBar[8]->setObjectName (tr ("CZMIL tool bar"));


  bCZMILwaveMonitor = new QToolButton (this);
  bCZMILwaveMonitor->setIcon (QIcon (":/icons/wave_monitor.png"));
  bCZMILwaveMonitor->setWhatsThis (CZMILwaveMonitorText);
  bCZMILwaveMonitor->setCheckable (true);
  tip =  options.description[CZMILWAVEMONITOR] + "  [" + options.hotkey[CZMILWAVEMONITOR] + "]";
  bCZMILwaveMonitor->setToolTip (tip);
  bCZMILwaveMonitor->setShortcut (QKeySequence (options.hotkey[CZMILWAVEMONITOR]));
  options.progButton[CZMILWAVEMONITOR] = bCZMILwaveMonitor;
  progGrp->addButton (bCZMILwaveMonitor, CZMILWAVEMONITOR);
  toolBar[8]->addWidget (bCZMILwaveMonitor);

  toolBar[8]->addSeparator ();
  toolBar[8]->addSeparator ();

  czmilReprocessButtons = new czmilReprocessButtonBox (this);
  czmilReprocessButtons->setIcon (QIcon (":/icons/czmil_reprocess_buttons_11.png"));
  czmilReprocessButtons->setToolTip (tr ("Select type of CZMIL points to reprocess")); 
  czmilReprocessButtons->setWhatsThis (czmilReprocessButtonsText);
  czmilReprocessButtons->setCheckable (false);
  connect (czmilReprocessButtons, SIGNAL (mousePressSignal (QMouseEvent *)), this, SLOT (slotCzmilReprocessButtonsPressed (QMouseEvent *)));
  toolBar[8]->addWidget (czmilReprocessButtons);

  bCZMILReprocessLand = new QToolButton (this);
  misc.button[CZMIL_REPROCESS_LAND_KEY] = bCZMILReprocessLand;
  bCZMILReprocessLand->setIcon (misc.buttonIcon[CZMIL_REPROCESS_LAND_KEY]);
  bCZMILReprocessLand->setToolTip (tr ("Reprocess CZMIL points in polygon mode (land)")); 
  bCZMILReprocessLand->setWhatsThis (czmilReprocessLandText);
  bCZMILReprocessLand->setCheckable (true);
  modeGrp->addButton (bCZMILReprocessLand, CZMIL_REPROCESS_LAND);
  toolBar[8]->addWidget (bCZMILReprocessLand);

  bCZMILReprocessWater = new QToolButton (this);
  misc.button[CZMIL_REPROCESS_WATER_KEY] = bCZMILReprocessWater;
  bCZMILReprocessWater->setIcon (misc.buttonIcon[CZMIL_REPROCESS_WATER_KEY]);
  bCZMILReprocessWater->setToolTip (tr ("Reprocess CZMIL points in polygon mode (water)")); 
  bCZMILReprocessWater->setWhatsThis (czmilReprocessWaterText);
  bCZMILReprocessWater->setCheckable (true);
  modeGrp->addButton (bCZMILReprocessWater, CZMIL_REPROCESS_WATER);
  toolBar[8]->addWidget (bCZMILReprocessWater);

  bCZMILReprocessShallowWater = new QToolButton (this);
  misc.button[CZMIL_REPROCESS_SHALLOW_WATER_KEY] = bCZMILReprocessShallowWater;
  bCZMILReprocessShallowWater->setIcon (misc.buttonIcon[CZMIL_REPROCESS_SHALLOW_WATER_KEY]);
  bCZMILReprocessShallowWater->setToolTip (tr ("Reprocess CZMIL points in polygon mode (shallow water)")); 
  bCZMILReprocessShallowWater->setWhatsThis (czmilReprocessShallowWaterText);
  bCZMILReprocessShallowWater->setCheckable (true);
  modeGrp->addButton (bCZMILReprocessShallowWater, CZMIL_REPROCESS_SHALLOW_WATER);
  toolBar[8]->addWidget (bCZMILReprocessShallowWater);

  bCZMILUrbanFilter = new QToolButton (this);
  misc.button[CZMIL_URBAN_FILTER_KEY] = bCZMILUrbanFilter;
  bCZMILUrbanFilter->setIcon (misc.buttonIcon[CZMIL_URBAN_FILTER_KEY]);
  bCZMILUrbanFilter->setToolTip (tr ("Run the CZMIL urban noise filter on the displayed data"));
  bCZMILUrbanFilter->setWhatsThis (CZMILUrbanFilterText);
  connect (bCZMILUrbanFilter, SIGNAL (clicked ()), this, SLOT (slotCZMILUrbanFilter ()));
  toolBar[8]->addWidget (bCZMILUrbanFilter);

  toolBar[8]->addSeparator ();
  toolBar[8]->addSeparator ();


  //  bCPFFilter is not added to progGrp because it is handled differently than the other programs in pfmEdit3D.
  //  It *IS* added to modeGrp because it sets a cursor mode (CPF_FILTER) that requires a polygon.

  /*  Kept for reference...

  bCPFFilter = new QToolButton (this);
  bCPFFilter->setIcon (QIcon (":/icons/cpf_filter.png"));
  bCPFFilter->setWhatsThis (cpfFilterText);
  bCPFFilter->setCheckable (true);
  tip = options.description[CPF_DEEP_FILTER] + "  [" + options.hotkey[CPF_DEEP_FILTER] + "]";
  bCPFFilter->setToolTip (tip);
  bCPFFilter->setShortcut (QKeySequence (options.hotkey[CPF_DEEP_FILTER]));
  options.progButton[CPF_DEEP_FILTER] = bCPFFilter;
  modeGrp->addButton (bCPFFilter, CPF_FILTER);
  toolBar[8]->addWidget (bCPFFilter);

  */


  //  Create CHARTS toolbar

  toolBar[9] = new QToolBar (tr ("CHARTS tool bar"));
  toolBar[9]->setToolTip (tr ("CHARTS tool bar"));
  addToolBar (toolBar[9]);
  toolBar[9]->setObjectName (tr ("CHARTS tool bar"));


  //  bHOFWaveFilter is not added to progGrp because it is handled differently than the other programs in pfmEdit3D.

  bHOFWaveFilter = new QToolButton (this);
  bHOFWaveFilter->setIcon (QIcon (":/icons/hof_filter.png"));
  bHOFWaveFilter->setWhatsThis (HOFWaveFilterText);
  bHOFWaveFilter->setCheckable (false);
  tip = options.description[HOFWAVEFILTER] + "  [" + options.hotkey[HOFWAVEFILTER] + "]";
  bHOFWaveFilter->setToolTip (tip);
  bHOFWaveFilter->setShortcut (QKeySequence (options.hotkey[HOFWAVEFILTER]));
  options.progButton[HOFWAVEFILTER] = bHOFWaveFilter;
  connect (bHOFWaveFilter, SIGNAL (clicked ()), this, SLOT (slotHOFWaveFilter ()));
  toolBar[9]->addWidget (bHOFWaveFilter);


  //  Create LAS toolbar

  toolBar[10] = new QToolBar (tr ("LAS tool bar"));
  toolBar[10]->setToolTip (tr ("LAS tool bar"));
  addToolBar (toolBar[10]);
  toolBar[10]->setObjectName (tr ("LAS tool bar"));


  bLASwaveMonitor = new QToolButton (this);
  bLASwaveMonitor->setIcon (QIcon (":/icons/LASwaveMonitor.png"));
  bLASwaveMonitor->setWhatsThis (LASwaveMonitorText);
  bLASwaveMonitor->setCheckable (true);
  tip =  options.description[LASWAVEMONITOR] + "  [" + options.hotkey[LASWAVEMONITOR] + "]";
  bLASwaveMonitor->setToolTip (tip);
  bLASwaveMonitor->setShortcut (QKeySequence (options.hotkey[LASWAVEMONITOR]));
  options.progButton[LASWAVEMONITOR] = bLASwaveMonitor;
  progGrp->addButton (bLASwaveMonitor, LASWAVEMONITOR);
  toolBar[10]->addWidget (bLASwaveMonitor);


  bHide = new QToolButton (this);
  bHide->setToolTip (tr ("Hide data based on flags or types"));
  bHide->setWhatsThis (hideText);
  bHide->setIcon (QIcon (":/icons/hide_user_flag.png"));
  connect (bHide, SIGNAL (clicked ()), this, SLOT (slotHideMenuShow ()));
  toolBar[5]->addWidget (bHide);


  bFlagReset = new QToolButton (this);
  bFlagReset->setIcon (QIcon (":/icons/clear_mask_flag.png"));
  bFlagReset->setToolTip (tr ("Clear hide/show flag settings"));
  bFlagReset->setWhatsThis (flagResetText);
  connect (bFlagReset, SIGNAL (clicked ()), this, SLOT (slotFlagReset ()));
  toolBar[5]->addWidget (bFlagReset);


  bMaskReset = new QToolButton (this);
  bMaskReset->setIcon (QIcon (":/icons/clear_mask_type.png"));
  bMaskReset->setToolTip (tr ("Clear hide/show type settings"));
  bMaskReset->setWhatsThis (maskResetText);
  connect (bMaskReset, SIGNAL (clicked ()), this, SLOT (slotMaskReset ()));
  toolBar[5]->addWidget (bMaskReset);


  //  Set the icon sizes so that the tool bars will set up properly.

  prev_icon_size = options.main_button_icon_size;

  QSize mainButtonIconSize (options.main_button_icon_size, options.main_button_icon_size);

  for (int32_t i = 0 ; i < NUM_TOOLBARS ; i++) toolBar[i]->setIconSize (mainButtonIconSize);


  //  Get the user's defaults if available

  if (!envin (&options, &misc, this))
    {
      //  Set the geometry from defaults since we didn't get any from the saved settings.

      this->resize (misc.draw_area_width, misc.draw_area_height);
      this->move (0, 0);
    }


  // Set the application font

  QApplication::setFont (options.font);


  //  Save the hofWaveFilter options to shared memory.

  misc.abe_share->filterShare.search_radius = options.hofWaveFilter_search_radius;
  misc.abe_share->filterShare.search_width = options.hofWaveFilter_search_width;
  misc.abe_share->filterShare.rise_threshold = options.hofWaveFilter_rise_threshold;
  misc.abe_share->filterShare.pmt_ac_zero_offset_required = options.hofWaveFilter_pmt_ac_zero_offset_required;
  misc.abe_share->filterShare.apd_ac_zero_offset_required = options.hofWaveFilter_apd_ac_zero_offset_required;


  //  Allocate the colors (these are default for color by depth).

  options.min_hsv_color[0] = misc.abe_share->min_hsv_color;
  options.max_hsv_color[0] = misc.abe_share->max_hsv_color;
  options.min_hsv_value[0] = misc.abe_share->min_hsv_value;
  options.max_hsv_value[0] = misc.abe_share->max_hsv_value;
  options.min_hsv_locked[0] = misc.abe_share->min_hsv_locked;
  options.max_hsv_locked[0] = misc.abe_share->max_hsv_locked;


  float hue_inc = (float) (options.max_hsv_color[0] - options.min_hsv_color[0]) / (float) (NUMSHADES + 1);

  for (int32_t j = 0 ; j < NUMSHADES ; j++)
    {
      misc.track_color_array[j].setHsv ((int32_t) (options.min_hsv_color[0] + j * hue_inc), 255, 255, 255); 

      misc.color_array[j][0] = misc.track_color_array[j].red ();
      misc.color_array[j][1] = misc.track_color_array[j].green ();
      misc.color_array[j][2] = misc.track_color_array[j].blue ();
      misc.color_array[j][3] = misc.track_color_array[j].alpha ();
    }

  options.ref_color[1] = options.ref_color[0];
  options.ref_color[1].setAlpha (options.slice_alpha);


  //  Allocate the colors to be used for color by line.  These are full value and partial value colors.

  hue_inc = (float) (options.max_hsv_color[0] - options.min_hsv_color[0]) / (float) (NUMSHADES + 1);

  for (int32_t j = 0 ; j < NUMSHADES ; j++)
    {
      QColor color;


      //  Every other color is partial value (darker).

      if (j % 2)
        {
          color.setHsv ((int32_t) (options.min_hsv_color[0] + j * hue_inc), 255, 255, 255);
        }
      else
        {
          color.setHsv ((int32_t) (options.min_hsv_color[0] + j * hue_inc), 255, 164, 255);
        }

      misc.line_color_array[j][0] = color.red ();
      misc.line_color_array[j][1] = color.green ();
      misc.line_color_array[j][2] = color.blue ();
      misc.line_color_array[j][3] = 255;
    }


  //  Allocate the undo blocks based on the max number of undo levels (user option).

  try
    {
      misc.undo.resize (options.undo_levels);
    }
  catch (std::bad_alloc&)
    {
      fprintf (stderr, "%s %s %s %d - undo - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  memset (misc.undo.data (), 0, options.undo_levels * sizeof (UNDO));


  //  Use geometry to get the width and height.

  QRect tmp = this->geometry ();
  int32_t width = tmp.width ();
  int32_t height = tmp.height ();


  hotkey_message = "<ul>";

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    hotkey_message += "<li>" + options.hotkey[i] + " = " + options.prog[i] + tr (";  action keys = ") + options.action[i] + "</li>";

  hotkey_message += "</ul><br>" +
    tr ("For more information on data types and other restrictions on the use of hot keys see <b>Ancillary Programs</b> in the "
        "<b>Preferences</b> dialog - <img source=\":/icons/prefs.png\">");


  misc.save_mode = options.function;
  misc.save_function = options.function;


  //  Set the map values from the defaults

  mapdef.draw_width = width;
  mapdef.draw_height = height;
  mapdef.exaggeration = options.exaggeration;

  mapdef.min_z_extents = options.min_window_size;
  mapdef.mode = NVMAPGL_POINT_MODE;
  mapdef.auto_scale = NVTrue;
  mapdef.projected = 0;

  mapdef.coasts = NVFalse;
  mapdef.landmask = NVFalse;
  mapdef.light_model = GL_LIGHT_MODEL_AMBIENT;

  mapdef.coast_color = Qt::white;
  mapdef.background_color = options.background_color;
  mapdef.scale_color = options.scale_color;
  mapdef.zoom_percent = options.zoom_percent;
  mapdef.draw_scale = options.draw_scale;


  //  If pfm3D is up we need to tell it to clear it's memory until we are finished with the 3D editor.  If we have a huge amount of
  //  memory in use it can slow things down too much.  The other end of this (i.e. setting PFMEDIT3D_CLOSED) happens in pfmView.

  misc.abeShare->lock ();
  {
    misc.abe_share->key = PFMEDIT3D_OPENED;
  }
  misc.abeShare->unlock ();


  //  Save the original bounds for use if we move the area from within pfmEdit3D.

  orig_bounds = misc.abe_share->edit_area;


  //  Read in the data so that we can set the bounds correctly.

  get_buffer (&misc, &options, NVFalse);


  //  Set number of points loaded in the window title

  QString title = tr ("%1 - Number of points - %2").arg (misc.program_version).arg (misc.abe_share->point_cloud_count);
  this->setWindowTitle (title);


  mapdef.initial_bounds.min_x = misc.displayed_area.min_x;
  mapdef.initial_bounds.min_y = misc.displayed_area.min_y;
  mapdef.initial_bounds.max_x = misc.displayed_area.max_x;
  mapdef.initial_bounds.max_y = misc.displayed_area.max_y;
  mapdef.initial_bounds.min_z = 999999999.0;
  mapdef.initial_bounds.max_z = -999999999.0;


  //  Compute the initial Z bounds.

  for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
    {
      if (!check_bounds (&options, &misc, i, NVTrue, NVFalse))
        {
          mapdef.initial_bounds.min_z = qMin (mapdef.initial_bounds.min_z, (double) -misc.data[i].z);
          mapdef.initial_bounds.max_z = qMax (mapdef.initial_bounds.max_z, (double) -misc.data[i].z);
        }
    }

  mapdef.initial_y_rotation = options.y_rotation;
  mapdef.initial_zx_rotation = options.zx_rotation;


  //  Save the displayed area for other ABE programs

  misc.abe_share->displayed_area = misc.displayed_area;


  QFrame *frame = new QFrame (this, 0);

  setCentralWidget (frame);


  //  Make the map.

  map = new nvMapGL (this, &mapdef, "pfmEdit3D");
  map->setWhatsThis (mapText);

  map->setStyleSheet ("QToolTip {border: 2px solid white; background: black; color: white; opacity: 200;}");


  misc.map_widget = map;


  //  Set the scale factor and offsets for the box scale numbers.

  map->setScaleFactor (options.z_factor);
  map->setScaleOffset (options.z_offset);
  map->setScaleOffsetColor (options.scaled_offset_z_color);


  //  Set the Z value display orientation.

  if (options.z_orientation > 0.0)
    {
      map->setScaleOrientation (true);
    }
  else
    {
      map->setScaleOrientation (false);
    }


  //  Connect to the signals from the map class.

  connect (map, SIGNAL (mousePressSignal (QMouseEvent *, double, double, double)), this, SLOT (slotMousePress (QMouseEvent *, double, double, double)));
  connect (map, SIGNAL (mouseDoubleClickSignal (QMouseEvent *, double, double, double)), this,
           SLOT (slotMouseDoubleClick (QMouseEvent *, double, double, double)));
  connect (map, SIGNAL (mouseReleaseSignal (QMouseEvent *, double, double, double)), this, SLOT (slotMouseRelease (QMouseEvent *, double, double, double)));
  connect (map, SIGNAL (mouseMoveSignal (QMouseEvent *, double, double, double, NVMAPGL_DEF)), this, 
           SLOT (slotMouseMove (QMouseEvent *, double, double, double, NVMAPGL_DEF)));
  connect (map, SIGNAL (wheelSignal (QWheelEvent *, double, double, double)), this, SLOT (slotWheel (QWheelEvent *, double, double, double)));
  connect (map, SIGNAL (resizeSignal (QResizeEvent *)), this, SLOT (slotResize (QResizeEvent *)));
  connect (map, SIGNAL (exaggerationChanged (float, float)), this, SLOT (slotExaggerationChanged (float, float)));


  //  Layouts, what fun!

  QVBoxLayout *vBox = new QVBoxLayout (frame);


  //  Set up the VBox for the 2D tracker box and the status information.

  QGroupBox *leftBox = new QGroupBox ();
  QVBoxLayout *leftBoxLayout = new QVBoxLayout ();
  leftBox->setLayout (leftBoxLayout);


  int32_t trackBoxWidth = 400, trackBoxHeight = 250;


  //  Set up the 2D tracker box.

  track_mapdef.projection = CYLINDRICAL_EQUIDISTANT;
  track_mapdef.draw_width = 100;
  track_mapdef.draw_height = 100;
  track_mapdef.overlap_percent = 0;
  track_mapdef.grid_inc_x = 0.0;
  track_mapdef.grid_inc_y = 0.0;

  track_mapdef.coasts = NVFalse;
  track_mapdef.landmask = NVFalse;

  track_mapdef.border = 0;
  track_mapdef.coast_color = Qt::white;
  track_mapdef.grid_color = Qt::white;
  track_mapdef.background_color = Qt::black;

  track_mapdef.initial_bounds.min_x = misc.abe_share->edit_area.min_x;
  track_mapdef.initial_bounds.min_y = misc.abe_share->edit_area.min_y;
  track_mapdef.initial_bounds.max_x = misc.abe_share->edit_area.max_x;
  track_mapdef.initial_bounds.max_y = misc.abe_share->edit_area.max_y;


  trackMap = new nvMap (this, &track_mapdef);
  trackMap->setMinimumSize (trackBoxWidth - 20, trackBoxHeight - 20);
  trackMap->setMaximumSize (trackBoxWidth - 20, trackBoxHeight - 20);


  trackMap->setWhatsThis (trackMapText);


  //  Connect to the signals from the map class.

  connect (trackMap, SIGNAL (mousePressSignal (QMouseEvent *, double, double)), this, SLOT (slotTrackMousePress (QMouseEvent *, double, double)));
  connect (trackMap, SIGNAL (mouseReleaseSignal (QMouseEvent *, double, double)), this, SLOT (slotTrackMouseRelease (QMouseEvent *, double, double)));
  connect (trackMap, SIGNAL (mouseMoveSignal (QMouseEvent *, double, double)), this, SLOT (slotTrackMouseMove (QMouseEvent *, double, double)));
  connect (trackMap, SIGNAL (resizeSignal (QResizeEvent *)), this, SLOT (slotTrackResize (QResizeEvent *)));
  connect (trackMap, SIGNAL (preRedrawSignal (NVMAP_DEF)), this, SLOT (slotTrackPreRedraw (NVMAP_DEF)));
  connect (trackMap, SIGNAL (postRedrawSignal (NVMAP_DEF)), this, SLOT (slotTrackPostRedraw (NVMAP_DEF)));


  notebook = new QTabWidget ();
  notebook->setTabPosition (QTabWidget::North);
  notebook->setMinimumSize (trackBoxWidth, trackBoxHeight);
  notebook->setMaximumSize (trackBoxWidth, trackBoxHeight);


  notebook->addTab (trackMap, tr ("2D Tracker"));


  //  Set up the histogram box.

  histogram_mapdef.projection = NO_PROJECTION;
  histogram_mapdef.draw_width = trackBoxWidth;
  histogram_mapdef.draw_height = trackBoxHeight;
  histogram_mapdef.overlap_percent = 0;
  histogram_mapdef.grid_inc_x = 0.0;
  histogram_mapdef.grid_inc_y = 0.0;

  histogram_mapdef.coasts = NVFalse;
  histogram_mapdef.landmask = NVFalse;

  histogram_mapdef.border = 0;
  histogram_mapdef.coast_color = Qt::white;
  histogram_mapdef.grid_color = QColor (160, 160, 160, 127);
  histogram_mapdef.background_color = Qt::black;


  histogram_mapdef.initial_bounds.min_x = 0;
  histogram_mapdef.initial_bounds.min_y = 0;
  histogram_mapdef.initial_bounds.max_x = trackBoxWidth;
  histogram_mapdef.initial_bounds.max_y = trackBoxHeight;


  //  Make the histogram.

  histogram = new nvMap (this, &histogram_mapdef);
  histogram->setWhatsThis (histText);


  //  Connect to the signals from the map class.

  connect (histogram, SIGNAL (resizeSignal (QResizeEvent *)), this, SLOT (slotHistogramResize (QResizeEvent *)));
  connect (histogram, SIGNAL (mouseMoveSignal (QMouseEvent *, double, double)), this, SLOT (slotHistogramMouseMove (QMouseEvent *, double, double)));
  connect (histogram, SIGNAL (preRedrawSignal (NVMAP_DEF)), this, SLOT (slotHistogramPreRedraw (NVMAP_DEF)));
  connect (histogram, SIGNAL (postRedrawSignal (NVMAP_DEF)), this, SLOT (slotHistogramPostRedraw (NVMAP_DEF)));


  notebook->addTab (histogram, tr ("Histogram"));


  //  The following sets up the area-based statistical filter delete/cancel notebook tab.

  QGroupBox *areabox = new QGroupBox (this);
  QVBoxLayout *areaboxLayout = new QVBoxLayout;
  areabox->setLayout (areaboxLayout);


  filterBox = new QGroupBox (tr ("000000 points selected"), this);
  filterBox->setToolTip (tr ("Please use the What's This help button then click here for full documentation"));
  filterBox->setWhatsThis (filterBoxText);
  QVBoxLayout *filterBoxLayout = new QVBoxLayout;
  filterBox->setLayout (filterBoxLayout);


  QHBoxLayout *sboxLayout = new QHBoxLayout;
  filterBoxLayout->addLayout (sboxLayout);

  QLabel *stdLabel = new QLabel (tr ("Standard Deviation"));
  stdLabel->setToolTip (tr ("Filter standard deviation value"));
  stdLabel->setWhatsThis (stdSliderText);
  sboxLayout->addWidget (stdLabel);

  stdSlider = new QSlider (Qt::Horizontal);
  stdSlider->setTracking (true);
  stdSlider->setInvertedControls (true);
  stdSlider->setInvertedAppearance (true);
  stdSlider->setRange (3, 40);
  stdSlider->setSingleStep (1);
  stdSlider->setPageStep (1);
  stdSlider->setValue (NINT (options.area_filter_std * 10.0));
  stdSlider->setToolTip (tr ("Filter standard deviation value"));
  stdSlider->setWhatsThis (stdSliderText);
  connect (stdSlider, SIGNAL (valueChanged (int)), this, SLOT (slotFilterStdChanged (int)));
  sboxLayout->addWidget (stdSlider, 1);


  stdValue = new QLineEdit ("2.4", this);
  stdValue->setAlignment (Qt::AlignCenter);
  QString std = QString ("%L1").arg (options.area_filter_std, 0, 'f', 1);
  stdValue->setText (std);
  stdValue->setToolTip (tr ("Filter standard deviation value"));
  stdValue->setWhatsThis (stdSliderText);
  connect (stdValue, SIGNAL (returnPressed ()), this, SLOT (slotFilterStdReturn ()));
  sboxLayout->addWidget (stdValue);


  areaboxLayout->addWidget (filterBox);


  QHBoxLayout *rangeLayout = new QHBoxLayout;
  areaboxLayout->addLayout (rangeLayout);

  rangeCheck = new QCheckBox (tr ("Limit Z"), this);
  rangeCheck->setChecked (options.filter_apply_z_range);
  rangeCheck->setToolTip (tr ("Check this box to limit the Z range of the data to be filtered"));
  rangeCheck->setWhatsThis (rangeCheckText);
  connect (rangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  rangeLayout->addWidget (rangeCheck);

  minZLabel = new QLabel (tr ("Min Z"));
  minZLabel->setToolTip (tr ("Minimum Z value for filter range"));
  minZLabel->setWhatsThis (minZValueText);
  rangeLayout->addWidget (minZLabel);

  minZValue = new QDoubleSpinBox (this);
  minZValue->setRange (-12000.0, 12000.0);
  minZValue->setSingleStep (1.0);
  minZValue->setValue (options.filter_min_z);
  minZValue->setToolTip (tr ("Minimum Z value for filter range"));
  minZValue->setWhatsThis (minZValueText);
  connect (minZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMinZValueChanged (double)));
  rangeLayout->addWidget (minZValue);


  maxZLabel = new QLabel (tr ("Max Z"));
  maxZLabel->setToolTip (tr ("Maximum Z value for filter range"));
  maxZLabel->setWhatsThis (maxZValueText);
  rangeLayout->addWidget (maxZLabel);

  maxZValue = new QDoubleSpinBox (this);
  maxZValue->setRange (-12000.0, 12000.0);
  maxZValue->setSingleStep (1.0);
  maxZValue->setValue (options.filter_max_z);
  maxZValue->setToolTip (tr ("Maximum Z value for filter range"));
  maxZValue->setWhatsThis (maxZValueText);
  connect (maxZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMaxZValueChanged (double)));
  rangeLayout->addWidget (maxZValue);


  QGroupBox *arbox = new QGroupBox (this);
  arbox->setFlat (true);
  QHBoxLayout *arboxLayout = new QHBoxLayout;
  arbox->setLayout (arboxLayout);


  QPushButton *filterApply = new QPushButton (tr ("Run"), this);
  filterApply->setToolTip (tr ("Run the statistical filter using the filter parameter values"));
  filterApply->setWhatsThis (filterApplyText);
  connect (filterApply, SIGNAL (clicked ()), this, SLOT (slotFilterApply ()));
  arboxLayout->addWidget (filterApply);


  filterDelete = new QPushButton (tr ("Delete"), this);
  filterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  filterDelete->setWhatsThis (tr ("Delete marked points"));
  connect (filterDelete, SIGNAL (clicked ()), this, SLOT (slotFilterDelete ()));
  arboxLayout->addWidget (filterDelete);


  filterCancel = new QPushButton (tr ("Cancel"), this);
  filterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  filterCancel->setWhatsThis (tr ("Discard filter results"));
  connect (filterCancel, SIGNAL (clicked ()), this, SLOT (slotFilterCancel ()));
  arboxLayout->addWidget (filterCancel);


  areaboxLayout->addWidget (arbox);


  notebook->addTab (areabox, tr ("Area-based Filter"));
  notebook->setTabEnabled (AREATAB, false);


  //  The following sets up the AVT statistical filter delete/cancel notebook tab.

  QGroupBox *avtbox = new QGroupBox (this);
  QVBoxLayout *avtboxLayout = new QVBoxLayout;
  avtbox->setLayout (avtboxLayout);


  statFilterBox = new QGroupBox (tr ("000000 points selected"), this);
  statFilterBox->setToolTip (tr ("Please use the What's This help button then click here for full documentation"));
  statFilterBox->setWhatsThis (avtFilterBoxText);
  QVBoxLayout *statFilterBoxLayout = new QVBoxLayout;
  statFilterBox->setLayout (statFilterBoxLayout);


  QHBoxLayout *avtstdboxLayout = new QHBoxLayout;
  statFilterBoxLayout->addLayout (avtstdboxLayout);

  QLabel *avtLabel = new QLabel (tr ("Standard Deviation"));
  avtLabel->setToolTip (tr ("Filter standard deviation value"));
  avtLabel->setWhatsThis (stdSliderText);
  avtstdboxLayout->addWidget (avtLabel);

  avtSlider = new QSlider (Qt::Horizontal);
  avtSlider->setTracking (true);
  avtSlider->setInvertedControls (true);
  avtSlider->setInvertedAppearance (true);
  avtSlider->setRange (3, 40);
  avtSlider->setSingleStep (1);
  avtSlider->setPageStep (1);
  avtSlider->setValue (NINT (options.stat_filter_std * 10.0));
  avtSlider->setToolTip (tr ("Filter standard deviation value"));
  avtSlider->setWhatsThis (stdSliderText);
  connect (avtSlider, SIGNAL (valueChanged (int)), this, SLOT (slotAvtFilterStdChanged (int)));
  avtstdboxLayout->addWidget (avtSlider, 1);


  avtValue = new QLineEdit ("2.4", this);
  avtValue->setAlignment (Qt::AlignCenter);
  std = QString ("%L1").arg (options.stat_filter_std, 0, 'f', 1);
  avtValue->setText (std);
  avtValue->setToolTip (tr ("Filter standard deviation value"));
  avtValue->setWhatsThis (stdSliderText);
  connect (avtValue, SIGNAL (returnPressed ()), this, SLOT (slotAvtFilterStdReturn ()));
  avtstdboxLayout->addWidget (avtValue);


  QHBoxLayout *radLayout = new QHBoxLayout;
  statFilterBoxLayout->addLayout (radLayout);

  radiusLabel = new QLabel (tr ("Radius"));
  radiusLabel->setToolTip (tr ("Radius in meters for computing standard deviation"));
  radiusLabel->setWhatsThis (radiusValueText);
  radLayout->addWidget (radiusLabel);

  radiusValue = new QDoubleSpinBox (this);
  radiusValue->setRange (1.0, 500.0);
  radiusValue->setSingleStep (1.0);
  radiusValue->setValue (options.stat_filter_radius);
  radiusValue->setToolTip (tr ("Radius in meters for computing standard deviation"));
  radiusValue->setWhatsThis (radiusValueText);
  connect (radiusValue, SIGNAL (valueChanged (double)), this, SLOT (slotRadValueChanged (double)));
  radLayout->addWidget (radiusValue);


  countLabel = new QLabel (tr ("Count"));
  countLabel->setToolTip (tr ("Minimum number of points in circle for standard deviation calculation"));
  countLabel->setWhatsThis (countValueText);
  radLayout->addWidget (countLabel);

  countValue = new QSpinBox (this);
  countValue->setRange (5, 500);
  countValue->setSingleStep (1);
  countValue->setValue (options.stat_filter_count);
  countValue->setToolTip (tr ("Minimum number of points in circle for standard deviation calculation"));
  countValue->setWhatsThis (countValueText);
  connect (countValue, SIGNAL (valueChanged (int)), this, SLOT (slotCountValueChanged (int)));
  radLayout->addWidget (countValue);


  avtPassLabel = new QLabel (tr ("Passes"));
  avtPassLabel->setToolTip (tr ("Number of filter passes to make through the data (0 = run until clean)"));
  avtPassLabel->setWhatsThis (passValueText);
  radLayout->addWidget (avtPassLabel);

  avtPassValue = new QSpinBox (this);
  avtPassValue->setRange (0, 10);
  avtPassValue->setSingleStep (1);
  avtPassValue->setValue (options.stat_filter_passes);
  avtPassValue->setToolTip (tr ("Number of filter passes to make through the data (0 = run until clean)"));
  avtPassValue->setWhatsThis (passValueText);
  connect (avtPassValue, SIGNAL (valueChanged (int)), this, SLOT (slotAvtPassValueChanged (int)));
  radLayout->addWidget (avtPassValue);


  avtboxLayout->addWidget (statFilterBox);


  QHBoxLayout *avtRangeLayout = new QHBoxLayout;
  avtboxLayout->addLayout (avtRangeLayout);

  avtRangeCheck = new QCheckBox (tr ("Limit Z"), this);
  avtRangeCheck->setChecked (options.filter_apply_z_range);
  avtRangeCheck->setToolTip (tr ("Check this box to limit the Z range of the data to be filtered"));
  avtRangeCheck->setWhatsThis (rangeCheckText);
  connect (avtRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  avtRangeLayout->addWidget (avtRangeCheck);

  avtMinZLabel = new QLabel (tr ("Min Z"));
  avtMinZLabel->setToolTip (tr ("Minimum Z value for filter range"));
  avtMinZLabel->setWhatsThis (minZValueText);
  avtRangeLayout->addWidget (minZLabel);

  avtMinZValue = new QDoubleSpinBox (this);
  avtMinZValue->setRange (-12000.0, 12000.0);
  avtMinZValue->setSingleStep (1.0);
  avtMinZValue->setValue (options.filter_min_z);
  avtMinZValue->setToolTip (tr ("Minimum Z value for filter range"));
  avtMinZValue->setWhatsThis (minZValueText);
  connect (avtMinZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMinZValueChanged (double)));
  avtRangeLayout->addWidget (avtMinZValue);


  avtMaxZLabel = new QLabel (tr ("Max Z"));
  avtMaxZLabel->setToolTip (tr ("Maximum Z value for filter range"));
  avtMaxZLabel->setWhatsThis (maxZValueText);
  avtRangeLayout->addWidget (avtMaxZLabel);

  avtMaxZValue = new QDoubleSpinBox (this);
  avtMaxZValue->setRange (-12000.0, 12000.0);
  avtMaxZValue->setSingleStep (1.0);
  avtMaxZValue->setValue (options.filter_max_z);
  avtMaxZValue->setToolTip (tr ("Maximum Z value for filter range"));
  avtMaxZValue->setWhatsThis (maxZValueText);
  connect (avtMaxZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMaxZValueChanged (double)));
  avtRangeLayout->addWidget (avtMaxZValue);


  QGroupBox *sarbox = new QGroupBox (this);
  sarbox->setFlat (true);
  QHBoxLayout *sarboxLayout = new QHBoxLayout;
  sarbox->setLayout (sarboxLayout);


  QPushButton *avtFilterApply = new QPushButton (tr ("Run"), this);
  avtFilterApply->setToolTip (tr ("Run the statistical filter using the filter parameter values"));
  avtFilterApply->setWhatsThis (filterApplyText);
  connect (avtFilterApply, SIGNAL (clicked ()), this, SLOT (slotFilterApply ()));
  sarboxLayout->addWidget (avtFilterApply);


  avtFilterDelete = new QPushButton (tr ("Delete"), this);
  avtFilterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  avtFilterDelete->setWhatsThis (tr ("Delete marked points"));
  connect (avtFilterDelete, SIGNAL (clicked ()), this, SLOT (slotFilterDelete ()));
  sarboxLayout->addWidget (avtFilterDelete);


  avtFilterCancel = new QPushButton (tr ("Cancel"), this);
  avtFilterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  avtFilterCancel->setWhatsThis (tr ("Discard filter results"));
  connect (avtFilterCancel, SIGNAL (clicked ()), this, SLOT (slotFilterCancel ()));
  sarboxLayout->addWidget (avtFilterCancel);


  avtboxLayout->addWidget (sarbox);


  notebook->addTab (avtbox, tr ("Statistical Filter"));
  notebook->setTabEnabled (STATTAB, false);


  //  The following sets up the Hockey Puck filter delete/cancel notebook tab.

  QGroupBox *hpbox = new QGroupBox (this);
  QVBoxLayout *hpboxLayout = new QVBoxLayout;
  hpbox->setLayout (hpboxLayout);


  hpFilterBox = new QGroupBox (tr ("000000 points selected"), this);
  hpFilterBox->setWhatsThis (hpFilterBoxText);
  QVBoxLayout *hpFilterBoxLayout = new QVBoxLayout;
  hpFilterBox->setLayout (hpFilterBoxLayout);

  QVBoxLayout *hpvLayout = new QVBoxLayout;
  hpFilterBoxLayout->addLayout (hpvLayout);
  QHBoxLayout *hpvTopLayout = new QHBoxLayout;
  hpvLayout->addLayout (hpvTopLayout);
  QHBoxLayout *hpvBottomLayout = new QHBoxLayout;
  hpvLayout->addLayout (hpvBottomLayout);


  QHBoxLayout *hpRadiusLayout = new QHBoxLayout;
  hpvTopLayout->addLayout (hpRadiusLayout);

  QLabel *hpRadiusLabel = new QLabel (tr ("Radius"));
  hpRadiusLabel->setToolTip (tr ("Hockey Puck filter radius value"));
  hpRadiusLabel->setWhatsThis (hpRadiusValueText);
  hpRadiusLayout->addWidget (hpRadiusLabel);


  hpRadiusValue = new QDoubleSpinBox (this);
  hpRadiusValue->setDecimals (2);
  hpRadiusValue->setRange (HP_MIN_RADIUS, HP_MAX_RADIUS);
  hpRadiusValue->setSingleStep (0.1);
  hpRadiusValue->setToolTip (tr ("Hockey Puck filter radius value"));
  hpRadiusValue->setWhatsThis (hpRadiusValueText);
  hpRadiusLayout->addWidget (hpRadiusValue, 1);


  QHBoxLayout *hpHeightLayout = new QHBoxLayout;
  hpvTopLayout->addLayout (hpHeightLayout);

  QLabel *hpHeightLabel = new QLabel (tr ("Height"));
  hpHeightLabel->setToolTip (tr ("Hockey Puck filter height value"));
  hpHeightLabel->setWhatsThis (hpHeightValueText);
  hpHeightLayout->addWidget (hpHeightLabel);

  hpHeightValue = new QDoubleSpinBox (this);
  hpHeightValue->setDecimals (2);
  hpHeightValue->setRange (HP_MIN_HEIGHT, HP_MAX_HEIGHT);
  hpHeightValue->setSingleStep (0.1);
  hpHeightValue->setToolTip (tr ("Hockey Puck filter height value"));
  hpHeightValue->setWhatsThis (hpHeightValueText);
  hpHeightLayout->addWidget (hpHeightValue, 1);


  QHBoxLayout *hpPassLayout = new QHBoxLayout;
  hpvTopLayout->addLayout (hpPassLayout);

  hpPassLabel = new QLabel (tr ("Passes"));
  hpPassLabel->setToolTip (tr ("Number of filter passes to make through the data (0 = run until clean)"));
  hpPassLabel->setWhatsThis (passValueText);
  hpPassLayout->addWidget (hpPassLabel);

  hpPassValue = new QSpinBox (this);
  hpPassValue->setRange (0, 10);
  hpPassValue->setSingleStep (1);
  hpPassValue->setValue (options.hp_filter_passes);
  hpPassValue->setToolTip (tr ("Number of filter passes to make through the data (0 = run until clean)"));
  hpPassValue->setWhatsThis (passValueText);
  connect (hpPassValue, SIGNAL (valueChanged (int)), this, SLOT (slotHpPassValueChanged (int)));
  hpPassLayout->addWidget (hpPassValue);


  QHBoxLayout *hpNeighborsLayout = new QHBoxLayout;
  hpvBottomLayout->addLayout (hpNeighborsLayout);

  QLabel *hpNeighborsLabel = new QLabel (tr ("Neighbors"));
  hpNeighborsLabel->setToolTip (tr ("Hockey Puck filter neighbors value"));
  hpNeighborsLabel->setWhatsThis (hpNeighborsValueText);
  hpNeighborsLayout->addWidget (hpNeighborsLabel);

  hpNeighborsValue = new QSpinBox (this);
  hpNeighborsValue->setRange (HP_MIN_NEIGHBORS, HP_MAX_NEIGHBORS);
  hpNeighborsValue->setSingleStep (1);
  hpNeighborsValue->setToolTip (tr ("Hockey Puck filter neighbors value"));
  hpNeighborsValue->setWhatsThis (hpNeighborsValueText);
  hpNeighborsLayout->addWidget (hpNeighborsValue, 1);


  QHBoxLayout *hpMultiNeighborsLayout = new QHBoxLayout;
  hpvBottomLayout->addLayout (hpMultiNeighborsLayout);

  QLabel *hpMultiNeighborsLabel = new QLabel (tr ("Multi-line Neighbors"));
  hpMultiNeighborsLabel->setToolTip (tr ("Hockey Puck filter multi-line neighbors value"));
  hpMultiNeighborsLabel->setWhatsThis (hpMultiNeighborsValueText);
  hpMultiNeighborsLayout->addWidget (hpMultiNeighborsLabel);

  hpMultiNeighborsValue = new QSpinBox (this);
  hpMultiNeighborsValue->setRange (HP_MIN_NEIGHBORS, HP_MAX_NEIGHBORS);
  hpMultiNeighborsValue->setSingleStep (1);
  hpMultiNeighborsValue->setToolTip (tr ("Hockey Puck filter multi-line neighbors value"));
  hpMultiNeighborsValue->setWhatsThis (hpMultiNeighborsValueText);
  hpMultiNeighborsLayout->addWidget (hpMultiNeighborsValue, 1);


  hpboxLayout->addWidget (hpFilterBox);


  QHBoxLayout *hpRangeLayout = new QHBoxLayout;
  hpboxLayout->addLayout (hpRangeLayout);

  hpRangeCheck = new QCheckBox (tr ("Limit Z"), this);
  hpRangeCheck->setChecked (options.filter_apply_z_range);
  hpRangeCheck->setToolTip (tr ("Check this box to limit the Z range of the data to be filtered"));
  hpRangeCheck->setWhatsThis (rangeCheckText);
  connect (hpRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  hpRangeLayout->addWidget (hpRangeCheck);


  hpMinZLabel = new QLabel (tr ("Min Z"));
  hpMinZLabel->setToolTip (tr ("Minimum Z value for filter range"));
  hpMinZLabel->setWhatsThis (minZValueText);
  hpRangeLayout->addWidget (hpMinZLabel);

  hpMinZValue = new QDoubleSpinBox (this);
  hpMinZValue->setRange (-12000.0, 12000.0);
  hpMinZValue->setSingleStep (1.0);
  hpMinZValue->setValue (options.filter_min_z);
  hpMinZValue->setToolTip (tr ("Minimum Z value for filter range"));
  hpMinZValue->setWhatsThis (minZValueText);
  connect (hpMinZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMinZValueChanged (double)));
  hpRangeLayout->addWidget (hpMinZValue);


  hpMaxZLabel = new QLabel (tr ("Max Z"));
  hpMaxZLabel->setToolTip (tr ("Maximum Z value for filter range"));
  hpMaxZLabel->setWhatsThis (maxZValueText);
  hpRangeLayout->addWidget (hpMaxZLabel);

  hpMaxZValue = new QDoubleSpinBox (this);
  hpMaxZValue->setRange (-12000.0, 12000.0);
  hpMaxZValue->setSingleStep (1.0);
  hpMaxZValue->setValue (options.filter_max_z);
  hpMaxZValue->setToolTip (tr ("Maximum Z value for filter range"));
  hpMaxZValue->setWhatsThis (maxZValueText);
  connect (hpMaxZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMaxZValueChanged (double)));
  hpRangeLayout->addWidget (hpMaxZValue);


  QGroupBox *hprbox = new QGroupBox (this);
  hprbox->setFlat (true);
  QHBoxLayout *hprboxLayout = new QHBoxLayout;
  hprbox->setLayout (hprboxLayout);

  QPushButton *hpFilterApply = new QPushButton (tr ("Run"), this);
  hpFilterApply->setToolTip (tr ("Run the filter using the filter parameter values"));
  hpFilterApply->setWhatsThis (hpFilterApplyText);
  connect (hpFilterApply, SIGNAL (clicked ()), this, SLOT (slotHpFilterApply ()));
  hprboxLayout->addWidget (hpFilterApply);


  hpFilterDelete = new QPushButton (tr ("Delete"), this);
  hpFilterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  hpFilterDelete->setWhatsThis (tr ("Delete marked points"));
  connect (hpFilterDelete, SIGNAL (clicked ()), this, SLOT (slotFilterDelete ()));
  hprboxLayout->addWidget (hpFilterDelete);


  hpFilterCancel = new QPushButton (tr ("Cancel"), this);
  hpFilterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  hpFilterCancel->setWhatsThis (tr ("Discard filter results"));
  connect (hpFilterCancel, SIGNAL (clicked ()), this, SLOT (slotFilterCancel ()));
  hprboxLayout->addWidget (hpFilterCancel);


  hpboxLayout->addWidget (hprbox);


  notebook->addTab (hpbox, tr ("Hockey Puck Filter"));
  notebook->setTabEnabled (HPTAB, false);


  //  The following sets up the CZMIL urban noise filter delete/cancel notebook tab.

  QGroupBox *czmilUrbanBox = new QGroupBox (this);
  QVBoxLayout *czmilUrbanBoxLayout = new QVBoxLayout;
  czmilUrbanBox->setLayout (czmilUrbanBoxLayout);


  cufFilterBox = new QGroupBox (tr ("000000 points selected"), this);
  cufFilterBox->setWhatsThis (cufFilterBoxText);
  QVBoxLayout *cufFilterBoxLayout = new QVBoxLayout;
  cufFilterBox->setLayout (cufFilterBoxLayout);

  QVBoxLayout *cufLayout = new QVBoxLayout;
  cufFilterBoxLayout->addLayout (cufLayout);
  QHBoxLayout *cufTopLayout = new QHBoxLayout;
  cufLayout->addLayout (cufTopLayout);
  QHBoxLayout *cufMidLayout = new QHBoxLayout;
  cufLayout->addLayout (cufMidLayout);
  QHBoxLayout *cufBotLayout = new QHBoxLayout;
  cufLayout->addLayout (cufBotLayout);


  QHBoxLayout *cufVRadiusLayout = new QHBoxLayout;
  cufTopLayout->addLayout (cufVRadiusLayout);

  QLabel *cufVRadiusLabel = new QLabel (tr ("Vicinity Radius"));
  cufVRadiusLabel->setToolTip (tr ("CZMIL urban vicinity radius value"));
  cufVRadiusLabel->setWhatsThis (cufVRadiusValueText);
  cufVRadiusLayout->addWidget (cufVRadiusLabel);

  cufVRadiusValue = new QDoubleSpinBox (this);
  cufVRadiusValue->setDecimals (2);
  cufVRadiusValue->setRange (HP_MIN_RADIUS, HP_MAX_RADIUS);
  cufVRadiusValue->setSingleStep (0.1);
  cufVRadiusValue->setToolTip (tr ("CZMIL urban vicinity radius value"));
  cufVRadiusValue->setWhatsThis (cufVRadiusValueText);
  cufVRadiusLayout->addWidget (cufVRadiusValue, 1);


  QHBoxLayout *cufVHeightLayout = new QHBoxLayout;
  cufTopLayout->addLayout (cufVHeightLayout);

  QLabel *cufVHeightLabel = new QLabel (tr ("Vicinity Height"));
  cufVHeightLabel->setToolTip (tr ("CZMIL urban vicinity height value"));
  cufVHeightLabel->setWhatsThis (cufVHeightValueText);
  cufVHeightLayout->addWidget (cufVHeightLabel);

  cufVHeightValue = new QDoubleSpinBox (this);
  cufVHeightValue->setDecimals (2);
  cufVHeightValue->setRange (HP_MIN_HEIGHT, HP_MAX_HEIGHT);
  cufVHeightValue->setSingleStep (0.1);
  cufVHeightValue->setToolTip (tr ("CZMIL urban vicinity height value"));
  cufVHeightValue->setWhatsThis (cufVHeightValueText);
  cufVHeightLayout->addWidget (cufVHeightValue, 1);


  QHBoxLayout *cufFRadiusLayout = new QHBoxLayout;
  cufMidLayout->addLayout (cufFRadiusLayout);

  QLabel *cufFRadiusLabel = new QLabel (tr ("HP Filter Radius"));
  cufFRadiusLabel->setToolTip (tr ("CZMIL urban noise filter Hockey Puck filter radius value"));
  cufFRadiusLabel->setWhatsThis (cufFRadiusValueText);
  cufFRadiusLayout->addWidget (cufFRadiusLabel);

  cufFRadiusValue = new QDoubleSpinBox (this);
  cufFRadiusValue->setDecimals (2);
  cufFRadiusValue->setRange (HP_MIN_RADIUS, HP_MAX_RADIUS);
  cufFRadiusValue->setSingleStep (0.1);
  cufFRadiusValue->setToolTip (tr ("CZMIL urban noise filter Hockey Puck filter radius value"));
  cufFRadiusValue->setWhatsThis (cufFRadiusValueText);
  cufFRadiusLayout->addWidget (cufFRadiusValue, 1);


  QHBoxLayout *cufFHeightLayout = new QHBoxLayout;
  cufMidLayout->addLayout (cufFHeightLayout);

  QLabel *cufFHeightLabel = new QLabel (tr ("HP Filter Height"));
  cufFHeightLabel->setToolTip (tr ("CZMIL urban noise filter Hockey Puck filter height value"));
  cufFHeightLabel->setWhatsThis (cufFHeightValueText);
  cufFHeightLayout->addWidget (cufFHeightLabel);

  cufFHeightValue = new QDoubleSpinBox (this);
  cufFHeightValue->setDecimals (2);
  cufFHeightValue->setRange (HP_MIN_HEIGHT, HP_MAX_HEIGHT);
  cufFHeightValue->setSingleStep (0.1);
  cufFHeightValue->setToolTip (tr ("CZMIL urban noise filter Hockey Puck filter height value"));
  cufFHeightValue->setWhatsThis (cufFHeightValueText);
  cufFHeightLayout->addWidget (cufFHeightValue, 1);


  QHBoxLayout *cufLDIndexLayout = new QHBoxLayout;
  cufBotLayout->addLayout (cufLDIndexLayout);

  QLabel *cufLDIndexLabel = new QLabel (tr ("Lower D_index"));
  cufLDIndexLabel->setToolTip (tr ("CZMIL urban noise filter lower D_index cutoff value"));
  cufLDIndexLabel->setWhatsThis (cufLDIndexText);
  cufLDIndexLayout->addWidget (cufLDIndexLabel);

  cufLDIndexValue = new QSpinBox (this);
  cufLDIndexValue->setRange (1, 255);
  cufLDIndexValue->setSingleStep (1);
  cufLDIndexValue->setToolTip (tr ("CZMIL urban noise filter lower D_index cutoff value"));
  cufLDIndexValue->setWhatsThis (cufLDIndexText);
  cufLDIndexLayout->addWidget (cufLDIndexValue);


  QHBoxLayout *cufUDIndexLayout = new QHBoxLayout;
  cufBotLayout->addLayout (cufUDIndexLayout);

  QLabel *cufUDIndexLabel = new QLabel (tr ("Upper D_index"));
  cufUDIndexLabel->setToolTip (tr ("CZMIL urban noise filter upper D_index cutoff value"));
  cufUDIndexLabel->setWhatsThis (cufUDIndexText);
  cufUDIndexLayout->addWidget (cufUDIndexLabel);

  cufUDIndexValue = new QSpinBox (this);
  cufUDIndexValue->setRange (1, 255);
  cufUDIndexValue->setSingleStep (1);
  cufUDIndexValue->setToolTip (tr ("CZMIL urban noise filter upper D_index cutoff value"));
  cufUDIndexValue->setWhatsThis (cufUDIndexText);
  cufUDIndexLayout->addWidget (cufUDIndexValue);


  czmilUrbanBoxLayout->addWidget (cufFilterBox);


  QGroupBox *czmilUrbanRBox = new QGroupBox (this);
  czmilUrbanRBox->setFlat (true);
  QHBoxLayout *czmilUrbanRBoxLayout = new QHBoxLayout;
  czmilUrbanRBox->setLayout (czmilUrbanRBoxLayout);

  QPushButton *czmilUrbanFilterApply = new QPushButton (tr ("Run"), this);
  czmilUrbanFilterApply->setToolTip (tr ("Run the filter using the filter parameter values"));
  czmilUrbanFilterApply->setWhatsThis (cufApplyText);
  connect (czmilUrbanFilterApply, SIGNAL (clicked ()), this, SLOT (slotCzmilUrbanFilterApply ()));
  czmilUrbanRBoxLayout->addWidget (czmilUrbanFilterApply);


  czmilUrbanFilterDelete = new QPushButton (tr ("Delete"), this);
  czmilUrbanFilterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  czmilUrbanFilterDelete->setWhatsThis (tr ("Delete marked points"));
  connect (czmilUrbanFilterDelete, SIGNAL (clicked ()), this, SLOT (slotFilterDelete ()));
  czmilUrbanRBoxLayout->addWidget (czmilUrbanFilterDelete);


  czmilUrbanFilterCancel = new QPushButton (tr ("Cancel"), this);
  czmilUrbanFilterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  czmilUrbanFilterCancel->setWhatsThis (tr ("Discard filter results"));
  connect (czmilUrbanFilterCancel, SIGNAL (clicked ()), this, SLOT (slotFilterCancel ()));
  czmilUrbanRBoxLayout->addWidget (czmilUrbanFilterCancel);


  czmilUrbanBoxLayout->addWidget (czmilUrbanRBox);


  notebook->addTab (czmilUrbanBox, tr ("CZMIL Urban Noise Filter"));
  notebook->setTabEnabled (CZMILURBANTAB, false);


  //  The following sets up the attribute filter delete/cancel notebook tab.

  QGroupBox *afbox = new QGroupBox (this);
  QVBoxLayout *afboxLayout = new QVBoxLayout;
  afbox->setLayout (afboxLayout);


  attrFilterBox = new QGroupBox (tr ("000000 points selected"), this);
  attrFilterBox->setWhatsThis (attrFilterBoxText);
  QVBoxLayout *attrFilterBoxLayout = new QVBoxLayout;
  attrFilterBox->setLayout (attrFilterBoxLayout);


  QHBoxLayout *attrLimitsLayout = new QHBoxLayout;
  attrFilterBoxLayout->addLayout (attrLimitsLayout);


  attrFilterName = new QLabel ("             ");
  attrLimitsLayout->addWidget (attrFilterName);

  attrFilterMin = new QLabel (tr ("Min: 0000"));
  attrLimitsLayout->addWidget (attrFilterMin);

  attrFilterMax = new QLabel (tr ("Max: 0000"));
  attrLimitsLayout->addWidget (attrFilterMax);


  QHBoxLayout *attrRangeLayout = new QHBoxLayout;
  attrFilterBoxLayout->addLayout (attrRangeLayout);


  QLabel *attrFilterRangeLabel = new QLabel (tr ("Filter ranges"), this);
  attrRangeLayout->addWidget (attrFilterRangeLabel);

  attrRangeText = new QLineEdit ("", this);
  attrRangeText->setAlignment (Qt::AlignLeft);
  attrRangeText->setWhatsThis (attrFilterApplyText);
  attrRangeLayout->addWidget (attrRangeText, 10);


  afboxLayout->addWidget (attrFilterBox);


  QHBoxLayout *attrZRangeLayout = new QHBoxLayout;
  afboxLayout->addLayout (attrZRangeLayout);

  attrRangeCheck = new QCheckBox (tr ("Limit Z"), this);
  attrRangeCheck->setChecked (options.filter_apply_z_range);
  attrRangeCheck->setToolTip (tr ("Check this box to limit the Z range of the data to be filtered"));
  attrRangeCheck->setWhatsThis (rangeCheckText);
  connect (attrRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  attrZRangeLayout->addWidget (attrRangeCheck);


  attrMinZLabel = new QLabel (tr ("Min Z"));
  attrMinZLabel->setToolTip (tr ("Minimum Z value for filter range"));
  attrMinZLabel->setWhatsThis (minZValueText);
  attrZRangeLayout->addWidget (attrMinZLabel);

  attrMinZValue = new QDoubleSpinBox (this);
  attrMinZValue->setRange (-12000.0, 12000.0);
  attrMinZValue->setSingleStep (1.0);
  attrMinZValue->setValue (options.filter_min_z);
  attrMinZValue->setToolTip (tr ("Minimum Z value for filter range"));
  attrMinZValue->setWhatsThis (minZValueText);
  connect (attrMinZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMinZValueChanged (double)));
  attrZRangeLayout->addWidget (attrMinZValue);


  attrMaxZLabel = new QLabel (tr ("Max Z"));
  attrMaxZLabel->setToolTip (tr ("Maximum Z value for filter range"));
  attrMaxZLabel->setWhatsThis (maxZValueText);
  attrZRangeLayout->addWidget (attrMaxZLabel);

  attrMaxZValue = new QDoubleSpinBox (this);
  attrMaxZValue->setRange (-12000.0, 12000.0);
  attrMaxZValue->setSingleStep (1.0);
  attrMaxZValue->setValue (options.filter_max_z);
  attrMaxZValue->setToolTip (tr ("Maximum Z value for filter range"));
  attrMaxZValue->setWhatsThis (maxZValueText);
  connect (attrMaxZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMaxZValueChanged (double)));
  attrZRangeLayout->addWidget (attrMaxZValue);


  QGroupBox *rbox = new QGroupBox (this);
  rbox->setFlat (true);
  QHBoxLayout *rboxLayout = new QHBoxLayout;
  rbox->setLayout (rboxLayout);

  QPushButton *attrFilterApply = new QPushButton (tr ("Run"), this);
  attrFilterApply->setToolTip (tr ("Run the filter using the above range settings"));
  attrFilterApply->setWhatsThis (attrFilterApplyText);
  connect (attrFilterApply, SIGNAL (clicked ()), this, SLOT (slotAttrFilterApply ()));
  rboxLayout->addWidget (attrFilterApply);


  attrFilterDelete = new QPushButton (tr ("Delete"), this);
  attrFilterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  attrFilterDelete->setWhatsThis (tr ("Delete marked points"));
  connect (attrFilterDelete, SIGNAL (clicked ()), this, SLOT (slotFilterDelete ()));
  rboxLayout->addWidget (attrFilterDelete);


  attrFilterHide = new QPushButton (tr ("Hide"), this);
  attrFilterHide->setToolTip (tr ("Hide filter results "));
  attrFilterHide->setWhatsThis (attrFilterHideText);
  connect (attrFilterHide, SIGNAL (clicked ()), this, SLOT (slotFilterHide ()));
  rboxLayout->addWidget (attrFilterHide);


  attrFilterInvert = new QPushButton (tr ("Invert"), this);
  attrFilterInvert->setToolTip (tr ("Invert filter results "));
  attrFilterInvert->setWhatsThis (attrFilterInvertText);
  connect (attrFilterInvert, SIGNAL (clicked ()), this, SLOT (slotAttrFilterInvert ()));
  rboxLayout->addWidget (attrFilterInvert);


  attrFilterCancel = new QPushButton (tr ("Cancel"), this);
  attrFilterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  attrFilterCancel->setWhatsThis (tr ("Discard filter results"));
  connect (attrFilterCancel, SIGNAL (clicked ()), this, SLOT (slotFilterCancel ()));
  rboxLayout->addWidget (attrFilterCancel);


  afboxLayout->addWidget (rbox);


  notebook->addTab (afbox, tr ("Attribute Filter"));
  notebook->setTabEnabled (ATTRTAB, false);


  //  The following sets up the hofWaveFilter delete/cancel notebook tab

  QGroupBox *hfbox = new QGroupBox (this);
  QVBoxLayout *hfboxLayout = new QVBoxLayout;
  hfbox->setLayout (hfboxLayout);


  hofFilterBox = new QGroupBox (tr ("HOF Wave Filter"), this);
  hofFilterBox->setWhatsThis (HOFWaveFilterText);
  QVBoxLayout *hofFilterBoxLayout = new QVBoxLayout;
  hofFilterBox->setLayout (hofFilterBoxLayout);

  
  QHBoxLayout *srboxLayout = new QHBoxLayout;
  hofFilterBoxLayout->addLayout (srboxLayout);

  QLabel *srLabel = new QLabel (tr ("Radius"));
  srboxLayout->addWidget (srLabel);

  srSlider = new QSlider ();
  srSlider->setOrientation (Qt::Horizontal);
  srSlider->setTracking (true);
  srSlider->setRange (1, 100);
  srSlider->setSingleStep (1);
  srSlider->setPageStep (1);
  srSlider->setValue (NINT (misc.abe_share->filterShare.search_radius * 10.0));
  srSlider->setToolTip (tr ("HOF waveform filter search radius"));
  srSlider->setWhatsThis (srSliderText);
  connect (srSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterSrChanged (int)));
  connect (srSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  srboxLayout->addWidget (srSlider, 100);


  srValue = new QLineEdit (" 00.0 ", this);
  srValue->setAlignment (Qt::AlignLeft);
  srValue->setMaximumWidth (40);
  QString sr = QString ("%L1").arg (misc.abe_share->filterShare.search_radius, 0, 'f', 1);
  srValue->setText (sr);
  srValue->setToolTip (tr ("HOF waveform filter search radius"));
  srValue->setWhatsThis (srSliderText);
  connect (srValue, SIGNAL (returnPressed ()), this, SLOT (slotHOFWaveFilterSrReturn ()));
  srboxLayout->addWidget (srValue);


  QHBoxLayout *swboxLayout = new QHBoxLayout;
  hofFilterBoxLayout->addLayout (swboxLayout);

  QLabel *swLabel = new QLabel (tr ("Width"));
  swboxLayout->addWidget (swLabel);


  swSlider = new QSlider ();
  swSlider->setOrientation (Qt::Horizontal);
  swSlider->setTracking (true);
  swSlider->setRange (3, 20);
  swSlider->setSingleStep (1);
  swSlider->setPageStep (1);
  swSlider->setValue (misc.abe_share->filterShare.search_width);
  swSlider->setToolTip (tr ("HOF waveform filter search width"));
  swSlider->setWhatsThis (swSliderText);
  connect (swSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterSwChanged (int)));
  connect (swSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  swboxLayout->addWidget (swSlider, 100);


  swValue = new QLineEdit (" 00 ", this);
  swValue->setAlignment (Qt::AlignLeft);
  swValue->setMaximumWidth (40);
  QString sw = QString ("%1").arg (misc.abe_share->filterShare.search_width);
  swValue->setText (sw);
  swValue->setToolTip (tr ("HOF waveform filter search width"));
  swValue->setWhatsThis (swSliderText);
  connect (swValue, SIGNAL (returnPressed ()), this, SLOT (slotHOFWaveFilterSwReturn ()));
  swboxLayout->addWidget (swValue);


  QHBoxLayout *rtboxLayout = new QHBoxLayout;
  hofFilterBoxLayout->addLayout (rtboxLayout);

  QLabel *rtLabel = new QLabel (tr ("Rise"));
  rtboxLayout->addWidget (rtLabel);


  rtSlider = new QSlider ();
  rtSlider->setOrientation (Qt::Horizontal);
  rtSlider->setTracking (true);
  rtSlider->setRange (3, 8);
  rtSlider->setSingleStep (1);
  rtSlider->setPageStep (1);
  rtSlider->setValue (misc.abe_share->filterShare.rise_threshold);
  rtSlider->setToolTip (tr ("HOF waveform filter rise threshold"));
  rtSlider->setWhatsThis (rtSliderText);
  connect (rtSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterRtChanged (int)));
  connect (rtSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  rtboxLayout->addWidget (rtSlider, 100);


  rtValue = new QLineEdit (" 00 ", this);
  rtValue->setAlignment (Qt::AlignLeft);
  rtValue->setMaximumWidth (40);
  QString rt = QString ("%1").arg (misc.abe_share->filterShare.rise_threshold);
  rtValue->setText (rt);
  rtValue->setToolTip (tr ("HOF waveform filter rise threshold"));
  rtValue->setWhatsThis (rtSliderText);
  connect (rtValue, SIGNAL (returnPressed ()), this, SLOT (slotHOFWaveFilterRtReturn ()));
  rtboxLayout->addWidget (rtValue);


  QHBoxLayout *pmtboxLayout = new QHBoxLayout;
  hofFilterBoxLayout->addLayout (pmtboxLayout);

  QLabel *pmtLabel = new QLabel ("PMT AC");
  pmtboxLayout->addWidget (pmtLabel);


  pmtSlider = new QSlider ();
  pmtSlider->setOrientation (Qt::Horizontal);
  pmtSlider->setTracking (true);
  pmtSlider->setRange (0, 30);
  pmtSlider->setSingleStep (1);
  pmtSlider->setPageStep (1);
  pmtSlider->setValue (misc.abe_share->filterShare.pmt_ac_zero_offset_required);
  pmtSlider->setToolTip (tr ("HOF waveform filter PMT AC zero offset threshold"));
  pmtSlider->setWhatsThis (pmtSliderText);
  connect (pmtSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterPMTChanged (int)));
  connect (pmtSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  pmtboxLayout->addWidget (pmtSlider, 100);


  pmtValue = new QLineEdit (" 00 ", this);
  pmtValue->setAlignment (Qt::AlignLeft);
  pmtValue->setMaximumWidth (40);
  QString pmt = QString ("%1").arg (misc.abe_share->filterShare.pmt_ac_zero_offset_required);
  pmtValue->setText (pmt);
  pmtValue->setToolTip (tr ("HOF waveform filter rise threshold"));
  pmtValue->setWhatsThis (pmtSliderText);
  connect (pmtValue, SIGNAL (returnPressed ()), this, SLOT (slotHOFWaveFilterPMTReturn ()));
  pmtboxLayout->addWidget (pmtValue);


  QHBoxLayout *apdboxLayout = new QHBoxLayout;
  hofFilterBoxLayout->addLayout (apdboxLayout);

  QLabel *apdLabel = new QLabel ("APD AC");
  apdboxLayout->addWidget (apdLabel);


  apdSlider = new QSlider ();
  apdSlider->setOrientation (Qt::Horizontal);
  apdSlider->setTracking (true);
  apdSlider->setRange (0, 30);
  apdSlider->setSingleStep (1);
  apdSlider->setPageStep (1);
  apdSlider->setValue (misc.abe_share->filterShare.apd_ac_zero_offset_required);
  apdSlider->setToolTip (tr ("HOF waveform filter APD AC zero offset threshold"));
  apdSlider->setWhatsThis (apdSliderText);
  connect (apdSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterAPDChanged (int)));
  connect (apdSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  apdboxLayout->addWidget (apdSlider, 100);


  apdValue = new QLineEdit (" 00 ", this);
  apdValue->setAlignment (Qt::AlignLeft);
  apdValue->setMaximumWidth (40);
  QString apd = QString ("%1").arg (misc.abe_share->filterShare.apd_ac_zero_offset_required);
  apdValue->setText (apd);
  apdValue->setToolTip (tr ("HOF waveform filter rise threshold"));
  apdValue->setWhatsThis (apdSliderText);
  connect (apdValue, SIGNAL (returnPressed ()), this, SLOT (slotHOFWaveFilterAPDReturn ()));
  apdboxLayout->addWidget (apdValue);


  hfboxLayout->addWidget (hofFilterBox);


  QGroupBox *hfarbox = new QGroupBox (this);
  hfarbox->setFlat (true);
  QHBoxLayout *hfarboxLayout = new QHBoxLayout;
  hfarbox->setLayout (hfarboxLayout);

  hofWaveFilterDelete = new QPushButton (tr ("Delete"), this);
  hofWaveFilterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  hofWaveFilterDelete->setWhatsThis (tr ("Delete marked points"));
  connect (hofWaveFilterDelete, SIGNAL (clicked ()), this, SLOT (slotFilterDelete ()));
  hfarboxLayout->addWidget (hofWaveFilterDelete);


  hofWaveFilterCancel = new QPushButton (tr ("Cancel"), this);
  hofWaveFilterCancel->setWhatsThis (tr ("Discard filter results"));
  hofWaveFilterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  connect (hofWaveFilterCancel, SIGNAL (clicked ()), this, SLOT (slotFilterCancel ()));
  hfarboxLayout->addWidget (hofWaveFilterCancel);


  hfboxLayout->addWidget (hfarbox);


  notebook->addTab (hfbox, tr ("HOF Waveform Filter"));
  notebook->setTabEnabled (HOFTAB, false);


  //  The following sets up the cpfDeepFilter notebook tab

  QGroupBox *cdfbox = new QGroupBox (this);
  QVBoxLayout *cdfboxLayout = new QVBoxLayout;
  cdfbox->setLayout (cdfboxLayout);


  cpfFilterBox = new QGroupBox (tr ("CPF Filter"), this);
  cpfFilterBox->setToolTip (tr ("Select options and define a polygon to enable the <b>Accept</b> button."));
  QVBoxLayout *cpfFilterBoxLayout = new QVBoxLayout;
  cpfFilterBox->setLayout (cpfFilterBoxLayout);

  
  QButtonGroup *cpfGrp = new QButtonGroup (this);
  connect (cpfGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotCPFGrp (int)));
  cpfGrp->setExclusive (true);


  QHBoxLayout *invboxLayout = new QHBoxLayout;
  cpfFilterBoxLayout->addLayout (invboxLayout);

  invCheck = new QCheckBox (tr ("Invalidate"), this);
  invCheck->setChecked (options.czmil_cpf_filter_inval);
  invCheck->setToolTip (tr ("Set filter to invalidate deep returns"));
  invCheck->setWhatsThis (invCheckText);
  cpfGrp->addButton (invCheck, 1);
  invboxLayout->addWidget (invCheck);

  QHBoxLayout *valboxLayout = new QHBoxLayout;
  cpfFilterBoxLayout->addLayout (valboxLayout);

  valCheck = new QCheckBox (tr ("Validate"), this);
  valCheck->setChecked (!options.czmil_cpf_filter_inval);
  valCheck->setToolTip (tr ("Set filter to validate deep returns"));
  valCheck->setWhatsThis (valCheckText);
  cpfGrp->addButton (valCheck, 0);
  valboxLayout->addWidget (valCheck);

  QLabel *valSpinLabel = new QLabel (tr ("Minimum amplitude"));
  valSpinLabel->setToolTip (tr ("Minimum deep return waveform amplitude"));
  valSpinLabel->setWhatsThis (valSpinText);
  valboxLayout->addWidget (valSpinLabel);

  valSpin = new QSpinBox (this);
  valSpin->setRange (0, CZMIL_MAX_PACKETS * 64);
  valSpin->setSingleStep (10);
  valSpin->setToolTip (tr ("Minimum deep return waveform amplitude"));
  valSpin->setWhatsThis (valSpinText);
  valboxLayout->addWidget (valSpin);


  cdfboxLayout->addWidget (cpfFilterBox);


  QGroupBox *cpfarbox = new QGroupBox (this);
  cpfarbox->setToolTip (tr ("Select options and define a polygon to enable the <b>Accept</b> button."));
  cpfarbox->setFlat (true);
  QHBoxLayout *cpfarboxLayout = new QHBoxLayout;
  cpfarbox->setLayout (cpfarboxLayout);

  QPushButton *cpfFilterClose = new QPushButton (tr ("Close"), this);
  cpfFilterClose->setToolTip (tr ("Close the CPF filter tab and discard the filter results"));
  connect (cpfFilterClose, SIGNAL (clicked ()), this, SLOT (slotTabClose ()));
  cpfarboxLayout->addWidget (cpfFilterClose);

  cpfarboxLayout->addStretch (10);

  cpfFilterAccept = new QPushButton (tr ("Accept"), this);
  cpfFilterAccept->setToolTip (tr ("Accept filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  cpfFilterAccept->setWhatsThis (tr ("Accept filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  connect (cpfFilterAccept, SIGNAL (clicked ()), this, SLOT (slotCPFFilterAccept ()));
  cpfarboxLayout->addWidget (cpfFilterAccept);
  cpfFilterAccept->setEnabled (false);

  cpfFilterDiscard = new QPushButton (tr ("Discard"), this);
  cpfFilterDiscard->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  cpfFilterDiscard->setWhatsThis (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  connect (cpfFilterDiscard, SIGNAL (clicked ()), this, SLOT (slotCPFFilterDiscard ()));
  cpfarboxLayout->addWidget (cpfFilterDiscard);


  cdfboxLayout->addWidget (cpfarbox);


  notebook->addTab (cdfbox, tr ("CPF Filter"));
  notebook->setTabEnabled (CPFTAB, false);


  //  The following sets up the LiDAR classification change accept/discard notebook tab.

  QGroupBox *lccbox = new QGroupBox (this);
  QVBoxLayout *lccboxLayout = new QVBoxLayout;
  lccbox->setLayout (lccboxLayout);


  lidarClassChangeBox = new QGroupBox (tr ("000000 points selected"), this);
  lidarClassChangeBox->setWhatsThis (lidarClassChangeBoxText);
  QVBoxLayout *lidarClassChangeBoxLayout = new QVBoxLayout;
  lidarClassChangeBox->setLayout (lidarClassChangeBoxLayout);


  QHBoxLayout *lidarClassChangeLimitsLayout = new QHBoxLayout;
  lidarClassChangeBoxLayout->addLayout (lidarClassChangeLimitsLayout);

  QPushButton *lidarClassChangePrefs = new QPushButton (tr ("Change From/To"), this);
  lidarClassChangePrefs->setToolTip (tr ("Change the <b>From</b> and/or <b>To</b> source and destination classifications"));
  lidarClassChangePrefs->setWhatsThis (tr ("Click this button to bring up the LiDAR change classification preferences dialog to allow you to "
                                           "change the <b>From</b> and/or <b>To</b> source and destination classifications"));
  connect (lidarClassChangePrefs, SIGNAL (clicked ()), this, SLOT (slotLidarClassChangePrefs ()));
  lidarClassChangeLimitsLayout->addWidget (lidarClassChangePrefs);

  lidarClassChangeFrom = new QLabel (tr ("From: 0,0,0,0,0,0,0,0"));
  lidarClassChangeLimitsLayout->addWidget (lidarClassChangeFrom);

  lidarClassChangeTo = new QLabel (tr ("To: 00"));
  lidarClassChangeLimitsLayout->addWidget (lidarClassChangeTo);

  lccboxLayout->addWidget (lidarClassChangeBox);

  QHBoxLayout *lidarClassZRangeLayout = new QHBoxLayout;
  lccboxLayout->addLayout (lidarClassZRangeLayout);

  lidarClassRangeCheck = new QCheckBox (tr ("Limit Z"), this);
  lidarClassRangeCheck->setChecked (options.filter_apply_z_range);
  lidarClassRangeCheck->setToolTip (tr ("Check this box to limit the Z range of the data to be filtered"));
  lidarClassRangeCheck->setWhatsThis (rangeCheckText);
  connect (lidarClassRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  lidarClassZRangeLayout->addWidget (lidarClassRangeCheck);


  lidarClassMinZLabel = new QLabel (tr ("Min Z"));
  lidarClassMinZLabel->setToolTip (tr ("Minimum Z value for filter range"));
  lidarClassMinZLabel->setWhatsThis (minZValueText);
  lidarClassZRangeLayout->addWidget (lidarClassMinZLabel);

  lidarClassMinZValue = new QDoubleSpinBox (this);
  lidarClassMinZValue->setRange (-12000.0, 12000.0);
  lidarClassMinZValue->setSingleStep (1.0);
  lidarClassMinZValue->setValue (options.filter_min_z);
  lidarClassMinZValue->setToolTip (tr ("Minimum Z value for filter range"));
  lidarClassMinZValue->setWhatsThis (minZValueText);
  connect (lidarClassMinZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMinZValueChanged (double)));
  lidarClassZRangeLayout->addWidget (lidarClassMinZValue);


  lidarClassMaxZLabel = new QLabel (tr ("Max Z"));
  lidarClassMaxZLabel->setToolTip (tr ("Maximum Z value for filter range"));
  lidarClassMaxZLabel->setWhatsThis (maxZValueText);
  lidarClassZRangeLayout->addWidget (lidarClassMaxZLabel);

  lidarClassMaxZValue = new QDoubleSpinBox (this);
  lidarClassMaxZValue->setRange (-12000.0, 12000.0);
  lidarClassMaxZValue->setSingleStep (1.0);
  lidarClassMaxZValue->setValue (options.filter_max_z);
  lidarClassMaxZValue->setToolTip (tr ("Maximum Z value for filter range"));
  lidarClassMaxZValue->setWhatsThis (maxZValueText);
  connect (lidarClassMaxZValue, SIGNAL (valueChanged (double)), this, SLOT (slotMaxZValueChanged (double)));
  lidarClassZRangeLayout->addWidget (lidarClassMaxZValue);


  QGroupBox *lccConfirmBox = new QGroupBox (this);
  lccConfirmBox->setFlat (true);
  QHBoxLayout *lccConfirmBoxLayout = new QHBoxLayout;
  lccConfirmBox->setLayout (lccConfirmBoxLayout);

  QPushButton *lidarClassChangeClose = new QPushButton (tr ("Close"), this);
  lidarClassChangeClose->setToolTip (tr ("Close the classification changes tab and discard any marked points"));
  connect (lidarClassChangeClose, SIGNAL (clicked ()), this, SLOT (slotTabClose ()));
  lccConfirmBoxLayout->addWidget (lidarClassChangeClose);

  lccConfirmBoxLayout->addStretch (10);

  QPushButton *lidarClassChangeAccept = new QPushButton (tr ("Accept"), this);
  lidarClassChangeAccept->setToolTip (tr ("Accept the classification changes for the marked points"));
  connect (lidarClassChangeAccept, SIGNAL (clicked ()), this, SLOT (slotLidarClassChangeAccept ()));
  lccConfirmBoxLayout->addWidget (lidarClassChangeAccept);

  QPushButton *lidarClassChangeDiscard = new QPushButton (tr ("Discard"), this);
  lidarClassChangeDiscard->setToolTip (tr ("Discard the classification changes for the marked points"));
  connect (lidarClassChangeDiscard, SIGNAL (clicked ()), this, SLOT (slotLidarClassChangeDiscard ()));
  lccConfirmBoxLayout->addWidget (lidarClassChangeDiscard);


  lccboxLayout->addWidget (lccConfirmBox);


  notebook->addTab (lccbox, tr ("LiDAR Classification Change"));
  notebook->setTabEnabled (LIDARCLASSTAB, false);


  if (options.track_tab)
    {
      notebook->setCurrentIndex (TRACKTAB);
    }
  else
    {
      notebook->setCurrentIndex (HISTTAB);
    }
  connect (notebook, SIGNAL (currentChanged (int)), this, SLOT (slotNotebookChanged (int)));


  leftBoxLayout->addWidget (notebook);


  //  Now we set up the information/status portion of the left side box.

  QGroupBox *filBox = new QGroupBox ();
  filBox->setMaximumWidth (trackBoxWidth);
  QHBoxLayout *filBoxLayout = new QHBoxLayout ();
  filBox->setLayout (filBoxLayout);

  pfmLabel = new QLabel (this);
  pfmLabel->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  pfmLabel->setWhatsThis (tr ("Name of PFM from which the the point nearest to the cursor was read"));
  filBoxLayout->addWidget (pfmLabel, 10);


  leftBoxLayout->addWidget (filBox);


  //  Set up the status and PFM layer tabbed notebook

  QGroupBox *noteBox = new QGroupBox ();
  QVBoxLayout *noteBoxLayout = new QVBoxLayout ();
  noteBox->setLayout (noteBoxLayout);
  noteBox->setMaximumWidth (trackBoxWidth);


  QGroupBox *statBox = new QGroupBox ();
  QGridLayout *statBoxLayout = new QGridLayout;
  statBox->setLayout (statBoxLayout);
  statBox->setMaximumWidth (trackBoxWidth);


  //  Get the normal background color.  We'll use it later.

  misc.widgetBackgroundColor = statBox->palette ().color (QWidget::backgroundRole ());


  //  The scaleBox widget is built in the utility library.

  for (int32_t i = 0 ; i < NUM_SCALE_LEVELS ; i++)
    {
      misc.scale[i] = new scaleBox (this);
      misc.scale[i]->setMinimumWidth (80);
      misc.scale[i]->setMaximumWidth (80);
      statBoxLayout->addWidget (misc.scale[i], i, 0, Qt::AlignLeft);

      misc.scale[i]->setContents (Qt::white, QString (""), 0);
    }
  connect (misc.scale[0], SIGNAL (mousePressSignal (QMouseEvent *)), this, SLOT (slotMinScalePressed (QMouseEvent *)));
  connect (misc.scale[NUM_SCALE_LEVELS - 1], SIGNAL (mousePressSignal (QMouseEvent *)), this, SLOT (slotMaxScalePressed (QMouseEvent *)));


  misc.widgetForegroundColor = QColor (0, 0, 0);


  labelColorString[0] = "#ffffff";
  labelColorString[1] = "#acacac";
  labelColor[0].setNamedColor (labelColorString[0]);
  labelColor[1].setNamedColor (labelColorString[1]);
  label_palette[0].setColor (QPalette::Window, labelColor[0]);
  label_palette[0].setColor (QPalette::WindowText, misc.widgetForegroundColor);
  label_palette[1].setColor (QPalette::Window, labelColor[1]);
  label_palette[1].setColor (QPalette::WindowText, misc.widgetForegroundColor);

  colorPalette[0] = colorPalette[1] = this->palette ();

  QLinearGradient gradient0 (200, 0, 0, 0);
  gradient0.setColorAt (0, QColor (255, 0, 0));
  gradient0.setColorAt (1, QColor (255, 255, 255));
  QBrush brush0 (gradient0);
  colorPalette[0].setBrush (QPalette::Window, brush0);

  QLinearGradient gradient1 (200, 0, 0, 0);
  gradient1.setColorAt (0, QColor (255, 0, 0));
  gradient1.setColorAt (1, QColor (172, 172, 172));
  QBrush brush1 (gradient1);
  colorPalette[1].setBrush (QPalette::Window, brush1);


  colorBoxPalette[0] = colorBoxPalette[1] = this->palette ();

  QLinearGradient gradient2 (200, 0, 0, 0);
  gradient2.setColorAt (0, QColor (0, 255, 255));
  gradient2.setColorAt (1, QColor (255, 255, 255));
  QBrush brush2 (gradient2);
  colorBoxPalette[0].setBrush (QPalette::Window, brush2);


  QLinearGradient gradient3 (200, 0, 0, 0);
  gradient3.setColorAt (0, QColor (0, 255, 255));
  gradient3.setColorAt (1, QColor (172, 172, 172));
  QBrush brush3 (gradient3);
  colorBoxPalette[1].setBrush (QPalette::Window, brush3);


  int32_t pos = 0;

  latName = new clickLabel ("Latitude", this);
  latName->setMinimumSize (latName->sizeHint ());
  latName->setAutoFillBackground (true);
  latName->setPalette (label_palette[pos % 2]);
  connect (latName, SIGNAL (clicked (QMouseEvent *, int)), SLOT (slotLatitudeClicked (QMouseEvent *, int)));
  statBoxLayout->addWidget (latName, pos, 1, Qt::AlignLeft, 1);

  latLabel = new QLabel (" N 90 00 00.000 ", this);
  latLabel->setMinimumSize (latLabel->sizeHint ());
  latLabel->setAutoFillBackground (true);
  latLabel->setPalette (label_palette[pos % 2]);
  statBoxLayout->addWidget (latLabel, pos, 2, Qt::AlignLeft);

  pos++;
  
  lonName = new QLabel ("Longitude", this);
  lonName->setMinimumSize (lonName->sizeHint ());
  lonName->setAutoFillBackground (true);
  lonName->setPalette (label_palette[pos % 2]);
  statBoxLayout->addWidget (lonName, pos, 1, Qt::AlignLeft, 1);

  lonLabel = new QLabel (" W 180 00 00.000 ", this);
  lonLabel->setMinimumSize (lonLabel->sizeHint ());
  lonLabel->setAutoFillBackground (true);
  lonLabel->setPalette (label_palette[pos % 2]);
  statBoxLayout->addWidget (lonLabel, pos, 2, Qt::AlignLeft);

  pos++;

  QLabel *recName = new QLabel ("Record/Sub-record", this);
  recName->setToolTip (tr ("Record and sub-record number of current point"));
  recName->setMinimumSize (recName->sizeHint ());
  recName->setAutoFillBackground (true);
  recName->setPalette (label_palette[pos % 2]);
  statBoxLayout->addWidget (recName, pos, 1, Qt::AlignLeft, 1);

  recLabel = new QLabel (this);
  recLabel->setToolTip (tr ("Record and sub-record number of current point"));
  recLabel->setAutoFillBackground (true);
  recLabel->setPalette (label_palette[pos % 2]);
  statBoxLayout->addWidget (recLabel, pos, 2, Qt::AlignLeft);

  pos++;

  QString attrstring = tr ("When the label is clicked it will change its background color to a gradient fading from red on the right "
                           "to the normal background color on the left.  This allows you to see at a glance what attribute you are "
                           "coloring the points by.<br><br>"
                           "<b>Hint: <i>Meta</i> is the <i>Microsoft/Ubuntu/Penguin</i> key.</b>");

  for (int32_t i = 0 ; i < PRE_ATTR + POST_ATTR; i++)
    {
      //  The clickLabel widget is built in the utility library.

      switch (i)
        {
        case 0:
          attrName[i] = new clickLabel ("Z value", this, i);
          attrName[i]->setToolTip (tr ("Click to color by Z value [%1]").arg (misc.hard_key[COLOR_BY_DEPTH]));
          attrName[i]->setWhatsThis (tr ("Click this label to color the data by Z value. %1").arg (attrstring));

          attrValue[i] = new QLabel (" 00000.00 ", this);
          attrValue[i]->setToolTip (tr ("Z value of current point"));
          break;

        case 1:
          attrName[i] = new clickLabel ("Line number", this, i);
          attrName[i]->setToolTip (tr ("Click to color by line number [%1]").arg (misc.hard_key[COLOR_BY_LINE]));
          attrName[i]->setWhatsThis (tr ("Click this label to color the data by line number. %1").arg (attrstring));

          attrValue[i] = new QLabel ("00000", this);
          attrValue[i]->setToolTip (tr ("Line number of current point"));
          break;

        case 2:
          attrName[i] = new clickLabel ("Horizontal uncertainty", this, i);
          attrName[i]->setToolTip (tr ("Click to color by horizontal uncertainty [%1]").arg (misc.hard_key[COLOR_BY_HORIZONTAL_ERROR]));
          attrName[i]->setWhatsThis (tr ("Click this label to color the data by horizontal uncertainty. %1").arg (attrstring));

          attrValue[i] = new QLabel (" 00.00 ", this);
          attrValue[i]->setToolTip (tr ("Horizontal uncertainty of current point"));
          break;

        case 3:
          attrName[i] = new clickLabel ("Vertical uncertainty", this, i);
          attrName[i]->setToolTip (tr ("Click to color by vertical uncertainty [%1]").arg (misc.hard_key[COLOR_BY_VERTICAL_ERROR]));
          attrName[i]->setWhatsThis (tr ("Click this label to color the data by vertical uncertainty. %1").arg (attrstring));

          attrValue[i] = new QLabel (" 00.00 ", this);
          attrValue[i]->setToolTip (tr ("Vertical uncertainty of current point"));
          break;

        default:
          attrName[i] = new clickLabel ("", this, i);
          attrName[i]->setToolTip (tr ("No attribute selected"));
          attrName[i]->setWhatsThis (tr ("No attribute selected"));

          attrValue[i] = new QLabel ("         ", this);
          attrValue[i]->setToolTip (tr ("No attribute selected"));
          break;
        }

      attrPalette[i] = attrName[i]->palette ();

      attrName[i]->setMinimumSize (attrName[i]->sizeHint ());
      attrName[i]->setAutoFillBackground (true);
      attrName[i]->setPalette (label_palette[pos % 2]);
      attrName[i]->setFrameStyle (QFrame::Panel | QFrame::Raised);
      attrName[i]->setLineWidth (1);

      statBoxLayout->addWidget (attrName[i], pos, 1, Qt::AlignLeft, 1);

      attrValue[i]->setMinimumSize (attrValue[i]->sizeHint ());
      attrValue[i]->setAutoFillBackground (true);
      attrValue[i]->setPalette (label_palette[pos % 2]);
      statBoxLayout->addWidget (attrValue[i], pos, 2, Qt::AlignLeft);

      pos++;
    }


  //  Make sure the names fit.

  statBoxLayout->setColumnStretch (1, 1);


  statbook = new QTabWidget ();
  statbook->setTabPosition (QTabWidget::North);
  connect (statbook, SIGNAL (currentChanged (int)), this, SLOT (slotStatbookChanged (int)));

  statbook->addTab (statBox, tr ("Status"));


  //  Set up the Hidden data notebook tab

  QGroupBox *hideBox = new QGroupBox ();
  QGridLayout *hideBoxLayout = new QGridLayout;
  hideBox->setLayout (hideBoxLayout);
  hideBox->setMaximumWidth (trackBoxWidth);
  hideBoxLayout->setColumnStretch (1, 1);


  hideMenu = new QMenu (this);
  hideMenu->setWhatsThis (hideMenuText);
  hideGrp = new QActionGroup (this);
  hideGrp->setExclusive (false);
  connect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));


  hideBoxLayout->addWidget (hideMenu);


  statbook->addTab (hideBox, tr ("Hidden Data"));


  //  Set up the Attributes notebook tab

  QGroupBox *attrBox = new QGroupBox ();
  QGridLayout *attrBoxLayout = new QGridLayout;
  attrBox->setLayout (attrBoxLayout);
  attrBox->setMaximumWidth (trackBoxWidth);


  pos = 0;


  attrstring = tr ("When the label is clicked it will set the selected attribute in the first <b>Status</b> tab "
                   "attribute slots.  If all of the attribute slots are populated, the last will be removed.");

  for (int32_t i = 0 ; i < NUM_ATTR ; i++)
    {
      attrBoxName[i] = new clickLabel ("", this, i);

      attrBoxValue[i] = new QLabel ("", this);


      attrBoxName[i]->setMaximumHeight (attrName[0]->sizeHint ().height ());
      attrBoxName[i]->setMinimumSize (attrBoxName[i]->sizeHint ());
      attrBoxName[i]->setAutoFillBackground (true);
      attrBoxName[i]->setPalette (label_palette[pos % 2]);
      attrBoxName[i]->setFrameStyle (QFrame::Panel | QFrame::Raised);
      attrBoxName[i]->setLineWidth (1);


      attrBoxValue[i]->setMaximumHeight (attrValue[0]->sizeHint ().height ());
      attrBoxValue[i]->setMinimumSize (attrBoxValue[i]->sizeHint ());
      attrBoxValue[i]->setAutoFillBackground (true);
      attrBoxValue[i]->setPalette (label_palette[pos % 2]);

      attrBoxLayout->addWidget (attrBoxName[i], pos, 0, Qt::AlignLeft, 1);
      attrBoxLayout->addWidget (attrBoxValue[i], pos, 1, Qt::AlignLeft);

      pos++;
    }


  attrBoxLayout->addItem (new QSpacerItem (1, 1, QSizePolicy::Expanding, QSizePolicy::Preferred), pos, 1, 1, 4, Qt::AlignBottom);


  //  Make sure the names fit.

  attrBoxLayout->setColumnStretch (pos, 1);


  statbook->addTab (attrBox, tr ("Attributes"));


  //  Set up the Layers notebook tab

  QGroupBox *layerBox = new QGroupBox ();
  QGridLayout *layerBoxLayout = new QGridLayout;
  layerBox->setLayout (layerBoxLayout);
  layerBox->setMaximumWidth (trackBoxWidth);
  layerBoxLayout->setColumnStretch (1, 1);


  QButtonGroup *layerCheckGrp = new QButtonGroup (this);
  layerCheckGrp->setExclusive (false);
  connect (layerCheckGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotLayerClicked (int)));


  for (int32_t i = 0 ; i < MAX_ABE_PFMS ; i++)
    {
      layerCheck[i] = new QCheckBox (this);
      layerCheckGrp->addButton (layerCheck[i], i);
      layerBoxLayout->addWidget (layerCheck[i], i, 0, Qt::AlignLeft);

      layerName[i] = new QLabel (this);
      layerBoxLayout->addWidget (layerName[i], i, 1, Qt::AlignLeft);
    }
 
  layerBoxLayout->setRowStretch (MAX_ABE_PFMS - 1, 1);


  statbook->addTab (layerBox, tr ("Layers"));


  leftBoxLayout->addWidget (statbook, 1);


  //  Set up the PFM layers statbook tab filenames

  statbook->setTabEnabled (3, false);
  for (int32_t pfm = 0 ; pfm < MAX_ABE_PFMS ; pfm++)
    {
      if (pfm < misc.abe_share->pfm_count)
        {
          QString name = QFileInfo (QString (misc.abe_share->open_args[pfm].list_path)).fileName ().remove (".pfm");

          layerName[pfm]->setText (name);

          if (pfm) statbook->setTabEnabled (3, true);

          if (misc.abe_share->display_pfm[pfm])
            {
              layerCheck[pfm]->setChecked (true);
            }
          else
            {
              layerCheck[pfm]->setChecked (false);
            }
          layerName[pfm]->show ();
          layerCheck[pfm]->show ();
        }
      else
        {
          layerName[pfm]->hide ();
          layerCheck[pfm]->hide ();
        }
    }


  exagBar = new QScrollBar (Qt::Vertical);
  exagBar->setTracking (true);
  exagBar->setInvertedAppearance (true);
  exagBar->setToolTip (tr ("Vertical exaggeration"));
  exagBar->setWhatsThis (exagBarText);
  exagPalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  exagPalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  exagPalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  exagPalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  exagBar->setPalette (exagPalette);
  exagBar->setMinimum (100);
  exagBar->setMaximum (10000);
  exagBar->setSingleStep (100);
  exagBar->setPageStep (500);
  connect (exagBar, SIGNAL (actionTriggered (int)), this, SLOT (slotExagTriggered (int)));
  connect (exagBar, SIGNAL (sliderReleased ()), this, SLOT (slotExagReleased ()));


  QGroupBox *sliceBox = new QGroupBox (tr ("Slice"));
  QVBoxLayout *sliceBoxLayout = new QVBoxLayout ();
  sliceBox->setLayout (sliceBoxLayout);


  QHBoxLayout *barBoxLayout = new QHBoxLayout ();


  sliceBar = new QScrollBar (Qt::Vertical);
  sliceBar->setTracking (true);
  sliceBar->setInvertedAppearance (true);
  sliceBar->setToolTip (tr ("Slice through the data (<b>Up Arrow</b>, <b>Down Arrow</b>, mouse scroll wheel, <b>Shift</b> left mouse button)"));
  sliceBar->setWhatsThis (sliceBarText);
  slicePalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  slicePalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  slicePalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  slicePalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  sliceBar->setPalette (slicePalette);
  connect (sliceBar, SIGNAL (actionTriggered (int)), this, SLOT (slotSliceTriggered (int)));
  barBoxLayout->addWidget (sliceBar);


  QVBoxLayout *ctrlBox = new QVBoxLayout ();

  transBar = new QScrollBar (Qt::Vertical);
  transBar->setTracking (true);
  transBar->setInvertedAppearance (true);
  transBar->setToolTip (tr ("Transparency of data outside of slice"));
  transBar->setWhatsThis (transText);
  transPalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  transPalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  transPalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  transPalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  transBar->setPalette (transPalette);
  transBar->setMinimum (0);
  transBar->setMaximum (MAX_TRANS_VALUE);
  transBar->setSingleStep (1);
  transBar->setPageStep (8);
  transBar->setValue (options.slice_alpha);
  connect (transBar, SIGNAL (actionTriggered (int)), this, SLOT (slotTransTriggered (int)));
  ctrlBox->addWidget (transBar, 1);


  transLabel = new QLabel ("000", this);
  transLabel->setAlignment (Qt::AlignCenter);
  transLabel->setMinimumSize (transLabel->sizeHint ());
  transLabel->setToolTip (tr ("Transparency value"));
  transLabel->setWhatsThis (transText);
  transLabel->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  transLabel->setLineWidth (2);
  QString string = QString ("%1").arg (options.slice_alpha, 2, 10, QChar('0'));
  transLabel->setText (string);
  ctrlBox->addWidget (transLabel);


  sizeBar = new QScrollBar (Qt::Vertical);
  sizeBar->setTracking (true);
  sizeBar->setInvertedAppearance (true);
  sizeBar->setToolTip (tr ("Size of slice as percentage of area"));
  sizeBar->setWhatsThis (sizeText);
  sizePalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  sizePalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  sizePalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  sizePalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  sizeBar->setPalette (sizePalette);
  sizeBar->setMinimum (1);
  sizeBar->setMaximum (MAX_SLICE_SIZE);
  sizeBar->setSingleStep (1);
  sizeBar->setPageStep (5);
  sizeBar->setValue (options.slice_percent);
  connect (sizeBar, SIGNAL (actionTriggered (int)), this, SLOT (slotSizeTriggered (int)));
  ctrlBox->addWidget (sizeBar, 1);

  sizeLabel = new QLabel ("00", this);
  sizeLabel->setAlignment (Qt::AlignCenter);
  sizeLabel->setMinimumSize (sizeLabel->sizeHint ());
  sizeLabel->setToolTip (tr ("Slice size as a percentage"));
  sizeLabel->setWhatsThis (sizeText);
  sizeLabel->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  sizeLabel->setLineWidth (2);
  sizeLabel->setNum (options.slice_percent);
  ctrlBox->addWidget (sizeLabel);

  barBoxLayout->addLayout (ctrlBox);

  sliceBoxLayout->addLayout (barBoxLayout);


  meterLabel = new QLabel ("00000.0", this);
  meterLabel->setAlignment (Qt::AlignCenter);
  meterLabel->setMinimumSize (meterLabel->sizeHint ());
  meterLabel->setToolTip (tr ("Slice size in meters"));
  meterLabel->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  meterLabel->setLineWidth (2);
  meterLabel->setText ("");

  sliceBoxLayout->addWidget (meterLabel);


  QHBoxLayout *hBox = new QHBoxLayout ();
  hBox->addWidget (leftBox);
  hBox->addWidget (exagBar);
  hBox->addWidget (map, 1);
  hBox->addWidget (sliceBox);
  vBox->addLayout (hBox, 1);


  //  Set View toolbar button states based on saved options.  We don't set the shortcut for the first three because they wil be triggered
  //  from the shortcuts in the pulldown menu.

  bDisplayManInvalid->setChecked (options.display_man_invalid);
  connect (bDisplayManInvalid, SIGNAL (clicked ()), this, SLOT (slotDisplayManInvalid ()));

  bDisplayFltInvalid->setChecked (options.display_flt_invalid);
  connect (bDisplayFltInvalid, SIGNAL (clicked ()), this, SLOT (slotDisplayFltInvalid ()));

  bDisplayNull->setChecked (options.display_null);
  connect (bDisplayNull, SIGNAL (clicked ()), this, SLOT (slotDisplayNull ()));

  bContour->setChecked (options.display_contours);
  connect (bContour, SIGNAL (clicked ()), this, SLOT (slotContour ()));

  bClearHighlight->setToolTip (tr ("Clear highlighted points [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));


  //  Set Feature toolbar button states based on saved options

  bDisplayFeature->setIcon (displayFeatureIcon[options.display_feature]);

  bDisplayChildren->setChecked (options.display_children);
  connect (bDisplayChildren, SIGNAL (clicked ()), this, SLOT (slotDisplayChildren ()));

  bDisplayFeatureInfo->setChecked (options.display_feature_info);
  connect (bDisplayFeatureInfo, SIGNAL (clicked ()), this, SLOT (slotDisplayFeatureInfo ()));


  //  Set Reference toolbar button states based on saved options

  bDisplayReference->setChecked (options.display_reference);
  connect (bDisplayReference, SIGNAL (clicked ()), this, SLOT (slotDisplayReference ()));


  //  Set Utilities toolbar button states based on saved options

  if (misc.abe_share->read_only) options.auto_unload = NVFalse;
  bUnload->setChecked (options.auto_unload);
  connect (bUnload, SIGNAL (clicked ()), this, SLOT (slotUnload ()));


  //  Set Edit toolbar button states based on saved options

  if (options.function == DELETE_POINT) bDeletePoint->setChecked (true);

  if (options.function == DELETE_RECTANGLE) bDeleteRect->setChecked (true);

  if (options.function == DELETE_POLYGON) bDeletePoly->setChecked (true);

  if (options.function == KEEP_POLYGON) bKeepPoly->setChecked (true);

  if (options.function == RESTORE_RECTANGLE) bRestoreRect->setChecked (true);

  if (options.function == RESTORE_POLYGON) bRestorePoly->setChecked (true);

  if (options.function == EDIT_FEATURE) bEditFeature->setChecked (true);

  if (options.function == CZMIL_REPROCESS_LAND) bCZMILReprocessLand->setChecked (true);
  if (options.function == CZMIL_REPROCESS_WATER) bCZMILReprocessWater->setChecked (true);
  if (options.function == CZMIL_REPROCESS_SHALLOW_WATER) bCZMILReprocessShallowWater->setChecked (true);


  //  Set all of the button hotkey tooltips and shortcuts

  for (int32_t i = 0 ; i < HOTKEYS ; i++) slotPrefHotKeyChanged (i);


  //  Connect the clickLabel signals.

  for (int32_t i = 0 ; i < PRE_ATTR + POST_ATTR; i++)
    {
      connect (attrName[i], SIGNAL (clicked (QMouseEvent *, int)), SLOT (slotColorByClicked (QMouseEvent *, int)));
    }

  for (int32_t i = 0 ; i < NUM_ATTR ; i++)
    {
      connect (attrBoxName[i], SIGNAL (clicked (QMouseEvent *, int)), SLOT (slotAttrBoxClicked (QMouseEvent *, int)));
    }


  //  Set the attributes for the current "top" PFM.

  setStatusAttributes ();


  //  If the saved value of color_index is pointing to an attribute we need to make sure the attribute is still available.

  if (options.color_index < 0 || options.color_index >= PRE_ATTR)
    {
      uint8_t hit = NVFalse;

      for (int32_t i = 0 ; i < misc.abe_share->open_args[0].head.num_ndx_attr ; i++)
        {
          QString tmp = QString (pfm_standard_attr_name (1, 0, misc.abe_share->open_args[0].head.ndx_attr_name[i]));

          for (int32_t j = 0 ; j < POST_ATTR ; j++)
            {
              if (options.attrStatName[j] == tmp)
                {
                  hit = NVTrue;
                  break;
                }
            }
          if (hit) break;
        }

      if (!hit) options.color_index = 0;
    }


  //  If we had color by some attribute set we need to allocate the color by attribute range (usually red to blue) instead
  //  of the color by depth range.

  if (options.color_index)
    {
      int32_t k = options.color_index;


      float hue_inc = (float) (options.max_hsv_color[k] - options.min_hsv_color[k]) / (float) (NUMSHADES + 1);

      for (int32_t j = 0 ; j < NUMSHADES ; j++)
        {
          misc.track_color_array[j].setHsv ((int32_t) (options.min_hsv_color[k] + j * hue_inc), 255, 255, 255); 

          misc.color_array[j][0] = misc.track_color_array[j].red ();
          misc.color_array[j][1] = misc.track_color_array[j].green ();
          misc.color_array[j][2] = misc.track_color_array[j].blue ();
          misc.color_array[j][3] = misc.track_color_array[j].alpha ();
        }

      options.ref_color[1] = options.ref_color[0];
      options.ref_color[1].setAlpha (options.slice_alpha);
    }


  //  Set the PFM user flag color index text and tooltips.

  flag[PRE_USER + 0]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[0]));
  flag[PRE_USER + 1]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[1]));
  flag[PRE_USER + 2]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[2]));
  flag[PRE_USER + 3]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[3]));
  flag[PRE_USER + 4]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[4]));
  flag[PRE_USER + 5]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[5]));
  flag[PRE_USER + 6]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[6]));
  flag[PRE_USER + 7]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[7]));
  flag[PRE_USER + 8]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[8]));
  flag[PRE_USER + 9]->setText (tr ("Mark %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[9]));

  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[0], "PFM_USER_01")) flag[PRE_USER + 0]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[1], "PFM_USER_02")) flag[PRE_USER + 1]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[2], "PFM_USER_03")) flag[PRE_USER + 2]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[3], "PFM_USER_04")) flag[PRE_USER + 3]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[4], "PFM_USER_05")) flag[PRE_USER + 4]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[5], "PFM_USER_06")) flag[PRE_USER + 5]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[6], "PFM_USER_07")) flag[PRE_USER + 6]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[7], "PFM_USER_08")) flag[PRE_USER + 7]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[8], "PFM_USER_09")) flag[PRE_USER + 8]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[9], "PFM_USER_10")) flag[PRE_USER + 9]->setVisible (false);


  for (int32_t i = 0 ; i < PFM_USER_FLAGS ; i++)
    {
      QString user = tr ("Hide PFM_USER_%1").arg (i, 2, 10, QChar('0'));
      hideAct[i] = hideMenu->addAction (user);
      hideAct[i]->setCheckable (true);
      hideAct[i]->setChecked (false);
      hideGrp->addAction (hideAct[i]);
    }

  hideAct[PFM_USER_FLAGS] = hideMenu->addAction (tr ("Hide highlighted data"));
  hideAct[PFM_USER_FLAGS]->setCheckable (true);
  hideAct[PFM_USER_FLAGS]->setChecked (false);
  hideGrp->addAction (hideAct[PFM_USER_FLAGS]);


  //  Add the invert button

  QPushButton *bInvertFlag = new QPushButton (tr ("Invert flags"), this);
  bInvertFlag->setToolTip (tr ("Invert the check boxes for flags/highlighted (show the checked data)"));
  connect (bInvertFlag, SIGNAL (clicked ()), this, SLOT (slotInvertHideFlags ()));
  QWidgetAction *invertFlagAction = new QWidgetAction (this);
  invertFlagAction->setDefaultWidget (bInvertFlag);
  hideMenu->addAction (invertFlagAction);


  //  Set the PFM user flag hide and show text and tooltips.

  hideAct[0]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[0]));
  hideAct[1]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[1]));
  hideAct[2]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[2]));
  hideAct[3]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[3]));
  hideAct[4]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[4]));
  hideAct[5]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[5]));
  hideAct[6]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[6]));
  hideAct[7]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[7]));
  hideAct[8]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[8]));
  hideAct[9]->setText (tr ("Hide %1 data").arg (misc.abe_share->open_args[0].head.user_flag_name[9]));

  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[0], "PFM_USER_01")) hideAct[0]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[1], "PFM_USER_02")) hideAct[1]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[2], "PFM_USER_03")) hideAct[2]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[3], "PFM_USER_04")) hideAct[3]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[4], "PFM_USER_05")) hideAct[4]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[5], "PFM_USER_06")) hideAct[5]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[6], "PFM_USER_07")) hideAct[6]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[7], "PFM_USER_08")) hideAct[7]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[8], "PFM_USER_09")) hideAct[8]->setVisible (false);
  if (!strcmp (misc.abe_share->open_args[0].head.user_flag_name[9], "PFM_USER_10")) hideAct[9]->setVisible (false);


  int32_t max_hide_entry = flags_offset;


  //  Add data types if there's more than one.

  if (misc.unique_count > 1)
    {
      hideMenu->addSeparator ();
      for (int32_t i = 0 ; i < misc.unique_count ; i++)
        {
          QString user = tr ("Hide %1 data").arg (PFM_data_type[misc.unique_type[i]]);
          hideAct[max_hide_entry] = hideMenu->addAction (user);
          hideAct[max_hide_entry]->setCheckable (true);
          hideAct[max_hide_entry]->setChecked (false);
          hideGrp->addAction (hideAct[max_hide_entry]);
          max_hide_entry++;
        }
    }


  //  Add channels if CZMIL data is present but no other data type is present.

  if (misc.czmil_chan_attr >= 0 && misc.unique_count == 1)
    {
      hideMenu->addSeparator ();
      for (int32_t i = 0 ; i < 9 ; i++)
        {
          char ch_str[128];
          czmil_get_channel_string (i, ch_str);
          QString user = tr ("Hide %1 data").arg (ch_str);
          hideAct[max_hide_entry] = hideMenu->addAction (user);
          hideAct[max_hide_entry]->setCheckable (true);
          hideAct[max_hide_entry]->setChecked (false);
          hideGrp->addAction (hideAct[max_hide_entry]);
          max_hide_entry++;
        }
    }


  //  Add the invert button

  if (max_hide_entry > flags_offset)
    {
      QPushButton *bInvertType = new QPushButton (tr ("Invert types"), this);
      bInvertType->setToolTip (tr ("Invert the check boxes for types (show the checked data)"));
      connect (bInvertType, SIGNAL (clicked ()), this, SLOT (slotInvertHideTypes ()));
      QWidgetAction *invertTypeAction = new QWidgetAction (this);
      invertTypeAction->setDefaultWidget (bInvertType);
      hideMenu->addAction (invertTypeAction);
    }


  //  Turn off the mask flag.  If we actually mask something, the next section will set the flag.

  options.mask_active = MASK_NONE;


  //  Check to see if hide/show is persistent and if hide/show is active.

  uint8_t hide_diff = NVFalse;
  uint8_t hide_flag = NVFalse;

  if (options.hide_persistent && max_hide_entry == options.hide_num)
    {
      //  Now check the current name against the saved names.

      for (int32_t i = 0 ; i < options.hide_num ; i++)
        {
          if (hideAct[i]->text () != options.hide_name[i])
            {
              hide_diff = NVTrue;
              break;
            }
        }


      //  If everything matches, hide what needs hiding.

      if (!hide_diff)
        {
          for (int32_t i = 0 ; i < options.hide_num ; i++)
            {
              //  Ignore highlighted data since it disappears when we exit.

              if (i != PFM_USER_FLAGS)
                {
                  if (options.hide_flag[i])
                    {
                      hide_flag = NVTrue;
                      hideAct[i]->setChecked (true);
                    }
                }
            }
        }
    }

          
  //  Save the names of the hide menu entries so we can check them later if the user has set hide_persistent.

  options.hide_num = max_hide_entry;
  for (int32_t i = 0 ; i < options.hide_num ; i++)
    {
      options.hide_name[i] = hideAct[i]->text ();
    }


  //  Right click popup menu

  popupMenu = new QMenu (map);
  connect (popupMenu, SIGNAL (triggered (QAction *)), this, SLOT (slotPopupMenu (QAction *)));

  for (int32_t i = 0 ; i < NUMPOPUPS ; i++) popup[i] = popupMenu->addAction (("            "));

  popupMenu->addSeparator ();
  popupHelp = popupMenu->addAction (tr ("Help"));
  connect (popupHelp, SIGNAL (triggered ()), this, SLOT (slotPopupHelp ()));


  //  Setup the file menu (don't set shortcuts for these menu items because they're handled by the keypress event with the associated buttons)

  saveExitAction = new QAction (tr ("Save and Exit"), this);
  saveExitAction->setStatusTip (tr ("Save changes and exit"));
  connect (saveExitAction, SIGNAL (triggered ()), this, SLOT (slotExitSave ()));

  maskExitAction = new QAction (tr ("Save, Exit, and Mask"), this);
  maskExitAction->setStatusTip (tr ("Save changes, exit, and filter mask in pfmView"));
  connect (maskExitAction, SIGNAL (triggered ()), this, SLOT (slotExitMask ()));

  QAction *noSaveExitAction = new QAction (tr ("Exit"), this);
  noSaveExitAction->setStatusTip (tr ("Exit without saving changes"));
  connect (noSaveExitAction, SIGNAL (triggered ()), this, SLOT (slotExitNoSave ()));


  QMenu *fileMenu = menuBar ()->addMenu (tr ("File"));
  fileMenu->addAction (saveExitAction);
  fileMenu->addAction (maskExitAction);
  fileMenu->addAction (noSaveExitAction);


  //  Setup the Edit menu

  QAction *prefsAct = new QAction (tr ("Preferences"), this);
  prefsAct->setStatusTip (tr ("Change program preferences"));
  prefsAct->setWhatsThis (prefsText);
  connect (prefsAct, SIGNAL (triggered ()), this, SLOT (slotPrefs ()));


  QMenu *editMenu = menuBar ()->addMenu (tr ("Edit"));
  editMenu->addAction (prefsAct);


  //  Setup the View menu

  messageAction = new QAction (tr ("Display Messages"), this);
  messageAction->setToolTip (tr ("Display error messages from program and sub-processes"));
  messageAction->setWhatsThis (messageText);
  connect (messageAction, SIGNAL (triggered ()), this, SLOT (slotMessage ()));

  QMenu *viewMenu = menuBar ()->addMenu (tr ("View"));
  viewMenu->addAction (messageAction);


  //  Setup the help menu.

  QAction *exHelp = new QAction (tr ("User Guide"), this);
  exHelp->setStatusTip (tr ("Open the Area-Based Editor user guide in a browser"));
  connect (exHelp, SIGNAL (triggered ()), this, SLOT (extendedHelp ()));

  QAction *hotHelp = new QAction (tr ("Hot Keys and GUI control"), this);
  hotHelp->setStatusTip (tr ("Help on GUI control and hot keys (fixed and user modifiable)"));
  connect (hotHelp, SIGNAL (triggered ()), this, SLOT (slotHotkeyHelp ()));

  QAction *toolHelp = new QAction (tr ("Tool bars"), this);
  toolHelp->setStatusTip (tr ("Help on tool bars"));
  connect (toolHelp, SIGNAL (triggered ()), this, SLOT (slotToolbarHelp ()));

  QAction *whatsThisAct = QWhatsThis::createAction (this);
  whatsThisAct->setIcon (QIcon (":/icons/contextHelp.png"));

  QAction *aboutAct = new QAction (tr ("About"), this);
  aboutAct->setStatusTip (tr ("Information about pfmEdit3D"));
  connect (aboutAct, SIGNAL (triggered ()), this, SLOT (about ()));

  QAction *acknowledgments = new QAction (tr ("Acknowledgments"), this);
  acknowledgments->setStatusTip (tr ("Information about supporting libraries and contributors"));
  connect (acknowledgments, SIGNAL (triggered ()), this, SLOT (slotAcknowledgments ()));

  QAction *aboutQtAct = new QAction (tr ("About Qt"), this);
  aboutQtAct->setStatusTip (tr ("Information about Qt"));
  connect (aboutQtAct, SIGNAL (triggered ()), this, SLOT (aboutQt ()));

  QMenu *helpMenu = menuBar ()->addMenu (tr ("Help"));
  helpMenu->addAction (exHelp);
  helpMenu->addAction (hotHelp);
  helpMenu->addAction (toolHelp);
  helpMenu->addAction (whatsThisAct);
  helpMenu->addSeparator ();
  helpMenu->addAction (aboutAct);
  helpMenu->addSeparator ();
  helpMenu->addAction (acknowledgments);
  helpMenu->addAction (aboutQtAct);


  setWidgetStates (NVTrue);


  //  Set up the status bars.

  QGroupBox *fpBox = new QGroupBox ();
  QGridLayout *fpBoxLayout = new QGridLayout;
  fpBox->setLayout (fpBoxLayout);


  filLabel = new QLabel (" N/A ", this);
  filLabel->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  filLabel->setToolTip (tr ("File name for current point"));
  filLabel->setWhatsThis (tr ("File name for current point"));
  filLabel->setMinimumSize (filLabel->sizeHint ());
  fpBoxLayout->addWidget (filLabel, 0, 0);

  linLabel = new QLabel (" N/A ", this);
  linLabel->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  linLabel->setToolTip (tr ("Line name for current point"));
  linLabel->setWhatsThis (tr ("Line name for current point"));
  linLabel->setAutoFillBackground (true);
  linLabel->setMinimumSize (linLabel->sizeHint ());
  fpBoxLayout->addWidget (linLabel, 0, 1);

  misc.statusProgLabel = new QLabel (this);
  misc.statusProgLabel->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  misc.statusProgLabel->setToolTip (tr ("Active mode"));
  misc.statusProgLabel->setWhatsThis (miscLabelText);
  misc.statusProgLabel->setAutoFillBackground (true);
  misc.statusProgPalette = misc.statusProgLabel->palette ();
  fpBoxLayout->addWidget (misc.statusProgLabel, 1, 0);


  //  Progress status bar

  misc.progStatus = new QStatusBar ();
  misc.progStatus->setSizeGripEnabled (false);
  misc.progStatus->setToolTip (tr ("Progress bar and label"));
  misc.progStatus->setWhatsThis (progStatusText);
  fpBoxLayout->addWidget (misc.progStatus, 1, 1);


  misc.statusProg = new QProgressBar (this);
  misc.statusProg->setRange (0, 100);
  misc.statusProg->setValue (0);
  misc.statusProg->setTextVisible (false);
  misc.progStatus->addWidget (misc.statusProg, 10);


  vBox->addWidget (fpBox);


  //  Set the tracking timer function.  This will track the cursor in other ABE program map windows.
  //  Also used to limit the number of points in the polygon functions.

  trackCursor = new QTimer (this);
  connect (trackCursor, SIGNAL (timeout ()), this, SLOT (slotTrackCursor ()));
  trackCursor->start (50);


  //  Set the blink timer (half second) function.

  blinkTimer = new QTimer (this);
  connect (blinkTimer, SIGNAL (timeout ()), this, SLOT (slotBlinkTimer ()));
  blinkTimer->start (500);


  setFunctionCursor (options.function);


  show ();


  //  Get the map center for the reset view slot.

  map->getMapCenter (&misc.map_center_x, &misc.map_center_y, &misc.map_center_z);


  redrawMap (NVTrue);


  //  Check for ancillary programs that may have been up when this program last ran

  if (options.kill_and_respawn)
    {
      for (int32_t i = 0 ; i < NUMPROGS ; i++)
        {
          //  If it was up..

          if (options.state[i] == 2)
            {
              //  Check for matching data type in displayed data.

              for (int32_t j = 0 ; j < misc.abe_share->point_cloud_count ; j++)
                {
                  //  If we get a match, kick off the program.

                  if (options.data_type[i][misc.data[j].type])
                    {
                      //  If there is a button associated with this program we want to press the button instead of
                      //  just calling ancillaryProgram.  Pressing the button will kick off the program and leave 
                      //  the button in the proper state.

                      if (options.progButton[i] != NULL)
                        {
                          options.progButton[i]->click ();
                        }
                      else
                        {
                          ancillaryProgram[i] = new ancillaryProg (this, map, &options, &misc, i, j);
                          connect (ancillaryProgram[i], SIGNAL (redrawSignal ()), this, SLOT (slotRedrawMap ()));
                        }
                      break;
                    }
                }
            }
        }
    }
  else
    {
      //  If it was running (only monitor programs), set the button state.  The assumption is that it is still up.

      for (int32_t i = 0 ; i < NUMPROGS ; i++)
        {
          //  If it was up..

          if (options.state[i] == 2 && options.progButton[i] != NULL) options.progButton[i]->setChecked (true);
        }
    }


  //  If we are flagging data we need to highlight it.

  if (options.flag_index) slotFlagMenu (flag[options.flag_index]);


  if (hide_flag)
    {
      slotHideMenu (NULL);
      if (options.hide_visible) statbook->setCurrentIndex (1);
    }
}



pfmEdit3D::~pfmEdit3D ()
{
}



void 
pfmEdit3D::setWidgetStates (uint8_t enable)
{
  //  If we changed the icon size in the preferences dialog we need to resize all of the buttons.

  if (options.main_button_icon_size != prev_icon_size)
    {
      QSize mainButtonIconSize (options.main_button_icon_size, options.main_button_icon_size);

      for (int32_t i = 0 ; i < NUM_TOOLBARS ; i++)
        {
          toolBar[i]->setIconSize (mainButtonIconSize);
          toolBar[i]->adjustSize ();
        }

      prev_icon_size = options.main_button_icon_size;
    }


  bExitNoSave->setEnabled (enable);
  bReset->setEnabled (enable);


  //  Only enable UNDO if we saved an undo block.

  if (misc.undo_count)
    {
      bUndo->setEnabled (enable);
    }
  else
    {
      bUndo->setEnabled (false);
    }


  bDisplayFeature->setEnabled (enable);

  if (!enable || (enable && options.display_feature))
    {
      bDisplayChildren->setEnabled (enable);
      bDisplayFeatureInfo->setEnabled (enable);

      bAddFeature->setEnabled (enable);
      if ((!enable || (enable && misc.visible_feature_count)) && !misc.abe_share->read_only)
        {
          bEditFeature->setEnabled (enable);
          bMoveFeature->setEnabled (enable);
          bDeleteFeature->setEnabled (enable);
          bVerifyFeatures->setEnabled (enable);
        }
      else
        {
          bEditFeature->setEnabled (false);
          bMoveFeature->setEnabled (false);
          bDeleteFeature->setEnabled (false);
          bVerifyFeatures->setEnabled (false);
        }
    }


  //  Only enable the Display Messages option if we have messages to display.

  if (enable && misc.messages->size ())
    {
      messageAction->setEnabled (enable);
    }
  else
    {
      messageAction->setEnabled (false);
    }


  //  Only enable these if we're not in read only mode.

  if (!misc.abe_share->read_only)
    {
      bExitSave->setEnabled (enable);
      bExitMask->setEnabled (enable);
      saveExitAction->setEnabled (enable);
      maskExitAction->setEnabled (enable);
      bUnload->setEnabled (enable);
    }
  else
    {
      bExitSave->setEnabled (false);
      bExitMask->setEnabled (false);
      saveExitAction->setEnabled (false);
      maskExitAction->setEnabled (false);
      bUnload->setEnabled (false);
    }


  bDisplayReference->setEnabled (enable);

  bDisplayManInvalid->setEnabled (enable);
  bDisplayFltInvalid->setEnabled (enable);
  bDisplayNull->setEnabled (enable);

  if (!enable || (enable && misc.num_lines)) bDisplayAll->setEnabled (enable);

  bUndisplaySingle->setEnabled (enable);
  bDisplayMultiple->setEnabled (enable);

  bContour->setEnabled (enable);

  bPrefs->setEnabled (enable);


  //  Stop is always the inverse.

  bStop->setEnabled (!enable);


  bDeletePoint->setEnabled (enable);
  bDeleteRect->setEnabled (enable);
  bDeletePoly->setEnabled (enable);
  bKeepPoly->setEnabled (enable);
  bHotkeyPoly->setEnabled (enable);
  bRestoreRect->setEnabled (enable);
  bRestorePoly->setEnabled (enable);

  bAreaFilter->setEnabled (enable);
  bStatFilter->setEnabled (enable);
  bHPFilter->setEnabled (enable);
  bAttrFilter->setEnabled (enable);


  //  We don't allow filtering by attribute if we're coloring by depth

  if (!options.color_index)
    {
      bAttrFilter->setEnabled (false);
    }
  else
    {
      bAttrFilter->setEnabled (enable);
    }


  if (misc.slice)
    {
      QString string;
      if (misc.slice_meters < 10.0)
        {
          string = QString ("%L1").arg (misc.slice_meters, 0, 'f', 2);
        }
      else if (misc.slice_meters < 100.0)
        {
          string = QString ("%L1").arg (misc.slice_meters, 0, 'f', 1);
        }
      else
        {
          string = QString ("%1").arg (NINT (misc.slice_meters));
        }
      meterLabel->setText (string);
    }
  else
    {
      meterLabel->setText ("");
    }


  //  Set the color of the attributes that are duplicated in the Status tab to a blue fade.

  for (int32_t j = 0 ; j < misc.abe_share->open_args[0].head.num_ndx_attr ; j++)
    {
      if (misc.attrBoxFlag[j])
        {
          attrBoxName[j]->setPalette (colorBoxPalette[j % 2]);
        }
      else
        {
          attrBoxName[j]->setPalette (label_palette[j % 2]);
        }
    }


  //  If we're coloring by an attribute we need to set the attribute name on the Attributes tab to a red fade.

  if (options.color_index >= PRE_ATTR)
    {
      int32_t j = misc.attrStatNum[options.color_index - PRE_ATTR];

      attrBoxName[j]->setPalette (colorPalette[j % 2]);
    }


  //  We don't want to allow the use of the filter mask buttons if the filter is already active.

  if (filter_active)
    {
      bFilterRectMask->setEnabled (false);
      bFilterPolyMask->setEnabled (false);
    }
  else
    {
      bFilterRectMask->setEnabled (enable);
      bFilterPolyMask->setEnabled (enable);
    }


  //  Check for latitude/longitude or northing/easting display

  if (options.lat_or_east)
    {
      latName->setText (tr ("Northing"));
      latName->setToolTip (tr ("Northing of cursor (zone %1), click to switch to latitude and longitude").arg (zoneName));
      latName->setWhatsThis (tr ("This is the UTM northing of the center of the bin that is nearest to the cursor.<br><br>"
                                 "<b>Click this button to switch to displaying latitude and longitude.</b>"));
      latLabel->setToolTip (tr ("Northing of cursor (zone %1)").arg (zoneName));

      lonName->setText (tr ("Easting"));
      lonName->setToolTip (tr ("Easting of cursor (zone %1)").arg (zoneName));
      lonName->setWhatsThis (tr ("This is the UTM easting of the center of the bin that is nearest to the cursor.<br><br>"
                                 "<b>Click this button to switch to displaying latitude and longitude.</b>"));
      lonLabel->setToolTip (tr ("Easting of cursor (zone %1)").arg (zoneName));
    }
  else
    {
      latName->setText (tr ("Latitude"));
      latName->setToolTip (tr ("Latitude of cursor, click to switch to northing and easting"));
      latName->setWhatsThis (tr ("This is the latitude of the center of the bin that is nearest to the cursor.  The format of the latitude "
                                 "may be changed in the Position Format tab of the <b>Preferences</b> dialog "
                                 "<img source=\":/icons/prefs.png\">.<br><br>"
                                 "<b>Click this button to switch to displaying northing and easting.</b>"));
      latLabel->setToolTip (tr ("Latitude of cursor"));

      lonName->setText (tr ("Longitude"));
      lonName->setWhatsThis (tr ("This is the longitude of the center of the bin that is nearest to the cursor.  The format of the longitude "
                                 "may be changed in the Position Format tab of the <b>Preferences</b> dialog "
                                 "<img source=\":/icons/prefs.png\">."));
      lonName->setToolTip (tr ("Longitude of cursor"));
      lonLabel->setToolTip (tr ("Longitude of cursor"));
    }



  bClearMasks->setEnabled (enable);


  if (!enable || (enable && options.display_reference)) 
    {
      bReferencePoly->setEnabled (enable);
      bUnreferencePoly->setEnabled (enable);
    }


  bFlag->setIcon (flagIcon[options.flag_index]);
  bFlag->setEnabled (enable);


  if (!misc.gsf_present)
    {
      bGSFMonitor->setEnabled (false);
    }
  else
    {
      bGSFMonitor->setEnabled (enable);
    }


  czmilReprocessButtons->setContents (QPixmap (":/icons/czmil_reprocess_buttons_11.png"));
  bCZMILReprocessLand->setIcon (QIcon (":/icons/czmil_reprocess_land_poly.png"));
  bCZMILReprocessWater->setIcon (QIcon (":/icons/czmil_reprocess_water_poly.png"));
  bCZMILReprocessShallowWater->setIcon (QIcon (":/icons/czmil_reprocess_shallow_water_poly.png"));


  //  We only allow CZMIL reprocessing when we have only one PFM opened.

  if (misc.czmil_proc_attr < 0 || misc.abe_share->pfm_count > 1 || !cr_available || !misc.czmil_present)
    {
      bCZMILReprocessLand->setEnabled (false);
      bCZMILReprocessWater->setEnabled (false);
      bCZMILReprocessShallowWater->setEnabled (false);
    }
  else
    {
      QString mask_icon = QString (":/icons/czmil_reprocess_buttons_%1%2.png").arg (options.czmil_reprocess_flags[0]).arg (options.czmil_reprocess_flags[1]);
      czmilReprocessButtons->setContents (QPixmap (mask_icon));

      if (options.czmil_reprocess_mode)
        {
          bCZMILReprocessLand->setIcon (QIcon (":/icons/czmil_reprocess_land_poly.png"));
          bCZMILReprocessWater->setIcon (QIcon (":/icons/czmil_reprocess_water_poly.png"));
          bCZMILReprocessShallowWater->setIcon (QIcon (":/icons/czmil_reprocess_shallow_water_poly.png"));
        }
      else
        {
          bCZMILReprocessLand->setIcon (QIcon (":/icons/czmil_reprocess_land_rect.png"));
          bCZMILReprocessWater->setIcon (QIcon (":/icons/czmil_reprocess_water_rect.png"));
          bCZMILReprocessShallowWater->setIcon (QIcon (":/icons/czmil_reprocess_shallow_water_rect.png"));
        }

      bCZMILReprocessLand->setEnabled (enable);
      bCZMILReprocessWater->setEnabled (enable);
      bCZMILReprocessShallowWater->setEnabled (enable);
    }


  if (!misc.czmil_present)
    {
      bCZMILwaveMonitor->setEnabled (false);
      //bCPFFilter->setEnabled (false);

      bCZMILUrbanFilter->setEnabled (false);
    }
  else
    {
      bCZMILwaveMonitor->setEnabled (enable);
      //bCPFFilter->setEnabled (enable);

      //  Check for D_index and urban noise flags attributes

      if (misc.czmil_d_index_attr < 0 || misc.czmil_urban_attr < 0)
        {
          bCZMILUrbanFilter->setEnabled (false);
        }
      else
        {
          bCZMILUrbanFilter->setEnabled (true);
        }
    }


  if (!misc.las_present)
    {
      bLASwaveMonitor->setEnabled (false);
    }
  else
    {
      bLASwaveMonitor->setEnabled (enable);
    }


  if (!misc.hydro_lidar_present)
    {
      bWaveformMonitor->setEnabled (false);
      bWaveWaterfallShallow->setEnabled (false);
      bWaveWaterfallDeep->setEnabled (false);
    }
  else
    {
      bWaveformMonitor->setEnabled (enable);
      bWaveWaterfallShallow->setEnabled (enable);
      bWaveWaterfallDeep->setEnabled (enable);
    }

  if (!misc.hof_present)
    {
      bHOFWaveFilter->setEnabled (false);
    }
  else
    {
      bHOFWaveFilter->setEnabled (enable);
    }

  if (!misc.lidar_present && !misc.las_present)
    {
      bChartsPic->setEnabled (false);
      bLidarMonitor->setEnabled (false);
    }
  else
    {
      //  We don't want to enable the chartsPic button for (only) LAS data but we do want to enable the lidarMonitor button.

      if (misc.lidar_present)
        {
          bChartsPic->setEnabled (enable);
        }
      else
        {
          bChartsPic->setEnabled (false);
        }

      bLidarMonitor->setEnabled (enable);
    }


  //  Make sure we have CZMIL, LAS, CHARTS HOF, or CHARTS TOF classification in order to enable the "Change LiDAR classification" button.

  if (misc.lidar_class_attr >= 0)
    {
      bChangeLidarClass->setEnabled (enable);
    }
  else
    {
      bChangeLidarClass->setEnabled (false);
    }


  //  We only want to enable the clear highlight/invert buttons if the highlights aren't caused by the "flag type" button.

  if (!enable || (enable && misc.highlight_count && !(options.flag_index)))
    {
      bInvertHighlight->setEnabled (enable);
      bClearHighlight->setEnabled (enable);
    }
  bHighlightPoly->setEnabled (enable);


  //  We only want to enable the clear poly button if the highlights aren't caused by the "flag type" button or if there are filter kill points.

  if (!enable || (enable && (misc.highlight_count | misc.filter_kill_count) && !(options.flag_index)))
    {
      bClearPoly->setEnabled (enable);
    }


  bMeasure->setEnabled (enable);
  bMaskInsideRect->setEnabled (enable);
  bMaskInsidePoly->setEnabled (enable);
  bMaskOutsideRect->setEnabled (enable);
  bMaskOutsidePoly->setEnabled (enable);

  bHide->setEnabled (enable);

  if (!enable)
    {
      bMaskClear->setEnabled (enable);
      bMaskReset->setEnabled (enable);
      bFlagReset->setEnabled (enable);
    }
  else
    {
      if (options.mask_active & MASK_SHAPE) bMaskClear->setEnabled (enable);
      if (options.mask_active & MASK_TYPE) bMaskReset->setEnabled (enable);
      if (options.mask_active & MASK_FLAG) bFlagReset->setEnabled (enable);
    }


  //  Set the "color by" labels.

  for (int32_t i = 0 ; i < PRE_ATTR + POST_ATTR ; i++)
    {
      attrName[i]->setPalette (label_palette[i % 2]);
      attrValue[i]->setPalette (label_palette[i % 2]);

      if (i >= PRE_ATTR)
        {
          int32_t attr_ndx = i - PRE_ATTR;

          if (attr_ndx < misc.abe_share->open_args[0].head.num_ndx_attr)
            {
              attrName[i]->setFrameStyle (QFrame::Panel | QFrame::Raised);
              attrName[i]->setEnabled (true);
            }
          else
            {
              attrName[i]->setPalette (blankPalette);
              attrName[i]->setFrameStyle (QFrame::Panel | QFrame::Plain);
              attrName[i]->setEnabled (false);

              attrValue[i]->setPalette (blankPalette);
            }
        }
    }


  int32_t ndx = options.color_index;
  attrName[ndx]->setPalette (colorPalette[ndx % 2]);


  for (int32_t i = 0 ; i < POST_ATTR ; i++)
    {
      if (misc.attrStatNum[i] > -1)
        {
          attrName[i + PRE_ATTR]->setEnabled (true);

          if (misc.attrStatNum[i] == misc.time_attr)
            {
              attrName[i + PRE_ATTR]->setToolTip (QString (tr ("Date/Time (minutes) - Click to color by this attribute.")));
              attrName[i + PRE_ATTR]->setWhatsThis (QString (tr ("Date/Time (minutes) - Click to color by this attribute.")));
              attrValue[i + PRE_ATTR]->setToolTip (tr ("Value of Date/Time (minutes) at the cursor."));
              attrValue[i + PRE_ATTR]->setWhatsThis (tr ("Value of Date/Time (minutes) at the cursor."));
            }
          else
            {
              attrName[i + PRE_ATTR]->setToolTip (options.attrStatName[i] + QString (tr (" - Click to color by this attribute.")));
              attrValue[i + PRE_ATTR]->setToolTip (tr ("Value of %1 at the cursor.").arg (options.attrStatName[i]));

              if (misc.attrStatNum[i] == misc.czmil_urban_attr)
                {
                  attrName[i + PRE_ATTR]->setWhatsThis (CZMILUrbanNoiseFlagsText);
                  attrValue[i + PRE_ATTR]->setWhatsThis (CZMILUrbanNoiseFlagsText);
                }
              else
                {
                  attrName[i + PRE_ATTR]->setWhatsThis (options.attrStatName[i] + QString (tr (" - Click to color by this attribute.")));
                  attrValue[i + PRE_ATTR]->setWhatsThis (tr ("Value of %1 at the cursor.").arg (options.attrStatName[i]));
                }
            }
        }
      else
        {
          attrName[i + PRE_ATTR]->setEnabled (false);
          attrName[i + PRE_ATTR]->setToolTip (tr ("No attribute selected"));
          attrName[i + PRE_ATTR]->setWhatsThis (tr ("No attribute selected"));
          attrValue[i + PRE_ATTR]->setToolTip (tr ("No attribute selected"));
          attrValue[i + PRE_ATTR]->setWhatsThis (tr ("No attribute selected"));
        }
    }


  //  This just makes sure that the palette is set correctly if we're scaling or offsetting the depth value.

  if (options.z_factor != 1.0 || options.z_offset != 0.0)
    {
      attrPalette[0].setColor (QPalette::WindowText, options.scaled_offset_z_color);
    }
  else
    {
      attrPalette[0].setColor (QPalette::WindowText, misc.widgetForegroundColor);
    }
  attrPalette[0].setColor (QPalette::Window, QColor (255, 255, 255));
  attrValue[0]->setPalette (attrPalette[0]);
}



void 
pfmEdit3D::setFunctionCursor (int32_t function)
{
  //  Default to point mode for the map.  If we want feature mode we'll switch it below.

  map->setMapMode (NVMAPGL_POINT_MODE);


  discardMovableObjects ();


  switch (function)
    {
    case DELETE_RECTANGLE:
      misc.statusProgLabel->setText (tr ("Rectangular delete mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (deleteRectCursor);
      bDeleteRect->setChecked (true);
      break;

    case RESTORE_RECTANGLE:
      misc.statusProgLabel->setText (tr ("Rectangular restore mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (restoreRectCursor);
      bRestoreRect->setChecked (true);
      break;

    case DELETE_POLYGON:
      misc.statusProgLabel->setText (tr ("Polygon delete mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (deletePolyCursor);
      bDeletePoly->setChecked (true);
      break;

    case RESTORE_POLYGON:
      misc.statusProgLabel->setText (tr ("Polygon restore mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (restorePolyCursor);
      bRestorePoly->setChecked (true);
      break;

    case KEEP_POLYGON:
      misc.statusProgLabel->setText (tr ("Delete outside polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (keepPolyCursor);
      bKeepPoly->setChecked (true);
      break;

    case HOTKEY_POLYGON:
      if (misc.hotkey_poly_count)
        {
          misc.statusProgLabel->setText (tr ("Press a hot key"));
        }
      else
        {
          misc.statusProgLabel->setText (tr ("Polygon hot key mode"));
        }
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (hotkeyPolyCursor);
      bHotkeyPoly->setChecked (true);
      break;

    case SET_REFERENCE:
      misc.statusProgLabel->setText (tr ("Polygon set reference mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (referencePolyCursor);
      bReferencePoly->setChecked (true);
      break;

    case UNSET_REFERENCE:
      misc.statusProgLabel->setText (tr ("Polygon unset reference mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (unreferencePolyCursor);
      bUnreferencePoly->setChecked (true);
      break;

    case MOVE_FEATURE:
      misc.statusProgLabel->setText (tr ("Move feature mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (moveFeatureCursor);
      map->setMapMode (NVMAPGL_FEATURE_MODE);
      bMoveFeature->setChecked (true);


      //  Have to unfreeze if we go to this mode.

      misc.marker_mode = 0;
      break;

    case DELETE_FEATURE:
      misc.statusProgLabel->setText (tr ("Invalidate feature mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (deleteFeatureCursor);
      map->setMapMode (NVMAPGL_FEATURE_MODE);
      bDeleteFeature->setChecked (true);


      //  Have to unfreeze if we go to this mode.

      misc.marker_mode = 0;
      break;

    case EDIT_FEATURE:
      misc.statusProgLabel->setText (tr ("Edit feature mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (addFeatureCursor);
      map->setMapMode (NVMAPGL_FEATURE_MODE);
      bEditFeature->setChecked (true);


      //  Have to unfreeze if we go to this mode.

      misc.marker_mode = 0;
      break;

    case ADD_FEATURE:
      misc.statusProgLabel->setText (tr ("Add feature mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (addFeatureCursor);
      bAddFeature->setChecked (true);


      //  Have to unfreeze if we go to this mode.

      misc.marker_mode = 0;


      //  Unlike the other FEATURE modes we don't set NVMAPGL_FEATURE_MODE for ADD_FEATURE because we will be
      //  searching for the nearest point so that we can attach a feature to it.


      break;

    case DELETE_POINT:
      misc.statusProgLabel->setText (tr ("Delete subrecord/record mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (Qt::PointingHandCursor);
      bDeletePoint->setChecked (true);
      break;

    case UNSET_SINGLE:
      misc.statusProgLabel->setText (tr ("Hide single file"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (Qt::ArrowCursor);


      //  Have to unfreeze if we go to this mode.

      misc.marker_mode = 0;
      break;

    case SET_MULTIPLE:
      misc.statusProgLabel->setText (tr ("Select multiple files mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (Qt::ArrowCursor);


      //  Have to unfreeze if we go to this mode.

      misc.marker_mode = 0;
      break;

    case ROTATE:
      misc.statusProgLabel->setText (tr ("Rotate data"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (rotateCursor);
      if (misc.need_sparse) map->setMapMode (NVMAPGL_SPARSE_MODE);
      break;

    case ZOOM:
      misc.statusProgLabel->setText (tr ("Zoom in/out"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (zoomCursor);
      if (misc.need_sparse) map->setMapMode (NVMAPGL_SPARSE_MODE);
      break;

    case RECT_FILTER_MASK:
      misc.statusProgLabel->setText (tr ("Rectangle filter mask mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (filterMaskRectCursor);
      bFilterRectMask->setChecked (true);
      break;

    case POLY_FILTER_MASK:
      misc.statusProgLabel->setText (tr ("Polygon filter mask mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (filterMaskPolyCursor);
      bFilterPolyMask->setChecked (true);
      break;

    case MASK_INSIDE_RECTANGLE:
      misc.statusProgLabel->setText (tr ("Hide data inside rectangle mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (maskCursor);

      misc.marker_mode = 0;
      break;

    case MASK_OUTSIDE_RECTANGLE:
      misc.statusProgLabel->setText (tr ("Hide data outside rectangle mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (maskCursor);

      misc.marker_mode = 0;
      break;

    case MASK_INSIDE_POLYGON:
      misc.statusProgLabel->setText (tr ("Hide data inside polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (maskCursor);

      misc.marker_mode = 0;
      break;

    case MASK_OUTSIDE_POLYGON:
      misc.statusProgLabel->setText (tr ("Hide data outside polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (maskCursor);

      misc.marker_mode = 0;
      break;

    case MEASURE:
      misc.statusProgLabel->setText (tr ("Measuring mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (measureCursor);
      break;

    case HIGHLIGHT_POLYGON:
      misc.statusProgLabel->setText (tr ("Highlight points in polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (highlightPolyCursor);
      bHighlightPoly->setChecked (true);
      break;

    case CLEAR_POLYGON:
      misc.statusProgLabel->setText (tr ("Clear highlights in polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (clearPolyCursor);
      bClearPoly->setChecked (true);
      break;

    case CZMIL_REPROCESS_LAND:
      misc.statusProgLabel->setText (tr ("Reprocess CZMIL points in polygon mode (land)"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      if (options.czmil_reprocess_mode)
        {
          map->setCursor (czmilReprocessPolyCursor);
        }
      else
        {
          map->setCursor (czmilReprocessRectCursor);
        }
      bCZMILReprocessLand->setChecked (true);
      break;

    case CZMIL_REPROCESS_WATER:
      misc.statusProgLabel->setText (tr ("Reprocess CZMIL points in polygon mode (water)"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      if (options.czmil_reprocess_mode)
        {
          map->setCursor (czmilReprocessPolyCursor);
        }
      else
        {
          map->setCursor (czmilReprocessRectCursor);
        }
      bCZMILReprocessWater->setChecked (true);
      break;

    case CZMIL_REPROCESS_SHALLOW_WATER:
      misc.statusProgLabel->setText (tr ("Reprocess CZMIL points in polygon mode (shallow water)"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      if (options.czmil_reprocess_mode)
        {
          map->setCursor (czmilReprocessPolyCursor);
        }
      else
        {
          map->setCursor (czmilReprocessRectCursor);
        }
      bCZMILReprocessShallowWater->setChecked (true);
      break;

    case SHIFT_SLICE:
      misc.statusProgLabel->setText (tr ("Shift slice mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (sliceCursor);
      break;

    case CPF_FILTER:
      misc.statusProgLabel->setText (tr ("Filter CPF data in polygon mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (cpfFilterCursor);
      //bCPFFilter->setChecked (true);
      break;

    case CHANGE_LIDAR_CLASS:
      misc.statusProgLabel->setText (tr ("Change LiDAR classification mode"));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (changeLidarClassCursor);
      bChangeLidarClass->setChecked (true);
      break;
    }
}



void 
pfmEdit3D::discardMovableObjects ()
{
  map->closeMovingList (&mv_tracker);
  trackMap->closeMovingPath (&mv_2D_tracker);
  map->closeMovingList (&mv_measure_anchor);
  map->closeMovingList (&mv_measure);
  map->discardRubberbandRectangle (&rb_rectangle);
  map->discardRubberbandPolygon (&rb_polygon);
  map->discardRubberbandLine (&rb_measure);
  map->discardRubberbandLine (&rb_dist);


  for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++) map->closeMovingList (&(multi_marker[i]));
}



void 
pfmEdit3D::leftMouse (int32_t mouse_x, int32_t mouse_y, double lon __attribute__ ((unused)), double lat __attribute__ ((unused)), double z __attribute__ ((unused)))
{
  int32_t *px, *py;
  uint8_t failed;

  void get_feature_event_time (MISC *misc);


  //  If the popup menu is up discard this mouse press

  if (popup_active)
    {
      //  Flush the OpenGL scene so the menu will disperse

      map->flush ();


      popup_active = NVFalse;
      return;
    }


  switch (options.function)
    {
    case DELETE_POINT:
      break;
    
    case DELETE_RECTANGLE:
    case RESTORE_RECTANGLE:
    case MASK_INSIDE_RECTANGLE:
    case MASK_OUTSIDE_RECTANGLE:
    case RECT_FILTER_MASK:

      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, mouse_x, mouse_y, &px, &py);


          //  Invalidate, restore, or mask the area.

          qApp->setOverrideCursor (Qt::WaitCursor);
          qApp->processEvents ();

          set_area (map, &options, &misc, px, py, 4, 1);

          qApp->restoreOverrideCursor ();


          map->discardRubberbandRectangle (&rb_rectangle);


          if (options.function == MASK_INSIDE_RECTANGLE || options.function == MASK_OUTSIDE_RECTANGLE) options.mask_active |= MASK_SHAPE;

          if (options.function == RECT_FILTER_MASK)
            {
              misc.filter_mask = NVTrue;


              //  If the filter message box is up then we are filter masking after the filter so we want to modify the 
              //  current filter points to remove those in masked areas.  We set the index to -1 to indicate that we've 
              //  removed the filter point.  We used to re-run the filter but, with the advent of the hofWaveFilter and
              //  the Hockey Puck filter this became too time consuming.

              if (misc.filter_kill_count)
                {
                  for (int32_t i = 0 ; i < misc.filter_kill_count ; i++)
                    {
                      if (misc.data[misc.filter_kill_list[i]].fmask) misc.filter_kill_list[i] = -1;
                    }
                }
            }


          redrawMap (NVTrue);


          //  In the event of a deletion that hits any frozen markers on the screen, we will unfreeze the markers.
          //  Otherwise, they will stay frozen and rooted

          if (misc.marker_mode)
            {
              if (misc.data[misc.frozen_point].val & PFM_MANUALLY_INVAL)
                {
                  misc.marker_mode = 0;
                }
              else if (misc.abe_share->mwShare.multiMode == 0 && misc.marker_mode == 2)
                {
                  for (int i = 1; i < MAX_STACK_POINTS; i++)
                    {
                      if (misc.data[misc.abe_share->mwShare.multiLocation[i]].val & PFM_MANUALLY_INVAL)
                        {
                          misc.marker_mode = 0;
                          break;
                        }
                    }
                }                                                  

              slotMouseMove((QMouseEvent *)NULL, misc.data[misc.frozen_point].x, misc.data[misc.frozen_point].y,
                            misc.data[misc.frozen_point].z, mapdef);
            }


          //  Even though we do this in redrawMap we need to do it again here because otherwise the damn markers disappear.
          //  It's OpenGL, I have no idea why.

          if (options.function == RECT_FILTER_MASK) overlayData (map, &options, &misc);


          setFunctionCursor (options.function);
        }
      else
        {
          map->anchorRubberbandRectangle (&rb_rectangle, mouse_x, mouse_y, options.edit_color, 2, Qt::SolidLine);
        }
      break;

    case DELETE_POLYGON:
    case RESTORE_POLYGON:
    case KEEP_POLYGON:
    case SET_REFERENCE:
    case UNSET_REFERENCE:
    case HOTKEY_POLYGON:
    case MASK_INSIDE_POLYGON:
    case MASK_OUTSIDE_POLYGON:
    case POLY_FILTER_MASK:
    case HIGHLIGHT_POLYGON:
    case CLEAR_POLYGON:
    case CPF_FILTER:
    case CHANGE_LIDAR_CLASS:
      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          int32_t count;
          map->closeRubberbandPolygon (rb_polygon, mouse_x, mouse_y, &count, &px, &py);


          //  Invalidate or restore the area.

          qApp->setOverrideCursor (Qt::WaitCursor);
          qApp->processEvents ();

          switch (options.function)
            {
            case DELETE_POLYGON:
            case RESTORE_POLYGON:
            case SET_REFERENCE:
            case UNSET_REFERENCE:
            case HIGHLIGHT_POLYGON:
            case CLEAR_POLYGON:
            case CHANGE_LIDAR_CLASS:

              set_area (map, &options, &misc, px, py, count, 0);


              //  In the event of a deletion that hits any frozen markers on the screen, we will unfreeze the markers.
              //  Otherwise, they will stay frozen and rooted

              if (misc.marker_mode)
                {
                  if (misc.data[misc.frozen_point].val & PFM_MANUALLY_INVAL)
                    {
                      misc.marker_mode = 0;
                    }
                  else if (misc.abe_share->mwShare.multiMode == 0 && misc.marker_mode == 2)
                    {
                      for (int i = 1; i < MAX_STACK_POINTS; i++)
                        {
                          if (misc.data[misc.abe_share->mwShare.multiLocation[i]].val & PFM_MANUALLY_INVAL)
                            {
                              misc.marker_mode = 0;
                              break;
                            }
                        }
                    }
                  slotMouseMove ((QMouseEvent *)NULL, misc.data[misc.frozen_point].x, misc.data[misc.frozen_point].y,
                                 misc.data[misc.frozen_point].z, mapdef);
                }


              if (options.function == CLEAR_POLYGON || options.function == HIGHLIGHT_POLYGON)
                {
                  //  Since we decided to manually clear or set some points let's make sure that we weren't highlighting by flag.

                  options.flag_index = 0;
                  bFlag->setIcon (flagIcon[options.flag_index]);
                  disconnect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
                  flag[0]->setChecked (true);
                  connect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
                }

              break;

            case POLY_FILTER_MASK:
              set_area (map, &options, &misc, px, py, count, 0);
              misc.filter_mask = NVTrue;


              //  If the filter message box is up then we are filter masking after the filter so we want to modify the 
              //  current filter points to remove those in masked areas.  We set the index to -1 to indicate that we've 
              //  removed the filter point.  We used to re-run the filter but, with the advent of the hofWaveFilter and
              //  the Hockey Puck filter this became too time consuming.

              if (misc.filter_kill_count)
                {
                  for (int32_t i = 0 ; i < misc.filter_kill_count ; i++)
                    {
                      if (misc.data[misc.filter_kill_list[i]].fmask) misc.filter_kill_list[i] = -1;
                    }
                }
              break;

            case MASK_INSIDE_POLYGON:
            case MASK_OUTSIDE_POLYGON:
              set_area (map, &options, &misc, px, py, count, 0);
              options.mask_active |= MASK_SHAPE;
              break;

            case KEEP_POLYGON:
              keep_area (map, &options, &misc, px, py, count);
              break;

            case HOTKEY_POLYGON:
              misc.hotkey_poly_count = count;


              //  We have to check because we might have freed this memory in ancillaryProg but, on
              //  the other hand, the user may have aborted the operation so we still might need to
              //  free it here.

              if (misc.hotkey_poly_x)
                {
                  free (misc.hotkey_poly_x);
                  free (misc.hotkey_poly_y);
                }

              misc.hotkey_poly_x = (int32_t *) malloc (count * sizeof (int32_t));
              misc.hotkey_poly_y = (int32_t *) malloc (count * sizeof (int32_t));

              if (misc.hotkey_poly_y == NULL)
                {
                  QString msg = tr ("Error allocating memory - %1 %2 %3 %4\n").arg (misc.progname).arg (__FILE__).arg (__FUNCTION__).arg (__LINE__);
                  qDebug () << msg;
                  clean_exit (-1);
                }

              for (int32_t i = 0 ; i < count ; i++)
                {
                  misc.hotkey_poly_x[i] = px[i];
                  misc.hotkey_poly_y[i] = py[i];
                }

              //  Now we wait for the user to press a hotkey.  Take a look at keyPressEvent and ancillaryProg.

              break;


            case CPF_FILTER:

              //  Get the validate spin box value.

              options.czmil_cpf_filter_amp_min = valSpin->value ();


              setWidgetStates (NVFalse);

              misc.busy = NVTrue;

              failed = NVFalse;
              cpfExf = new externalFilter (this, map, &options, &misc, CPF_DEEP_FILTER, &failed, px, py, count);
              if (failed)
                {
                  qApp->restoreOverrideCursor ();
                  misc.busy = NVFalse;
                  redrawMap (NVFalse);
                }
              else
                {
                  connect (cpfExf, SIGNAL (externalFilterDone ()), this, SLOT (slotCPFExternalFilterDone ()));
                  connect (cpfExf, SIGNAL (readStandardErrorSignal (QProcess *)), this, SLOT (slotReadStandardError (QProcess *)));
                }

              map->setFocus (Qt::OtherFocusReason);

              break;
            }


          prev_poly_x = -1;


          //  Don't redraw if we're setting up for a hotkey polygon or we're running CZMIL filtering

          if (options.function != HOTKEY_POLYGON && options.function != CPF_FILTER)
            {
              qApp->restoreOverrideCursor ();

              map->discardRubberbandPolygon (&rb_polygon);
              redrawMap (NVTrue);


              //  Even though we do this in redrawMap we need to do it again here because otherwise the damn markers disappear.
              //  It's OpenGL, I have no idea why.

              if (options.function == POLY_FILTER_MASK) overlayData (map, &options, &misc);
            }
        }
      else
        {
          //  Since we decided to manually highlight data points we want to turn off any highlighting based on flag type

          if (options.function == HIGHLIGHT_POLYGON)
            {
              options.flag_index = 0;
              bFlag->setIcon (flagIcon[options.flag_index]);
              disconnect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
              flag[0]->setChecked (true);
              connect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
            }


          //  Start the polygon.

          map->anchorRubberbandPolygon (&rb_polygon, mouse_x, mouse_y, options.edit_color, LINE_WIDTH, NVTrue, Qt::SolidLine);
          prev_poly_x = -1;
        }
      break;


    case CZMIL_REPROCESS_LAND:
    case CZMIL_REPROCESS_WATER:
    case CZMIL_REPROCESS_SHALLOW_WATER:

      //  Polygon mode

      if (options.czmil_reprocess_mode)
        {
          if (map->rubberbandPolygonIsActive (rb_polygon))
            {
              //  Save the function mode since we know we're going to exit and restart.

              misc.save_mode = options.function;


              int32_t count;
              map->closeRubberbandPolygon (rb_polygon, mouse_x, mouse_y, &count, &px, &py);


              //  Reprocess the polygon.

              qApp->setOverrideCursor (Qt::WaitCursor);
              qApp->processEvents ();

              failed = NVFalse;
              czmil_reprocess = new czmilReprocess (this, map, &options, &misc, px, py, count, options.function, &failed);
              if (failed)
                {
                  qApp->restoreOverrideCursor ();
                  redrawMap (NVTrue);
                }
              else
                {
                  //  Note that we don't have a "process done" signal connection here.  The reason being that the czmilReprocessFlag will
                  //  be set instead.  That flag is monitored in slotTrackCursor.  We do it that way so we can delay the close/save/reopen
                  //  if the error messages dialog is up.  We do however automatically close the error messages dialog so we don't hang
                  //  this up.

                  if (displayMessage_dialog != NULL) displayMessage_dialog->closeMessage ();


                  //  We probably don't want to process the warnings from czmilReprocess.

                  if (!options.czmil_suppress_warnings) connect (czmil_reprocess, SIGNAL (readStandardErrorSignal (QProcess *)), this,
                                                                 SLOT (slotReadStandardError (QProcess *)));
                }
            }
          else
            {
              //  Start the polygon.

              map->anchorRubberbandPolygon (&rb_polygon, mouse_x, mouse_y, options.edit_color, LINE_WIDTH, NVTrue, Qt::SolidLine);
              prev_poly_x = -1;
            }
        }


      //  Rectangle mode

      else
        {
          if (map->rubberbandRectangleIsActive (rb_rectangle))
            {
              //  Save the function mode since we know we're going to exit and restart.

              misc.save_mode = options.function;


              map->closeRubberbandRectangle (rb_rectangle, mouse_x, mouse_y, &px, &py);


              //  Reprocess the rectangle.

              qApp->setOverrideCursor (Qt::WaitCursor);
              qApp->processEvents ();


              //  Note that we're forcing the point count to 0 to indicate to czmilReprocess that we're processing a rectangle.

              failed = NVFalse;
              czmil_reprocess = new czmilReprocess (this, map, &options, &misc, px, py, 0, options.function, &failed);
              if (failed)
                {
                  qApp->restoreOverrideCursor ();
                  redrawMap (NVTrue);
                }
              else
                {
                  //  Note that we don't have a "process done" signal connection here.  The reason being that the czmilReprocessFlag will
                  //  be set instead.  That flag is monitored in slotTrackCursor.  We do it that way so we can delay the close/save/reopen
                  //  if the error messages dialog is up.  We do however automatically close the error messages dialog so we don't hang
                  //  this up.

                  if (displayMessage_dialog != NULL) displayMessage_dialog->closeMessage ();


                  //  We probably don't want to process the warnings from czmilReprocess.

                  if (!options.czmil_suppress_warnings) connect (czmil_reprocess, SIGNAL (readStandardErrorSignal (QProcess *)), this,
                                                                 SLOT (slotReadStandardError (QProcess *)));
                }
            }
          else
            {
              map->anchorRubberbandRectangle (&rb_rectangle, mouse_x, mouse_y, options.edit_color, 2, Qt::SolidLine);
            }
        }
      break;


    case MEASURE:
      if (map->rubberbandLineIsActive (rb_measure))
        {
          map->discardRubberbandLine (&rb_measure);
          map->closeMovingList (&mv_measure_anchor);
          map->closeMovingList (&mv_measure);
          map->flush ();
        }
      else
        {
          if (mv_measure_anchor < 0)
            {
              map->setMovingList (&mv_measure_anchor, marker, 16, mouse_x, mouse_y, 0.0, 2, options.edit_color);
            }
          else
            {
              map->setMovingList (&mv_measure, marker, 16, mouse_x, mouse_y, 0.0, 2, options.edit_color);
            }


          //  Snap the start of the line to the nearest point

          int32_t pix_x, pix_y;
          map->get2DCoords (misc.data[misc.nearest_point].x, misc.data[misc.nearest_point].y, -misc.data[misc.nearest_point].z, &pix_x, &pix_y);

          map->anchorRubberbandLine (&rb_measure, pix_x, pix_y, options.edit_color, LINE_WIDTH, Qt::SolidLine);
          line_anchor.x = misc.data[misc.nearest_point].x;
          line_anchor.y = misc.data[misc.nearest_point].y;
          line_anchor.z = misc.data[misc.nearest_point].z;
        }
      break;


    case ADD_FEATURE:

      //  Add feature at nearest_point

      misc.add_feature_index = misc.nearest_point;


      memset (&misc.new_record, 0, sizeof (BFDATA_RECORD));

      get_feature_event_time (&misc);

      misc.new_record.record_number = misc.bfd_header.number_of_records;
      misc.new_record.length = 0.0;
      misc.new_record.width = 0.0;
      misc.new_record.height = 0.0;
      misc.new_record.confidence_level = 3;
      misc.new_record.depth = (float) misc.data[misc.nearest_point].z;
      misc.new_record.horizontal_orientation = 0.0;
      misc.new_record.vertical_orientation = 0.0;
      strcpy (misc.new_record.description, "");
      strcpy (misc.new_record.remarks, "");
      misc.new_record.latitude = misc.data[misc.nearest_point].y;
      misc.new_record.longitude = misc.data[misc.nearest_point].x;
      strcpy (misc.new_record.analyst_activity, "NAVOCEANO BHY");
      misc.new_record.equip_type = 3;
      misc.new_record.nav_system = 1;
      misc.new_record.platform_type = 4;
      misc.new_record.sonar_type = 3;

      misc.new_record.poly_count = 0;

      editFeatureNum (-1);

      misc.feature_mod = NVTrue;

      break;


    case EDIT_FEATURE:
      if (misc.nearest_feature != -1) editFeatureNum (misc.nearest_feature);
      break;


    case DELETE_FEATURE:
      if (misc.nearest_feature != -1)
        {
          //  Invalidate feature at nearest_feature

          BFDATA_RECORD bfd_record;
          if (binaryFeatureData_read_record (misc.bfd_handle, misc.nearest_feature, &bfd_record) < 0)
            {
              QString msg = QString (binaryFeatureData_strerror ());
              QMessageBox::warning (this, tr ("Invalidate Feature"), tr ("Unable to read feature record\nReason: %1").arg (msg));
              break;
            }


          //  Zero out the confidence value

          bfd_record.confidence_level = misc.feature[misc.nearest_feature].confidence_level = 0;


          if (binaryFeatureData_write_record (misc.bfd_handle, misc.nearest_feature, &bfd_record, NULL, NULL) < 0)
            {
              QString msg = QString (binaryFeatureData_strerror ());
              QMessageBox::warning (this, tr ("Invalidate Feature"), tr ("Unable to update feature record\nReason: %1").arg (msg));
              break;
            }

          misc.feature_mod = NVTrue;


          options.function = misc.save_function;
          setFunctionCursor (options.function);
          redrawMap (NVFalse);


          //  If the mosaic viewer was running, tell it to redraw.

          misc.abe_share->key = FEATURE_FILE_MODIFIED;
        }
      break;


    case MOVE_FEATURE:

      if (move_feature)
        {
          if (misc.nearest_feature != -1)
            {
              //  Try to unset the PFM_SELECTED_FEATURE/PFM_DESIGNATED_SOUNDING flag on what should have been the last feature data point.

              if (misc.nearest_feature_point != -1)
                {
                  misc.data[misc.nearest_feature_point].val &= ~PFM_SELECTED_FEATURE;
                  misc.data[misc.nearest_feature_point].val &= ~PFM_DESIGNATED_SOUNDING;
                }


              //  Move the nearest_feature to the nearest_point;

              BFDATA_RECORD bfd_record;
              BFDATA_POLYGON bfd_polygon;

              binaryFeatureData_read_record (misc.bfd_handle, misc.feature[misc.nearest_feature].record_number, &bfd_record);

              if (bfd_record.poly_count) binaryFeatureData_read_polygon (misc.bfd_handle, misc.feature[misc.nearest_feature].record_number, &bfd_polygon);

              bfd_record.latitude = misc.feature[misc.nearest_feature].latitude = misc.data[misc.nearest_point].y;
              bfd_record.longitude = misc.feature[misc.nearest_feature].longitude = misc.data[misc.nearest_point].x;
              bfd_record.depth = misc.feature[misc.nearest_feature].depth = misc.data[misc.nearest_point].z;

              binaryFeatureData_write_record (misc.bfd_handle, misc.feature[misc.nearest_feature].record_number, &bfd_record, &bfd_polygon, NULL);

              misc.feature_mod = NVTrue;


              //  Set PFM_SELECTED_FEATURE/PFM_DESIGNATED_SOUNDING on the nearest point to the feature but only if the feature is Hydrographic.

              if (bfd_record.feature_type == BFDATA_HYDROGRAPHIC)
                {
                  if (bfd_record.parent_record)
                    {
                      misc.data[misc.nearest_point].val |= PFM_DESIGNATED_SOUNDING;
                    }
                  else
                    {
                      misc.data[misc.nearest_point].val |= PFM_SELECTED_FEATURE;
                    }
                }

              misc.add_feature_index = -1;


              options.function = misc.save_function;
              move_feature = 0;
              setFunctionCursor (options.function);
              redrawMap (NVFalse);
            }
        }
      else
        {
          //  Now we need to snap to points, not features.

          map->setMapMode (NVMAPGL_POINT_MODE);

          misc.nearest_feature_point = misc.nearest_point;
          move_feature++;
        }
      break;


    case UNSET_SINGLE:
      if (misc.nearest_point != -1)
        {
          if (!misc.num_lines)
            {
              misc.num_lines = misc.line_count - 1;

              for (int32_t i = 0, j = 0 ; i < misc.line_count ; i++)
                {
                  if (misc.data[misc.nearest_point].line != misc.line_number[i])
                    {
                      misc.line_num[j] = misc.line_number[i];
                      j++;
                    }
                }
            }
          else
            {
              for (int32_t i = 0, j = 0 ; i < misc.num_lines ; i++)
                {
                  if (misc.data[misc.nearest_point].line != misc.line_num[i])
                    {
                      misc.line_num[j] = misc.line_num[i];
                      j++;
                    }
                }
              misc.num_lines--;
            }

          bDisplayAll->setEnabled (true);

          redrawMap (NVFalse);
        }
      break;


    case SET_MULTIPLE:
      if (misc.nearest_point != -1)
        {
          local_line_num[local_num_lines] = misc.data[misc.nearest_point].line;
          local_num_lines++;

          bDisplayMultiple->setEnabled (false);
          bDisplayAll->setEnabled (true);
        }
      break;
    }
}



void 
pfmEdit3D::midMouse (int32_t mouse_x __attribute__ ((unused)), int32_t mouse_y __attribute__ ((unused)), double lon __attribute__ ((unused)),
                     double lat __attribute__ ((unused)), double z __attribute__ ((unused)))
{
  switch (options.function)
    {
    case DELETE_POINT:
      if (misc.nearest_point != -1)
        {
          uint32_t rec = misc.data[misc.nearest_point].rec;

          for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
            {
              if (misc.data[i].rec == rec)
                {
                  store_undo (&misc, options.undo_levels, misc.data[i].val, i, NULL);
                  misc.data[i].val |= PFM_MANUALLY_INVAL;


                  //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
                  //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
                  //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
                  //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
                  //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

                  misc.data[i].val &= ~PFM_SUSPECT;
                }
            }
          end_undo_block (&misc);


          //
          //                      if we are frozen, automatically unfreeze
          //

          if (misc.marker_mode) misc.marker_mode = 0;
        }

      redrawMap (NVTrue);
      break;

    case DELETE_RECTANGLE:
    case RESTORE_RECTANGLE:
    case MASK_INSIDE_RECTANGLE:
    case MASK_OUTSIDE_RECTANGLE:
    case RECT_FILTER_MASK:
      map->discardRubberbandRectangle (&rb_rectangle);
      break;

    case DELETE_POLYGON:
    case RESTORE_POLYGON:
    case KEEP_POLYGON:
    case SET_REFERENCE:
    case HOTKEY_POLYGON:
    case MASK_INSIDE_POLYGON:
    case MASK_OUTSIDE_POLYGON:
    case POLY_FILTER_MASK:
    case HIGHLIGHT_POLYGON:
    case CLEAR_POLYGON:
    case CZMIL_REPROCESS_LAND:
    case CZMIL_REPROCESS_WATER:
    case CZMIL_REPROCESS_SHALLOW_WATER:
    case CPF_FILTER:
    case CHANGE_LIDAR_CLASS:
      map->discardRubberbandPolygon (&rb_polygon);
      prev_poly_x = -1;
      break;

    case MEASURE:
      map->discardRubberbandLine (&rb_measure);
      map->closeMovingList (&mv_measure_anchor);
      map->closeMovingList (&mv_measure);
      break;

    case ADD_FEATURE:
    case EDIT_FEATURE:
    case DELETE_FEATURE:
    case UNSET_SINGLE:
      options.function = misc.save_function;
      break;

    case MOVE_FEATURE:
      move_feature = 0;
      options.function = misc.save_function;
      break;

    case SET_MULTIPLE:
      bDisplayMultiple->setEnabled (true);
      options.function = misc.save_function;
      misc.num_lines = local_num_lines = 0;
      break;
    }
  setFunctionCursor (options.function);
}



void 
pfmEdit3D::slotMouseDoubleClick (QMouseEvent *e __attribute__ ((unused)), double lon __attribute__ ((unused)), double lat __attribute__ ((unused)),
                                 double z __attribute__ ((unused)))
{
  if (misc.busy) return;


  //  Lock in nearest point so mouse moves won't change it if we want to run one of the ancillary programs.

  lock_point = misc.nearest_point;


  //  Flip the double_click flag.  The right-click menu sets this to NVTrue so it will flip to NVFalse.
  //  Left-click sets it to NVFalse so it will filp to NVTrue;

  double_click = !double_click;


  switch (options.function)
    {
    case SET_MULTIPLE:

      if (misc.nearest_point != -1)
        {
          local_line_num[local_num_lines] = misc.data[misc.nearest_point].line;
          local_num_lines++;

          bDisplayMultiple->setEnabled (false);
          bDisplayAll->setEnabled (true);
        }

      for (int32_t i = 0 ; i < local_num_lines ; i++) misc.line_num[i] = local_line_num[i];
      misc.num_lines = local_num_lines;
      redrawMap (NVTrue);
      options.function = misc.save_function;
      setFunctionCursor (options.function);
      break;

    default:

      //  Double click is hard-wired to turn off slicing except in a few cases.

      if (misc.slice) slotPopupMenu (popup[NUMPOPUPS - 1]);
      break;
    }
}



//!  Freakin' menus!

void 
pfmEdit3D::rightMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat, double z __attribute__ ((unused)))
{
  QString tmp;


  //  If we already have a context menu up, flush the OpenGL scene so the previous one will go away

  if (popup_active) map->flush ();


  menu_cursor_lon = lon;
  menu_cursor_lat = lat;
  menu_cursor_x = mouse_x;
  menu_cursor_y = mouse_y;


  QPoint pos (mouse_x, mouse_y);


  //  Lock in nearest point so mouse moves won't change it if we want to run one of the ancillary programs.

  lock_point = misc.nearest_point;


  //  Popups need global positioning

  QPoint global_pos = map->mapToGlobal (pos);


  //  In most cases popup[2] through popup[NUMPOPUPS-1] are not used so we set them to false.  If we need them for specific buttons
  //  we'll set them to true in the switch.

  popup[0]->setVisible (true);
  popup[1]->setVisible (true);
  for (int32_t i = 2 ; i < NUMPOPUPS ; i++) popup[i]->setVisible (false);


  switch (options.function)
    {
    case DELETE_POINT:

      //  If you move the "Delete point" option from popup[0] look through the code for calls to slotPopupMenu with popup[0] as the argument.

      tmp = tr ("Delete point [%1]").arg (options.hotkey[DELETE_SINGLE_POINT]);
      popup[0]->setText (tmp);
      popup[1]->setText (tr ("Delete record (e.g. ping)"));
      popup[2]->setText (tr ("Delete file"));
      popup[2]->setVisible (true);
      popup[3]->setText (tr ("Highlight point"));
      popup[3]->setVisible (true);
      popup[4]->setText (tr ("Highlight record"));
      popup[4]->setVisible (true);


      //  Don't allow point marking if we have filter points.

      if (!misc.filter_kill_count)
        {
          popup[3]->setEnabled (true);
          popup[4]->setEnabled (true);
        }
      else
        {
          popup[3]->setEnabled (false);
          popup[4]->setEnabled (false);
        }



      //  If you need to change the freeze/unfreeze popup position you need to change the FREEZE_POPUP value in pfmEdit3DDef.hpp

      if (misc.marker_mode)
        {
          popup[FREEZE_POPUP]->setText (tr ("Unfreeze marker"));
        }
      else
        {
          popup[FREEZE_POPUP]->setText (tr ("Freeze marker"));
        }
      popup[FREEZE_POPUP]->setVisible (true);


      //  Check for the data type of the nearest point and add the possible ancillary programs

      if (misc.data[misc.nearest_point].type == PFM_GSF_DATA)
        {
          popup[8]->setText (tr ("Run %1 [%2]").arg (options.name[EXAMGSF]).arg (options.hotkey[EXAMGSF]));
          popup[8]->setVisible (true);
          popup_prog[8] = EXAMGSF;
        }

      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case DELETE_RECTANGLE:
      popup[0]->setText (tr ("Close rectangle and delete subrecords (beams)"));
      popup[1]->setText (tr ("Close rectangle and delete records (pings)"));
      popup[2]->setText (tr ("Discard rectangle"));
      popup[2]->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case RESTORE_RECTANGLE:
      popup[0]->setText (tr ("Close rectangle and restore data"));
      popup[1]->setText (tr ("Discard rectangle"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case DELETE_POLYGON:
      popup[0]->setText (tr ("Close polygon and delete subrecords (beams)"));
      popup[1]->setText (tr ("Close polygon and delete records (pings)"));
      popup[2]->setText (tr ("Discard polygon"));
      popup[2]->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case KEEP_POLYGON:
      popup[0]->setText (tr ("Close polygon and delete subrecords (beams)"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case RESTORE_POLYGON:
      popup[0]->setText (tr ("Close polygon and restore data"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case SET_REFERENCE:
      popup[0]->setText (tr ("Close polygon and set reference data flag"));
      popup[1]->setText (tr ("Discard polygon"));
      popup[2]->setText (tr ("Delete all reference soundings"));
      popup[2]->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case UNSET_REFERENCE:
      popup[0]->setText (tr ("Close polygon and unset reference data flag"));
      popup[1]->setText (tr ("Discard polygon"));
      popup[2]->setText (tr ("Delete all reference soundings"));
      popup[2]->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case HOTKEY_POLYGON:


      //  If we have a defined hotkey polygon, check for the data types in the polygon and activate the entries for
      //  possible ancillary programs.

      if (!misc.hotkey_poly_count)
        {
          popup[0]->setText (tr ("Close polygon (press hot key to run program)"));
          popup[1]->setText (tr ("Discard polygon"));
        }
      else
        {
          popup[0]->setVisible (false);
          popup[1]->setVisible (false);


          //  We have to convert to double so that the "inside" function will work.

          double *mx = (double *) malloc (misc.hotkey_poly_count * sizeof (double));

          if (mx == NULL)
            {
              fprintf (stderr, "%s %s %s %d - mx - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          double *my = (double *) malloc (misc.hotkey_poly_count * sizeof (double));

          if (my == NULL)
            {
              fprintf (stderr, "%s %s %s %d - my - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          for (int32_t i = 0 ; i < misc.hotkey_poly_count ; i++)
            {
              mx[i] = (double) misc.hotkey_poly_x[i];
              my[i] = (double) misc.hotkey_poly_y[i];
            }

          int32_t hits = 0, px, py;
          uint8_t init2D = NVTrue;
          for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
            {
              //  Check for single line display.

              if (!misc.num_lines || check_line (&misc, misc.data[i].line))
                {
                  //  Do not use null points.  Do not use invalid points unless the display_man_invalid, display_flt_invalid, or
                  //  display_null flag is set.  Do not use masked points. Do not check points that are not on the display.

                  if (!check_bounds (&options, &misc, i, NVTrue, misc.slice))
                    {
                      //  Convert the X, Y, and Z value to a projected pixel position

                      map->get2DCoords (misc.data[i].x, misc.data[i].y, -misc.data[i].z, &px, &py, &init2D);


                      //  Now check the point against the polygon.

                      if (inside_polygon2 (mx, my, misc.hotkey_poly_count, (double) px, (double) py))
                        {
                          if (misc.data[i].type == PFM_SHOALS_1K_DATA) hits |= 0x01;
                          if (misc.data[i].type == PFM_SHOALS_TOF_DATA) hits |= 0x02;
                          if (misc.data[i].type == PFM_CHARTS_HOF_DATA) hits |= 0x04;
                        }

                      if (hits == 0x07) break;
                    }
                }
            }


          //  Check for features if present

          init2D = NVTrue;
          if (options.display_feature && misc.visible_feature_count)
            {
              for (uint32_t i = 0 ; i < misc.bfd_header.number_of_records ; i++)
                {
                  if (misc.feature[i].confidence_level)
                    {
                      //  Convert the X, Y, and Z value to a projected pixel position

                      map->get2DCoords (misc.feature[i].longitude, misc.feature[i].latitude, -misc.feature[i].depth, &px, &py, &init2D);


                      //  Now check the point against the polygon.

                      if (inside_polygon2 (mx, my, misc.hotkey_poly_count, (double) px, (double) py))
                        {
                          hits |= 0x8;
                          break;
                        }
                    }
                }
            }


          free (mx);
          free (my);


          //  Old CHARTS HOF format (saved for possible re-use)

          if (hits & 0x01)
            {
            }


          //  CHARTS TOF format (saved for possible re-use)

          if (hits & 0x02)
            {
            }


          //  CHARTS HOF format

          if (hits & 0x04)
            {
              popup[10]->setText (tr ("Run %1 [%2]").arg (options.name[HOFRETURNKILL]).arg (options.hotkey[HOFRETURNKILL]));
              popup[10]->setVisible (true);
              popup_prog[10] = HOFRETURNKILL;
              popup[11]->setText (tr ("Run %1 [%2]").arg (options.name[HOFRETURNKILL_SWA]).arg (options.hotkey[HOFRETURNKILL_SWA]));
              popup[11]->setVisible (true);
              popup_prog[11] = HOFRETURNKILL_SWA;
            }


          //  Feature format

          if (hits & 0x08)
            {
              popup[12]->setText (tr ("Invalidate features"));
              popup[12]->setVisible (true);
              popup_prog[12] = INVALIDATE_FEATURES;
            }
        }
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case UNSET_SINGLE:
      popup[0]->setText (tr ("Hide line"));
      popup[1]->setText (tr ("Leave hide single line mode"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case SET_MULTIPLE:
      popup[0]->setText (tr ("Accept selected lines"));
      popup[1]->setText (tr ("Leave select multiple line mode"));
      popup[2]->setText (tr ("Select multiple lines from list"));
      popup[2]->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case MOVE_FEATURE:
      popup[0]->setText (tr ("Move feature"));
      popup[1]->setText (tr ("Leave move feature mode"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case DELETE_FEATURE:
      popup[0]->setText (tr ("invalidate feature"));
      popup[1]->setText (tr ("Leave invalidate feature mode"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case EDIT_FEATURE:
      popup[0]->setText (tr ("Edit feature"));
      popup[1]->setText (tr ("Leave edit feature mode"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case ADD_FEATURE:
      popup[0]->setText (tr ("Add feature"));
      popup[1]->setText (tr ("Leave add feature mode"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case RECT_FILTER_MASK:
    case POLY_FILTER_MASK:
      if (options.function == RECT_FILTER_MASK)
        {
          popup[0]->setText (tr ("Close rectangle and save mask"));
          popup[1]->setText (tr ("Discard rectangle"));
        }
      else
        {
          popup[0]->setText (tr ("Close polygon and save mask"));
          popup[1]->setText (tr ("Discard polygon"));
        }
      popup[2]->setText (tr ("Clear all masks"));
      if (options.deep_filter_only)
        {
          tmp = tr ("Set deep filter only (Yes)");
        }
      else
        {
          tmp = tr ("Set deep filter only (No)");
        }
      popup[3]->setText (tmp);
      popup[2]->setVisible (true);
      popup[3]->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case MASK_INSIDE_RECTANGLE:
    case MASK_OUTSIDE_RECTANGLE:
      popup[0]->setText (tr ("Close rectangle and hide data"));
      popup[1]->setText (tr ("Discard rectangle"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case MASK_INSIDE_POLYGON:
    case MASK_OUTSIDE_POLYGON:
      popup[0]->setText (tr ("Close polygon and hide data"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case MEASURE:
      popup[0]->setVisible (false);
      popup[1]->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case HIGHLIGHT_POLYGON:
      popup[0]->setText (tr ("Close polygon and highlight points"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case CLEAR_POLYGON:
      popup[0]->setText (tr ("Close polygon and un-highlight points"));
      popup[1]->setText (tr ("Discard polygon"));
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case CZMIL_REPROCESS_LAND:
    case CZMIL_REPROCESS_WATER:
    case CZMIL_REPROCESS_SHALLOW_WATER:
      if (options.czmil_reprocess_mode)
        {
          popup[0]->setText (tr ("Close polygon and mark points for reprocessing"));
          popup[1]->setText (tr ("Discard polygon"));
          popup[2]->setText (tr ("Change CZMIL reprocessing options"));
          popup[3]->setText (tr ("Switch to CZMIL reprocessing rectangle mode"));
        }
      else
        {
          popup[0]->setText (tr ("Close rectangle and mark points for reprocessing"));
          popup[1]->setText (tr ("Discard rectangle"));
          popup[2]->setText (tr ("Change CZMIL reprocessing options"));
          popup[3]->setText (tr ("Switch to CZMIL reprocessing polygon mode"));
        }
      popup[2]->setVisible (true);
      popup[3]->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case CPF_FILTER:
      popup[0]->setText (tr ("Close polygon and mark points for filtering"));
      popup[1]->setText (tr ("Discard polygon"));
      popup[2]->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case CHANGE_LIDAR_CLASS:
      popup[0]->setText (tr ("Close polygon and change LiDAR classifications"));
      popup[1]->setText (tr ("Change selected classification type (%1)").arg (options.lidar_class));
      popup[2]->setText (tr ("Discard polygon"));
      popup[2]->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;
    }

  if (misc.slice)
    {
      popup[NUMPOPUPS - 1]->setText (tr ("Turn off slicing"));
      popup[NUMPOPUPS - 1]->setVisible (true);
    }
}



/*!
  IMPORTANT NOTE:  Never use misc.nearest_point in slotPopupMenu.  Use lock_point instead.  This is set when we right-click,
  double-click, or press a key so that subsequent mouse movement will not affect what we were trying to do when we clicked or pressed.
*/

void 
pfmEdit3D::slotPopupMenu (QAction *ac)
{
  int32_t value, ret;
  QStringList lst, items;
  QString res_str, item, boxSize;
  bool ok;


  void kill_records (nvMapGL *map, OPTIONS *options, MISC *misc, int32_t *rb, int32_t x, int32_t y);


  //  Nice little debug statement for the next time I add a popup slot and forget to change this code.

  if (NUMPOPUPS > 15)
    {
      QString msg = QString ("%1 %2 %3 - Someone tell Jan to fix this function because NUMPOPUPS has changed.").arg (__FILE__).arg (__FUNCTION__).arg
        (__LINE__);
      misc.messages->append (msg);
    }

  popup_active = NVFalse;


  //  Clear the context menu by flushing the OpenGL scene

  map->flush ();


  //  Each of the following popup slots will execute whatever was set up in rightMouse above.  Instead of 
  //  commenting each piece and then having to change it each time I move something I'll let the rightMouse
  //  function serve as documentation for each piece.

  if (ac == popup[0])
    {
      switch (options.function)
        {
        case DELETE_POINT:
          if (lock_point != -1)
            {
              //
              //  If we are frozen, automatically unfreeze
              //

              if (misc.marker_mode) misc.marker_mode = 0;


              //  Single point kills are a real waste of undo ;-)

              store_undo (&misc, options.undo_levels, misc.data[lock_point].val, lock_point, NULL);
              misc.data[lock_point].val = PFM_MANUALLY_INVAL;


              //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
              //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
              //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
              //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
              //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

              misc.data[lock_point].val &= ~PFM_SUSPECT;

              end_undo_block (&misc);

              redrawMap (NVTrue);
            }
          break;

        case SET_MULTIPLE:
          slotMouseDoubleClick (NULL, 0.0, 0.0, 0.0);
          break;

        case DELETE_RECTANGLE:
        case RESTORE_RECTANGLE:
        case UNSET_SINGLE:
        case MOVE_FEATURE:
        case DELETE_FEATURE:
        case EDIT_FEATURE:
        case ADD_FEATURE:
        case DELETE_POLYGON:
        case RESTORE_POLYGON:
        case KEEP_POLYGON:
        case SET_REFERENCE:
        case UNSET_REFERENCE:
        case HOTKEY_POLYGON:
        case MASK_INSIDE_RECTANGLE:
        case MASK_OUTSIDE_RECTANGLE:
        case MASK_INSIDE_POLYGON:
        case MASK_OUTSIDE_POLYGON:
        case RECT_FILTER_MASK:
        case POLY_FILTER_MASK:
        case HIGHLIGHT_POLYGON:
        case CLEAR_POLYGON:
        case CZMIL_REPROCESS_LAND:
        case CZMIL_REPROCESS_WATER:
        case CZMIL_REPROCESS_SHALLOW_WATER:
        case CPF_FILTER:
        case CHANGE_LIDAR_CLASS:
          leftMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat, menu_cursor_z);
          break;
        }
    }
  else if (ac == popup[1])
    {
      switch (options.function)
        {
        case DELETE_RECTANGLE:
          kill_records (map, &options, &misc, &rb_rectangle, menu_cursor_x, menu_cursor_y);

          redrawMap (NVTrue);

          setFunctionCursor (options.function);
          break;

        case DELETE_POLYGON:
          kill_records (map, &options, &misc, &rb_polygon, menu_cursor_x, menu_cursor_y);

          redrawMap (NVTrue);

          setFunctionCursor (options.function);
          break;

        case CHANGE_LIDAR_CLASS:
          slotLidarClassChangePrefs ();
          break;

        default:
          midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat, menu_cursor_z);
          break;
        }
    }
  else if (ac == popup[2])
    {
      switch (options.function)
        {
        case DELETE_RECTANGLE:
        case DELETE_POLYGON:
        case CHANGE_LIDAR_CLASS:
          midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat, menu_cursor_z);
          break;

        case RECT_FILTER_MASK:
        case POLY_FILTER_MASK:
          slotClearFilterMasks ();
          break;


          //  Add files to the delete file queue.

        case DELETE_POINT:
          if (lock_point != -1)
            {
              char fn[512];
              int16_t type;
              read_list_file (misc.pfm_handle[misc.data[lock_point].pfm], misc.data[lock_point].file, fn, &type);

              res_str = tr ("Add file %1 in PFM %2 to the delete file queue?").arg (pfm_basename (fn)).arg 
                (pfm_basename (misc.abe_share->open_args[misc.data[lock_point].pfm].list_path));
              ret = QMessageBox::information (this, tr ("pfmEdit3D Delete File Queue"), res_str, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);

              if (ret == QMessageBox::Yes)
                {
                  //  No point in listing a file twice so check for it first.

                  ok = NVTrue;
                  for (int32_t i = 0 ; i < misc.abe_share->delete_file_queue_count ; i++)
                    {
                      if (misc.abe_share->delete_file_queue[i][0] == misc.data[lock_point].pfm &&
                          misc.abe_share->delete_file_queue[i][1] == misc.data[lock_point].file)
                        {
                          ok = NVFalse;
                          break;
                        }
                    }

                  if (ok)
                    {
                      misc.abe_share->delete_file_queue[misc.abe_share->delete_file_queue_count][0] = misc.data[lock_point].pfm;
                      misc.abe_share->delete_file_queue[misc.abe_share->delete_file_queue_count][1] = misc.data[lock_point].file;
                      misc.abe_share->delete_file_queue_count++;
                    }
                }
            }
          break;

        case SET_REFERENCE:
        case UNSET_REFERENCE:
          ret = QMessageBox::information (this, "pfmEdit3D", tr ("Do you really want to delete all visible reference soundings?"),
                                          QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);

          if (ret == QMessageBox::Yes)
            {
              for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
                {
                  //  Check for single line display.

                  if (!misc.num_lines || check_line (&misc, misc.data[i].line))
                    {
                      //  Since you can't access this option without display_reference being set we don't have to
                      //  worry about check_bounds not passing reference points.

                      if (!check_bounds (&options, &misc, i, NVTrue, misc.slice))
                        {
                          store_undo (&misc, options.undo_levels, misc.data[i].val, i, NULL);
                          if (misc.data[i].val & PFM_REFERENCE)
                            {
                              misc.data[i].val |= PFM_MANUALLY_INVAL;


                              //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
                              //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
                              //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
                              //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
                              //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

                              misc.data[i].val &= ~PFM_SUSPECT;
                            }
                        }
                    }
                }
              end_undo_block (&misc);

              redrawMap (NVFalse);
            }
          break;

        case SET_MULTIPLE:
          displayLines_dialog = new displayLines (this, &misc);

          connect (displayLines_dialog, SIGNAL (dataChangedSignal ()), this, SLOT (slotDisplayLinesDataChanged ()));
          break;

        case CZMIL_REPROCESS_LAND:
        case CZMIL_REPROCESS_WATER:
        case CZMIL_REPROCESS_SHALLOW_WATER:


          //  Open the preferences dialog and move to the CZMIL tab.

          options.pref_tab = 6;
          slotPrefs ();
          break;
        }
    }
  else if (ac == popup[3])
    {
      switch (options.function)
        {
        case DELETE_POINT:
          try
            {
              misc.highlight.resize (misc.highlight_count + 1);
            }
          catch (std::bad_alloc&)
            {
              fprintf (stderr, "%s %s %s %d - highlight - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          misc.highlight[misc.highlight_count] = lock_point;
          misc.highlight_count++;


          //  Since we decided to manually highlight a data point we want to turn off any highlighting based on flag type

          options.flag_index = 0;
          bFlag->setIcon (flagIcon[options.flag_index]);
          disconnect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
          flag[0]->setChecked (true);
          connect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));

          bClearHighlight->setEnabled (true);

          redrawMap (NVFalse);
          break;

        case RECT_FILTER_MASK:
        case POLY_FILTER_MASK:
            value = 0;
            if (!options.deep_filter_only) value = 1;

            lst << tr ("Yes") << tr ("No");

            res_str = QInputDialog::getItem (this, "pfmEdit3D", tr ("Deep filter only:"), lst, value, false, &ok);

            if (ok)
              {
                if (res_str.contains (tr ("Yes")))
                  {
                    options.deep_filter_only = NVTrue;
                  }
                else
                  {
                    options.deep_filter_only = NVFalse;
                  }
              }
          break;

        case CZMIL_REPROCESS_LAND:
        case CZMIL_REPROCESS_WATER:
        case CZMIL_REPROCESS_SHALLOW_WATER:
          if (options.czmil_reprocess_mode)
            {
              options.czmil_reprocess_mode = NVFalse;
            }
          else
            {
              options.czmil_reprocess_mode = NVTrue;
            }

          setWidgetStates (NVTrue);
          setFunctionCursor (options.function);
          break;
        }
    }
  else if (ac == popup[4])
    {
      switch (options.function)
        {
        case DELETE_POINT:

          if (misc.highlight_count)
            {
              misc.highlight.clear ();
              misc.highlight_count = 0;
            }

          for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
            {
              if (!(misc.data[i].val & (PFM_DELETED | PFM_REFERENCE | PFM_INVAL)))
                {
                  if (!check_bounds (&options, &misc, i, NVTrue, misc.slice))
                    {
                      if (misc.data[i].pfm == misc.data[lock_point].pfm && misc.data[i].file == misc.data[lock_point].file &&
                          misc.data[i].rec == misc.data[lock_point].rec)
                        {
                          try
                            {
                              misc.highlight.resize (misc.highlight_count + 1);
                            }
                          catch (std::bad_alloc&)
                            {
                              fprintf (stderr, "%s %s %s %d - highlight - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
                              exit (-1);
                            }

                          misc.highlight[misc.highlight_count] = i;
                          misc.highlight_count++;
                        }
                    }
                }
            }


          //  Since we decided to manually highlight a data point we want to turn off any highlighting based on flag type

          options.flag_index = 0;
          bFlag->setIcon (flagIcon[options.flag_index]);
          disconnect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
          flag[0]->setChecked (true);
          connect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));

          bClearHighlight->setEnabled (true);

          redrawMap (NVFalse);
          break;

        case RECT_FILTER_MASK:
        case POLY_FILTER_MASK:
          break;
        }
    }
  else if (ac == popup[5])
    {
      switch (options.function)
        {
          //  This slot is called when the Freeze/Unfreeze action has been clicked. It will set the proper text for the right
          //  context menu based on whether we are in freeze mode or not.  If we are unfreezing, we'll set the context menu
          //  text to be Freeze Point and close the focus cursor moving path for the overplot as it will be handled in
          //  slotMouseMove.  If we are freezing, we put the Unfreeze Point text on the context menu and indicate that the focus
          //  cursor is present as a legitimate multi-waveform

        case DELETE_POINT:

          //  Save the marker position just in case we are freezing the marker.

          misc.frozen_point = lock_point;


          if (misc.marker_mode)
            {
              //  Just in case CZMILwaveMonitor is running.

              misc.abe_share->mwShare.multiPresent[0] = -1;

              misc.marker_mode = 0;
            }
          else
            {
              //  Just in case CZMILwaveMonitor is running.

              misc.abe_share->mwShare.multiPresent[0] = 0;

              misc.marker_mode = 1;
            }
          break;
        }
    }
  else if (ac == popup[6])
    {
    }
  else if (ac == popup[7])
    {
    }
  else if (ac == popup[8])
    {
      runPopupProg (8);
    }
  else if (ac == popup[9])
    {
      runPopupProg (9);
    }
  else if (ac == popup[10])
    {
      runPopupProg (10);
    }
  else if (ac == popup[11])
    {
      runPopupProg (11);
    }
  else if (ac == popup[12])
    {
      runPopupProg (12);
    }
  else if (ac == popup[13])
    {
      runPopupProg (13);
    }
  else if (ac == popup[NUMPOPUPS - 1])
    {
      //  Turn off slicing if it is enabled.

      misc.slice = NVFalse;

      misc.slice_min = misc.ortho_min;
      sliceBar->setValue (misc.ortho_min);


      //  Turn on depth testing in the map widget

      map->setDepthTest (NVTrue);


      redrawMap (NVTrue);
    }
}



void 
pfmEdit3D::slotPopupHelp ()
{
  switch (options.function)
    {
    case DELETE_RECTANGLE:
      QWhatsThis::showText (QCursor::pos ( ), deleteRectText, map);
      break;

    case RESTORE_RECTANGLE:
      QWhatsThis::showText (QCursor::pos ( ), restoreRectText, map);
      break;

    case DELETE_POINT:
      QWhatsThis::showText (QCursor::pos ( ), deletePointText, map);
      break;

    case UNSET_SINGLE:
      QWhatsThis::showText (QCursor::pos ( ), undisplaySingleText, map);
      break;

    case SET_MULTIPLE:
      QWhatsThis::showText (QCursor::pos ( ), displayMultipleText, map);
      break;

    case MOVE_FEATURE:
      QWhatsThis::showText (QCursor::pos ( ), moveFeatureText, map);
      break;

    case DELETE_FEATURE:
      QWhatsThis::showText (QCursor::pos ( ), deleteFeatureText, map);
      break;

    case EDIT_FEATURE:
      QWhatsThis::showText (QCursor::pos ( ), editFeatureText, map);
      break;

    case ADD_FEATURE:
      QWhatsThis::showText (QCursor::pos ( ), addFeatureText, map);
      break;

    case DELETE_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), deletePolyText, map);
      break;

    case RESTORE_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), restorePolyText, map);
      break;

    case KEEP_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), keepPolyText, map);
      break;

    case SET_REFERENCE:
      QWhatsThis::showText (QCursor::pos ( ), referencePolyText, map);
      break;

    case UNSET_REFERENCE:
      QWhatsThis::showText (QCursor::pos ( ), unreferencePolyText, map);
      break;

    case HOTKEY_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), hotkeyPolyText, map);
      break;

    case RECT_FILTER_MASK:
      QWhatsThis::showText (QCursor::pos ( ), filterRectMaskText, map);
      break;

    case POLY_FILTER_MASK:
      QWhatsThis::showText (QCursor::pos ( ), filterPolyMaskText, map);
      break;

    case MASK_INSIDE_RECTANGLE:
      QWhatsThis::showText (QCursor::pos ( ), maskInsideRectText, map);
      break;

    case MASK_OUTSIDE_RECTANGLE:
      QWhatsThis::showText (QCursor::pos ( ), maskOutsideRectText, map);
      break;

    case MASK_INSIDE_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), maskInsidePolyText, map);
      break;

    case MASK_OUTSIDE_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), maskOutsidePolyText, map);
      break;

    case MEASURE:
      QWhatsThis::showText (QCursor::pos ( ), measureText, map);
      break;

    case HIGHLIGHT_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), highlightPolyText, map);
      break;

    case CLEAR_POLYGON:
      QWhatsThis::showText (QCursor::pos ( ), clearPolyText, map);
      break;

    case CZMIL_REPROCESS_LAND:
      QWhatsThis::showText (QCursor::pos ( ), czmilReprocessLandText, map);
      break;

    case CZMIL_REPROCESS_WATER:
      QWhatsThis::showText (QCursor::pos ( ), czmilReprocessWaterText, map);
      break;

    case CZMIL_REPROCESS_SHALLOW_WATER:
      QWhatsThis::showText (QCursor::pos ( ), czmilReprocessShallowWaterText, map);
      break;

    case CPF_FILTER:
      QWhatsThis::showText (QCursor::pos ( ), cpfFilterText, map);
      break;

    case CHANGE_LIDAR_CLASS:
      QWhatsThis::showText (QCursor::pos ( ), changeLidarClassText, map);
      break;
    }

  if (options.function != DELETE_POINT) midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat, menu_cursor_z);
}



void 
pfmEdit3D::runPopupProg (int32_t prog)
{
  if (options.function == HOTKEY_POLYGON && misc.hotkey_poly_count)
    {
      uint8_t failed = NVFalse;
      hkp = new hotkeyPolygon (this, map, &options, &misc, popup_prog[prog], &failed);
      if (failed)
        {
          redrawMap (NVTrue);
        }
      else
        {
          connect (hkp, SIGNAL (hotkeyPolygonDone ()), this, SLOT (slotHotkeyPolygonDone ()));
          connect (hkp, SIGNAL (readStandardErrorSignal (QProcess *)), this, SLOT (slotReadStandardError (QProcess *)));
        }
    }
  else
    {
      runProg (popup_prog[prog]);
    }
}



/*!
  Timer - timeout signal.  Very much like an X workproc.  This tracks the cursor in associated
  programs, not in this window.  This is active whenever the mouse leaves this window.
*/

void
pfmEdit3D::slotTrackCursor ()
{
  char                  ltstring[25], lnstring[25];
  QString               string;
  static NV_F64_COORD2  prev_xy;
  static int32_t        prevMultiMode = misc.abe_share->mwShare.multiMode;


  //  This is sort of ancillary to the track_cursor function.  What we want to do is monitor the number of error messages in the
  //  error message QStringList and when it changes pop up the messages dialog.  This way was a lot easier than implementing signals
  //  and slots or trying to call parent methods from children.

  if (misc.messages->size () != misc.num_messages)
    {
      misc.num_messages = misc.messages->size ();
      slotMessage ();
      if (misc.num_messages) messageAction->setEnabled (true);
    }


  //  If we've just reprocessed some CZMIL data we want to exit/redraw/return from pfmView but we don't want to do it if the error
  //  message dialog is being displayed.  In that case we'll wait until the user has looked at the messages and closed the dialog.

  if (misc.czmilReprocessFlag && displayMessage_dialog == NULL)
    {
      moveWindow = 200;
      slotExit (0);
    }


  if (misc.busy) return;


  //  When Single is toggled to NN or vice versa in Waveform Viewer, remove markers and flush the map so no lingering markers hang around.
  //  This is hard to control due to the XOR logic of the markers and in certain instances, markers would hang around

  if (prevMultiMode != misc.abe_share->mwShare.multiMode)
    {
      prevMultiMode = misc.abe_share->mwShare.multiMode;

      discardMovableObjects ();          

      map->flush ();
    }


  //
  //                      if we've closed the CZMILwaveMonitor module and we were not in single waveform mode,
  //                      close the moving paths of all the multiple waveforms and switch to single waveform
  //                      mode so we will only display our lone cursor in pfmEdit3D.
  //

  if (!misc.abe_share->mwShare.waveMonitorRunning && !misc.abe_share->mwShare.multiMode)
    {
      for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++) map->closeMovingList (&(multi_marker[i]));
      misc.abe_share->mwShare.multiMode = 1;
    }


  //  If we have changed the feature file contents in another program, we need to draw the features.

  if (misc.abe_share->key == FEATURE_FILE_MODIFIED)
    {
      //  Try to open the feature file and read the features into memory.

      if (strcmp (misc.abe_share->open_args[0].target_path, "NONE"))
        {
          if (misc.bfd_open) binaryFeatureData_close_file (misc.bfd_handle);
          misc.bfd_open = NVFalse;

          if ((misc.bfd_handle = binaryFeatureData_open_file (misc.abe_share->open_args[0].target_path, &misc.bfd_header, BFDATA_UPDATE)) >= 0)
            {
              if (binaryFeatureData_read_all_short_features (misc.bfd_handle, &misc.feature) < 0)
                {
                  QString msg = QString (binaryFeatureData_strerror ());
                  QMessageBox::warning (this, "pfmEdit3D", tr ("Unable to read feature records\nReason: %1").arg (msg));
                  binaryFeatureData_close_file (misc.bfd_handle);
                }
              else
                {
                  misc.bfd_open = NVTrue;
                }
            }
        }


      redrawMap (NVFalse);


      //  Wait 2 seconds so that all associated programs will see the modified flag.

#ifdef NVWIN3X
      Sleep (2000);
#else
      sleep (2);
#endif
      misc.abe_share->key = 0;
      misc.abe_share->modcode = NO_ACTION_REQUIRED; 
    }


  NV_F64_COORD2 xy;
  xy.y = misc.abe_share->cursor_position.y;
  xy.x = misc.abe_share->cursor_position.x;

  if (misc.abe_share->active_window_id != misc.process_id && xy.y >= misc.displayed_area.min_y && xy.y <= misc.displayed_area.max_y && 
      xy.x >= misc.displayed_area.min_x && xy.x <= misc.displayed_area.max_x && xy.y != prev_xy.y && xy.x != prev_xy.x)
    {
      //  Find the nearest point to the cursor's location in whatever window has the focus.

      double min_dist = 999999999.0;
      int32_t hit = -1;
      for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
        {
          double dist = sqrt (((xy.y - misc.data[i].y) * (xy.y - misc.data[i].y)) + ((xy.x - misc.data[i].x) * (xy.x - misc.data[i].x)));

          if (dist < min_dist)
            {
              min_dist = dist;
              hit = i;
            }
        }


      if (hit >= 0)
        {
          //  First set the 3D tracker.

          int32_t pix_x, pix_y;
          map->get2DCoords (misc.data[hit].x, misc.data[hit].y, -misc.data[hit].z, &pix_x, &pix_y);


          //  This particular situation caused a lingering of shot markers so a clearing of markers is called here.  This is a weird situation.

          if (misc.abe_share->active_window_id != misc.process_id - 1) discardMovableObjects ();


          map->setMovingList (&mv_tracker, marker, 16, pix_x, pix_y, 0.0, 2, options.tracker_color);


          //  Then set the 2D tracker (unless the trackMap isn't up)

          if (options.track_tab)
            {
              QBrush b1;
              trackMap->setMovingPath (&mv_2D_tracker, tracker_2D, misc.data[hit].x, misc.data[hit].y, 2, options.tracker_color, b1, NVFalse, Qt::SolidLine);
            }
        }


      double deg, min, sec;
      char       hem;

      strcpy (ltstring, fixpos (xy.y, &deg, &min, &sec, &hem, POS_LAT, options.position_form));
      strcpy (lnstring, fixpos (xy.x, &deg, &min, &sec, &hem, POS_LON, options.position_form));

      if (options.lat_or_east)
        {
          double x = xy.x * NV_DEG_TO_RAD;
          double y = xy.y * NV_DEG_TO_RAD;

          pj_transform (misc.pj_latlon, misc.pj_utm, 1, 1, &x, &y, NULL);

          strcpy (ltstring, QString ("%L1").arg (y, 0, 'f', 2).toLatin1 ());
          strcpy (lnstring, QString ("%L1").arg (x, 0, 'f', 2).toLatin1 ()); 
       }

      latLabel->setText (ltstring);
      lonLabel->setText (lnstring);
    }

  prev_xy = xy;
}



//!  Blink timer slot.

void
pfmEdit3D::slotBlinkTimer ()
{
  static int32_t     blink = 0, lblink = 0;


  //  Since this is always a child process of something we want to exit if we see the CHILD_PROCESS_FORCE_EXIT key.  We also want to exit if
  //  pfmView has set the PFMEDIT_KILL switch.

  if (misc.abe_share->key == CHILD_PROCESS_FORCE_EXIT || misc.abe_share->key == PFMEDIT_KILL) slotExit (2);


  //  We want to blink the mask reset button (every half_second) if we have a mask set so that the user will know.

  if (options.mask_active)
    {
      if (blink)
        {
          if (options.mask_active & MASK_SHAPE) bMaskClear->setIcon (QIcon (":/icons/clear_mask_shape.png"));
          if (options.mask_active & MASK_TYPE) bMaskReset->setIcon (QIcon (":/icons/clear_mask_type.png"));
          if (options.mask_active & MASK_FLAG) bFlagReset->setIcon (QIcon (":/icons/clear_mask_flag.png"));
          blink = 0;
        }
      else
        {
          if (options.mask_active & MASK_SHAPE) bMaskClear->setIcon (QIcon (":/icons/clear_mask_shape_invert.png"));
          if (options.mask_active & MASK_TYPE) bMaskReset->setIcon (QIcon (":/icons/clear_mask_type_invert.png"));
          if (options.mask_active & MASK_FLAG) bFlagReset->setIcon (QIcon (":/icons/clear_mask_flag_invert.png"));
          blink = 1;
        }

      qApp->processEvents ();
    }


  //  Blink the displayAll button if we're not displaying all lines.

  if (misc.num_lines)
    {
      if (lblink)
        {
          bDisplayAll->setIcon (QIcon (":/icons/displayall.png"));
          lblink = 0;
        }
      else
        {
          bDisplayAll->setIcon (QIcon (":/icons/displayall_inverse.png"));
          lblink = 1;
        }

      qApp->processEvents ();
    }


  //  Check to see if a child process (that was started from a button) has been killed externally.

  if (misc.abe_share->killed)
    {
      for (int32_t i = 0 ; i < NUMPROGS ; i++)
        {
          if (misc.abe_share->killed == options.kill_switch[i])
            {
              options.progButton[i]->setChecked (false);
              misc.abe_share->killed = 0;
            }
        }
    }
}



//!  Mouse press signal from the map class.

void 
pfmEdit3D::slotMousePress (QMouseEvent *e, double lon, double lat, double z)
{
  if (misc.busy) return;


  if (e->button () == Qt::LeftButton)
    {
      //  Check for the control key modifier.  If it's set, we want to rotate the image.

      if (e->modifiers () == Qt::ControlModifier)
        {
          //  Get the cursor position at this time.

          start_ctrl_x = e->x ();
          start_ctrl_y = e->y ();

          misc.save_function = options.function;
          options.function = ROTATE;
          setFunctionCursor (options.function);
        }
      else if (e->modifiers () == Qt::ShiftModifier)
        {
          //  Get the cursor Y position at this time and compute the distance to the top of the display.

          start_shift_y = e->y ();


          //  We need to make sure that the slice scroll bar hits the top before the cursor hits the top of the window.

          NVMAPGL_DEF l_mapdef = map->getMapdef ();

          height_shift_y = l_mapdef.draw_height - (l_mapdef.draw_height - start_shift_y);


          //  Have to start the slice (this just sets it to the minimum location).

          slotSliceTriggered (-1);

          misc.save_function = options.function;
          options.function = SHIFT_SLICE;
          setFunctionCursor (options.function);
        }
      else
        {
          if (options.function == DELETE_POINT)
            {
              if (misc.nearest_point != -1)
                {
                  //  Check for the shift key modifier.  If it's set, we want to delete the point.

                  if (e->modifiers () == Qt::ShiftModifier)
                    {
                      //  If we are frozen, automatically unfreeze

                      if (misc.marker_mode) misc.marker_mode = 0;

                      store_undo (&misc, options.undo_levels, misc.data[misc.nearest_point].val, misc.nearest_point, NULL);

                      misc.data[misc.nearest_point].val = PFM_MANUALLY_INVAL;


                      //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
                      //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
                      //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
                      //  the point was good.  In that case (or if it is left invalid) it should no longer be marked as suspect.  This
                      //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

                      misc.data[misc.nearest_point].val &= ~PFM_SUSPECT;

                      end_undo_block (&misc);
                    }
                }
            }

          leftMouse (e->x (), e->y (), lon, lat, z);
        }
    }

  if (e->button () == Qt::MidButton)
    {
      //  Check for the control key modifier.  If it's set, we want to center on the cursor.

      if (e->modifiers () == Qt::ControlModifier)
        {
          map->setMapCenter (lon, lat, z);

          overlayData (map, &options, &misc);
        }
      else
        {
          midMouse (e->x (), e->y (), lon, lat, z);
        }
    }

  if (e->button () == Qt::RightButton)
    {
      //  Check for the control key modifier.  If it's set, we want to zoom based on cursor movement.

      if (e->modifiers () == Qt::ControlModifier)
        {
          start_ctrl_y = e->y ();

          misc.save_function = options.function;
          options.function = ZOOM;
          setFunctionCursor (options.function);
        }
      else
        {
          rightMouse (e->x (), e->y (), lon, lat, z);
        }
    }
}



//!  Mouse release signal from the map class.

void 
pfmEdit3D::slotMouseRelease (QMouseEvent *e, double lon __attribute__ ((unused)), double lat __attribute__ ((unused)), double z __attribute__ ((unused)))
{
  if (misc.busy) return;


  if (e->button () == Qt::LeftButton)
    {
      if (options.function == ROTATE)
        {
          options.function = misc.save_function;
          setFunctionCursor (options.function);


          //  All we have to do is flush here since setFunctionCursor will set the map mode back to NVMAPGL_POINT_MODE and
          //  we haven't actually changed the point data.

          if (misc.need_sparse) map->flush ();

          overlayData (map, &options, &misc);
        }
      else if (options.function == SHIFT_SLICE)
        {
          options.function = misc.save_function;
          setFunctionCursor (options.function);


          //  All we have to do is flush here since setFunctionCursor will set the map mode back to NVMAPGL_POINT_MODE and
          //  we haven't actually changed the point data.

          if (misc.need_sparse) map->flush ();

          overlayData (map, &options, &misc);
        }
      else
        {
          popup_active = NVFalse;
        }
    }

  if (e->button () == Qt::MidButton) popup_active = NVFalse;

  if (e->button () == Qt::RightButton)
    {
      if (options.function == ZOOM)
        {
          options.function = misc.save_function;
          setFunctionCursor (options.function);


          //  All we have to do is flush here since setFunctionCursor will set the map mode back to NVMAPGL_POINT_MODE and
          //  we haven't actually changed the point data.

          if (misc.need_sparse) map->flush ();

          overlayData (map, &options, &misc);
        }
    }
}



void 
pfmEdit3D::geo_xyz_label (double lat, double lon, double z)
{
  char ltstring[25], lnstring[25], hem;
  QString val_string;
  double deg, min, sec;


  strcpy (ltstring, fixpos (lat, &deg, &min, &sec, &hem, POS_LAT, options.position_form));
  strcpy (lnstring, fixpos (lon, &deg, &min, &sec, &hem, POS_LON, options.position_form));

  if (options.lat_or_east)
    {
      double x = lon * NV_DEG_TO_RAD;
      double y = lat * NV_DEG_TO_RAD;

      pj_transform (misc.pj_latlon, misc.pj_utm, 1, 1, &x, &y, NULL);

      strcpy (ltstring, QString ("%L1").arg (y, 0, 'f', 2).toLatin1 ());
      strcpy (lnstring, QString ("%L1").arg (x, 0, 'f', 2).toLatin1 ());
    }

  latLabel->setText (ltstring);
  lonLabel->setText (lnstring);


  val_string = QString ("%L1").arg (options.z_orientation * z * options.z_factor + options.z_offset, 0, 'f', 2);
  attrValue[0]->setText (val_string);
}



//!  Mouse wheel signal from the map class.

void
pfmEdit3D::slotWheel (QWheelEvent *e, double lon __attribute__ ((unused)), double lat __attribute__ ((unused)), double z __attribute__ ((unused)))
{
  if (e->delta () > 0)
    {
      //  Zoom in or out when pressing the Ctrl key and using the mouse wheel.

      if (e->modifiers () == Qt::ControlModifier)
        {
          map->zoomInPercent ();

          overlayData (map, &options, &misc);
        }


      //  Otherwise we're slicing.

      else
        {
          slotSliceTriggered (QAbstractSlider::SliderSingleStepSub);
        }
    }
  else
    {
      //  Zoom in or out when pressing the Ctrl key and using the mouse wheel.

      if (e->modifiers () == Qt::ControlModifier)
        {
          map->zoomOutPercent ();

          overlayData (map, &options, &misc);
        }


      //  Otherwise we're slicing.

      else
        {
          slotSliceTriggered (QAbstractSlider::SliderSingleStepAdd);
        }
    }
}



//!  Mouse move signal from the map class.

void
pfmEdit3D::slotMouseMove (QMouseEvent *e, double lon, double lat, double z, NVMAPGL_DEF l_mapdef)
{
  NV_I32_COORD2             xy;
  NV_F64_COORD3             hot = {999.0, 999.0, 999.0}, hot_feature = {999.0, 999.0, 999.0};
  static NV_I32_COORD2      prev_xy = {-1, -1};
  static NV_F64_COORD3      prev_hot;
  double                    dist, min_dist;
  static int32_t            prev_nearest_point = -1;
  static int32_t            save_nearest_point;


  void get_nearest_kill_point (MISC *misc, double lat, double lon, NV_F64_COORD3 *hot);
  uint8_t compare_to_stack (int32_t current_point, double dist, MISC *misc, double z);

  if (misc.marker_mode) save_nearest_point = misc.nearest_point;


  if (misc.busy) return;


  //  Let other ABE programs know which window we're in.

  misc.abe_share->active_window_id = misc.process_id;


  //  Get rid of the tracking cursor from slotTrackCursor ().  But only if it already exists, otherwise we
  //  will be creating a new one (which we don't want to do).

  if (mv_tracker >= 0) map->closeMovingList (&mv_tracker);
  if (mv_2D_tracker >= 0 && options.track_tab) trackMap->closeMovingPath (&mv_2D_tracker);


  //  In case slot invoked manually with null qmouseevent use the x and y of the prev nearest_point

  if (e)
    {
      xy.x = e->x ();
      xy.y = e->y ();
    }
  else
    {
      //  Highlighting uses this too so we will go with the incoming arguments of lon and lat

      map->get2DCoords (lon, lat, -z, &xy.x, &xy.y);
    }


  //  Track the cursor (not the marker) position for other ABE programs.

  misc.abe_share->cursor_position.y = lat;
  misc.abe_share->cursor_position.x = lon;
  misc.abe_share->cursor_position.z = z;


  //  Only process if the pointer position has changed pixels and we haven't frozen all of the markers.

  if (xy.x == prev_xy.x && xy.y == prev_xy.y) return;


  //  If we've frozen the central cursor or all of the cursors we want to draw them regardless of the mode we're in.

  if (misc.marker_mode)
    {
      //  If we're in multicursor mode, draw all the cursors.

      if (misc.marker_mode == 2)
        {
          if (!misc.abe_share->mwShare.multiMode) DrawMultiCursors ();
        }
      else
        { 
          int32_t pix_x, pix_y;

          map->get2DCoords (misc.data[misc.frozen_point].x, misc.data[misc.frozen_point].y, -misc.data[misc.frozen_point].z, &pix_x, &pix_y);

          map->setMovingList (&(multi_marker[0]), marker, 16, pix_x, pix_y, 0.0, 2, misc.abe_share->mwShare.multiColors[0]);
        }
    }


  QString rec_string = "", fil_string = "", pfm_string = "", lin_string = "", distance_string = "", h_string = "", v_string = "", lin_num_string = "";
  double distance[2], azimuth[2];
  char nearest_line[512];


  switch (options.function)
    {
    case ROTATE:
      if (start_ctrl_x != xy.x || start_ctrl_y != xy.y)
        {
          int32_t diff_x = xy.x - start_ctrl_x;


          //  This will remove all markers from the screen during a rotate

          discardMovableObjects();


          if (diff_x)
            {
              //  Flip the sign if we are above the center point looking at the top of the surface or below
              //  the center point looking at the bottom.  This allows the Y rotation from the mouse click/drag
              //  to be more intuitive.

              double zxrot = map->getZXRotation ();
              int32_t half = l_mapdef.draw_height / 2;


              //  If we're within 15 degrees of flat rotate normally for the front side.

              if ((zxrot < 15.0 && zxrot > -15.0) || (zxrot > 345.0) || (zxrot < -345.0))
                {
                  //  Don't do anything.
                }


              //  Reverse rotate reverse for the back side.

              else if ((zxrot > -195.0 && zxrot < -165.0) || (zxrot < 195.0 && zxrot > 165.0))
                {
                  diff_x = -diff_x;
                }


              //  Otherwise, check to see which side is up.

              else
                {
                  if ((xy.y < half && ((zxrot > 0.0 && zxrot < 180.0) || (zxrot < -180.0))) ||
                      (xy.y > half && ((zxrot < 0.0 && zxrot > -180.0) || (zxrot > 180.0)))) diff_x = -diff_x;
                }

              map->rotateY ((double) diff_x / 5.0);
            }

          int32_t diff_y = xy.y - start_ctrl_y;

          if (diff_y) map->rotateZX ((double) diff_y / 5.0);

          start_ctrl_x = xy.x;
          start_ctrl_y = xy.y;
        }
      break;

    case ZOOM:
      if (start_ctrl_y != xy.y)
        {
          int32_t diff_y = xy.y - start_ctrl_y;

          //  This will remove all markers from the screen during a zoom

          discardMovableObjects ();


          if (diff_y < -10)
            {
              map->zoomInPercent ();
              start_ctrl_y = xy.y;
            }
          else if (diff_y > 10)
            {
              map->zoomOutPercent ();
              start_ctrl_y = xy.y;
            }
        }
      break;

    case SHIFT_SLICE:
      if (start_shift_y != xy.y)
        {
          //  Compute the difference between the current location and the previous location (in Y).

          int32_t diff_y = xy.y - start_shift_y;


          //  This will remove all markers from the screen during a shift slice

          discardMovableObjects();


          //  Don't do anything if there was no change.

          if (diff_y)
            {
              //  Compute the increment as a percentage of the overall slice scrollbar range.

              int32_t incr = NINT ((float) diff_y * ((float) misc.ortho_range / (float) height_shift_y));

              misc.slice_min -= incr;

              misc.slice_max = misc.slice_min + misc.slice_size;


              //  Save the previous position.

              start_shift_y = xy.y;


              //  Now move the slice bar slider.

              slotSliceTriggered (-1);
            }
        }
      break;

    case DELETE_POINT:
    case UNSET_SINGLE:
    case SET_MULTIPLE:
    case ADD_FEATURE:
    case EDIT_FEATURE:
    case MOVE_FEATURE:
    case DELETE_FEATURE:
    case MEASURE:

      //  Draw the frozen multi_markers.

      if (misc.marker_mode == 2)
        {
          //  If we're in multicursor mode, draw all the cursors.

          if (!misc.abe_share->mwShare.multiMode)
            {
              DrawMultiCursors ();
            }
          else
            {
              int32_t pix_x, pix_y;
              map->get2DCoords (misc.data[misc.frozen_point].x, misc.data[misc.frozen_point].y, -misc.data[misc.frozen_point].z, &pix_x, &pix_y);

              map->setMovingList (&(multi_marker[0]), marker, 16, pix_x, pix_y, 0.0, 2, misc.abe_share->mwShare.multiColors[0]);
            }
        }
      else
        {
          //  Clear the nearest point stack.

          for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++)
            {
              misc.nearest_stack.point[i] = -1;
              misc.nearest_stack.dist[i] = 9999999999.0;
            }


          if (!move_feature) misc.nearest_feature = -1;
          hot_feature.x = -1;


          //  If we've got filter kill points on screen we want to find the nearest kill point first and lock it in before finding
          //  the nearest stack points.  This allows us to snap to only kill points but then get the surrounding valid points
          //  whether they're kill points or not.  This will set the nearest_point, the hot.x and hot.y positions, and the stack[0]
          //  point and distance (forced to -999.0 so it can't be replaced).

          if (misc.filter_kill_count)
            {
              //  We have to get the nearest marker point (i.e. highlighted) since the mouseMove signal returns the nearest of 
              //  all of the displayed points (unless they're sliced out).
              //  IMPORTANT NOTE: We're replacing the lat, lon, and z values passed from the nvMapGL mouseMoveSignal because we
              //  need for the exteranl programs (e.g. waveWaterfall) to find the nearest geographic points to the filter
              //  highlighted point not the point closest to the cursor.

              map->getMarker3DCoords (e->x (), e->y (), &lon, &lat, &z);

              get_nearest_kill_point (&misc, lat, lon, &hot);
            }


          //  Find the valid point or feature nearest (geographically) to the cursor.

          //  Working with existing features instead of points.

          if ((options.function == EDIT_FEATURE || options.function == DELETE_FEATURE || (options.function == MOVE_FEATURE && !move_feature)) &&
              options.display_feature && misc.bfd_header.number_of_records)
            {
              min_dist = 999999999.0;
              for (uint32_t i = 0 ; i < misc.bfd_header.number_of_records ; i++)
                {
                  if (options.display_man_invalid || options.display_flt_invalid || misc.feature[i].confidence_level)
                    {
                      if (!check_bounds (&options, &misc, misc.feature[i].longitude, misc.feature[i].latitude, misc.feature[i].depth, PFM_USER,
                                         NVFalse, 0, NVFalse, misc.slice))
                        {
                          if (options.display_children || !misc.feature[i].parent_record)
                            {
                              dist = sqrt ((double) ((lat - misc.feature[i].latitude) * (lat - misc.feature[i].latitude)) + 
                                           (double) ((lon - misc.feature[i].longitude) * (lon - misc.feature[i].longitude)));

                              if (dist < min_dist)
                                {
                                  misc.nearest_feature = i;
                                  hot_feature.x = misc.feature[i].longitude;
                                  hot_feature.y = misc.feature[i].latitude;
                                  hot_feature.z = misc.feature[i].depth;
                                  min_dist = dist;
                                }
                            }
                        }
                    }
                }
            }


          for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
            {
              //  Check for single line display.

              if (!misc.num_lines || check_line (&misc, misc.data[i].line))
                {
                  //  Do not use null points unless display_null flag is set.  Do not use invalid points unless the display_man_invalid or
                  //  display_flt_invalid flag is set.  Do not use masked points. Do not check points that are not on the display.

                  if (!check_bounds (&options, &misc, i, NVTrue, misc.slice))
                    {
                      //  Find the nearest point.

                      dist = sqrt ((double) ((lat - misc.data[i].y) * (lat - misc.data[i].y)) +
                                   (double) ((lon - misc.data[i].x) * (lon - misc.data[i].x)));


                      //  If we're trying to display nulls we need to temporarily modify the Z value so that it is at the bottom of the 
                      //  3D cube so that we can snap the cursor to the point.

                      uint8_t null_set = NVFalse;
                      if (misc.data[i].z >= misc.null_val[misc.data[i].pfm])
                        {
                          misc.data[i].z = -misc.bounds.min_z;
                          null_set = NVTrue;
                        }


                      //  Check the points against the points in the nearest points stack.

                      if (compare_to_stack (i, dist, &misc, z))
                        {
                          //  If the return was true then this is the minimum distance so far.

                          misc.nearest_point = misc.nearest_stack.point[0];

                          hot.x = misc.data[i].x;
                          hot.y = misc.data[i].y;
                          hot.z = misc.data[i].z;
                        }


                      //  If we're trying to display nulls we need to change the null point Z value back to the original null value.

                      if (null_set) misc.data[i].z = misc.null_val[misc.data[i].pfm];
                    }
                }
            }


          //  Only track the nearest position if we haven't frozen the marker.

          if (!misc.marker_mode)
            {
              misc.abe_share->cursor_position.y = misc.data[misc.nearest_point].y;
              misc.abe_share->cursor_position.x = misc.data[misc.nearest_point].x;
              misc.abe_share->cursor_position.z = misc.data[misc.nearest_point].z;
            }


          //  Update the status bars

          //
          //  if our point has moved or we have just selected a new overplot we will go
          //  into this code block.
          //

          if (misc.nearest_point != -1 && (misc.nearest_point != prev_nearest_point || options.function == MEASURE))
            {
              //  Show distance and azimuth to last highlighted point in status bar

              if (misc.highlight_count)
                {
                  invgp (NV_A0, NV_B0, misc.data[misc.highlight[misc.highlight_count - 1]].y, misc.data[misc.highlight[misc.highlight_count - 1]].x,
                         misc.data[misc.nearest_point].y, misc.data[misc.nearest_point].x, &distance[0], &azimuth[0]);

                  if (distance[0] < 0.01) azimuth[0] = 0.0;

                  distance_string = tr ("Distance : %L1 (m)  Azimuth : %L2 (degrees)").arg (distance[0], 0, 'f', 2).arg (azimuth[0], 0, 'f', 2);

                  misc.statusProgLabel->setToolTip (tr ("Distance and azimuth from last highlighted point"));
                  misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, misc.widgetBackgroundColor);
                  misc.statusProgLabel->setPalette (misc.statusProgPalette);
                  misc.statusProgLabel->setText (distance_string);
                }
              else
                {
                  misc.statusProgLabel->setToolTip ("");
                }


              //  Locking shared memory.

              misc.abeShare->lock ();


              //  External programs monitor abe_share->nearest_point to trigger updates of their windows.  They use
              //  abe_share->mwShare.multiRecord[0] for the current record to display.  In this way we can lock the
              //  main screen by not updating abe_share->mwShare.multiRecord[0] and still trigger the external programs
              //  to update (such as for multiple waveforms in CZMILwaveMonitor or waveWaterfall).

              int16_t type;
              read_list_file (misc.pfm_handle[misc.data[misc.nearest_point].pfm], misc.data[misc.nearest_point].file,
                              misc.abe_share->nearest_filename, &type);

              strcpy (nearest_line, read_line_file (misc.pfm_handle[misc.data[misc.nearest_point].pfm],
                                                    misc.data[misc.nearest_point].line % SHARED_LINE_MULT));


              //  Push the nearest stack points into the ABE shared memory for use by CZMILwaveMonitor, waveWaterfall,
              //  and lidarMonitor.

              for (int32_t j = 0 ; j < MAX_STACK_POINTS ; j++)
                {
                  //  Normal, unfrozen mode.

                  if (!misc.marker_mode)
                    {
                      misc.abe_share->mwShare.multiSubrecord[j] = misc.data[misc.nearest_stack.point[j]].sub;
                      misc.abe_share->mwShare.multiPfm[j] = misc.data[misc.nearest_stack.point[j]].pfm;
                      misc.abe_share->mwShare.multiFile[j] = misc.data[misc.nearest_stack.point[j]].file;
                      misc.abe_share->mwShare.multiLine[j] = misc.data[misc.nearest_stack.point[j]].line % SHARED_LINE_MULT;
                      misc.abe_share->mwShare.multiPoint[j].x = misc.data[misc.nearest_stack.point[j]].x;
                      misc.abe_share->mwShare.multiPoint[j].y = misc.data[misc.nearest_stack.point[j]].y;
                      misc.abe_share->mwShare.multiPoint[j].z = misc.data[misc.nearest_stack.point[j]].z;
                      misc.abe_share->mwShare.multiLocation[j] = misc.nearest_stack.point[j];
                      misc.abe_share->mwShare.multiType[j] = misc.data[misc.nearest_stack.point[j]].type;
                      misc.abe_share->mwShare.multiPresent[j] = misc.data[misc.nearest_stack.point[j]].rec;
                      misc.abe_share->mwShare.multiRecord[j] = misc.data[misc.nearest_stack.point[j]].rec;
                    }


                  //  If we're frozen, we don't want to replace the [0] record and we want to push the nearest
                  //  MAX_STACK_POINTS - 1 records up one level.

                  else
                    {
                      if (j)
                        {
                          misc.abe_share->mwShare.multiSubrecord[j] = misc.data[misc.nearest_stack.point[j - 1]].sub;
                          misc.abe_share->mwShare.multiPfm[j] = misc.data[misc.nearest_stack.point[j - 1]].pfm;
                          misc.abe_share->mwShare.multiFile[j] = misc.data[misc.nearest_stack.point[j - 1]].file;
                          misc.abe_share->mwShare.multiLine[j] = misc.data[misc.nearest_stack.point[j - 1]].line % SHARED_LINE_MULT;
                          misc.abe_share->mwShare.multiPoint[j].x = misc.data[misc.nearest_stack.point[j - 1]].x;
                          misc.abe_share->mwShare.multiPoint[j].y = misc.data[misc.nearest_stack.point[j - 1]].y;
                          misc.abe_share->mwShare.multiPoint[j].z = misc.data[misc.nearest_stack.point[j - 1]].z;
                          misc.abe_share->mwShare.multiLocation[j] = misc.nearest_stack.point[j - 1];
                          misc.abe_share->mwShare.multiType[j] = misc.data[misc.nearest_stack.point[j - 1]].type;
                          misc.abe_share->mwShare.multiPresent[j] = misc.data[misc.nearest_stack.point[j - 1]].rec;
                          misc.abe_share->mwShare.multiRecord[j] = misc.data[misc.nearest_stack.point[j - 1]].rec;
                        }
                    }
                }
              misc.abe_share->mwShare.multiNum = MAX_STACK_POINTS;


              //  Trigger some of the external applications to update.

              misc.abe_share->nearest_point = misc.nearest_point;


              //  Unlock shared memory.

              misc.abeShare->unlock ();


              //  If we're working with features display the position of the feature instead of the nearest point.

              if ((options.function == EDIT_FEATURE || options.function == DELETE_FEATURE ||
                   (options.function == MOVE_FEATURE && !move_feature)) && options.display_feature && 
                  misc.bfd_header.number_of_records)
                {
                  geo_xyz_label (misc.feature[misc.nearest_feature].latitude,  misc.feature[misc.nearest_feature].longitude,
                                 misc.feature[misc.nearest_feature].depth);
                }
              else
                {
                  geo_xyz_label (misc.data[misc.nearest_point].y,  misc.data[misc.nearest_point].x, misc.data[misc.nearest_point].z);
                }


              //  If we are frozen multiRecord[0] is useless to us, so we must check and use nearest point

              if (!misc.marker_mode)
                {
                  rec_string = QString ("%1/%2").arg (misc.abe_share->mwShare.multiRecord[0]).arg (misc.abe_share->mwShare.multiSubrecord[0]);
                  lin_num_string = QString ("%1").arg (misc.abe_share->mwShare.multiLine[0]);

                  pfm_string = QFileInfo (QString (misc.abe_share->open_args[misc.abe_share->mwShare.multiPfm[0]].list_path)).fileName ().remove (".pfm");
                  pfmLabel->setToolTip (QString (misc.abe_share->open_args[misc.abe_share->mwShare.multiPfm[0]].list_path));
                }
              else
                {
                  rec_string = QString ("%1/%2").arg (misc.data[misc.nearest_point].rec).arg (misc.data[misc.nearest_point].sub);
                  lin_num_string = QString ("%1").arg (misc.data[misc.nearest_point].line);

                  pfm_string = QFileInfo (QString (misc.abe_share->open_args[misc.data[misc.nearest_point].pfm].list_path)).fileName().remove(".pfm");
                  pfmLabel->setToolTip (QString (misc.abe_share->open_args[misc.data[misc.nearest_point].pfm].list_path));
                }

              fil_string =  QString (pfm_basename (misc.abe_share->nearest_filename));
              filLabel->setToolTip (QString (misc.abe_share->nearest_filename));
              lin_string = QString (nearest_line);


              recLabel->setText (rec_string);
              attrValue[1]->setText (lin_num_string);
              filLabel->setText (fil_string);
              pfmLabel->setText (pfm_string);
              linLabel->setText (lin_string);

              h_string = QString ("%L1").arg (misc.data[misc.nearest_point].herr, 0, 'f', 2);
              attrValue[2]->setText (h_string);

              v_string = QString ("%L1").arg (misc.data[misc.nearest_point].verr, 0, 'f', 2);
              attrValue[3]->setText (v_string);


              int32_t pfm = misc.data[misc.nearest_point].pfm;


              for (int32_t i = 0 ; i < misc.max_attr ; i++)
                {
                  QString attr_string;

                  if (i == misc.time_attr)
                    {
                      //  Don't try to format the NULL time string.

		      if (misc.data[misc.nearest_point].attr[i] < 0.0)
			{
			  attr_string = QString ("N/A");
			}
		      else
			{
                          int32_t year, jday, hour, minute;
                          float second;
                          time_t tv_sec = NINT (misc.data[misc.nearest_point].attr[i] * 60.0);

                          cvtime (tv_sec, 0, &year, &jday, &hour, &minute, &second);
                          attr_string = QString ("%1-%2 %3:%4").arg (year + 1900).arg (jday, 3, 10, QChar('0')).arg
                            (hour, 2, 10, QChar('0')).arg (minute, 2, 10, QChar('0'));
                        }
                    }
                  else if (i == misc.czmil_urban_attr)
                    {
                      QString binary = QString ("%1 = ").arg (NINT (misc.data[misc.nearest_point].attr[i]), 5, 2, QChar ('0'));
                      char integer[24];
                      sprintf (integer, misc.attr_format[pfm][i], misc.data[misc.nearest_point].attr[i]);
                      attr_string = binary + QString (integer);
                    }
                  else
                    {
                      attr_string.sprintf (misc.attr_format[pfm][i], misc.data[misc.nearest_point].attr[i]);
                    }

                  attrBoxValue[i]->setText (attr_string);
                }


              //  Populate the Status tab attributes.

              for (int32_t i = 0 ; i < POST_ATTR ; i++)
                {
                  if (misc.attrStatNum[i] > -1)
                    {
                      int32_t j = misc.attrStatNum[i];

                      attrName[i + PRE_ATTR]->setText (attrBoxName[j]->text ());
                      attrValue[i + PRE_ATTR]->setText (attrBoxValue[j]->text ());
                    }
                }
            }


          //  If we're working with a feature set the correct hot position

          if (hot_feature.x != -1)
            {
              hot.x = hot_feature.x;
              hot.y = hot_feature.y;
              hot.z = hot_feature.z;
            }


          //  Set the delete, set, unset point marker.

          if (!misc.abe_share->mwShare.multiMode && (options.function == DELETE_POINT))
            {
              DrawMultiCursors ();
            }
          else
            {
              //  If we're frozen, use the frozen point for the marker position.

              if (misc.marker_mode)
                {
                  hot.x = misc.data[misc.frozen_point].x;
                  hot.y = misc.data[misc.frozen_point].y;
                  hot.z = misc.data[misc.frozen_point].z;
                }

              int32_t pix_x, pix_y;
              map->get2DCoords (hot.x, hot.y, -hot.z, &pix_x, &pix_y);

              map->setMovingList (&(multi_marker[0]), marker, 16, pix_x, pix_y, 0.0, 2, misc.abe_share->mwShare.multiColors[0]);
            }


          if (options.function == MEASURE)
            {
              if (map->rubberbandLineIsActive (rb_measure))
                {
                  //  Snap the end of the line to the nearest point

                  int32_t pix_x, pix_y;
                  map->get2DCoords (misc.data[misc.nearest_point].x, misc.data[misc.nearest_point].y, -misc.data[misc.nearest_point].z, &pix_x, &pix_y);

                  map->dragRubberbandLine (rb_measure, pix_x, pix_y);


                  invgp (NV_A0, NV_B0, misc.data[misc.nearest_point].y, misc.data[misc.nearest_point].x, line_anchor.y, line_anchor.x, &distance[0],
                         &azimuth[0]);

                  if (distance[0] < 0.01) azimuth[0] = 0.0;


                  NV_F64_COORD3 coords;
                  map->getFaux3DCoords (line_anchor.x, line_anchor.y, -line_anchor.z, xy.x, xy.y, &coords);
                  invgp (NV_A0, NV_B0, coords.y, coords.x, line_anchor.y, line_anchor.x, &distance[1], &azimuth[1]);

                  if (distance[1] < 0.01) azimuth[1] = 0.0;

                  distance_string = tr ("Nearest Point Dist: %L1  Az: %L2  deltaZ: %L3     Cursor Dist: %L4  Az: %L5  deltaZ: %L6").arg (distance[0], 0, 'f', 2).arg 
                    (azimuth[0], 0, 'f', 2).arg (line_anchor.z - misc.data[misc.nearest_point].z, 0, 'f', 2).arg (distance[1], 0, 'f', 2).arg (azimuth[1], 0, 'f', 2).arg 
                    (line_anchor.z - (-coords.z), 0, 'f', 2);

                  misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, misc.widgetBackgroundColor);
                  misc.statusProgLabel->setPalette (misc.statusProgPalette);
                  misc.statusProgLabel->setText (distance_string);
                  misc.statusProgLabel->setVisible (true);
                }
            }
        }


      //  Set the previous nearest point positions.
             
      prev_hot = hot;
      break;


    case DELETE_RECTANGLE:
    case RESTORE_RECTANGLE:
    case MASK_INSIDE_RECTANGLE:
    case MASK_OUTSIDE_RECTANGLE:
    case RECT_FILTER_MASK:
      if (map->rubberbandRectangleIsActive (rb_rectangle)) map->dragRubberbandRectangle (rb_rectangle, xy.x, xy.y);

      geo_xyz_label (lat, lon, -z);
      recLabel->setText (rec_string);
      attrValue[1]->setText (lin_num_string);
      filLabel->setText (fil_string);
      pfmLabel->setText (pfm_string);
      linLabel->setText (lin_string);
      break;

    case DELETE_POLYGON:
    case RESTORE_POLYGON:
    case KEEP_POLYGON:
    case SET_REFERENCE:
    case UNSET_REFERENCE:
    case HOTKEY_POLYGON:
    case MASK_INSIDE_POLYGON:
    case MASK_OUTSIDE_POLYGON:
    case POLY_FILTER_MASK:
    case HIGHLIGHT_POLYGON:
    case CLEAR_POLYGON:
    case CPF_FILTER:
    case CHANGE_LIDAR_CLASS:

      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          if (!polygon_flip)
            {
              polygon_flip = NVTrue;

              if (prev_poly_x > -1)
                {
                  map->vertexRubberbandPolygon (rb_polygon, prev_poly_x, prev_poly_y);
                }
              else
                {
                  map->vertexRubberbandPolygon (rb_polygon, xy.x, xy.y);
                }
            }
          else
            {
              map->dragRubberbandPolygon (rb_polygon, xy.x, xy.y);
              prev_poly_x = xy.x;
              prev_poly_y = xy.y;
              polygon_flip = NVFalse;
            }
        }

      geo_xyz_label (lat, lon, -z);
      recLabel->setText (rec_string);
      attrValue[1]->setText (lin_num_string);
      filLabel->setText (fil_string);
      pfmLabel->setText (pfm_string);
      linLabel->setText (lin_string);

      break;

    case CZMIL_REPROCESS_LAND:
    case CZMIL_REPROCESS_WATER:
    case CZMIL_REPROCESS_SHALLOW_WATER:

      //  Polygon mode

      if (options.czmil_reprocess_mode)
        {
          if (map->rubberbandPolygonIsActive (rb_polygon))
            {
              if (!polygon_flip)
                {
                  polygon_flip = NVTrue;

                  if (prev_poly_x > -1)
                    {
                      map->vertexRubberbandPolygon (rb_polygon, prev_poly_x, prev_poly_y);
                    }
                  else
                    {
                      map->vertexRubberbandPolygon (rb_polygon, xy.x, xy.y);
                    }
                }
              else
                {
                  map->dragRubberbandPolygon (rb_polygon, xy.x, xy.y);
                  prev_poly_x = xy.x;
                  prev_poly_y = xy.y;
                  polygon_flip = NVFalse;
                }
            }
        }


      //  Rectangle mode

      else
        {
          if (map->rubberbandRectangleIsActive (rb_rectangle)) map->dragRubberbandRectangle (rb_rectangle, xy.x, xy.y);
        }

      geo_xyz_label (lat, lon, -z);
      recLabel->setText (rec_string);
      attrValue[1]->setText (lin_num_string);
      filLabel->setText (fil_string);
      pfmLabel->setText (pfm_string);
      linLabel->setText (lin_string);
      break;
   }


  //  Track the cursor in the 2D tracker box (if the 2D tracker is up).

  if (options.track_tab)
    {
      QBrush b1;
      trackMap->setMovingPath (&mv_2D_tracker, tracker_2D, misc.abe_share->cursor_position.x, misc.abe_share->cursor_position.y, 2,
                               options.tracker_color, b1, NVFalse, Qt::SolidLine);
    }


  //  Set the previous cursor.

  prev_xy = xy;

  prev_nearest_point = misc.nearest_point;


  //  If we are frozen, restore the misc.nearest_point

  if (misc.marker_mode) misc.nearest_point = save_nearest_point;
}



/*!
  - DrawMultiCursors

  - This method will loop through the nearest neighbors, and place the multi-cursor on the proper shot
    whether we are in a normal top-down view or a sliced view.
*/

void 
pfmEdit3D::DrawMultiCursors ()
{
  uint8_t withinSlice;                     //       boolean check to see if shot is within current volumetric slice

           
  uint8_t init2D = NVTrue;
  for (int32_t i = MAX_STACK_POINTS - 1 ; i >= 0 ; i--) 
    {
      withinSlice = NVTrue;
     
      if ((misc.abe_share->mwShare.multiPoint[i].x >= misc.displayed_area.min_x) && 
          (misc.abe_share->mwShare.multiPoint[i].x <= misc.displayed_area.max_x) && 
          (misc.abe_share->mwShare.multiPoint[i].y >= misc.displayed_area.min_y) && 
          (misc.abe_share->mwShare.multiPoint[i].y <= misc.displayed_area.max_y) &&
          (misc.abe_share->mwShare.multiPresent[i] != -1) && (withinSlice))
        {
          int32_t pix_x, pix_y;
          map->get2DCoords (misc.abe_share->mwShare.multiPoint[i].x, misc.abe_share->mwShare.multiPoint[i].y, -misc.abe_share->mwShare.multiPoint[i].z,
                            &pix_x, &pix_y, &init2D);

          map->setMovingList (&(multi_marker[i]), marker, 16, pix_x, pix_y, 0.0, 2, misc.abe_share->mwShare.multiColors[i]);
        }
      else
        {
          map->closeMovingList (&(multi_marker[i]));
        }
    }
}



/*!
  Resize signal from the map class.  With Qt 4.6.2 this seems to be redundant (and can cause all kinds of problems) so
  this is just a stub at the moment.
*/

void
pfmEdit3D::slotResize (QResizeEvent *e __attribute__ ((unused)))
{
#if QT_VERSION < 0x040602
  redrawMap (NVTrue);
#endif
}



//!  This is where we load all of our data in to OpenGL display lists (in nvMapGL.cpp).

void
pfmEdit3D::redrawMap (uint8_t redraw2D)
{
  static uint8_t startup = NVTrue;


  void setScale (float attr_min, float attr_max, MISC *misc, OPTIONS *options, uint8_t min_lock, uint8_t max_lock);
  uint8_t compute_bounds (nvMapGL *map, OPTIONS *options, MISC *misc, int32_t *viewable_count, float *color_min, float *color_max);


  misc.busy = NVTrue;


  if (startup)
    {
      map->enableSignals ();
      trackMap->enableSignals ();
      histogram->enableSignals ();
      startup = NVFalse;
    }


  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  discardMovableObjects ();


  //  Erase all text from the map.

  map->clearText ();


  //  Erase all objects (like spheres) from the map.

  map->clearDisplayLists ();


  //  Erase all data points from the map.

  map->clearDataPoints ();


  //  Erase all contour lines from the map.

  map->clearLines ();


  //  If we have more than sparse_limit points, erase all sparse points from the map.

  if (misc.need_sparse) map->clearSparsePoints ();


  setWidgetStates (NVFalse);


  NVMAPGL_DEF l_mapdef = map->getMapdef ();


  misc.draw_area_height = l_mapdef.draw_height;
  misc.draw_area_width = l_mapdef.draw_width;

  map->setCursor (Qt::WaitCursor);

  misc.drawing_canceled = NVFalse;


  //  Compute the Z range of the data.

  int32_t viewable_count;

  misc.bounds = mapdef.initial_bounds;
  misc.bounds.min_z = CHRTRNULL;
  misc.bounds.max_z = -CHRTRNULL;


  //  Compute the min and max values for defining the 3D space and optionally for coloring.

  uint8_t bounds_changed = compute_bounds (map, &options, &misc, &viewable_count, &misc.attr_min, &misc.attr_max);


  //  Compute the top level for drawing contours in the 3D space.  This is 1 percent above the top level so that it won't
  //  mask any data points.

  misc.contour_level = misc.bounds.max_z + ((misc.bounds.max_z - misc.bounds.min_z) * 0.01);


  //  If we're not coloring by attribute and the min or max hsv lock is set we need to check to see if we want to use the locked value(s).

  misc.attr_color_min = misc.color_min_z = misc.attr_min;
  misc.attr_color_max = misc.color_max_z = misc.attr_max;


  uint8_t min_lock = NVFalse, max_lock = NVFalse;
  if (!options.color_index)
    {
      if (misc.abe_share->min_hsv_locked)
        {
          if (misc.color_max_z > -misc.abe_share->min_hsv_value)
            {
              misc.color_max_z = -misc.abe_share->min_hsv_value;
              max_lock = NVTrue;
            }
        }

      if (misc.abe_share->max_hsv_locked)
        {
          if (misc.color_min_z < -misc.abe_share->max_hsv_value)
            {
              misc.color_min_z = -misc.abe_share->max_hsv_value;
              min_lock = NVTrue;
            }
        }
    }
  else
    {
      int32_t ndx = options.color_index;
      if (options.min_hsv_locked[ndx])
        {
          if (misc.attr_color_min < options.min_hsv_value[ndx])
            {
              misc.attr_color_min = options.min_hsv_value[ndx];
              min_lock = NVTrue;
            }
        }

      if (options.max_hsv_locked[ndx])
        {
          if (misc.attr_color_max > options.max_hsv_value[ndx])
            {
              misc.attr_color_max = options.max_hsv_value[ndx];
              max_lock = NVTrue;
            }
        }

      misc.attr_color_range = misc.attr_color_max - misc.attr_color_min;
    }

  misc.color_range_z = misc.color_max_z - misc.color_min_z;


  //  Only display if there are viewable points.

  if (viewable_count)
    {
      setScale (misc.attr_min, misc.attr_max, &misc, &options, min_lock, max_lock);


      range_x = misc.bounds.max_x - misc.bounds.min_x;
      range_y = misc.bounds.max_y - misc.bounds.min_y;
      range_z = misc.bounds.max_z - misc.bounds.min_z;


      //  Add 10 percent to the X, Y, and Z bounds.

      misc.bounds.min_x -= (range_x * 0.10);
      misc.bounds.max_x += (range_x * 0.10);

      misc.bounds.min_y -= (range_y * 0.10);
      misc.bounds.max_y += (range_y * 0.10);

      if (range_z == 0.0)
        {
          misc.bounds.min_z -= 1.0;
          misc.bounds.max_z += 1.0;
          range_z = misc.bounds.max_z - misc.bounds.min_z;
        }
      else
        {
          misc.bounds.min_z -= (range_z * 0.10);
          misc.bounds.max_z += (range_z * 0.10);
        }


      if (bounds_changed) map->setBounds (misc.aspect_bounds);


      if (!misc.slice) compute_ortho_values (map, &misc, &options, sliceBar, NVTrue);


      int32_t c_index = -1;
      int32_t prev_xcoord = -1;
      int32_t prev_ycoord = -1;
      int32_t min_z_index = -1;
      int32_t max_z_index = -1;
      float min_z = 999999999.0;
      float max_z = -999999999.0;
      uint8_t save_min[4], save_max[4];
      uint8_t null_point;
      float z_value;
      float numshades = (float) (NUMSHADES - 1);


      //  Check for display invalid and flag invalid

      if (options.flag_index == PFM_USER_FLAGS && (options.display_man_invalid || options.display_flt_invalid)) setFlags (&misc, &options);


      //  Use the normal color array or the line color array.

      uint8_t (*color_array_ptr)[4] = misc.color_array;
      if (options.color_index == 1) color_array_ptr = misc.line_color_array;


      for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
        {
          //  Check for single line display.

          if (!misc.num_lines || check_line (&misc, misc.data[i].line))
            {
              //  Check against the viewing area.

              int32_t trans = 0;
              if ((trans = check_bounds (&options, &misc, i, NVTrue, misc.slice)) < 2)
                {
                  z_value = misc.data[i].z;
                  null_point = NVFalse;


                  //  Check for null point.  We want to place these on the bottom of the displayed cube.

                  if (misc.data[i].z >= misc.null_val[misc.data[i].pfm])
                    {
                      null_point = NVTrue;
                      z_value = -misc.bounds.min_z;
                    }


                  //  Use the edit color for reference, null data, and invalid data if the invalid_edit_color flag is set.

                  if ((misc.data[i].val & PFM_REFERENCE) || null_point || (options.invalid_edit_color && (misc.data[i].val & PFM_INVAL)))
                    {
                      c_index = -1;
                    }


                  //  Check for color by attribute

                  else
                    {
                      if (!options.color_index)
                        {
                          //  Color by depth

                          c_index = NINT (numshades - ((-misc.data[i].z) - misc.color_min_z) / misc.color_range_z * numshades);
                        }
                      else if (options.color_index == 1)
                        {
                          c_index = NINT (numshades - (((float) (misc.data[i].line_index) / (float) misc.line_count) * numshades));
                        }
                      else if (options.color_index == 2)
                        {
                          //  Color by horizontal uncertainty

                          c_index = NINT (numshades - (misc.data[i].herr - misc.attr_color_min) / misc.attr_color_range * numshades);
                        }
                      else if (options.color_index == 3)
                        {
                          //  Color by vertical uncertainty

                          c_index = NINT (numshades - (misc.data[i].verr - misc.attr_color_min) / misc.attr_color_range * numshades);
                        }


                      //  Color by PFM attribute

                      else if (options.color_index >= PRE_ATTR && options.color_index < PRE_ATTR + POST_ATTR)
                        {
                          int32_t ndx = misc.attrStatNum[options.color_index - PRE_ATTR];
                          c_index = NINT (numshades - (misc.data[i].attr[ndx] - misc.attr_color_min) / misc.attr_color_range * numshades);
                        }


                      //  Check for out of range data.

                      if (c_index < 0) c_index = 0;

                      if (c_index > NUMSHADES - 1) c_index = NUMSHADES - 1;
                    }


                  uint8_t tmp[4];


                  //  If the color index was out of range, use the reference color.

                  if (c_index < 0)
                    {
                      tmp[0] = options.ref_color[trans].red ();
                      tmp[1] = options.ref_color[trans].green ();
                      tmp[2] = options.ref_color[trans].blue ();
                      tmp[3] = options.ref_color[trans].alpha ();
                    }
                  else
                    {
                      //  If the point is outside the slice (transparent)...

                      if (trans)
                        {
                          //  Set to gray...

                          if (options.slice_gray)
                            {
                              tmp[0] = 127;
                              tmp[1] = 127;
                              tmp[2] = 127;
                            }


                          //  or color...

                          else
                            {
                              memcpy (tmp, color_array_ptr[c_index], 3);
                            }


                          //  with transparency.

                          tmp[3] = options.slice_alpha;
                        }
                      else
                        {
                          //  Normal color

                          memcpy (tmp, color_array_ptr[c_index], 4);
                        }
                    }


                  //  If we have more than sparse_limit points we want to load a sparse layer for rotating and zooming.

                  if (misc.need_sparse)
                    {
                      //  If we are slicing and we need sparse data for rotation and zoom, only display sparse points
                      //  outside of the slice.  If we aren't slicing, display them all.

                      if (!misc.slice || !trans) map->setDataPoints (misc.data[i].x, misc.data[i].y, -z_value, tmp, 0, NVFalse);


                      //  Get the min and max displayed Z values for each bin.

                      if (prev_xcoord != -1 && (misc.data[i].xcoord != prev_xcoord || misc.data[i].ycoord != prev_ycoord))
                        {
                          if (min_z_index >= 0)
                            {
                              map->setSparsePoints (misc.data[min_z_index].x, misc.data[min_z_index].y, -misc.data[min_z_index].z, save_min, 0, NVFalse);


                              //  If we're slicing and we need sparse data for zoom and rotate, only load points inside the slice.

                              if (misc.slice && trans) map->setDataPoints (misc.data[min_z_index].x, misc.data[min_z_index].y, -misc.data[min_z_index].z,
                                                                           tmp, 0, NVFalse);
                            }

                          if (max_z_index >= 0 && max_z_index != min_z_index)
                            {
                              map->setSparsePoints (misc.data[max_z_index].x, misc.data[max_z_index].y, -misc.data[max_z_index].z, save_max, 0, NVFalse);


                              //  If we're slicing and we need sparse data for zoom and rotate, only load points inside the slice.

                              if (misc.slice && trans) map->setDataPoints (misc.data[max_z_index].x, misc.data[max_z_index].y, -misc.data[max_z_index].z,
                                                                           tmp, 0, NVFalse);
                            }

                          min_z_index = max_z_index = -1;
                          min_z = 999999999.0;
                          max_z = -999999999.0;
                        }


                      if (null_point)
                        {
                          if (z_value < min_z)
                            {
                              min_z = z_value;
                              min_z_index = i;
                              memcpy (save_min, tmp, 4);
                            }
                        }
                      else
                        {
                          if (misc.data[i].z < min_z)
                            {
                              min_z = misc.data[i].z;
                              min_z_index = i;
                              memcpy (save_min, tmp, 4);
                            }
                        }


                      //  Don't use null points to determine max value for the  bin.

                      if (!null_point && misc.data[i].z > max_z)
                        {
                          max_z = misc.data[i].z;
                          max_z_index = i;
                          memcpy (save_max, tmp, 4);
                        }

                      prev_xcoord = misc.data[i].xcoord;
                      prev_ycoord = misc.data[i].ycoord;
                    }


                  //  We don't need sparse data for rotating or zooming so we set all of the data points.

                  else
                    {
                      map->setDataPoints (misc.data[i].x, misc.data[i].y, -z_value, tmp, 0, NVFalse);
                    }
                }
            }
        }


      //  If we have more than sparse_limit points we want to load a sparse layer for rotating and zooming.

      if (misc.need_sparse)
        {
          if (min_z_index >= 0)
            {
              map->setSparsePoints (misc.data[min_z_index].x, misc.data[min_z_index].y, -misc.data[min_z_index].z, save_min, 0, NVFalse);

              if (misc.slice) map->setDataPoints (misc.data[min_z_index].x, misc.data[min_z_index].y, -misc.data[min_z_index].z, save_min, 0, NVFalse);

            }

          if (max_z_index >= 0 && max_z_index != min_z_index)
            {
              map->setSparsePoints (misc.data[max_z_index].x, misc.data[max_z_index].y, -misc.data[max_z_index].z, save_max, 0, NVFalse);

              if (misc.slice) map->setDataPoints (misc.data[max_z_index].x, misc.data[max_z_index].y, -misc.data[max_z_index].z, save_max, 0, NVFalse);
            }

          map->setSparsePoints (0.0, 0.0, 0.0, save_min, options.point_size, NVTrue);
        }


      map->setDataPoints (0.0, 0.0, 0.0, save_min, options.point_size, NVTrue);
    }


  //  Set the features.

  if (!misc.drawing_canceled)
    {
      overlayFlag (map, &options, &misc);
      overlayData (map, &options, &misc);


      //  If we have highlighted or filter kill points set the map tooltip so the user knows what to do.

      if (options.highlight_tip && (misc.highlight_count || misc.filter_kill_count))
        {
          map->setToolTip (tr ("<p style='white-space:pre'>Press <b>Del</b> to delete points or <b>Ins</b> to clear points")); 
        }
      else
        {
          map->setToolTip ("");
        } 
    }


  //  We want to flush the map just in case we didn't have any points to draw.

  map->flush ();


  //  Redraw the 2D tracker unless this redraw was caused by an operation that has no effect on the 2D map or the 2D map isn't selected.

  if (options.track_tab)
    {
      if (redraw2D) trackMap->redrawMap (NVTrue);
    }
  else
    {
      //  Draw contours if requested.

      if (options.display_contours) slotTrackPreRedraw (track_mapdef);


      histogram->redrawMap (NVTrue);
    }


  //  If we're not auto-scaling we need to turn it off.

  map->setAutoScale (options.auto_scale);


  misc.busy = NVFalse;

  setWidgetStates (NVTrue);

  setFunctionCursor (options.function);

  qApp->restoreOverrideCursor ();
}



void 
pfmEdit3D::slotMaskClear ()
{
  for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
    {
      if (misc.data[i].mask & MASK_SHAPE) misc.data[i].mask &= UNMASK_SHAPE;
    }
  options.mask_active &= UNMASK_SHAPE;
  bMaskClear->setIcon (QIcon (":/icons/clear_mask_shape.png"));


  //  Turn off slicing if it was on (this will also call redrawMap ()).

  slotPopupMenu (popup[NUMPOPUPS - 1]);
}



void 
pfmEdit3D::slotMaskReset ()
{
  //  Clear the "hide" buttons

  disconnect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));

  if (misc.unique_count > 1)
    {
      for (int32_t i = 0 ; i < misc.unique_count ; i++)
        {
          hideAct[i + flags_offset]->setChecked (false);
          hideAct[i + flags_offset]->setEnabled (true);
        }
    }
  if (misc.czmil_chan_attr >= 0 && misc.unique_count == 1)
    {
      for (int32_t i = 0 ; i < 9 ; i++)
        {
          hideAct[i + flags_offset]->setChecked (false);
          hideAct[i + flags_offset]->setEnabled (true);
        }
    }
  connect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));

  for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++) if (misc.data[i].mask & MASK_TYPE) misc.data[i].mask &= UNMASK_TYPE;
  options.mask_active &= UNMASK_TYPE;
  bMaskReset->setIcon (QIcon (":/icons/clear_mask_type.png"));


  //  Turn off slicing if it was on (this will also call redrawMap ()).

  slotPopupMenu (popup[NUMPOPUPS - 1]);
}



void 
pfmEdit3D::slotFlagReset ()
{
  //  Clear the "hide" buttons

  disconnect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));

  for (int32_t i = 0 ; i < flags_offset ; i++)
    {
      hideAct[i]->setChecked (false);
      hideAct[i]->setEnabled (true);
    }

  connect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));

  for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++) if (misc.data[i].mask & MASK_FLAG) misc.data[i].mask &= UNMASK_FLAG;
  options.mask_active &= UNMASK_FLAG;
  bFlagReset->setIcon (QIcon (":/icons/clear_mask_flag.png"));


  //  Turn off slicing if it was on (this will also call redrawMap ()).

  slotPopupMenu (popup[NUMPOPUPS - 1]);
}



void 
pfmEdit3D::slotReset ()
{
  misc.poly_count = 0;

  misc.slice = NVFalse;

  misc.slice_min = misc.ortho_min;
  sliceBar->setValue (misc.ortho_min);


  //  Turn on depth testing in the map widget

  map->setDepthTest (NVTrue);


  discardMovableObjects ();


  map->setMapCenter (misc.map_center_x, misc.map_center_y, misc.map_center_z);


  map->resetZoom ();
  map->setYRotation (mapdef.initial_y_rotation);
  map->setZXRotation (mapdef.initial_zx_rotation);


  //  Always turn auto-scaling on whenever we reset.  If we're not auto-scaling it will be turned off again after
  //  we redraw the map.

  map->setAutoScale (NVTrue);


  redrawMap (NVTrue);
}



void 
pfmEdit3D::slotLevel ()
{
  misc.poly_count = 0;

  discardMovableObjects ();

  map->setZXRotation (0.0);

  redrawMap (NVTrue);
}



void 
pfmEdit3D::slotNorthUp ()
{
  misc.poly_count = 0;

  discardMovableObjects ();

  map->setMapCenter (misc.map_center_x, misc.map_center_y, misc.map_center_z);


  map->resetZoom ();
  map->setYRotation (0.0);
  map->setZXRotation (90.0);


  //  Always turn auto-scaling on whenever we reset.  If we're not auto-scaling it will be turned off again after
  //  we redraw the map.

  map->setAutoScale (NVTrue);


  redrawMap (NVTrue);
}



void 
pfmEdit3D::slotSliceTriggered (int action)
{
  if (!misc.busy)
    {
      //  Have to compute the ortho values and set the slider to minimum before we slice for the first time.

      if (!misc.slice) compute_ortho_values (map, &misc, &options, sliceBar, NVTrue);


      map->setDepthTest (NVFalse);


      //  Note that we're setting the misc.slice flag to true after we set the slice location.  This is so we can check to
      //  see if this is the first slice.  The reason being that we don't want to add to the start location if this is the
      //  first slice.

      switch (action)
        {
        case QAbstractSlider::SliderSingleStepAdd:
          if (misc.slice) misc.slice_min -= misc.slice_size;
          misc.slice_max = misc.slice_min + misc.slice_size;
          break;

        case QAbstractSlider::SliderPageStepAdd:
          if (misc.slice) misc.slice_min -= misc.slice_size;
          misc.slice_max = misc.slice_min + misc.slice_size;
          break;

        case QAbstractSlider::SliderSingleStepSub:
          if (misc.slice) misc.slice_max += misc.slice_size;
          misc.slice_min = misc.slice_max - misc.slice_size;
          break;

        case QAbstractSlider::SliderPageStepSub:
          if (misc.slice) misc.slice_max += misc.slice_size;
          misc.slice_min = misc.slice_max - misc.slice_size;
          break;

        case QAbstractSlider::SliderMove:
          if (misc.slice) misc.slice_min = sliceBar->value ();
          misc.slice_max = misc.slice_min + misc.slice_size;
          break;
        }


      if (misc.slice_min < misc.ortho_min)
        {
          misc.slice_min = misc.ortho_min;
          misc.slice_max = misc.ortho_min + misc.slice_size;
          if (action != QAbstractSlider::SliderMove) sliceBar->setValue (misc.slice_min);
        }
      else if (misc.slice_max > misc.ortho_max)
        {
          misc.slice_max = misc.ortho_max;
          misc.slice_min = misc.ortho_max - misc.slice_size;
          if (action != QAbstractSlider::SliderMove) sliceBar->setValue (misc.slice_max);
        }
      else
        {
          if (action != QAbstractSlider::SliderMove) sliceBar->setValue (misc.slice_min);
        }


      misc.slice = NVTrue;


      slicing = NVTrue;
      redrawMap (NVFalse);
      slicing = NVFalse;


      //  Even though we do this in redrawMap we need to do it again here because otherwise the damn markers disappear.
      //  It's OpenGL, I have no idea why.

      overlayData (map, &options, &misc);
    }
}



void 
pfmEdit3D::slotSizeTriggered (int action)
{
  if (!misc.busy)
    {
      switch (action)
        {
        case QAbstractSlider::SliderSingleStepAdd:
        case QAbstractSlider::SliderPageStepAdd:
          options.slice_percent--;
          if (options.slice_percent <= 0) options.slice_percent = 1;
          break;

        case QAbstractSlider::SliderSingleStepSub:
        case QAbstractSlider::SliderPageStepSub:
          options.slice_percent++;
          if (options.slice_percent > MAX_SLICE_SIZE) options.slice_percent = MAX_SLICE_SIZE;
          break;

        case QAbstractSlider::SliderMove:
          options.slice_percent = sizeBar->value ();
          break;
        }


      if (action != QAbstractSlider::SliderMove) sizeBar->setValue (options.slice_percent);


      int32_t val = sliceBar->value ();

      sizeLabel->setNum (options.slice_percent);

      compute_ortho_values (map, &misc, &options, sliceBar, NVFalse);
      map->setDepthTest (NVFalse);

      misc.slice_min = val;
      misc.slice_max = misc.slice_min + misc.slice_size;

      if (misc.slice_max > misc.ortho_max)
        {
          misc.slice_min = misc.ortho_max - misc.slice_size;
          misc.slice_max = misc.ortho_max;
        }

      slicing = NVTrue;
      redrawMap (NVFalse);
      slicing = NVFalse;


      //  Even though we do this in redrawMap we need to do it again here because otherwise the damn markers disappear.
      //  It's OpenGL, I have no idea why.

      overlayData (map, &options, &misc);
    }
}



void 
pfmEdit3D::slotTransTriggered (int action)
{
  if (!misc.busy)
    {
      switch (action)
        {
        case QAbstractSlider::SliderSingleStepAdd:
        case QAbstractSlider::SliderPageStepAdd:
          options.slice_alpha--;
          if (options.slice_alpha < 0) options.slice_alpha = 1;
          break;

        case QAbstractSlider::SliderSingleStepSub:
        case QAbstractSlider::SliderPageStepSub:
          options.slice_alpha++;
          if (options.slice_alpha > MAX_TRANS_VALUE) options.slice_alpha = MAX_TRANS_VALUE;
          break;

        case QAbstractSlider::SliderMove:
          options.slice_alpha = transBar->value ();
          break;
        }


      if (action != QAbstractSlider::SliderMove) transBar->setValue (options.slice_alpha);


      //  Set the colors.

      float hue_inc = (float) (misc.abe_share->max_hsv_color - misc.abe_share->min_hsv_color) / (float) (NUMSHADES + 1);

      for (int32_t j = 0 ; j < NUMSHADES ; j++)
        {
          QColor color;
          color.setHsv ((int32_t) (misc.abe_share->min_hsv_color + j * hue_inc), 255, 255, 255); 

          misc.color_array[j][0] = color.red ();
          misc.color_array[j][1] = color.green ();
          misc.color_array[j][2] = color.blue ();
          misc.color_array[j][3] = color.alpha ();
        }

      options.ref_color[1] = options.ref_color[0];
      options.ref_color[1].setAlpha (options.slice_alpha);


      transLabel->setText (tr ("%L1").arg (options.slice_alpha, 2, 10, QChar('0')));


      redrawMap (NVFalse);
    }
}



void 
pfmEdit3D::slotExagTriggered (int action)
{
  if (!misc.busy)
    {
      switch (action)
        {
        case QAbstractSlider::SliderSingleStepAdd:
          if (options.exaggeration >= 1.0)
            {
              options.exaggeration -= 1.0;

              if (options.exaggeration < 1.0) options.exaggeration = 1.0;

              map->setExaggeration (options.exaggeration);

              redrawMap (NVFalse);
            }
          break;

        case QAbstractSlider::SliderPageStepAdd:
          if (options.exaggeration >= 5.0)
            {
              options.exaggeration -= 5.0;

              if (options.exaggeration < 1.0) options.exaggeration = 1.0;

              map->setExaggeration (options.exaggeration);

              redrawMap (NVFalse);
            }
          break;

        case QAbstractSlider::SliderSingleStepSub:
          options.exaggeration += 1.0;
          if (options.exaggeration > 100.0) options.exaggeration = 100.0;
          map->setExaggeration (options.exaggeration);

          redrawMap (NVFalse);

          break;

        case QAbstractSlider::SliderPageStepSub:
          options.exaggeration += 5.0;
          if (options.exaggeration > 100.0) options.exaggeration = 100.0;
          map->setExaggeration (options.exaggeration);

          redrawMap (NVFalse);

          break;

        case QAbstractSlider::SliderMove:
          exagBar->setToolTip (tr ("Vertical exaggeration = %L1").arg ((float) exagBar->value () / 100.0, 5, 'f', 3));
          break;
        }


      //  Even though we do this in redrawMap we need to do it again here because otherwise the damn markers disappear.
      //  It's OpenGL, I have no idea why.

      overlayData (map, &options, &misc);
    }
}



void 
pfmEdit3D::slotExagReleased ()
{
  options.exaggeration = (float) exagBar->value () / 100.0;
  map->setExaggeration (options.exaggeration);

  redrawMap (NVFalse);
}



void 
pfmEdit3D::slotExaggerationChanged (float exaggeration, float apparent_exaggeration)
{
  if (exaggeration < 1.0)
    {
      exagBar->setEnabled (false);
    }
  else
    {
      exagBar->setEnabled (true);
    }

  exagBar->setToolTip (tr ("Vertical exaggeration = %L1").arg (apparent_exaggeration, 5, 'f', 3));

  options.exaggeration = exaggeration;

  disconnect (exagBar, SIGNAL (actionTriggered (int)), this, SLOT (slotExagTriggered (int)));
  exagBar->setValue (NINT (options.exaggeration * 100.0));
  connect (exagBar, SIGNAL (actionTriggered (int)), this, SLOT (slotExagTriggered (int)));
}



void
pfmEdit3D::keyReleaseEvent (QKeyEvent *e)
{
  //  We're checking for the release of the Shift key if we are in "shift slice" mode.

  if (options.function == SHIFT_SLICE && e->key () == Qt::Key_Shift)
    {
      options.function = misc.save_function;
      setFunctionCursor (options.function);


      //  All we have to do is flush here since setFunctionCursor will set the map mode back to NVMAPGL_POINT_MODE and
      //  we haven't actually changed the point data.

      if (misc.need_sparse) map->flush ();

      overlayData (map, &options, &misc);
    }
}



void
pfmEdit3D::keyPressEvent (QKeyEvent *e)
{
  void actionKey (pfmEdit3D *parent, nvMapGL *map, MISC *misc, QString key, int32_t lock_point);


  //  Lock in nearest point so mouse moves won't change it.

  lock_point = misc.nearest_point;


  //  If we're drawing and a key is pressed we want to cancel drawing.

  if (misc.busy)
    {
      misc.drawing_canceled = NVTrue;
      return;
    }


  //  First check for the hard-wired keys.

  switch (e->key ())
    {
      //  Arrow keys.
      //
      //  Unmodified up arrow key will start slicing
      //
      //  Ctrl-arrow keys will move in the direction of the arrow with some percentage of overlap.  This implies that
      //  we want to save our edits and we must move the edit area in the viewer to match where we've moved.  If we
      //  moved outside of the area displayed in the viewer we must also move the viewer window.  The amount of 
      //  overlap is specified in the Preferences dialog for each program.

    case Qt::Key_Escape:

      //  Turn off slicing if it was on (this will also call redrawMap ()).

      if (misc.slice) slotPopupMenu (popup[NUMPOPUPS - 1]);

      return;
      break;

    case Qt::Key_Return:
      return;
      break;

    case Qt::Key_Left:
      if (e->modifiers () == Qt::ControlModifier)
        {
          //  Reset the area.

          double xsize = orig_bounds.max_x - orig_bounds.min_x;
          double overlap_mult = (double) (100.0 - options.overlap_percent) / 100.0;

          orig_bounds.min_x = orig_bounds.min_x - xsize * overlap_mult;
          orig_bounds.max_x = orig_bounds.max_x - xsize * overlap_mult;

          if (orig_bounds.min_x < misc.total_mbr.min_x)
            {
              orig_bounds.min_x = misc.total_mbr.min_x;
              orig_bounds.max_x = orig_bounds.min_x + xsize;
            }


          //  Set the new shared bounds so that pfmView will know what to display.

          misc.abe_share->edit_area = orig_bounds;


          //  Exit with the special output for pfmView so it will know what to do.

          moveWindow = 100;
          slotExit (0);
        }
      else
        {
          map->rotateY (-options.rotation_increment);
        }
      return;
      break;

    case Qt::Key_PageUp:
      slotExagTriggered (QAbstractSlider::SliderSingleStepSub);
      return;
      break;

    case Qt::Key_Right:
      if (e->modifiers () == Qt::ControlModifier)
        {
          //  Reset the area.

          double xsize = orig_bounds.max_x - orig_bounds.min_x;
          double overlap_mult = (double) (100.0 - options.overlap_percent) / 100.0;

          orig_bounds.min_x = orig_bounds.min_x + xsize * overlap_mult;
          orig_bounds.max_x = orig_bounds.max_x + xsize * overlap_mult;

          if (orig_bounds.max_x > misc.total_mbr.max_x)
            {
              orig_bounds.max_x = misc.total_mbr.max_x;
              orig_bounds.min_x = orig_bounds.max_x - xsize;
            }


          //  Set the new shared bounds so that pfmView will know what to display.

          misc.abe_share->edit_area = orig_bounds;


          //  Exit with the special output for pfmView so it will know what to do.

          moveWindow = 100;
          slotExit (0);
        }
      else
        {
          map->rotateY (options.rotation_increment);
        }
      return;
      break;

    case Qt::Key_PageDown:
      slotExagTriggered (QAbstractSlider::SliderSingleStepAdd);
      return;
      break;

    case Qt::Key_Up:

      if (e->modifiers () == Qt::ControlModifier)
        {
          //  Reset the area.

          double ysize = orig_bounds.max_y - orig_bounds.min_y;
          double overlap_mult = (double) (100.0 - options.overlap_percent) / 100.0;

          orig_bounds.min_y = orig_bounds.min_y + ysize * overlap_mult;
          orig_bounds.max_y = orig_bounds.max_y + ysize * overlap_mult;

          if (orig_bounds.max_y > misc.total_mbr.max_y)
            {
              orig_bounds.max_y = misc.total_mbr.max_y;
              orig_bounds.min_y = orig_bounds.max_y - ysize;
            }


          //  Set the new shared bounds so that pfmView will know what to display.

          misc.abe_share->edit_area = orig_bounds;


          //  Exit with the special output for pfmView so it will know what to do.

          moveWindow = 100;
          slotExit (0);
        }
      else
        {
          slotSliceTriggered (QAbstractSlider::SliderPageStepSub);
        }
      return;
      break;

    case Qt::Key_Down:

      if (e->modifiers () == Qt::ControlModifier)
        {
          //  Reset the area.

          double ysize = orig_bounds.max_y - orig_bounds.min_y;
          double overlap_mult = (double) (100.0 - options.overlap_percent) / 100.0;

          orig_bounds.min_y = orig_bounds.min_y - ysize * overlap_mult;
          orig_bounds.max_y = orig_bounds.max_y - ysize * overlap_mult;

          if (orig_bounds.min_y < misc.total_mbr.min_y)
            {
              orig_bounds.min_y = misc.total_mbr.min_y;
              orig_bounds.max_y = orig_bounds.min_y + ysize;
            }


          //  Set the new shared bounds so that pfmView will know what to display.

          misc.abe_share->edit_area = orig_bounds;


          //  Exit with the special output for pfmView so it will know what to do.

          moveWindow = 100;
          slotExit (0);
        }
      else
        {
          //  If we're not already slicing, do nothing.

          if (misc.slice) slotSliceTriggered (QAbstractSlider::SliderPageStepAdd);
        }
      return;
      break;
    }


  //  Looking for ancillary program hot keys and action keys.

  uint8_t hit = NVFalse;
  QString key = e->text ();


  //  Get the keystring.  We need this for Del, Ins, Home, or End if we want to use them for ancillary program hot keys.

  QKeySequence keySeq = e->key ();
  QString keyString = keySeq.toString ();
  if (keyString == "Del" || keyString == "Ins" || keyString == "Home" || keyString == "End") key = keyString;


  //  If key is NULL this is a modifier so we don't want to check it against the hot/action keys.

  if (key == "") return;


  //  Concatenate the (single) modifier and the key value.  There's probably a better way to do this but I don't know
  //  what it is at the moment.

  QString modifier = "";
  if (e->modifiers () == Qt::AltModifier) modifier = "Alt+";
  if (e->modifiers () == Qt::ControlModifier) modifier = "Ctrl+";
  if (e->modifiers () == Qt::MetaModifier) modifier = "Meta+";

  if (!modifier.isEmpty ()) key = modifier + keyString;


  //  Check for the hard-wired "color-by" hot keys

  for (int32_t i = 0 ; i < HARD_KEYS ; i++)
    {
      if (key.toUpper () == misc.hard_key[i].toUpper ())
        {
          slotColorByClicked (NULL, i);
          return;
        }
    }


  //  Next, check for special purpose keys not associated with an actual program

  if (!key.compare (options.hotkey[DELETE_FILTER_HIGHLIGHTED], Qt::CaseInsensitive))
    {
      if (filter_active || attr_filter_active || hof_filter_active)
        {
          slotFilterDelete ();
          return;
        }


      //  If we have highlighted points, kill them.

      if (misc.highlight_count)
        {
          for (int32_t i = 0 ; i < misc.highlight_count ; i++)
            {
              int32_t j = misc.highlight[i];

              store_undo (&misc, options.undo_levels, misc.data[j].val, j, NULL);
              misc.data[j].val |= PFM_MANUALLY_INVAL;


              //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
              //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
              //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
              //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
              //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

              misc.data[j].val &= ~PFM_SUSPECT;
            }
          end_undo_block (&misc);

          misc.highlight.clear ();
          misc.highlight_count = 0;

          options.function = misc.save_function;

          redrawMap (NVTrue);
        }

      return;
    }


  if (!key.compare (options.hotkey[CANCEL_FILTER_HIGHLIGHTED], Qt::CaseInsensitive))
    {
      if (filter_active || attr_filter_active || hof_filter_active)
        {
          slotFilterCancel ();
          return;
        }


      //  If we have highlighted points, check for the reason they're highlighted and then clear that.

      if (misc.highlight_count)
        {
          uint32_t flag = 0;

          switch (options.flag_index)
            {
              //  No user flags or flag invalid.

            case 0:
            case 5:
              flag = PFM_INVAL;
              break;

            case 1:
              flag = PFM_SUSPECT;
              break;

            case 2:
              flag = PFM_SELECTED_SOUNDING;
              break;

            case 3:
              flag = PFM_SELECTED_FEATURE;
              break;

            case 4:
              flag = PFM_DESIGNATED_SOUNDING;
              break;

            case (PRE_USER + 0):
              flag = PFM_USER_01;
              break;

            case (PRE_USER + 1):
              flag = PFM_USER_02;
              break;

            case (PRE_USER + 2):
              flag = PFM_USER_03;
              break;

            case (PRE_USER + 3):
              flag = PFM_USER_04;
              break;

            case (PRE_USER + 4):
              flag = PFM_USER_05;
              break;

            case (PRE_USER + 5):
              flag = PFM_USER_06;
              break;

            case (PRE_USER + 6):
              flag = PFM_USER_07;
              break;

            case (PRE_USER + 7):
              flag = PFM_USER_08;
              break;

            case (PRE_USER + 8):
              flag = PFM_USER_09;
              break;

            case (PRE_USER + 9):
              flag = PFM_USER_10;
              break;
            }


          for (int32_t i = 0 ; i < misc.highlight_count ; i++)
            {
              int32_t j = misc.highlight[i];

              if (misc.data[j].val & flag)
                {
                  store_undo (&misc, options.undo_levels, misc.data[j].val, j, NULL);
                  misc.data[j].val &= ~flag;
                  end_undo_block (&misc);
                }
            }


          misc.highlight.clear ();
          misc.highlight_count = 0;

          options.function = misc.save_function;

          redrawMap (NVTrue);
        }

      return;
    }


  //  Delete a single point.  Note that we're calling slotPopupMenu with popup[0].  If you change the right click menu DELETE_POINT
  //  option you have to change this as well.

  if (options.function == DELETE_POINT && !key.compare (options.hotkey[DELETE_SINGLE_POINT], Qt::CaseInsensitive))
    {
      slotPopupMenu (popup[0]);
      return;
    }


  //  Toggle freezing of the main marker and the multi...[0] data that is shared with other programs.

  if ((options.function == DELETE_POINT || options.function == MEASURE) && !key.compare (options.hotkey[FREEZE_ALL], Qt::CaseInsensitive))
    {
      slotPopupMenu (popup[FREEZE_POPUP]);
      return;
    }


  //  Toggle freezing of all of the markers and all of the multi... data that is shared with other programs.

  if ((options.function == DELETE_POINT || options.function == MEASURE) && !key.compare (options.hotkey[FREEZE], Qt::CaseInsensitive))
    {
      //  Only do the overall freeze if multiMode is not set to single.

      if (!misc.abe_share->mwShare.multiMode)
        {
          if (misc.marker_mode)
            {
              misc.marker_mode = 0;
            }
          else
            {
              misc.marker_mode = 2;
            }


          //  Save the marker position

          misc.frozen_point = misc.nearest_point;

          redrawMap (NVFalse);
        }


      //  If you only have one cursor, this will freeze it.

      else
        {
          slotPopupMenu (popup[FREEZE_POPUP]);
        }

      return;
    }


  //  Finally check against the ancillary programs.

  if (options.function != HOTKEY_POLYGON)
    {
      for (int32_t i = 0 ; i < NUMPROGS ; i++)
        {
          //  If option.data_type[i][0] for the requested program is set (no data type) we want to allow key presses.

          if (options.function == DELETE_POINT || options.function == ADD_FEATURE || options.function == EDIT_FEATURE ||
              options.function == MEASURE || options.data_type[i][0])
            {
              //  Check type of nearest point against acceptable types for each program.  This will
              //  actually let us use a hot key in more than one place (probably not a good idea
              //  anyway).

              int16_t type;
              char tmp[1024];
              read_list_file (misc.pfm_handle[misc.data[lock_point].pfm], misc.data[lock_point].file, tmp, &type);

              if (!type || options.data_type[i][type] || options.data_type[i][0])
                {
                  int32_t hot_action_type = 0;
                  if (!key.compare (options.hotkey[i], Qt::CaseInsensitive)) hot_action_type = 1;


                  //  Check for an occurrence of any of possible multiple action keys

                  if (!options.action[i].isEmpty ())
                    {
                      for (int32_t j = 0 ; j < 20 ; j++)
                        {
                          QString cut = options.action[i].section (',', j, j);
                          if (cut.isEmpty ()) break;
                          if (!key.compare (cut, Qt::CaseInsensitive)) hot_action_type = 2;
                        }
                    }

                  if (hot_action_type == 1)
                    {
                      //  Don't do anything if this is a hotkey polygon only program.

                      if (options.hk_poly_filter[i] == 1)
                        {
                          QMessageBox::warning (this, tr ("Delete Point Hot Key"),
                                                tr ("The key <b><i>%1</i></b> is only defined for hot key polygon/area mode. "
                                                    "You may have been trying to run another program. The available programs are:<br><br>%2").arg
                                                (e->text ()).arg (hotkey_message));
                          hit = NVTrue;
                          break;
                        }


                      //  Kick off the program.

                      runProg (i);
                      hit = NVTrue;
                      break;
                    }
                  else if (hot_action_type == 2)
                    {
                      //  Just in case we're going to call chartsPic we want to save the nearest feature snippet file name.
  
                      if (misc.nearest_feature != -1 && options.function == EDIT_FEATURE) 
                        {
                          sprintf (misc.abe_share->snippet_file_name, "%s/%s_%05d.jpg",
                                   pfm_dirname (misc.abe_share->open_args[0].target_path),
                                   pfm_basename (misc.abe_share->open_args[0].target_path),
                                   misc.feature[misc.nearest_feature].record_number);
                        }
                      else
                        {
                          sprintf (misc.abe_share->snippet_file_name, "NONE");
                        }


                      //  Have to check for the mosaicView zoom key so that we can tell pfmView to tell any linked apps
                      //  that we're zooming.

                      if (key == options.action[MOSAICVIEW].section (',', 0, 0)) misc.abe_share->zoom_requested = NVTrue;


                      actionKey (this, map, &misc, key, lock_point);
                      hit = NVTrue;
                      break;
                    }
                }
            }
        }


      if (!hit)
        {
          QMessageBox::warning (this, tr ("Hot/Action Key"),
                                tr ("The key <b><i>%1</i></b> either is not defined for ancillary program operations or the data"
                                    "type of the nearest point does not match the approved data types for "
                                    "this key.  The available programs are:<br><br>%2").arg (e->text ()).arg (hotkey_message));
        }
    }
  else
    {
      for (int32_t i = 0 ; i < NUMPROGS ; i++)
        {
          if (!key.compare (options.hotkey[i], Qt::CaseInsensitive) && options.hk_poly_eligible[i])
            {
              uint8_t failed = NVFalse;
              hkp = new hotkeyPolygon (this, map, &options, &misc, i, &failed);
              if (failed)
                {
                  redrawMap (NVFalse);
                }
              else
                {
                  connect (hkp, SIGNAL (hotkeyPolygonDone ()), this, SLOT (slotHotkeyPolygonDone ()));
                  connect (hkp, SIGNAL (readStandardErrorSignal (QProcess *)), this, SLOT (slotReadStandardError (QProcess *)));
                }

              hit = NVTrue;
              break;
            }
        }


      if (!hit) 
        {
          QMessageBox::warning (this, tr ("Polygon Hot Key"),
                                tr ("The key <b><i>%1</i></b> is not defined for hot key polygon operations. "
                                    "You may have been trying to use another program. "
                                    "The available programs are:<br><br>%2").arg (e->text ()).arg (hotkey_message));
        }
    }

  e->accept ();
}



void 
pfmEdit3D::slotRedrawMap ()
{
  redrawMap (NVTrue);
}



void 
pfmEdit3D::slotHotkeyPolygonDone ()
{
  misc.statusProg->reset ();

  qApp->restoreOverrideCursor ();

  qApp->processEvents ();


  redrawMap (NVTrue);
}



void 
pfmEdit3D::closeEvent (QCloseEvent *event __attribute__ ((unused)))
{
  if (misc.abe_share->read_only)
    {
      slotExit (2);
    }
  else
    {
      int32_t ret = QMessageBox::Yes;
      ret = QMessageBox::information (this, "pfmEdit3D", tr ("Do you wish to save any changes you may have made?"),
                                      QMessageBox::Yes,  QMessageBox::No,  QMessageBox::NoButton);

      if (ret == QMessageBox::Yes)
        {
          slotExit (0);
        }
      else
        {
          slotExit (2);
        }
    }
}



//!  A bunch of slots.

void 
pfmEdit3D::slotExitSave ()
{
  slotExit (0);
}



void 
pfmEdit3D::slotExitMask ()
{
  slotExit (1);
}



void 
pfmEdit3D::slotExitNoSave ()
{
  slotExit (2);
}



void 
pfmEdit3D::slotExit (int id)
{
  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  //  Clear up any undo memory we've used.

  for (int32_t i = 0 ; i < misc.undo_count ; i++)
    {
      if (misc.undo[i].count)
        {
          misc.undo[i].val.clear ();
          misc.undo[i].num.clear ();
        }
    }
  misc.undo_count = 0;


  //  Clear up any highlight memory we had.

  if (misc.highlight_count)
    {
      misc.highlight.clear ();
      misc.highlight_count = 0;
    }


  //  Shortcut to make sure we filter mask.

  uint8_t override_mask = NVFalse;
  if (id == 1)
    {
      id = 0;
      misc.filtered = NVTrue;
      override_mask = NVTrue;
    }


  //  Make sure the default function is one of the delete functions or the CZMIL reprocessing functions.

  options.function = misc.save_mode;


  //  Check for ancillary programs that can respawn to see if they are up and running.  If they are, set the state to 2.

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      if (options.state[i])
        {
          if (options.state[i] == 2 || (ancillaryProgram[i] && ancillaryProgram[i]->state () == QProcess::Running))
            {
              options.state[i] = 2;
            }
          else
            {
              options.state[i] = 1;
            }
        }
    }


  //  Force persistent running ancillary programs (those with a non-zero state value) to exit cleanly (if
  //  we're doing "kill and respawn").

  if (options.kill_and_respawn) misc.abe_share->key = ANCILLARY_FORCE_EXIT;


  //  Get the last rotation values

  options.zx_rotation = map->getZXRotation ();
  options.y_rotation = map->getYRotation ();


  //  Save the user's defaults

  envout (&options, &misc, this);


  //  Close the BFD file if it was open

  if (misc.bfd_open) binaryFeatureData_close_file (misc.bfd_handle);


  //  We output a 0 if no changes have been made, a 1 if changes to the PFM structure have been made or a 2
  //  if just feature changes have been made.  The second value (after the comma) is the filtered flag.  This
  //  tells pfmView that we have filtered and it needs to filter mask the area (unless the user has turned off
  //  auto filter masking in the preferences dialog.

  int32_t put_ret = 0;

  if (!options.auto_filter_mask && !override_mask) misc.filtered = NVFalse;


  finishing = NVFalse;

  switch (id)
    {
      //  Normal exit and save.

    case 0:
      map->setCursor (Qt::WaitCursor);

      put_ret = put_buffer (&misc);

      if (misc.feature_mod || put_ret || misc.filtered || misc.czmilReprocessFlag)
        {
          if (put_ret && (options.auto_unload || force_auto_unload))
            {
              fprintf (stdout, "Edit return status:%d,%d\n", moveWindow + 1, misc.filtered);
              fflush (stdout);
              finishing = NVTrue;
              autoUnload ();
            }
          else
            {
              if (put_ret || misc.filtered)
                {
                  fprintf (stdout, "Edit return status:%d,%d\n", moveWindow + 1, misc.filtered);
                }
              else 
                {
                  fprintf (stdout, "Edit return status:%d,%d\n", moveWindow + 2, misc.filtered);
                }
              fflush (stdout);
            }
        }
      else
        {
          fprintf (stdout, "Edit return status:%d,0\n", moveWindow + 0);
          fflush (stdout);
        }
      break;


      //  Exit without saving

    case 2:

      //  Check for feature changes.

      if (misc.feature_mod)
        {
          fprintf (stdout, "Edit return status:2,0\n");
        }


      //  Check for external CZMIL reprocessing.

      else if (misc.czmilReprocessFlag)
        {
          fprintf (stdout, "Edit return status:1,0\n");
        }


      //  Otherwise, no redraw in pfmView.

      else
        {
          fprintf (stdout, "Edit return status:0,0\n");
        }
      fflush (stdout);
      break;
    }


  if (!finishing) 
    {
      for (int32_t pfm = 0 ; pfm < misc.abe_share->pfm_count ; pfm++) close_pfm_file (misc.pfm_handle[pfm]);

      clean_exit (0);
    }
}



void 
pfmEdit3D::slotPrefs ()
{
  if (prefs_dialog) prefs_dialog->close ();

  prefs_dialog = new prefs (this, &options, &misc);


  //  Put the dialog in the middle of the screen.

  prefs_dialog->move (x () + width () / 2 - prefs_dialog->width () / 2, y () + height () / 2 - prefs_dialog->height () / 2);


  connect (prefs_dialog, SIGNAL (dataChangedSignal ()), this, SLOT (slotPrefDataChanged ()));
  connect (prefs_dialog, SIGNAL (hotKeyChangedSignal (int32_t)), this, SLOT (slotPrefHotKeyChanged (int32_t)));
}



//!  Changed some of the preferences

void 
pfmEdit3D::slotPrefDataChanged ()
{
  bContour->setChecked (options.display_contours);

  bDisplayReference->setChecked (options.display_reference);
  bDisplayNull->setChecked (options.display_null);
  bDisplayFeature->setIcon (displayFeatureIcon[options.display_feature]);
  bDisplayChildren->setChecked (options.display_children);
  bDisplayFeatureInfo->setChecked (options.display_feature_info);

  if (options.function == DELETE_POINT) bDeletePoint->setChecked (true);
  if (options.function == DELETE_RECTANGLE) bDeleteRect->setChecked (true);
  if (options.function == DELETE_POLYGON) bDeletePoly->setChecked (true);
  if (options.function == KEEP_POLYGON) bKeepPoly->setChecked (true);


  options.ref_color[1] = options.ref_color[0];
  options.ref_color[1].setAlpha (options.slice_alpha);


  map->setBackgroundColor (options.background_color);
  map->setScaleColor (options.scale_color);
  map->enableScale (options.draw_scale);
  map->setZoomPercent (options.zoom_percent);
  map->setScaleFactor (options.z_factor);
  map->setScaleOffset (options.z_offset);
  map->setScaleOffsetColor (options.scaled_offset_z_color);

  slicePalette.setColor (QPalette::Normal, QPalette::Base, options.background_color);
  slicePalette.setColor (QPalette::Normal, QPalette::Window, options.background_color);
  slicePalette.setColor (QPalette::Inactive, QPalette::Base, options.background_color);
  slicePalette.setColor (QPalette::Inactive, QPalette::Window, options.background_color);
  sliceBar->setPalette (slicePalette);


  //  Check for changes to ancillary programs that have associated buttons.

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      if (options.progButton[i])
        {
          QString tip =  options.description[i] + "  [" + options.hotkey[i] + "]";
          options.progButton[i]->setToolTip (tip);
          options.progButton[i]->setShortcut (QKeySequence (options.hotkey[i]));
        }
    }

  filterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  filterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  avtFilterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  avtFilterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  hpFilterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  hpFilterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  attrFilterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
  attrFilterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
  if (misc.hof_present)
    {
      hofWaveFilterDelete->setToolTip (tr ("Delete filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
      hofWaveFilterCancel->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
    }
  if (misc.czmil_present)
    {
      cpfFilterAccept->setToolTip (tr ("Accept filter results [%1]").arg (options.hotkey[DELETE_FILTER_HIGHLIGHTED]));
      cpfFilterDiscard->setToolTip (tr ("Discard filter results [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));
    }
  bClearHighlight->setToolTip (tr ("Clear highlighted points [%1]").arg (options.hotkey[CANCEL_FILTER_HIGHLIGHTED]));

  for (int32_t i = 0 ; i < HOTKEYS ; i++) slotPrefHotKeyChanged (i);


  //  Set the Z value display orientation.

  if (options.z_orientation > 0.0)
    {
      map->setScaleOrientation (true);
    }
  else
    {
      map->setScaleOrientation (false);
    }


  //  Save the user's defaults

  envout (&options, &misc, this);


  //  Redraw the data (which is needed because we may have changed the exaggeration, among other things).

  if (!misc.busy) redrawMap (NVTrue);
}



void 
pfmEdit3D::slotPrefHotKeyChanged (int32_t i)
{
  QString tip = misc.buttonText[i] + "  [" + options.buttonAccel[i] + "]";
  if (misc.button[i])
    {
      misc.button[i]->setToolTip (tip);
      misc.button[i]->setShortcut (QKeySequence (options.buttonAccel[i]));
    }
  else if (misc.action[i])
    {
      misc.action[i]->setShortcut (QKeySequence (options.buttonAccel[i]));
    }
}



//  Slot for children to call for standard error output.

void 
pfmEdit3D::slotReadStandardError (QProcess *proc)
{
  readStandardError (proc);
}



//  Generic standard error reader that puts info into the message list.

void
pfmEdit3D::readStandardError (QProcess *proc)
{
  static QString resp_string = "";


  QByteArray response = proc->readAllStandardError ();


  //  Parse the return response for line feeds

  for (int32_t i = 0 ; i < response.length () ; i++)
    {
      if (response.at (i) == '\n')
        {
          //  Let's not print out the "QProcess: Destroyed while process is still running" messages that come from
          //  killing ancillary programs.  Also ignore the pfm io cache setting message.

          if (!resp_string.contains ("Destroyed while") && !resp_string.contains ("Setting MAXIMUM Cache Size") &&
              !resp_string.contains ("failed to add paths") && !resp_string.contains ("DrvShareLists") &&
              !resp_string.contains ("OpenGL Warning"))
            {
              misc.messages->append (resp_string);


              //  Just in case we had the status bar going we need to shut it down on an error.

              qApp->restoreOverrideCursor ();
              misc.statusProg->reset ();
              misc.statusProg->setRange (0, 100);
              misc.statusProgLabel->setVisible (false);
              misc.statusProg->setTextVisible (false);
              qApp->processEvents ();
            }


          resp_string = "";
        }
      else
        {
          resp_string += response.at (i);
        }
    }
}



//  This is just a slot so that we can start it from the View menu.  The actual work is done in displayMessage.

void
pfmEdit3D::slotMessage ()
{
  if (displayMessage_dialog == NULL)
    {
      displayMessage_dialog = new displayMessage (this, &options, &misc);

      connect (displayMessage_dialog, SIGNAL (closed ()), this, SLOT (slotMessageDialogClosed ()));
    }

  displayMessage_dialog->setMessage ();
}



void
pfmEdit3D::slotMessageDialogClosed ()
{
  displayMessage_dialog = NULL;
}



void
pfmEdit3D::slotFeatureMenu (QAction *action)
{
  for (int32_t i = 0 ; i < 4 ; i++)
    {
      if (action == displayFeature[i])
        {
          options.display_feature = i;
          break;
        }
    }

  bDisplayFeature->setIcon (displayFeatureIcon[options.display_feature]);

  if (options.display_feature)
    {
      bDisplayChildren->setEnabled (true);
      bDisplayFeatureInfo->setEnabled (true);
      bAddFeature->setEnabled (true);
      bDeleteFeature->setEnabled (true);
      bEditFeature->setEnabled (true);
      bVerifyFeatures->setEnabled (true);

      setWidgetStates (NVTrue);

      redrawMap (NVFalse);
      map->flush ();
    }
  else
    {
      bDisplayChildren->setEnabled (false);
      bDisplayFeatureInfo->setEnabled (false);
      bAddFeature->setEnabled (false);
      bDeleteFeature->setEnabled (false);
      bEditFeature->setEnabled (false);
      bVerifyFeatures->setEnabled (false);


      //  Make sure we aren't using one of the "feature" editor functions

      if (options.function == ADD_FEATURE || options.function == DELETE_FEATURE ||
          options.function == MOVE_FEATURE || options.function == EDIT_FEATURE)
        {
          options.function = DELETE_POINT;
          setFunctionCursor (options.function);
        }

      redrawMap (NVFalse);
    }
}



void
pfmEdit3D::slotDisplayChildren ()
{
  if (bDisplayChildren->isChecked ())
    {
      options.display_children = NVTrue;

      redrawMap (NVFalse);
    }
  else
    {
      options.display_children = NVFalse;

      redrawMap (NVFalse);

      map->flush ();
    }
}



void
pfmEdit3D::slotDisplayFeatureInfo ()
{
  if (bDisplayFeatureInfo->isChecked ())
    {
      options.display_feature_info = NVTrue;

      redrawMap (NVFalse);
    }
  else
    {
      options.display_feature_info = NVFalse;

      redrawMap (NVFalse);

      map->flush ();
    }
}



void
pfmEdit3D::slotVerifyFeatures ()
{
  BFDATA_RECORD bfd_record;

  for (uint32_t i = 0 ; i < misc.bfd_header.number_of_records ; i++)
    {
      if (misc.feature[i].confidence_level)
        {
          if (!check_bounds (&options, &misc, misc.feature[i].longitude, misc.feature[i].latitude, misc.feature[i].depth, PFM_USER, NVFalse, 0,
                             NVFalse, NVFalse))
            {
              binaryFeatureData_read_record (misc.bfd_handle, i, &bfd_record);

              bfd_record.confidence_level = 5;

              binaryFeatureData_write_record (misc.bfd_handle, i, &bfd_record, NULL, NULL);
            }
        }
    }


  misc.feature_mod = NVTrue;


  if (misc.bfd_open) binaryFeatureData_close_file (misc.bfd_handle);
  misc.bfd_open = NVFalse;

  if ((misc.bfd_handle = binaryFeatureData_open_file (misc.abe_share->open_args[0].target_path, &misc.bfd_header, BFDATA_UPDATE)) >= 0)
    {
      if (binaryFeatureData_read_all_short_features (misc.bfd_handle, &misc.feature) < 0)
        {
          QString msg = QString (binaryFeatureData_strerror ());
          QMessageBox::warning (this, "pfmEdit3D", tr ("Unable to read feature records\nReason: %1").arg (msg));
          binaryFeatureData_close_file (misc.bfd_handle);
        }
      else
        {
          misc.bfd_open = NVTrue;
        }
    }


  redrawMap (NVFalse);

  map->flush ();
}



void
pfmEdit3D::slotDisplayReference ()
{
  if (bDisplayReference->isChecked ())
    {
      options.display_reference = NVTrue;
      bReferencePoly->setEnabled (true);
      bUnreferencePoly->setEnabled (true);
    }
  else
    {
      options.display_reference = NVFalse;
      bReferencePoly->setEnabled (false);
      bUnreferencePoly->setEnabled (false);


      //  Make sure we aren't using one of the "reference" editor functions

      if (options.function == SET_REFERENCE || options.function == UNSET_REFERENCE)
        {
          options.function = DELETE_POINT;
          setFunctionCursor (options.function);
        }

    }

  redrawMap (NVFalse);
}



void
pfmEdit3D::slotDisplayManInvalid ()
{
  if (bDisplayManInvalid->isChecked ())
    {
      bDisplayNull->setEnabled (true);

      options.display_man_invalid = NVTrue;
    }
  else
    {
      options.display_man_invalid = NVFalse;
    }



  //  If we had highlighting on prior to displaying invalid, redo it.

  if (options.flag_index) setFlags (&misc, &options);


  //  Always turn auto-scaling on whenever we turn invalid data on or off.  If we're not auto-scaling it will be turned off again after
  //  we redraw the map.

  map->setAutoScale (NVTrue);


  redrawMap (NVTrue);
}



void
pfmEdit3D::slotDisplayFltInvalid ()
{
  if (bDisplayFltInvalid->isChecked ())
    {
      bDisplayNull->setEnabled (true);

      options.display_flt_invalid = NVTrue;
    }
  else
    {
      options.display_flt_invalid = NVFalse;
    }


  //  If we had highlighting on prior to displaying invalid, redo it.

  if (options.flag_index) setFlags (&misc, &options);


  //  Always turn auto-scaling on whenever we turn invalid data on or off.  If we're not auto-scaling it will be turned off again after
  //  we redraw the map.

  map->setAutoScale (NVTrue);


  redrawMap (NVTrue);
}



void
pfmEdit3D::slotDisplayNull ()
{
  if (bDisplayNull->isChecked ())
    {
      options.display_null = NVTrue;
    }
  else
    {
      options.display_null = NVFalse;
    }


  //  If we had highlighting on prior to displaying invalid, redo it.

  if (options.flag_index) setFlags (&misc, &options);


  //  Always turn auto-scaling on whenever we turn invalid data on or off.  If we're not auto-scaling it will be turned off again after
  //  we redraw the map.

  map->setAutoScale (NVTrue);


  redrawMap (NVTrue);
}



void
pfmEdit3D::slotDisplayAll ()
{
  bUndisplaySingle->setEnabled (true);
  bDisplayMultiple->setEnabled (true);

  bDisplayAll->setEnabled (false);

  bDisplayAll->setIcon (QIcon (":/icons/displayall.png"));

  misc.num_lines = local_num_lines = 0;

  options.function = misc.save_function;

  redrawMap (NVTrue);
}



void
pfmEdit3D::slotUndisplaySingle ()
{
  if (options.function != UNSET_SINGLE) misc.save_function = options.function;
  options.function = UNSET_SINGLE;
  setFunctionCursor (options.function);
}



void
pfmEdit3D::slotDisplayMultiple ()
{
  if (options.function != SET_MULTIPLE) misc.save_function = options.function;
  options.function = SET_MULTIPLE;
  setFunctionCursor (options.function);
}



void 
pfmEdit3D::slotInvertHighlight ()
{
  if (misc.highlight_count)
    {
      uint8_t *temp_highlight_flag = (uint8_t *) calloc (misc.abe_share->point_cloud_count, sizeof (uint8_t));
      if (temp_highlight_flag == NULL)
        {
          fprintf (stderr, "%s %s %s %d - temp_highlight_flag - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
          exit (-1);
        }

      for (int32_t i = 0 ; i < misc.highlight_count ; i++) temp_highlight_flag[misc.highlight[i]] = NVTrue;

      misc.highlight.clear ();
      misc.highlight_count = misc.abe_share->point_cloud_count - misc.highlight_count;


      try
        {
          misc.highlight.resize (misc.highlight_count);
        }
      catch (std::bad_alloc&)
        {
          fprintf (stderr, "%s %s %s %d - highlight - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
          exit (-1);
        }
      memset (misc.highlight.data (), 0, misc.highlight_count * sizeof (int32_t));

      int32_t new_count = 0;
      for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
        {
          if (!temp_highlight_flag[i])
            {
              misc.highlight[new_count] = i;
              new_count++;
            }
        }

      free (temp_highlight_flag);
    }

  redrawMap (NVFalse);
}



void 
pfmEdit3D::slotClearHighlight ()
{
  if (misc.highlight_count)
    {
      misc.highlight.clear ();
      misc.highlight_count = 0;
    }

  bClearHighlight->setEnabled (false);


  //  Since we decided to manually clear all of the highlighted points let's make sure that we weren't highlighting by flag.

  options.flag_index = 0;
  bFlag->setIcon (flagIcon[options.flag_index]);
  disconnect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
  flag[0]->setChecked (true);
  connect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));


  redrawMap (NVFalse);
}



void
pfmEdit3D::slotContour ()
{
  if (bContour->isChecked ())
    {
      options.display_contours = NVTrue;
    }
  else
    {
      options.display_contours = NVFalse;
    }

  redrawMap (NVTrue);
}



void 
pfmEdit3D::slotDisplayLinesDataChanged ()
{
  if (misc.num_lines) bDisplayAll->setEnabled (true);

  redrawMap (NVTrue);
}



void 
pfmEdit3D::slotLockValueDone (uint8_t accepted)
{
  if (accepted)
    {
      //  We have to reset the colors here because we might have changed the min or max color.

      int32_t k = options.color_index;


      float hue_inc = (float) (options.max_hsv_color[k] - options.min_hsv_color[k]) / (float) (NUMSHADES + 1);

      for (int32_t j = 0 ; j < NUMSHADES ; j++)
        {
          misc.track_color_array[j].setHsv ((int32_t) (options.min_hsv_color[k] + j * hue_inc), 255, 255, 255); 

          misc.color_array[j][0] = misc.track_color_array[j].red ();
          misc.color_array[j][1] = misc.track_color_array[j].green ();
          misc.color_array[j][2] = misc.track_color_array[j].blue ();
          misc.color_array[j][3] = misc.track_color_array[j].alpha ();
        }

      options.ref_color[1] = options.ref_color[0];
      options.ref_color[1].setAlpha (options.slice_alpha);


      //  If we changed the depth colors, locks, or values (index 0) then we need to set shared memory to inform the editors when they start up.

      if (!k)
        {
          misc.abe_share->min_hsv_color = options.min_hsv_color[0];
          misc.abe_share->max_hsv_color = options.max_hsv_color[0];
          misc.abe_share->min_hsv_value = options.min_hsv_value[0];
          misc.abe_share->max_hsv_value = options.max_hsv_value[0];
          misc.abe_share->min_hsv_locked = options.min_hsv_locked[0];
          misc.abe_share->max_hsv_locked = options.max_hsv_locked[0];


          //  We need to tell pfmView that we changed the settings.

          misc.abe_share->settings_changed = NVTrue;
        }


      redrawMap (NVTrue);
    }
}



//  If we selected the track map or the histogram we want to turn off redrawing of the other.

void 
pfmEdit3D::slotNotebookChanged (int index)
{
  if (index == TRACKTAB)
    {
      options.track_tab = NVTrue;
      trackMap->redrawMap (NVTrue);
    }

  if (index == HISTTAB)
    {
      options.track_tab = NVFalse;
      histogram->redrawMap (NVTrue);
    }
}



//  If we selected the Hidden Data tab, check to see if we want the highlighted data button enabled.

void 
pfmEdit3D::slotStatbookChanged (int index)
{
  if (index == 1)
    {
      //  We want to disable the "highlighted" button if there are no highlighted points and enable it if there are.

      if (misc.highlight_count)
        {
          hideAct[PFM_USER_FLAGS]->setEnabled (true);
        }
      else
        {
          hideAct[PFM_USER_FLAGS]->setEnabled (false);
        }
    }
}



void 
pfmEdit3D::slotMinScalePressed (QMouseEvent *e __attribute__ ((unused)))
{
  if (misc.busy) return;

  lockValue *lck = new lockValue (map, &options, &misc, NVTrue, options.color_index);
  connect (lck, SIGNAL (lockValueDone (uint8_t)), this, SLOT (slotLockValueDone (uint8_t)));


  //  Put the dialog in the middle of the screen.

  lck->move (x () + width () / 2 - lck->width () / 2, y () + height () / 2 - lck->height () / 2);
}



void 
pfmEdit3D::slotMaxScalePressed (QMouseEvent *e __attribute__ ((unused)))
{
  if (misc.busy) return;

  lockValue *lck = new lockValue (map, &options, &misc, NVFalse, options.color_index);
  connect (lck, SIGNAL (lockValueDone (uint8_t)), this, SLOT (slotLockValueDone (uint8_t)));


  //  Put the dialog in the middle of the screen.

  lck->move (x () + width () / 2 - lck->width () / 2, y () + height () / 2 - lck->height () / 2);
}



//!  Latitude button was pushed to change to/from northing/easting.

void 
pfmEdit3D::slotLatitudeClicked (QMouseEvent *e __attribute__ ((unused)), int id __attribute__ ((unused)))
{
  options.lat_or_east ^= 1;
  setWidgetStates (NVTrue);
}



void 
pfmEdit3D::slotMode (int id)
{
  misc.save_function = options.function;


  options.function = id;


  setFunctionCursor (options.function);


  switch (options.function)
    {
      //  Save the last edit mode in case we exit

    case DELETE_RECTANGLE:
    case DELETE_POLYGON:
    case DELETE_POINT:
    case KEEP_POLYGON:
    case EDIT_FEATURE:
      misc.save_mode = options.function;
      break;


      //  The CPF filter is handled differently

    case CPF_FILTER:

      cpfFilterBox->setTitle (tr ("CPF Filter"));


      //  Set the box values.

      invCheck->setChecked (options.czmil_cpf_filter_inval);
      valCheck->setChecked (!options.czmil_cpf_filter_inval);
      valSpin->setValue (options.czmil_cpf_filter_amp_min);


      //  Enable the buttons.

      valSpin->setEnabled (!options.czmil_cpf_filter_inval);


      notebook->setTabEnabled (CPFTAB, true);
      notebook->setCurrentIndex (CPFTAB);


      //  Clear the filter kill list if present

      if (misc.filter_kill_count)
        {
          misc.filter_kill_list.clear ();
          misc.filter_kill_count = 0;
        }
      break;


      //  CHANGE_LIDAR_CLASS is handled almost exactly like CPF_FILTER

    case CHANGE_LIDAR_CLASS:

      lidarClassChangeBox->setTitle (tr ("Change LiDAR Classification"));


      //  Set the label values.

      slotLidarClassChangeDataChanged ();


      notebook->setTabEnabled (LIDARCLASSTAB, true);
      notebook->setCurrentIndex (LIDARCLASSTAB);


      //  Clear the filter kill list if present

      if (misc.filter_kill_count)
        {
          misc.filter_kill_list.clear ();
          misc.filter_kill_count = 0;
        }
      break;
    }


  //  If the previous function was CPF filter or LiDAR classification change, we need to get rid of
  //  any marked points and go back to the histogram or 2D tracker.

  if (misc.save_function == CPF_FILTER || misc.save_function == CHANGE_LIDAR_CLASS)
    {
      //  Just in case the user was changing the from/to classes...

      if (lidar_class_dialog) lidar_class_dialog->close ();


      call_mode = NVFalse;
      slotTabClose ();
      call_mode = NVTrue;
    }
}



void 
pfmEdit3D::slotUnload ()
{
  if (options.auto_unload)
    {
      options.auto_unload = NVFalse;
    }
  else
    {
      options.auto_unload = NVTrue;
    }
}



void 
pfmEdit3D::slotUnloadError (QProcess::ProcessError error)
{
  switch (error)
    {
    case QProcess::FailedToStart:
      QMessageBox::critical (this, tr ("pfmEdit3D unload process"), tr ("Unable to start the unload process!"));
      break;

    case QProcess::Crashed:
      QMessageBox::critical (this, tr ("pfmEdit3D unload process"), tr ("The unload process crashed!"));
      break;

    case QProcess::Timedout:
      QMessageBox::critical (this, tr ("pfmEdit3D unload process"), tr ("The unload process timed out!"));
      break;

    case QProcess::WriteError:
      QMessageBox::critical (this, tr ("pfmEdit3D unload process"), tr ("There was a write error to the unload process!"));
      break;

    case QProcess::ReadError:
      QMessageBox::critical (this, tr ("pfmEdit3D unload process"), tr ("There was a read error from the unload process!"));
      break;

    case QProcess::UnknownError:
      QMessageBox::critical (this, tr ("pfmEdit3D unload process"), tr ("The unload process died with an unknown error!"));
      break;
    }

  unload_done = NVTrue;
}



//!  This is the return from the unload QProcess (when finished)

void 
pfmEdit3D::slotUnloadDone (int exitCode __attribute__ ((unused)), QProcess::ExitStatus exitStatus __attribute__ ((unused)))
{
  unload_prog->setValue (100);

  if (unloadProc->exitCode ())
    {
      QMessageBox::critical (this, tr ("Unload"), tr ("An error occurred during unload, some or all of your edits were not saved!"
                                                      "\nPlease run pfm_unload separately to see what the error condition was."));
    }


  //  This is cheating ;-)  We're using the finishing flag to force it not to close after unload if we did a Ctrl-Arrow key move
  //  from the keypress event.  Normally we call unload only on exit.  See keyPressEvent above for more info.

  //  Need the extra comparison or an exit will be blocked, test it with pfm of 0, the last guy.  If this is not the last pfm file
  //  we want to set unload_done to true so the loop in autoUnload can progress.  We also close the pfm file as we are done with the unload for it.

  if ((finishing) && (unload_pfm))
    {
      close_pfm_file (misc.pfm_handle[unload_pfm]);

      unload_done = NVTrue;
      return;
    }

  close_pfm_file (misc.pfm_handle[unload_pfm]);


  //  Only exit after all of the PFM files have been unloaded (0 will always be the last unloaded).


  if (unload_pfm)
    {
      unload_done = NVTrue;
      return;
    }


  //  pfmView will read this value and know that we have modified stuff.

  fprintf (stdout, "Edit return status:1,0\n");
  fflush (stdout);


  clean_exit (0);
}



/*!
  This is the stderr read return from the unload QProcess.  We're dumping the percentage to
  stderr in the unload program so we can track it here.
*/

void 
pfmEdit3D::slotUnloadReadyReadStandardError ()
{
  int32_t percent;
  static uint8_t flip = NVTrue;

  QByteArray response = unloadProc->readAllStandardError ();
  char *res = response.data ();

  if (strchr (res, '%'))
    {
      sscanf (res, "%d%%", &percent);

      if (percent == 100 && flip)
        {
          percent = 0;
          QString progText =
            tr ("Resetting modified flags for %1").arg (QFileInfo (QString (misc.abe_share->open_args[unload_pfm].list_path)).fileName ().remove (".pfm"));

          unload_prog->setLabelText (progText);
          flip = NVFalse;
        }

      if (percent == 100 && !flip) flip = NVTrue;

      unload_prog->setValue (percent);
      qApp->processEvents();
    }
}



//!  Kick off the unload QProcess

void
pfmEdit3D::autoUnload ()
{
  QString arg;
  QStringList arguments;


  unload_done = NVTrue;
  unloadProc = NULL;
  unload_prog = NULL;


  //  Unload bottom to top since the 0 level PFM is always displayed.  This way we can always be assured of
  //  doing the exit from the "finished" slot.

  for (int32_t pfm = misc.abe_share->pfm_count - 1 ; pfm >= 0 ; pfm--)
    {
      //  Wait for the "finished" slot to tell us we're done unloading and have closed the file.

      while (!unload_done)
        {
#ifdef NVWIN3X
          Sleep (2000);
#else
          sleep (2);
#endif
        }


      //  Only unload PFM files that were displayed.

      if (misc.abe_share->display_pfm[pfm])
        {
          if (unloadProc != NULL) delete unloadProc;

          unloadProc = new QProcess (this);


          arguments.clear ();
          arguments += misc.abe_share->open_args[pfm].list_path;
          arguments += QString ("-Q");

          arg.sprintf ("-a %.9f,%.9f,%.9f,%.9f", mapdef.initial_bounds.max_y, mapdef.initial_bounds.min_y, 
                       mapdef.initial_bounds.max_x, mapdef.initial_bounds.min_x);
          arguments += arg;


          if (unload_prog != NULL) delete unload_prog;


          QString progText = tr ("Reading PFM data from %1").arg (QFileInfo (QString (misc.abe_share->open_args[pfm].list_path)).fileName ().remove (".pfm"));

          unload_prog = new QProgressDialog (progText, 0, 0, 100, this);

          unload_prog->setWindowTitle (tr ("Unloading"));
          unload_prog->setWindowModality (Qt::WindowModal);
          unload_prog->show ();

          qApp->processEvents();


          connect (unloadProc, SIGNAL (finished (int, QProcess::ExitStatus)), this, 
                   SLOT (slotUnloadDone (int, QProcess::ExitStatus)));
          connect (unloadProc, SIGNAL (readyReadStandardError ()), this, SLOT (slotUnloadReadyReadStandardError ()));
          connect (unloadProc, SIGNAL (error (QProcess::ProcessError)), this, SLOT (slotUnloadError (QProcess::ProcessError)));


          //  Let the "finished" slot know which PFM we're unloading.

          unload_pfm = pfm;
          unload_done = NVFalse;


          unloadProc->start (options.unload_prog, arguments);
        }
      else
        {
          unload_done = NVTrue;
        }
    }
}



void
pfmEdit3D::slotStop ()
{
  misc.drawing_canceled = NVTrue;
}



void 
pfmEdit3D::slotEditFeatureDataChanged ()
{
  misc.add_feature_index = -1;


  misc.feature_mod = NVTrue;
  redrawMap (NVFalse);


  //  If the mosaic viewer was running, tell it to redraw.

  misc.abe_share->key = FEATURE_FILE_MODIFIED;
}



void 
pfmEdit3D::editFeatureNum (int32_t feature_number)
{
  editFeature_dialog = new editFeature (this, &options, &misc, feature_number);
  connect (editFeature_dialog, SIGNAL (dataChangedSignal ()), this, SLOT (slotEditFeatureDataChanged ()));
  connect (editFeature_dialog, SIGNAL (readStandardErrorSignal (QProcess *)), this, SLOT (slotReadStandardError (QProcess *)));

  editFeature_dialog->show ();
}



//!  An Attribute tab attribute button was clicked.

void 
pfmEdit3D::slotAttrBoxClicked (QMouseEvent *e __attribute__ ((unused)), int id)
{
  QString name = QString (pfm_standard_attr_name (1, 0, misc.abe_share->open_args[0].head.ndx_attr_name[id]));


  //  If it was on, turn it off...

  if (misc.attrBoxFlag[id])
    {
      //  It's not polite to turn off the color-by attribute...

      if (options.color_index >= PRE_ATTR && misc.attrStatNum[options.color_index - PRE_ATTR] == id)
        {
          QMessageBox::warning (this, tr ("pfmView"), tr ("You cannot unselect the attribute that is being used for the color scale."));
          return;
        }


      for (int32_t i = 0 ; i < POST_ATTR ; i++)
        {
          int32_t j = misc.attrStatNum[i];

          if (name == pfm_standard_attr_name (1, 0, misc.abe_share->open_args[0].head.ndx_attr_name[j]))
            {
              misc.attrStatNum[i] = -1;
              options.attrStatName[i] = "";
              attrName[i + PRE_ATTR]->setText (tr ("No attribute selected"));
              attrValue[i + PRE_ATTR]->setText ("");
              misc.attrBoxFlag[id] = 0;
            }
        }
    }
  else
    {
      //  Check to see if we have any open slots...

      int32_t hitcnt = 0;
      for (int32_t i = 0 ; i < NUM_ATTR ; i++) hitcnt += misc.attrBoxFlag[i];

      if (hitcnt == POST_ATTR)
        {
          QMessageBox::warning (this, tr ("pfmView"), tr ("All attributes on the Status tab are already assigned.\n"
                                                          "You must unselect one or more of the selected attributes."));
          return;
        }


      //  Find the first open slot on the Status tab...

      for (int32_t i = 0 ; i < POST_ATTR ; i++)
        {
          if (misc.attrStatNum[i] == -1)
            {
              misc.attrStatNum[i] = id;
              options.attrStatName[i] = name;


              //  Check for the POSIX Time attribute

              if (id == misc.time_attr) name = QString (tr ("Date/Time (minutes)"));


              attrName[i + PRE_ATTR]->setText (name);
              attrName[i + PRE_ATTR]->setToolTip (name + QString (tr (" - To change, select attribute on Attributes tab.")));
              attrName[i + PRE_ATTR]->setWhatsThis (name + QString (tr (" - To change, select attribute on Attributes tab.")));
              attrValue[i + PRE_ATTR]->setToolTip (tr ("This is the value of %1 at the cursor").arg (name));
              break;
            }
        }

      misc.attrBoxFlag[id] = 1;
    }


  setWidgetStates (NVTrue);
}



//!  Attribute button was pushed to change "color by" option.

void 
pfmEdit3D::slotColorByClicked (QMouseEvent *e __attribute__ ((unused)), int id)
{
  //  Check to see if we were trying to run the attribute filter.  If so we want to discard it.

  if (attr_filter_active) slotFilterCancel ();


  options.color_index = id;


  int32_t k = options.color_index;


  float hue_inc = (float) (options.max_hsv_color[k] - options.min_hsv_color[k]) / (float) (NUMSHADES + 1);

  for (int32_t j = 0 ; j < NUMSHADES ; j++)
    {
      misc.track_color_array[j].setHsv ((int32_t) (options.min_hsv_color[k] + j * hue_inc), 255, 255, 255); 

      misc.color_array[j][0] = misc.track_color_array[j].red ();
      misc.color_array[j][1] = misc.track_color_array[j].green ();
      misc.color_array[j][2] = misc.track_color_array[j].blue ();
      misc.color_array[j][3] = misc.track_color_array[j].alpha ();
    }

  options.ref_color[1] = options.ref_color[0];
  options.ref_color[1].setAlpha (options.slice_alpha);


  redrawMap (NVFalse);
}



void
pfmEdit3D::slotFlagMenu (QAction *action)
{
  for (int32_t i = 0 ; i < PRE_USER + PFM_USER_FLAGS ; i++)
    {
      if (action == flag[i])
        {
          options.flag_index = i;
          break;
        }
    }


  setFlags (&misc, &options);


  bFlag->setIcon (flagIcon[options.flag_index]);
  disconnect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));
  flag[0]->setChecked (true);
  connect (flagGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotFlagMenu (QAction *)));

  redrawMap (NVFalse);


  //  Even though we do this in redrawMap we need to do it again here because otherwise the damn markers disappear.
  //  It's OpenGL, I have no idea why.

  overlayData (map, &options, &misc);
}



void
pfmEdit3D::slotProg (int id)
{
  if (options.progButton[id]->isChecked ())
    {
      runProg (id);


      //  Switch to DELETE_POINT mode.

      bDeletePoint->click ();
    }
  else
    {
      killProg (id);
    }
}



void
pfmEdit3D::slotHideMenuShow ()
{
  statbook->setCurrentIndex (1);
}



void
pfmEdit3D::slotHideMenu (QAction *action __attribute__ ((unused)))
{
  uint32_t flags[PFM_USER_FLAGS] =
    {PFM_USER_01, PFM_USER_02, PFM_USER_03, PFM_USER_04, PFM_USER_05, PFM_USER_06, PFM_USER_07, PFM_USER_08, PFM_USER_09, PFM_USER_10};
  uint8_t flag_hidden = NVFalse, type_hidden = NVFalse;


  for (int32_t i = 0 ; i < PFM_USER_FLAGS + 1 ; i++)
    {
      if (hideAct[i]->isChecked ())
        {
          flag_hidden = NVTrue;
          options.hide_flag[i] = NVTrue;
        }
      else
        {
          options.hide_flag[i] = NVFalse;
        }
    }


  //  Check for hiding data types if we have more than one.

  if (misc.unique_count > 1)
    {
      for (int32_t i = 0 ; i < misc.unique_count ; i++)
        {
          if (hideAct[i + flags_offset]->isChecked ())
            {
              type_hidden = NVTrue;
              options.hide_flag[i + flags_offset] = NVTrue;
            }
          else
            {
              options.hide_flag[i + flags_offset] = NVFalse;
            }
        }
    }


  //  Check for hiding CZMIL channels if we have only CZMIL data and we have the channel number as an attribute.

  if (misc.czmil_chan_attr >= 0 && misc.unique_count == 1)
    {
      for (int32_t i = 0 ; i < 9 ; i++)
        {
          if (hideAct[i + flags_offset]->isChecked ())
            {
              type_hidden = NVTrue;
              options.hide_flag[i + flags_offset] = NVTrue;
            }
          else
            {
              options.hide_flag[i + flags_offset] = NVFalse;
            }
        }
    }


  //  If any of the hide flags are set we need to set the mask_active flag (even if nothing is actually hidden right now).

  if (type_hidden)
    {
      options.mask_active |= MASK_TYPE;
    }
  else
    {
      options.mask_active &= UNMASK_TYPE;
    }

  if (flag_hidden)
    {
      options.mask_active |= MASK_FLAG;
    }
  else
    {
      options.mask_active &= UNMASK_FLAG;
    }


  //  Now we set the flags.

  for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
    {
      //  Hide by flags

      for (int32_t j = 0 ; j < PFM_USER_FLAGS ; j++)
        {
          if (options.hide_flag[j])
            {
              if (misc.data[i].val & flags[j]) misc.data[i].mask |= MASK_FLAG;
            }
          else
            {
              if (misc.data[i].val & flags[j]) misc.data[i].mask &= UNMASK_FLAG;
            }
        }


      //  Hide by data type

      if (misc.unique_count > 1)
        {
          for (int32_t j = 0 ; j < misc.unique_count ; j++)
            {
              if (options.hide_flag[j + flags_offset])
                {
                  if (misc.data[i].type == misc.unique_type[j])
                    {
                      misc.data[i].mask |= MASK_TYPE;
                    }
                }
              else
                {
                  if (misc.data[i].type == misc.unique_type[j])
                    {
                      misc.data[i].mask &= UNMASK_TYPE;
                    }
                }
            }
        }


      //  Hide by CZMIL channel number

      if (misc.czmil_chan_attr >= 0 && misc.unique_count == 1)
        {
          for (int32_t j = 0 ; j < 9 ; j++)
            {
              /***************************************** IMPORTANT NOTE ********************************************\

                 CZMIL channel number offset by 1.  See comments in get_buffer.cpp!

              \***************************************** IMPORTANT NOTE ********************************************/

              if (options.hide_flag[j + flags_offset])
                {
                  if (misc.data[i].attr[misc.czmil_chan_attr] == j + 1)
                    {
                      misc.data[i].mask |= MASK_TYPE;
                    }
                }
              else
                {
                  if (misc.data[i].attr[misc.czmil_chan_attr] == j + 1)
                    {
                      misc.data[i].mask &= UNMASK_TYPE;
                    }
                }
            }
        }
    }


  //  Hide highlighted

  if (options.hide_flag[PFM_USER_FLAGS])
    {
      for (int32_t i = 0 ; i < misc.highlight_count ; i++)
        {
          misc.data[misc.highlight[i]].mask |= MASK_FLAG;
        }
    }
  else
    {
      for (int32_t i = 0 ; i < misc.highlight_count ; i++)
        {
          misc.data[misc.highlight[i]].mask &= UNMASK_FLAG;
        }
    }


  redrawMap (NVFalse);
}



void
pfmEdit3D::slotInvertHideFlags ()
{
  disconnect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));

  for (int32_t i = 0 ; i < PFM_USER_FLAGS ; i++)
    {
      if (hideAct[i]->isChecked ())
        {
          hideAct[i]->setChecked (false);
        }
      else
        {
          hideAct[i]->setChecked (true);
        }
    }


  //  Don't mess with highlighted unless there are highlighted points.

  if (misc.highlight_count)
    {
      if (hideAct[PFM_USER_FLAGS]->isChecked ())
        {
          hideAct[PFM_USER_FLAGS]->setChecked (false);
        }
      else
        {
          hideAct[PFM_USER_FLAGS]->setChecked (true);
        }
    }

  connect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));

  slotHideMenu (NULL);
}



void
pfmEdit3D::slotInvertHideTypes ()
{
  disconnect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));

  if (misc.unique_count > 1)
    {
      for (int32_t i = 0 ; i < misc.unique_count ; i++)
        {
          if (hideAct[i + flags_offset]->isChecked ())
            {
              hideAct[i + flags_offset]->setChecked (false);
            }
          else
            {
              hideAct[i + flags_offset]->setChecked (true);
            }
        }
    }


  //  Check for hiding CZMIL channels if we have only CZMIL data and we have the channel number as an attribute.

  if (misc.czmil_chan_attr >= 0 && misc.unique_count == 1)
    {
      for (int32_t i = 0 ; i < 9 ; i++)
        {
          if (hideAct[i + flags_offset]->isChecked ())
            {
              hideAct[i + flags_offset]->setChecked (false);
            }
          else
            {
              hideAct[i + flags_offset]->setChecked (true);
            }
        }
    }

  connect (hideGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHideMenu (QAction *)));

  slotHideMenu (NULL);
}



//!  Slot called to toggle display of a PFM layer.

void 
pfmEdit3D::slotLayerClicked (int id)
{
  if (layerCheck[id]->checkState () == Qt::Checked)
    {
      misc.abe_share->display_pfm[id] = NVTrue;
    }
  else
    {
      misc.abe_share->display_pfm[id] = NVFalse;
    }

  redrawMap (NVTrue);
}



void
pfmEdit3D::extendedHelp ()
{
  if (getenv ("PFM_ABE") == NULL)
    {
      QMessageBox::warning (this, tr ("pfmEdit3D User Guide"), tr ("The PFM_ABE environment variable is not set.\n"
                                                                   "This must point to the folder that contains the bin folder containing the PFM_ABE programs."));
      return;
    }



  QString html_help_file;
  QString dir (getenv ("PFM_ABE"));
  html_help_file = dir + SEPARATOR + "doc" + SEPARATOR + "PFM_ABE_User_Guide.html";


  QFileInfo hf (html_help_file);

  if (!hf.exists () || !hf.isReadable ())
    {
      QMessageBox::warning (this, tr ("pfmEdit3D User Guide"), tr ("Can't find the Area Based Editor documentation file: %1").arg (html_help_file));
      return;
    }

  QDesktopServices::openUrl (QUrl (html_help_file, QUrl::TolerantMode));
}



void
pfmEdit3D::slotHotkeyHelp ()
{
  hotkeyHelp *hk = new hotkeyHelp (this, &options, &misc);


  //  Put the dialog in the middle of the screen.

  hk->move (x () + width () / 2 - hk->width () / 2, y () + height () / 2 - hk->height () / 2);

  hk->show ();
}



void
pfmEdit3D::slotToolbarHelp ()
{
  QWhatsThis::showText (QCursor::pos ( ), toolbarText, map);
}



void 
pfmEdit3D::slotAreaFilter ()
{
  options.filter_type = AREA_FILTER;

  slotFilter ();
}



void 
pfmEdit3D::slotStatFilter ()
{
  options.filter_type = STAT_FILTER;

  slotFilter ();
}



void 
pfmEdit3D::slotHPFilter ()
{
  options.filter_type = HP_FILTER;

  slotFilter ();
}



void 
pfmEdit3D::slotCZMILUrbanFilter ()
{
  options.filter_type = CZMIL_URBAN_FILTER;

  slotFilter ();
}



void 
pfmEdit3D::slotFilter ()
{
  notebook->setTabEnabled (AREATAB + options.filter_type, true);
  notebook->setCurrentIndex (AREATAB + options.filter_type);


  filter_active = NVTrue;

  if (misc.filter_kill_count)
    {
      misc.filter_kill_list.clear ();
      misc.filter_kill_count = 0;


      //  Clear the marker points.

      map->clearMarkerPoints ();


      redrawMap (NVFalse);
    }


  QString tmp;

  switch (options.filter_type)
    {
    case AREA_FILTER:
      filterBox->setTitle (tr ("0 points selected"));
      stdValue->setText (QString ("%L1").arg (options.area_filter_std, 0, 'f', 1));
      break;

    case STAT_FILTER:
      statFilterBox->setTitle (tr ("0 points selected"));
      avtValue->setText (QString ("%L1").arg (options.stat_filter_std, 0, 'f', 1));
      radiusValue->setValue (options.stat_filter_radius);
      countValue->setValue (options.stat_filter_count);
      break;

    case HP_FILTER:
      hpFilterBox->setTitle (tr ("0 points selected"));
      hpRadiusValue->setValue (options.hp_filter_radius);
      hpHeightValue->setValue (options.hp_filter_height);
      hpNeighborsValue->setValue (options.hp_filter_neighbors);
      hpMultiNeighborsValue->setValue (options.hp_filter_ml_neighbors);
      break;

    case CZMIL_URBAN_FILTER:
      cufFilterBox->setTitle (tr ("0 points selected"));
      cufVRadiusValue->setValue (options.czmil_urban_radius[0]);
      cufVHeightValue->setValue (options.czmil_urban_height[0]);
      cufLDIndexValue->setValue (options.czmil_urban_d_index[0]);
      cufFRadiusValue->setValue (options.czmil_urban_radius[1]);
      cufFHeightValue->setValue (options.czmil_urban_height[1]);
      cufUDIndexValue->setValue (options.czmil_urban_d_index[1]);
      break;
    }
}



void 
pfmEdit3D::slotClearFilterMasks ()
{
  //  Clear any filter masked areas.
  
  for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++) misc.data[i].fmask = NVFalse;

  misc.filter_mask = NVFalse;


  //  If the filter message box is up then we are clearing the filter masks after the filter so we want to discard the current filter points
  //  then filter again.

  if (filter_active) slotFilter ();


  redrawMap (NVFalse);
}



void
pfmEdit3D::slotFilterStdChanged (int value)
{
  options.area_filter_std = (float) value / 10.0;
  stdValue->setText (QString ("%L1").arg (options.area_filter_std, 0, 'f', 1));
}



void
pfmEdit3D::slotFilterStdReturn ()
{
  float value = (float) stdValue->text ().toDouble ();
  int32_t ivalue = NINT (value * 10.0);

  if (ivalue < stdSlider->minimum () || ivalue > stdSlider->maximum ()) ivalue = stdSlider->value ();

  stdSlider->setValue (ivalue);
}



void
pfmEdit3D::slotFilterApply ()
{
  uint8_t Filter (OPTIONS *options, MISC *misc, double *mx, double *my, int32_t count);
  uint8_t czmilUrbanFilter (OPTIONS *options, MISC *misc);


  if (options.filter_type == CZMIL_URBAN_FILTER)
    {
      setWidgetStates (NVFalse);

      qApp->setOverrideCursor (Qt::WaitCursor);
      qApp->processEvents ();

      czmilUrbanFilter (&options, &misc);
    }
  else
    {
      //  Set the state (enabled/disabled) of the min and max spin boxes

      minZLabel->setEnabled (options.filter_apply_z_range);
      minZValue->setEnabled (options.filter_apply_z_range);
      maxZLabel->setEnabled (options.filter_apply_z_range);
      maxZValue->setEnabled (options.filter_apply_z_range);
      avtMinZLabel->setEnabled (options.filter_apply_z_range);
      avtMinZValue->setEnabled (options.filter_apply_z_range);
      avtMaxZLabel->setEnabled (options.filter_apply_z_range);
      avtMaxZValue->setEnabled (options.filter_apply_z_range);
      hpMinZLabel->setEnabled (options.filter_apply_z_range);
      hpMinZValue->setEnabled (options.filter_apply_z_range);
      hpMaxZLabel->setEnabled (options.filter_apply_z_range);
      hpMaxZValue->setEnabled (options.filter_apply_z_range);

      double mx[4], my[4];

      mx[0] = misc.abe_share->edit_area.min_x;
      my[0] = misc.abe_share->edit_area.min_y;
      mx[1] = misc.abe_share->edit_area.min_x;
      my[1] = misc.abe_share->edit_area.max_y;
      mx[2] = misc.abe_share->edit_area.max_x;
      my[2] = misc.abe_share->edit_area.max_y;
      mx[3] = misc.abe_share->edit_area.max_x;
      my[3] = misc.abe_share->edit_area.min_y;

      setWidgetStates (NVFalse);

      qApp->setOverrideCursor (Qt::WaitCursor);
      qApp->processEvents ();

      Filter (&options, &misc, mx, my, 4);
    }

  qApp->restoreOverrideCursor ();
  qApp->processEvents ();


  switch (options.filter_type)
    {
    case AREA_FILTER:
      if (misc.filter_kill_count == 1)
        {
          filterBox->setTitle (tr ("1 point selected"));
        }
      else
        {
          filterBox->setTitle (tr ("%1 points selected").arg (misc.filter_kill_count));
        }

      stdValue->setText (QString ("%L1").arg (options.area_filter_std, 0, 'f', 1));
      break;

    case STAT_FILTER:
      if (misc.filter_kill_count == 1)
        {
          statFilterBox->setTitle (tr ("1 point selected"));
        }
      else
        {
          statFilterBox->setTitle (tr ("%1 points selected").arg (misc.filter_kill_count));
        }

      avtValue->setText (QString ("%L1").arg (options.stat_filter_std, 0, 'f', 1));
      radiusValue->setValue (options.stat_filter_radius);
      countValue->setValue (options.stat_filter_count);
      break;

    case HP_FILTER:
      if (misc.filter_kill_count == 1)
        {
          hpFilterBox->setTitle (tr ("1 point selected"));
        }
      else
        {
          hpFilterBox->setTitle (tr ("%1 points selected").arg (misc.filter_kill_count));
        }


      hpRadiusValue->setValue (options.hp_filter_radius);
      hpHeightValue->setValue (options.hp_filter_height);
      hpNeighborsValue->setValue (options.hp_filter_neighbors);
      hpMultiNeighborsValue->setValue (options.hp_filter_ml_neighbors);
      break;

    case CZMIL_URBAN_FILTER:
      if (misc.filter_kill_count == 1)
        {
          cufFilterBox->setTitle (tr ("1 point selected"));
        }
      else
        {
          cufFilterBox->setTitle (tr ("%1 points selected").arg (misc.filter_kill_count));
        }

      cufVRadiusValue->setValue (options.czmil_urban_radius[0]);
      cufVHeightValue->setValue (options.czmil_urban_height[0]);
      cufLDIndexValue->setValue (options.czmil_urban_d_index[0]);
      cufFRadiusValue->setValue (options.czmil_urban_radius[1]);
      cufFHeightValue->setValue (options.czmil_urban_height[1]);
      cufUDIndexValue->setValue (options.czmil_urban_d_index[1]);
      break;
    }


  if (misc.filter_kill_count)
    {
      redrawMap (NVTrue);


      //  Force redrawing of the filter kill points

      overlayFlag (map, &options, &misc);
    }


  map->setFocus (Qt::OtherFocusReason);
}



void
pfmEdit3D::slotHpFilterApply ()
{
  options.hp_filter_radius = hpRadiusValue->value ();
  options.hp_filter_height = hpHeightValue->value ();
  options.hp_filter_neighbors = hpNeighborsValue->value ();
  options.hp_filter_ml_neighbors = hpMultiNeighborsValue->value ();

  slotFilterApply ();

  map->setFocus (Qt::OtherFocusReason);
}



void
pfmEdit3D::slotCzmilUrbanFilterApply ()
{
  options.czmil_urban_radius[0] = cufVRadiusValue->value ();
  options.czmil_urban_height[0] = cufVHeightValue->value ();
  options.czmil_urban_d_index[0] = cufLDIndexValue->value ();
  options.czmil_urban_radius[1] = cufFRadiusValue->value ();
  options.czmil_urban_height[1] = cufFHeightValue->value ();
  options.czmil_urban_d_index[1] = cufUDIndexValue->value ();

  slotFilterApply ();

  map->setFocus (Qt::OtherFocusReason);
}



void
pfmEdit3D::slotFilterDelete ()
{
  //  Only do this if we're running the HOF wave filter, otherwise the sliders don't exist.

  if (hof_filter_active)
    {
      misc.busy = NVFalse;

      srSlider->setEnabled (true);
      swSlider->setEnabled (true);
      rtSlider->setEnabled (true);
    }


  filter_active = NVFalse;
  attr_filter_active = NVFalse;
  hof_filter_active = NVFalse;


  //  Use PFM_MANUALLY_INVAL flag since the user has to confirm.

  for (int32_t i = 0 ; i < misc.filter_kill_count ; i++)
    {
      //  Negative one means we've masked the point after filtering.

      if (misc.filter_kill_list[i] >= 0)
        {
          store_undo (&misc, options.undo_levels, misc.data[misc.filter_kill_list[i]].val, misc.filter_kill_list[i], NULL);

          misc.data[misc.filter_kill_list[i]].val |= PFM_MANUALLY_INVAL;


          //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
          //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
          //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
          //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
          //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

          misc.data[misc.filter_kill_list[i]].val &= ~PFM_SUSPECT;
        }
    }
  end_undo_block (&misc);
  misc.filtered = NVTrue;


  misc.filter_kill_list.clear ();
  misc.filter_kill_count = 0;


  //  Clear the marker points.

  map->clearMarkerPoints ();


  notebook->setTabEnabled (TRACKTAB, true);
  notebook->setTabEnabled (HISTTAB, true);

  if (options.track_tab)
    {
      notebook->setCurrentIndex (TRACKTAB);
    }
  else
    {
      notebook->setCurrentIndex (HISTTAB);
    }

  notebook->setTabEnabled (AREATAB, false);
  notebook->setTabEnabled (STATTAB, false);
  notebook->setTabEnabled (HPTAB, false);
  notebook->setTabEnabled (ATTRTAB, false);
  notebook->setTabEnabled (CZMILURBANTAB, false);
  notebook->setTabEnabled (HOFTAB, false);
  notebook->setTabEnabled (CPFTAB, false);
  notebook->setTabEnabled (LIDARCLASSTAB, false);


  //  Just in case we were using the CLEAR_POLYGON or FILTER_MASK functions we need to set the function back to the last used edit mode.

  options.function = misc.save_mode;
  setFunctionCursor (options.function);

  redrawMap (NVTrue);
}



void
pfmEdit3D::slotFilterHide ()
{
  if (misc.filter_kill_count)
    {
      options.mask_active |= MASK_TYPE;


      //  Hide the points in the kill list, then clear the kill list.

      for (int32_t i = 0 ; i < misc.filter_kill_count ; i++)
	{
	  //  Negative one means we've masked the point after filtering.

	  if (misc.filter_kill_list[i] >= 0)
	    {
	      misc.data[misc.filter_kill_list[i]].mask |= MASK_TYPE;
	    }
	}


      misc.filter_kill_list.clear ();
      misc.filter_kill_count = 0;


      //  Clear the marker points.

      map->clearMarkerPoints ();


      //  Just in case we were using the CLEAR_POLYGON or FILTER_MASK functions we need to set the function back to the last used edit mode.

      options.function = misc.save_mode;
      setFunctionCursor (options.function);

      redrawMap (NVTrue);
    }
}



void
pfmEdit3D::slotFilterCancel ()
{
  //  Only do this if we're running the HOF wave filter, otherwise the sliders don't exist.

  if (hof_filter_active)
    {
      misc.busy = NVFalse;

      srSlider->setEnabled (true);
      swSlider->setEnabled (true);
      rtSlider->setEnabled (true);
    }


  filter_active = NVFalse;
  attr_filter_active = NVFalse;
  hof_filter_active = NVFalse;

  misc.filter_kill_list.clear ();
  misc.filter_kill_count = 0;


  //  Clear the marker points.

  map->clearMarkerPoints ();


  notebook->setTabEnabled (TRACKTAB, true);
  notebook->setTabEnabled (HISTTAB, true);

  if (options.track_tab)
    {
      notebook->setCurrentIndex (TRACKTAB);
    }
  else
    {
      notebook->setCurrentIndex (HISTTAB);
    }


  notebook->setTabEnabled (AREATAB, false);
  notebook->setTabEnabled (STATTAB, false);
  notebook->setTabEnabled (HPTAB, false);
  notebook->setTabEnabled (ATTRTAB, false);
  notebook->setTabEnabled (CZMILURBANTAB, false);
  notebook->setTabEnabled (HOFTAB, false);
  notebook->setTabEnabled (CPFTAB, false);
  notebook->setTabEnabled (LIDARCLASSTAB, false);


  //  Just in case we were using the CLEAR_POLYGON or FILTER_MASK functions we need to set the function back to the last used edit mode.

  options.function = misc.save_mode;
  setFunctionCursor (options.function);


  //  If we had highlighting on prior to the filter, redo it.

  if (options.flag_index) setFlags (&misc, &options);


  redrawMap (NVTrue);
}



void
pfmEdit3D::slotAvtFilterStdChanged (int value)
{
  options.stat_filter_std = (float) value / 10.0;
  avtValue->setText (QString ("%L1").arg (options.stat_filter_std, 0, 'f', 1));
}



void
pfmEdit3D::slotAvtFilterStdReturn ()
{
  float value = (float) avtValue->text ().toDouble ();
  int32_t ivalue = NINT (value * 10.0);

  if (ivalue < avtSlider->minimum () || ivalue > avtSlider->maximum ()) ivalue = avtSlider->value ();

  avtSlider->setValue (ivalue);
}



void
pfmEdit3D::slotAvtFilterApply ()
{
  options.stat_filter_radius = radiusValue->value ();
  options.stat_filter_count = countValue->value ();

  slotFilterApply ();

  map->setFocus (Qt::OtherFocusReason);
}



void 
pfmEdit3D::slotRadValueChanged (double value)
{
  options.stat_filter_radius = value;
}



void 
pfmEdit3D::slotCountValueChanged (int value)
{
  options.stat_filter_count = value;
}



void 
pfmEdit3D::slotAvtPassValueChanged (int value)
{
  options.stat_filter_passes = value;
}



void 
pfmEdit3D::slotHpPassValueChanged (int value)
{
  options.hp_filter_passes = value;
}



void 
pfmEdit3D::slotRangeCheckStateChanged (int state)
{
  if (state == Qt::Checked)
    {
      options.filter_apply_z_range = NVTrue;
    }
  else
    {
      options.filter_apply_z_range = NVFalse;
    }


  //  Set the state of the Z range limitations for all of the filters.

  disconnect (rangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  disconnect (hpRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  disconnect (attrRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  disconnect (lidarClassRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));

  rangeCheck->setChecked (options.filter_apply_z_range);
  hpRangeCheck->setChecked (options.filter_apply_z_range);
  attrRangeCheck->setChecked (options.filter_apply_z_range);
  lidarClassRangeCheck->setChecked (options.filter_apply_z_range);

  connect (rangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  connect (hpRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  connect (attrRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));
  connect (lidarClassRangeCheck, SIGNAL (stateChanged (int)), this, SLOT (slotRangeCheckStateChanged (int)));


  minZLabel->setEnabled (options.filter_apply_z_range);
  minZValue->setEnabled (options.filter_apply_z_range);
  maxZLabel->setEnabled (options.filter_apply_z_range);
  maxZValue->setEnabled (options.filter_apply_z_range);

  hpMinZLabel->setEnabled (options.filter_apply_z_range);
  hpMinZValue->setEnabled (options.filter_apply_z_range);
  hpMaxZLabel->setEnabled (options.filter_apply_z_range);
  hpMaxZValue->setEnabled (options.filter_apply_z_range);

  attrMinZLabel->setEnabled (options.filter_apply_z_range);
  attrMinZValue->setEnabled (options.filter_apply_z_range);
  attrMaxZLabel->setEnabled (options.filter_apply_z_range);
  attrMaxZValue->setEnabled (options.filter_apply_z_range);

  lidarClassMinZLabel->setEnabled (options.filter_apply_z_range);
  lidarClassMinZValue->setEnabled (options.filter_apply_z_range);
  lidarClassMaxZLabel->setEnabled (options.filter_apply_z_range);
  lidarClassMaxZValue->setEnabled (options.filter_apply_z_range);
}



void 
pfmEdit3D::slotMinZValueChanged (double value)
{
  options.filter_min_z = (float) value;
  minZValue->setValue (options.filter_min_z);
  hpMinZValue->setValue (options.filter_min_z);
  attrMinZValue->setValue (options.filter_min_z);
  lidarClassMinZValue->setValue (options.filter_min_z);
}



void 
pfmEdit3D::slotMaxZValueChanged (double value)
{
  options.filter_max_z = (float) value;
  maxZValue->setValue (options.filter_max_z);
  hpMaxZValue->setValue (options.filter_max_z);
  attrMaxZValue->setValue (options.filter_max_z);
  lidarClassMaxZValue->setValue (options.filter_max_z);
}



void
pfmEdit3D::slotAttrFilter ()
{
  switch (options.color_index)
    {
    case 1:
      attrFilterName->setText (tr ("Line number"));
      attrRangeText->setText (options.lineFilterText);
      break;

    case 2:
      attrFilterName->setText (tr ("Horizontal error"));
      attrRangeText->setText (options.horzFilterText);
      break;

    case 3:
      attrFilterName->setText (tr ("Vertical error"));
      attrRangeText->setText (options.vertFilterText);
      break;

    default:
      int32_t ndx = misc.attrStatNum[options.color_index - PRE_ATTR];
      attrFilterName->setText (QString (misc.abe_share->open_args[0].head.ndx_attr_name[ndx]));
      attrRangeText->setText (options.attrFilterText[ndx]);
      break;
    }


  notebook->setTabEnabled (ATTRTAB, true);
  notebook->setCurrentIndex (ATTRTAB);


  //  Enable or disable the Z range limits based on the filter_apply_z_range setting.

  attrMinZLabel->setEnabled (options.filter_apply_z_range);
  attrMinZValue->setEnabled (options.filter_apply_z_range);
  attrMaxZLabel->setEnabled (options.filter_apply_z_range);
  attrMaxZValue->setEnabled (options.filter_apply_z_range);
}



void
pfmEdit3D::slotAttrFilterApply ()
{
  void attrTextFilter (OPTIONS *options, MISC *misc, float *amin, float *amax, int32_t count);


  attr_filter_active = NVTrue;


  //  Set the tool tip so that the user can see the entire range without having to enter the widget.

  attrRangeText->setToolTip (attrRangeText->text ());


  //  Figure out how many decimal places we need for min and max based on the attribute.

  float decimals = 0;

  switch (options.color_index)
    {
    case 1:
      decimals = 0;
      break;

    case 2:
    case 3:
      decimals = 2;
      break;

    default:
      int32_t ndx = misc.attrStatNum[options.color_index - PRE_ATTR];
      decimals = NINT (log10 (misc.abe_share->open_args[0].head.ndx_attr_scale[ndx]));
      break;
    }

  attrFilterMin->setText (tr ("Min: %L1").arg (misc.attr_min, 0, 'f', decimals, QChar ('0')));
  attrFilterMax->setText (tr ("Max: %L1").arg (misc.attr_max, 0, 'f', decimals, QChar ('0')));


  if (misc.filter_kill_count)
    {
      misc.filter_kill_list.clear ();
      misc.filter_kill_count = 0;


      //  Clear the marker points.

      map->clearMarkerPoints ();


      redrawMap (NVFalse);
    }


  setWidgetStates (NVFalse);


  QString rangeText = attrRangeText->text ().simplified (). remove (' ');


  //  First check for invalid characters.

  for (int32_t i = 0 ; i < rangeText.length () ; i++)
    {
      if (!(rangeText.at (i).isDigit ()))
        {
          if (rangeText.at (i) != ',' && rangeText.at (i) != '-' && rangeText.at (i) != '>' && rangeText.at (i) != '.')
            {
              QMessageBox::warning (this, tr ("pfmEdit3D Attribute Filter"),
                                    tr ("Invalid character %1 in attribute filter range text").arg (rangeText.at (i)));
              return;
            }
        }
    }


  //  Now split the string into sections divided by commas.

  QStringList ranges = rangeText.split (",", QString::SkipEmptyParts);


  int32_t range_count = ranges.size ();


  if (range_count == 0) return;


  //  Allocate the memory for the filter ranges.

  float *amin, *amax;

  amin = (float *) calloc (range_count, sizeof (float));
  if (amin == NULL)
    {
      perror ("Allocating amin memory in slotAttrFilterApply");
      exit (-1);
    }


  amax = (float *) calloc (range_count, sizeof (float));
  if (amax == NULL)
    {
      perror ("Allocating amax memory in slotAttrFilterApply");
      exit (-1);
    }


  for (int32_t i = 0 ; i < range_count ; i++)
    {
      if (ranges.at (i).contains ('>'))
        {
          QString aminString = ranges.at (i).section ('>', 0, 0);
          QString amaxString = ranges.at (i).section ('>', 1, 1);

          amin[i] = aminString.toFloat ();
          amax[i] = amaxString.toFloat ();
	}
      else
        {
          amin[i] = amax[i] = ranges.at (i).toFloat ();
        }


      //  Check for inverted range.

      if (amin[i] > amax[i])
        {
          QMessageBox::warning (this, tr ("pfmEdit3D Attribute Filter"),
                                tr ("Range minimum %1 greater than range maximum %2").arg (amin[i]).arg (amax[i]));
          free (amin);
          free (amax);
          return;
        }
    }


  //  Save the ranges for this attribute.

  switch (options.color_index)
    {
    case 1:
      options.lineFilterText = attrRangeText->text ();
      break;

    case 2:
      options.horzFilterText = attrRangeText->text ();
      break;

    case 3:
      options.vertFilterText = attrRangeText->text ();
      break;

    default:
      int32_t ndx = misc.attrStatNum[options.color_index - PRE_ATTR];
      options.attrFilterText[ndx] = attrRangeText->text ();
      break;
    }



  attrTextFilter (&options, &misc, amin, amax, range_count);


  free (amin);
  free (amax);


  if (misc.filter_kill_count == 1)
    {
      attrFilterBox->setTitle (tr ("1 point selected"));
    }
  else
    {
      attrFilterBox->setTitle (tr ("%1 points selected").arg (misc.filter_kill_count));
    }


  redrawMap (NVTrue);


  //  Force redrawing of the filter kill points

  overlayData (map, &options, &misc);


  //  Set focus back to the map widget so that the Del key works.

  map->setFocus (Qt::OtherFocusReason);
}



void
pfmEdit3D::slotAttrFilterInvert ()
{
  //  Pull the ranges out of the text string.

  QString rangeText = attrRangeText->text ().simplified (). remove (' ');


  //  Check for invalid characters (just in case someone has entered ranges and not appiled them).

  for (int32_t i = 0 ; i < rangeText.length () ; i++)
    {
      if (!(rangeText.at (i).isDigit ()))
        {
          if (rangeText.at (i) != ',' && rangeText.at (i) != '-' && rangeText.at (i) != '>' && rangeText.at (i) != '.')
            {
              QMessageBox::warning (this, tr ("pfmEdit3D Attribute Filter"),
                                    tr ("Invalid character %1 in attribute filter range text").arg (rangeText.at (i)));
              return;
            }
        }
    }


  //  Split the string into sections divided by commas.

  QStringList ranges = rangeText.split (",", QString::SkipEmptyParts);


  int32_t range_count = ranges.size ();


  if (range_count == 0) return;


  //  Allocate the memory for the filter ranges.

  int64_t *amin, *amax;

  amin = (int64_t *) calloc (range_count, sizeof (int64_t));
  if (amin == NULL)
    {
      perror ("Allocating amin memory in slotAttrFilterApply");
      exit (-1);
    }


  amax = (int64_t *) calloc (range_count, sizeof (int64_t));
  if (amax == NULL)
    {
      perror ("Allocating amax memory in slotAttrFilterApply");
      exit (-1);
    }


  //  Figure out how many decimal places we need based on the attribute.

  float multiplier = 1.0;

  switch (options.color_index)
    {
    case 1:
      multiplier = 1.0;
      break;

    case 2:
    case 3:
      multiplier = 100.0;
      break;

    default:
      int32_t ndx = misc.attrStatNum[options.color_index - PRE_ATTR];
      multiplier = misc.abe_share->open_args[0].head.ndx_attr_scale[ndx];
      break;
    }


  //  Get the ranges in integer form multiplied by 10 to the number of decimal places.

  for (int32_t i = 0 ; i < range_count ; i++)
    {
      if (ranges.at (i).contains ('>'))
        {
          QString aminString = ranges.at (i).section ('>', 0, 0);
          QString amaxString = ranges.at (i).section ('>', 1, 1);

          amin[i] = NINT (aminString.toFloat () * multiplier);
          amax[i] = NINT (amaxString.toFloat () * multiplier);
	}
      else
        {
          amin[i] = amax[i] = NINT (ranges.at (i).toFloat () * multiplier);
        }
    }


  //  Limt the beginning to the min and max regardless of what the user input.

  int64_t imin = NINT (misc.attr_min * multiplier);
  int64_t imax = NINT (misc.attr_max * multiplier);

  amin[0] = qMax (amin[0], imin);
  amax[range_count - 1] = qMin (amax[range_count - 1], imax);


  int64_t start = 0;
  int32_t new_count = 0;
  std::vector<int64_t> fmin, fmax;


  //  Get the first range depending on whether the original range starts on the minimum

  try
    {
      fmin.resize (new_count + 1);
    }
  catch (std::bad_alloc&)
    {
      fprintf (stderr, "%s %s %s %d - fmin - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  try
    {
      fmax.resize (new_count + 1);
    }
  catch (std::bad_alloc&)
    {
      fprintf (stderr, "%s %s %s %d - fmax - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }


  //  The first range doesn't start at minimum...

  if (imin != amin[0])
    {
      fmin[new_count] = imin;
      fmax[new_count] = amin[0] - 1;
      start = 0;
    }


  //  The first range starts at the minimum...

  else
    {
      fmin[new_count] = amax[0] + 1;

      if (range_count > 1)
        {
          fmax[new_count] = amin[1] - 1;
        }
      else
        {
          fmax[new_count] = imax;
        }

      start = 1;
    }

  new_count++;


  //  If we have more than one range we need to do the intermediate ranges...

  if (range_count > 1)
    {
      for (int32_t i = start ; i < range_count - 1 ; i++)
        {
          try
            {
              fmin.resize (new_count + 1);
            }
          catch (std::bad_alloc&)
            {
              fprintf (stderr, "%s %s %s %d - fmin - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          try
            {
              fmax.resize (new_count + 1);
            }
          catch (std::bad_alloc&)
            {
              fprintf (stderr, "%s %s %s %d - fmax - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          fmin[new_count] = amax[i] + 1;
          fmax[new_count] = amin[i + 1] - 1;

          new_count++;
        }
    }


  //  Get the last range depending on whether the original range ends on the maximum

  if (imax != amax[range_count - 1])
    {
      //  Don't do this if the first max is the same as the complete range max.

      if (range_count > 1 || fmax[new_count - 1] != imax)
        {
          try
            {
              fmin.resize (new_count + 1);
            }
          catch (std::bad_alloc&)
            {
              fprintf (stderr, "%s %s %s %d - fmin - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          try
            {
              fmax.resize (new_count + 1);
            }
          catch (std::bad_alloc&)
            {
              fprintf (stderr, "%s %s %s %d - fmax - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          fmin[new_count] = amax[range_count - 1] + 1;
          fmax[new_count] = imax;

          new_count++;
        }
    }

  free (amin);
  free (amax);

  QString new_ranges;

  for (int32_t i = 0 ; i < new_count ; i++)
    {
      QString rng;
      rng.setNum ((float) fmin[i] / multiplier);

      new_ranges += rng;

      if (fmin[i] != fmax[i])
        {
          new_ranges += ">";
          rng.setNum ((float) fmax[i] / multiplier);
          new_ranges += rng;
        }

      if (i != new_count - 1) new_ranges += ",";
    }

  fmin.clear ();
  fmax.clear ();


  //  Put the new ranges into the string and run the filter again

  attrRangeText->setText (new_ranges);

  slotAttrFilterApply ();
}



void 
pfmEdit3D::slotCzmilReprocessButtonsPressed (QMouseEvent *e)
{
  if (misc.busy) return;


  uint8_t temp_flags[2];

  for (int32_t i = 0 ; i < 2 ; i++) temp_flags[i] = options.czmil_reprocess_flags[i];

  float yloc = (float) e->y () / (float) czmilReprocessButtons->height ();

  if (yloc <= 0.5)
    {
      temp_flags[0] ^= 1;
    }
  else
    {
      temp_flags[1] ^= 1;
    }


  // Don't allow zero (i.e. no data types selected).

  if (temp_flags[0] + temp_flags[1])
    {
      for (int32_t i = 0 ; i < 2 ; i++) options.czmil_reprocess_flags[i] = temp_flags[i];

      setWidgetStates (NVTrue);
    }
}



void 
pfmEdit3D::slotHOFWaveFilter ()
{
  //  Turn off slicing and clear line hiding if we need to.

  if (misc.slice) slotPopupMenu (popup[NUMPOPUPS - 1]);
  if (misc.num_lines) slotDisplayAll ();


  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  hofFilterBox->setTitle (tr ("HOF Wave Filter"));


  //  Disconnect the sliders so setting them won't trigger the slots.

  disconnect (srSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterSrChanged (int)));
  disconnect (srSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  disconnect (swSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterSwChanged (int)));
  disconnect (swSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  disconnect (rtSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterRtChanged (int)));
  disconnect (rtSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  disconnect (pmtSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterPMTChanged (int)));
  disconnect (pmtSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  disconnect (apdSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterAPDChanged (int)));
  disconnect (apdSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));


  srSlider->setValue (NINT (misc.abe_share->filterShare.search_radius * 10.0));
  swSlider->setValue (misc.abe_share->filterShare.search_width);
  rtSlider->setValue (misc.abe_share->filterShare.rise_threshold);
  pmtSlider->setValue (misc.abe_share->filterShare.pmt_ac_zero_offset_required);
  apdSlider->setValue (misc.abe_share->filterShare.apd_ac_zero_offset_required);


  //  Now we can reconnect the signals and slots.

  connect (srSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterSrChanged (int)));
  connect (srSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  connect (swSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterSwChanged (int)));
  connect (swSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  connect (rtSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterRtChanged (int)));
  connect (rtSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  connect (pmtSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterPMTChanged (int)));
  connect (pmtSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));
  connect (apdSlider, SIGNAL (valueChanged (int)), this, SLOT (slotHOFWaveFilterAPDChanged (int)));
  connect (apdSlider, SIGNAL (sliderReleased ()), this, SLOT (slotHOFWaveFilter ()));


  notebook->setTabEnabled (HOFTAB, true);
  notebook->setCurrentIndex (HOFTAB);


  hof_filter_active = NVTrue;
  srSlider->setEnabled (false);
  swSlider->setEnabled (false);
  rtSlider->setEnabled (false);


  if (misc.filter_kill_count)
    {
      misc.filter_kill_list.clear ();
      misc.filter_kill_count = 0;
    }


  setWidgetStates (NVFalse);

  misc.busy = NVTrue;

  uint8_t failed = NVFalse;
  hofExf = new externalFilter (this, map, &options, &misc, HOFWAVEFILTER, &failed);
  if (failed)
    {
      qApp->restoreOverrideCursor ();
      misc.busy = NVFalse;
      redrawMap (NVFalse);
    }
  else
    {
      connect (hofExf, SIGNAL (externalFilterDone ()), this, SLOT (slotHofExternalFilterDone ()));
      connect (hofExf, SIGNAL (readStandardErrorSignal (QProcess *)), this, SLOT (slotReadStandardError (QProcess *)));
    }

  map->setFocus (Qt::OtherFocusReason);
}



void
pfmEdit3D::slotHOFWaveFilterSrChanged (int value)
{
  options.hofWaveFilter_search_radius = misc.abe_share->filterShare.search_radius = (double) value / 10.0;
  srValue->setText (QString ("%L1").arg (options.hofWaveFilter_search_radius, 0, 'f', 2));
}



void
pfmEdit3D::slotHOFWaveFilterSrReturn ()
{
  float value = (float) srValue->text ().toDouble ();
  int32_t ivalue = NINT (value * 10.0);

  if (ivalue < srSlider->minimum () || ivalue > srSlider->maximum ()) ivalue = srSlider->value ();

  srSlider->setValue (ivalue);

  slotHOFWaveFilter ();

  map->setFocus (Qt::OtherFocusReason);
}



void
pfmEdit3D::slotHOFWaveFilterSwChanged (int value)
{
  options.hofWaveFilter_search_width = misc.abe_share->filterShare.search_width = value;
  swValue->setText (QString ("%1").arg (options.hofWaveFilter_search_width));
}



void
pfmEdit3D::slotHOFWaveFilterSwReturn ()
{
  int32_t value = swValue->text ().toInt ();

  if (value < swSlider->minimum () || value > swSlider->maximum ()) value = swSlider->value ();

  swSlider->setValue (value);

  slotHOFWaveFilter ();

  map->setFocus (Qt::OtherFocusReason);
}



void
pfmEdit3D::slotHOFWaveFilterRtChanged (int value)
{
  options.hofWaveFilter_rise_threshold = misc.abe_share->filterShare.rise_threshold = value;
  rtValue->setText (QString ("%1").arg (options.hofWaveFilter_rise_threshold));
}



void
pfmEdit3D::slotHOFWaveFilterRtReturn ()
{
  int32_t value = rtValue->text ().toInt ();

  if (value < rtSlider->minimum () || value > rtSlider->maximum ()) value = rtSlider->value ();

  rtSlider->setValue (value);

  slotHOFWaveFilter ();

  map->setFocus (Qt::OtherFocusReason);
}



void
pfmEdit3D::slotHOFWaveFilterPMTChanged (int value)
{
  options.hofWaveFilter_pmt_ac_zero_offset_required = misc.abe_share->filterShare.pmt_ac_zero_offset_required = value;
  pmtValue->setText (QString ("%1").arg (options.hofWaveFilter_pmt_ac_zero_offset_required));
}



void
pfmEdit3D::slotHOFWaveFilterPMTReturn ()
{
  int32_t value = pmtValue->text ().toInt ();

  if (value < pmtSlider->minimum () || value > pmtSlider->maximum ()) value = pmtSlider->value ();

  pmtSlider->setValue (value);

  slotHOFWaveFilter ();

  map->setFocus (Qt::OtherFocusReason);
}



void
pfmEdit3D::slotHOFWaveFilterAPDChanged (int value)
{
  options.hofWaveFilter_apd_ac_zero_offset_required = misc.abe_share->filterShare.apd_ac_zero_offset_required = value;
  apdValue->setText (QString ("%1").arg (options.hofWaveFilter_apd_ac_zero_offset_required));
}



void
pfmEdit3D::slotHOFWaveFilterAPDReturn ()
{
  int32_t value = apdValue->text ().toInt ();

  if (value < apdSlider->minimum () || value > apdSlider->maximum ()) value = apdSlider->value ();

  apdSlider->setValue (value);

  slotHOFWaveFilter ();

  map->setFocus (Qt::OtherFocusReason);
}



void 
pfmEdit3D::slotHofExternalFilterDone ()
{
  qApp->restoreOverrideCursor ();
  qApp->processEvents ();

  srSlider->setEnabled (true);
  swSlider->setEnabled (true);
  rtSlider->setEnabled (true);


  if (misc.filter_kill_count == 1)
    {
      hofFilterBox->setTitle (tr ("1 point selected"));
    }
  else
    {
      hofFilterBox->setTitle (tr ("%1 points selected").arg (misc.filter_kill_count));
    }


  redrawMap (NVTrue);
}



void 
pfmEdit3D::slotCPFGrp (int id)
{
  options.czmil_cpf_filter_inval = id;

  valSpin->setEnabled (!options.czmil_cpf_filter_inval);
}



void
pfmEdit3D::slotCPFFilterAccept ()
{
  uint8_t undo_flag = NVFalse;

  filter_active = NVFalse;


  for (int32_t i = 0 ; i < misc.filter_kill_count ; i++)
    {
      //  If we are invalidating, all we have to do is mark the point

      if (options.czmil_cpf_filter_inval)
        {
          //  Negative one means we've masked the point after filtering.

          if (misc.filter_kill_list[i] >= 0)
            {
              undo_flag = NVTrue;
              store_undo (&misc, options.undo_levels, misc.data[misc.filter_kill_list[i]].val, misc.filter_kill_list[i], NULL);

              misc.data[misc.filter_kill_list[i]].val |= PFM_MANUALLY_INVAL;


              //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
              //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
              //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
              //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
              //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

              misc.data[misc.filter_kill_list[i]].val &= ~PFM_SUSPECT;
            }
        }
      else
        {
          //  Negative one means we've masked the point after filtering so we have to set the point back to invalid.  For 
          //  everything else we just leave it alone since the externalFilter function set the point to manual.  We do, however,
          //  have to move the temporary filter undo data to the normal undo vector.

          if (misc.filter_kill_list[i] >= 0)
            {
              undo_flag = NVTrue;
              store_undo (&misc, options.undo_levels, misc.filter_undo.val[i], misc.filter_undo.num[i], NULL);
            }
          else
            {
              misc.data[misc.filter_kill_list[i]].val = misc.filter_undo.val[i];
            }
        }
    }

  if (undo_flag) end_undo_block (&misc);


  misc.filtered = NVTrue;


  misc.filter_kill_list.clear ();
  misc.filter_kill_count = 0;


  //  Clear the marker points.

  map->clearMarkerPoints ();


  //  Since this is a mode we don't want to close the CPF Filter tab until we switch modes, so we'll just redraw.  We also need to make sure we
  //  stay in this mode in case we were using either the CLEAR_POLYGON function or one of the FILTER_MASK functions.

  options.function = CPF_FILTER;
  setFunctionCursor (options.function);

  redrawMap (NVTrue);
}



void
pfmEdit3D::slotCPFFilterDiscard ()
{
  filter_active = NVFalse;


  //  If we were validating and we reject the points we have to set the validity back to what it was.

  if (!options.czmil_cpf_filter_inval)
    {
      for (int32_t i = 0 ; i < misc.filter_undo.count ; i++)
        {
          misc.data[misc.filter_undo.num[i]].val = misc.filter_undo.val[i];
        }
    }


  misc.filter_undo.val.clear ();
  misc.filter_undo.num.clear ();
  misc.filter_undo.count = 0;


  misc.filter_kill_list.clear ();
  misc.filter_kill_count = 0;


  //  Clear the marker points.

  map->clearMarkerPoints ();


  //  Since this is a mode we don't want to close the CPF Filter tab until we switch modes, so we'll just redraw.  We also need to make sure we
  //  stay in this mode in case we were using either the CLEAR_POLYGON function or one of the FILTER_MASK functions.

  options.function = CPF_FILTER;
  setFunctionCursor (options.function);

  redrawMap (NVTrue);
}



void 
pfmEdit3D::slotCPFExternalFilterDone ()
{
  qApp->restoreOverrideCursor ();
  qApp->processEvents ();


  if (misc.filter_kill_count == 1)
    {
      cpfFilterBox->setTitle (tr ("1 point selected"));
    }
  else
    {
      cpfFilterBox->setTitle (tr ("%1 points selected").arg (misc.filter_kill_count));
    }


  redrawMap (NVTrue);


  cpfFilterAccept->setEnabled (true);
}



//  This slot starts LiDAR classification change preference dialog.

void 
pfmEdit3D::slotLidarClassChangePrefs ()
{
  if (lidar_class_dialog) lidar_class_dialog->close ();

  lidar_class_dialog = new lidarClassDialog (this, &options, &misc);
  connect (lidar_class_dialog, SIGNAL (dataChangedSignal ()), this, SLOT (slotLidarClassChangeDataChanged ()));


  //  Put the dialog in the middle of the screen.

  lidar_class_dialog->move (x () + width () / 2 - lidar_class_dialog->width () / 2, y () + height () / 2 - lidar_class_dialog->height () / 2);
}



void 
pfmEdit3D::slotLidarClassChangeDataChanged ()
{
  //  Set the label values.

  QString classes = "From: ";
  QString num;

  if (options.from_lidar_class[0])
    {
      classes += "Any";
    }
  else
    {
      for (int32_t i = 1 ; i < LIDAR_CLASSES ; i++)
        {
          uint8_t started = NVFalse;

          if (options.from_lidar_class[i])
            {
              if (started) classes += ",";
              num.setNum (misc.lidar_class_num[i]);
              classes += num;
              started = NVTrue;
            }
        }
    }

  lidarClassChangeFrom->setText (classes);

  num.setNum (misc.lidar_class_num[options.lidar_class]);
  classes = tr ("To: %1").arg (num);
  lidarClassChangeTo->setText (classes);
}



void
pfmEdit3D::slotLidarClassChangeAccept ()
{
  uint8_t undo_flag = NVFalse;

  for (int32_t i = 0 ; i < misc.filter_kill_count ; i++)
    {
      //  Negative one means we've masked the point after filtering.

      if (misc.filter_kill_list[i] >= 0)
        {
          undo_flag = NVTrue;
          store_undo (&misc, options.undo_levels, misc.data[misc.filter_kill_list[i]].val, misc.filter_kill_list[i], 
                      &misc.data[misc.filter_kill_list[i]].attr[misc.lidar_class_attr]);


          //  We're using PFM_SELECTED_SOUNDING (application specific) to indicate that we've changed a CZMIL or LAS
          //  classification attribute.

          misc.data[misc.filter_kill_list[i]].val |= (PFM_SELECTED_SOUNDING | PFM_MODIFIED);
          misc.data[misc.filter_kill_list[i]].lidar_class_change = NVTrue;
          misc.data[misc.filter_kill_list[i]].attr[misc.lidar_class_attr] = misc.lidar_class_num[options.lidar_class];
        }
    }

  if (undo_flag) end_undo_block (&misc);


  misc.filter_kill_list.clear ();
  misc.filter_kill_count = 0;


  //  Clear the marker points.

  map->clearMarkerPoints ();


  //  Since this is a mode we don't want to close the LiDAR Change Classification Tab, so we'll just redraw.  We also need to make sure we
  //  stay in this mode in case we were using either the CLEAR_POLYGON function or one of the FILTER_MASK functions.

  options.function = CHANGE_LIDAR_CLASS;
  setFunctionCursor (options.function);

  redrawMap (NVTrue);
}



void
pfmEdit3D::slotLidarClassChangeDiscard ()
{
  misc.filter_kill_list.clear ();
  misc.filter_kill_count = 0;


  //  Clear the marker points.

  map->clearMarkerPoints ();


  //  Since this is a mode we don't want to close the LiDAR Change Classification Tab, so we'll just redraw.  We also need to make sure we
  //  stay in this mode in case we were using either the CLEAR_POLYGON function or one of the FILTER_MASK functions.

  options.function = CHANGE_LIDAR_CLASS;
  setFunctionCursor (options.function);

  redrawMap (NVTrue);
}



//  This is a generic close slot used for tabs that are persistent.  At the moment that is only the tabs for CPF_FILTER and CHANGE_LIDAR_CLASS.

void 
pfmEdit3D::slotTabClose ()
{
  uint8_t redraw_flag = NVFalse;


  notebook->setTabEnabled (TRACKTAB, true);
  notebook->setTabEnabled (HISTTAB, true);

  if (options.track_tab)
    {
      notebook->setCurrentIndex (TRACKTAB);
    }
  else
    {
      notebook->setCurrentIndex (HISTTAB);
    }

  notebook->setTabEnabled (AREATAB, false);
  notebook->setTabEnabled (STATTAB, false);
  notebook->setTabEnabled (HPTAB, false);
  notebook->setTabEnabled (ATTRTAB, false);
  notebook->setTabEnabled (CZMILURBANTAB, false);
  notebook->setTabEnabled (HOFTAB, false);
  notebook->setTabEnabled (CPFTAB, false);
  notebook->setTabEnabled (LIDARCLASSTAB, false);


  //  If we were validating in cpfDeepFilter and we just switched modes (instead of accepting or discarding) we have to set the validity back,
  //  clear the filter undo memory.

  if (options.function == CPF_FILTER)
    {
      if (!options.czmil_cpf_filter_inval)
        {
          for (int32_t i = 0 ; i < misc.filter_undo.count ; i++)
            {
              misc.data[misc.filter_undo.num[i]].val = misc.filter_undo.val[i];
            }

          redraw_flag = NVTrue;
        }

      misc.filter_undo.val.clear ();
      misc.filter_undo.num.clear ();
      misc.filter_undo.count = 0;
    }


  //  Clear the filter kill list if present

  if (misc.filter_kill_count)
    {
      misc.filter_kill_list.clear ();
      misc.filter_kill_count = 0;

      redraw_flag = NVTrue;
    }

  if (redraw_flag) redrawMap (NVFalse);


  //  This will switch the mode back to the last used edit mode (unless it was called from slotMode).

  if (call_mode) slotMode (misc.save_mode);
}



//!  Undo the last edit operation.

void 
pfmEdit3D::slotUndo ()
{
  undo (&misc);

  redrawMap (NVTrue);
}



//!  Kill ancillary programs.

uint8_t 
pfmEdit3D::killProg (int id)
{
  if (ancillaryProgram[id] && ancillaryProgram[id]->state () == QProcess::Running)
    {
      if (options.state[id])
        {
          misc.abeShare->lock ();
          misc.abe_share->key = options.kill_switch[id];
          misc.abeShare->unlock ();


          //  Give the ancillary process time enough to see the kill flag.

#ifdef NVWIN3X
          Sleep (1000);
#else
          sleep (1);
#endif


          misc.abe_share->key = 0;
          options.state[id] = 1;
        }


      //  Try to kill it even if we kill switched it.

      ancillaryProgram[id]->kill ();

      delete (ancillaryProgram[id]);
      ancillaryProgram[id] = NULL;

      return (NVTrue);
    }


  //  Check for the state set to 2.  This would mean that we started the editor and the program was already up and running.

  if (options.state[id] == 2)
    {
      misc.abeShare->lock ();
      misc.abe_share->key = options.kill_switch[id];
      misc.abeShare->unlock ();


      //  Give the ancillary process time enough to see the kill flag.

#ifdef NVWIN3X
      Sleep (1000);
#else
      sleep (1);
#endif


      misc.abe_share->key = 0;
      options.state[id] = 1;
    }

  return (NVFalse);
}



//!  Run ancillary programs (not in hotkey polygon mode).

void 
pfmEdit3D::runProg (int id)
{
  //  We want to toggle external GUI programs that are capable of kill and respawn.

  uint8_t killed = killProg (id);


  //  If the program is not a "kill/respawn" GUI or it is and it wasn't killed immediately
  //  prior to this, we want to start the program.

  if (!options.state[id] || (options.state[id] == 1 && !killed))
    {
      ancillaryProgram[id] = new ancillaryProg (this, map, &options, &misc, id, lock_point);
      connect (ancillaryProgram[id], SIGNAL (redrawSignal ()), this, SLOT (slotRedrawMap ()));
      connect (ancillaryProgram[id], SIGNAL (redrawSignal ()), this, SLOT (slotRedrawMap ()));
      connect (ancillaryProgram[id], SIGNAL (readStandardErrorSignal (QProcess *)), this, SLOT (slotReadStandardError (QProcess *)));


      //  Reset our state to 1 for respawn programs.  We're using the 2 to determine if it was
      //  "respawned" when we started.  We'll set it back to 2 on exit if we still have
      //  options.kill_and_respawn set.

      if (options.state[id]) options.state[id] = 1;
    }
}



//!  Mouse press signal from the 2D trackMap map class.

void 
pfmEdit3D::slotTrackMousePress (QMouseEvent *e, double lon __attribute__ ((unused)), double lat __attribute__ ((unused)))
{
  //  If we're drawing and a mouse button is pressed we want to quit drawing.

  if (misc.busy) 
    {
      misc.drawing_canceled = NVTrue;
      return;
    }  

  if (e->button () == Qt::LeftButton)
    {
      switch (options.function)
        {
          //  Placeholder
        }
    }

  if (e->button () == Qt::MidButton) {};
  if (e->button () == Qt::RightButton) {};
}



//!  Mouse release signal from the 2D trackMap map class.  Right now we don't really need this.

void 
pfmEdit3D::slotTrackMouseRelease (QMouseEvent *e, double lon __attribute__ ((unused)), double lat __attribute__ ((unused)))
{
  if (e->button () == Qt::LeftButton) {};
  if (e->button () == Qt::MidButton) {};
  if (e->button () == Qt::RightButton) {};
}



//!  Mouse move signal from the 2D trackMap map class.

void
pfmEdit3D::slotTrackMouseMove (QMouseEvent *e __attribute__ ((unused)), double lon __attribute__ ((unused)), double lat __attribute__ ((unused)))
{
  char                      ltstring[25], lnstring[25], hem;
  double                    deg, min, sec;
  NV_F64_COORD2             xy;
  QString                   y_string, x_string;
  BIN_RECORD                bin;
  double                    dist;
  static int32_t            prev_nearest_point = -1;


  uint8_t compare_to_stack (int32_t current_point, double dist, MISC *misc, double z);


  //  If it's still drawing don't do anything

  if (misc.busy) return;


  //  Let other ABE programs know which window we're in.  This is a special case since we have two map windows here.  We're lying about the 
  //  active window ID (process ID) by subtracting 1 from this program's PID.  This makes trackCursor track it locally.

  misc.abe_share->active_window_id = misc.process_id - 1;


  //  Get rid of the main cursor so we don't have multiple cursors on the main screen.

  for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++) map->closeMovingList (&(multi_marker[i]));


  //  Track the cursor position for other ABE programs

  misc.abe_share->cursor_position.y = lat;
  misc.abe_share->cursor_position.x = lon;


  //  Try to find the highest layer with a valid value to display in the status bar.

  xy.y = lat;
  xy.x = lon;
  int32_t hit = -1;

  for (int32_t pfm = 0 ; pfm < misc.abe_share->pfm_count ; pfm++)
    {
      if (misc.abe_share->display_pfm[pfm] && bin_inside_ptr (&misc.abe_share->open_args[pfm].head, xy))
        {
          read_bin_record_xy (misc.pfm_handle[pfm], xy, &bin);

          hit = pfm;
          break;
        }
    }


  QString rec_string = "", fil_string = "", pfm_string = "", lin_string = "", distance_string = "", h_string = "", v_string = "", lin_num_string = "", ltString, lnString;
  char nearest_line[512];


  switch (options.function)
    {
    case DELETE_POINT:
    case UNSET_SINGLE:
    case SET_MULTIPLE:
    case ADD_FEATURE:
    case EDIT_FEATURE:
    case MOVE_FEATURE:
    case DELETE_FEATURE:
    case MEASURE:

      //  We only change the status PFM values if we were actually over a PFM data area

      if (hit >= 0) pfmLabel->setText (QFileInfo (misc.abe_share->open_args[hit].list_path).fileName ().remove (".pfm"));


      //  Clear the nearest point stack.

      for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++)
        {
          misc.nearest_stack.point[i] = -1;
          misc.nearest_stack.dist[i] = 9999999999.0;
        }


      misc.nearest_point = -1;


      for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
        {
          //  Check for single line display.

          if (!misc.num_lines || check_line (&misc, misc.data[i].line))
            {
              //  Do not use null points.  Do not use invalid points unless the display_man_invalid, display_flt_invalid, or
              //  display_null flag is set.  Do not use masked points. Do not check points that are not on the display.

              if (!check_bounds (&options, &misc, i, NVTrue, misc.slice))
                {
                  dist = sqrt ((double) ((lat - misc.data[i].y) * (lat - misc.data[i].y)) +
                               (double) ((lon - misc.data[i].x) * (lon - misc.data[i].x)));


                  //  Check the points against the points in the nearest points stack.

                  if (compare_to_stack (i, dist, &misc, 0.0))
                    {
                      //  If the return was true then this is the minimum distance so far.

                      misc.nearest_point = misc.nearest_stack.point[0];
                    }
                }
            }
        }


      //  Update the status bars

      // 
      //                      if our point has moved or we have just selected a new overplot we will go
      //                      into this code block.
      //

      if (misc.nearest_point != -1 && misc.nearest_point != prev_nearest_point)
        {
          //  Locking shared memory.

          misc.abeShare->lock ();


          //  External programs monitor abe_share->nearest_point to trigger updates of their windows.  They use
          //  abe_share->mwShare.multiRecord[0] for the current record to display.  In this way we can lock the
          //  main screen by not updating abe_share->mwShare.multiRecord[0] and still trigger the external programs
          //  to update (such as for multiple waveforms in CZMILwaveMonitor or waveWaterfall).

          int16_t type;
          read_list_file (misc.pfm_handle[misc.data[misc.nearest_point].pfm], misc.data[misc.nearest_point].file, misc.abe_share->nearest_filename, &type);

          strcpy (nearest_line, read_line_file (misc.pfm_handle[misc.data[misc.nearest_point].pfm], misc.data[misc.nearest_point].line % SHARED_LINE_MULT));


          //  Push the nearest stack points into the ABE shared memory for use by CZMILwaveMonitor, waveWaterfall,
          //  and lidarMonitor.

          for (int32_t j = 0 ; j < MAX_STACK_POINTS ; j++)
            {
              //  Normal, unfrozen mode.

              if (!misc.marker_mode)
                {
                  misc.abe_share->mwShare.multiSubrecord[j] = misc.data[misc.nearest_stack.point[j]].sub;
                  misc.abe_share->mwShare.multiPfm[j] = misc.data[misc.nearest_stack.point[j]].pfm;
                  misc.abe_share->mwShare.multiFile[j] = misc.data[misc.nearest_stack.point[j]].file;
                  misc.abe_share->mwShare.multiLine[j] = misc.data[misc.nearest_stack.point[j]].line % SHARED_LINE_MULT;
                  misc.abe_share->mwShare.multiPoint[j].x = misc.data[misc.nearest_stack.point[j]].x;
                  misc.abe_share->mwShare.multiPoint[j].y = misc.data[misc.nearest_stack.point[j]].y;
                  misc.abe_share->mwShare.multiPoint[j].z = misc.data[misc.nearest_stack.point[j]].z;
                  misc.abe_share->mwShare.multiLocation[j] = misc.nearest_stack.point[j];
                  misc.abe_share->mwShare.multiType[j] = misc.data[misc.nearest_stack.point[j]].type;
                  misc.abe_share->mwShare.multiPresent[j] = misc.data[misc.nearest_stack.point[j]].rec;
                  misc.abe_share->mwShare.multiRecord[j] = misc.data[misc.nearest_stack.point[j]].rec;
                }


              //  If we're frozen, we don't want to replace the [0] record and we want to push the nearest
              //  MAX_STACK_POINTS - 1 records up one level.

              else
                {
                  if (j)
                    {
                      misc.abe_share->mwShare.multiSubrecord[j] = misc.data[misc.nearest_stack.point[j - 1]].sub;
                      misc.abe_share->mwShare.multiPfm[j] = misc.data[misc.nearest_stack.point[j - 1]].pfm;
                      misc.abe_share->mwShare.multiFile[j] = misc.data[misc.nearest_stack.point[j - 1]].file;
                      misc.abe_share->mwShare.multiLine[j] = misc.data[misc.nearest_stack.point[j - 1]].line % SHARED_LINE_MULT;
                      misc.abe_share->mwShare.multiPoint[j].x = misc.data[misc.nearest_stack.point[j - 1]].x;
                      misc.abe_share->mwShare.multiPoint[j].y = misc.data[misc.nearest_stack.point[j - 1]].y;
                      misc.abe_share->mwShare.multiPoint[j].z = misc.data[misc.nearest_stack.point[j - 1]].z;
                      misc.abe_share->mwShare.multiLocation[j] = misc.nearest_stack.point[j - 1];
                      misc.abe_share->mwShare.multiType[j] = misc.data[misc.nearest_stack.point[j - 1]].type;
                      misc.abe_share->mwShare.multiPresent[j] = misc.data[misc.nearest_stack.point[j - 1]].rec;
                      misc.abe_share->mwShare.multiRecord[j] = misc.data[misc.nearest_stack.point[j - 1]].rec;
                    }
                }
            }
          misc.abe_share->mwShare.multiNum = MAX_STACK_POINTS;


          //  Trigger some of the external applications to update.

          misc.abe_share->nearest_point = misc.nearest_point;


          //  Unlock shared memory.

          misc.abeShare->unlock ();


          //  If we're working with features display the position of the feature instead of the nearest point.

          if ((options.function == EDIT_FEATURE || options.function == DELETE_FEATURE ||
               (options.function == MOVE_FEATURE && !move_feature)) && options.display_feature && 
              misc.bfd_header.number_of_records)
            {
              geo_xyz_label (misc.feature[misc.nearest_feature].latitude,  misc.feature[misc.nearest_feature].longitude,
                             misc.feature[misc.nearest_feature].depth);
            }
          else
            {
              geo_xyz_label (misc.data[misc.nearest_point].y,  misc.data[misc.nearest_point].x, misc.data[misc.nearest_point].z);
            }


          //  If we are frozen multiRecord[0] is useless to us, so we must check and use nearest point

          if (!misc.marker_mode)
            {
              rec_string = QString ("%1/%2").arg (misc.abe_share->mwShare.multiRecord[0]).arg (misc.abe_share->mwShare.multiSubrecord[0]);
              lin_num_string = QString ("%1").arg (misc.abe_share->mwShare.multiLine[0]);

              pfm_string = QFileInfo (QString (misc.abe_share->open_args[misc.abe_share->mwShare.multiPfm[0]].list_path)).fileName ().remove (".pfm");
              pfmLabel->setToolTip (QString (misc.abe_share->open_args[misc.abe_share->mwShare.multiPfm[0]].list_path));
            }
          else
            {
              rec_string = QString ("%1/%2").arg (misc.data[misc.nearest_point].rec).arg (misc.data[misc.nearest_point].sub);
              lin_num_string = QString ("%1").arg (misc.data[misc.nearest_point].line);

              pfm_string = QFileInfo (QString (misc.abe_share->open_args[misc.data[misc.nearest_point].pfm].list_path)).fileName().remove(".pfm");
              pfmLabel->setToolTip (QString (misc.abe_share->open_args[misc.data[misc.nearest_point].pfm].list_path));
            }

          fil_string = QString (pfm_basename (misc.abe_share->nearest_filename));
          filLabel->setToolTip (QString (misc.abe_share->nearest_filename));
          lin_string = QString (nearest_line);


          recLabel->setText (rec_string);
          attrValue[1]->setText (lin_num_string);
          filLabel->setText (fil_string);
          pfmLabel->setText (pfm_string);
          linLabel->setText (lin_string);

          h_string = QString ("%L1").arg (misc.data[misc.nearest_point].herr, 0, 'f', 2);
          attrValue[2]->setText (h_string);

          v_string = QString ("%L1").arg (misc.data[misc.nearest_point].verr, 0, 'f', 2);
          attrValue[3]->setText (v_string);


          int32_t pfm = misc.data[misc.nearest_point].pfm;


          for (int32_t i = 0 ; i < POST_ATTR ; i++)
            {
              QString attr_string;

              int32_t j = misc.attrStatNum[i];

              if (j == misc.time_attr)
                {
                  if (misc.data[misc.nearest_point].attr[j] > ABE_NULL_TIME)
                    {
                      int32_t year, jday, hour, minute;
                      float second;
                      time_t tv_sec = NINT (misc.data[misc.nearest_point].attr[j] * 60.0);

                      cvtime (tv_sec, 0, &year, &jday, &hour, &minute, &second);
                      attr_string = QString ("%1-%2 %3:%4").arg (year + 1900).arg (jday, 3, 10, QChar('0')).arg (hour, 2, 10, QChar('0')).arg (minute, 2, 10, QChar('0'));
                    }
                  else
                    {
                      attr_string = "N/A";
                    }
                }
              else
                {
                  attr_string.sprintf (misc.attr_format[pfm][j], misc.data[misc.nearest_point].attr[j]);
                }

              attrValue[i + PRE_ATTR]->setText (attr_string);
            }
        }
      break;
    }


  prev_nearest_point = misc.nearest_point;


  strcpy (ltstring, fixpos (lat, &deg, &min, &sec, &hem, POS_LAT, options.position_form));
  strcpy (lnstring, fixpos (lon, &deg, &min, &sec, &hem, POS_LON, options.position_form));

  if (options.lat_or_east)
    {
      double x = lon * NV_DEG_TO_RAD;
      double y = lat * NV_DEG_TO_RAD;

      pj_transform (misc.pj_latlon, misc.pj_utm, 1, 1, &x, &y, NULL);

      strcpy (ltstring, QString ("%L1").arg (y, 0, 'f', 2).toLatin1 ());
      strcpy (lnstring, QString ("%L1").arg (x, 0, 'f', 2).toLatin1 ());
    }

  latLabel->setText (ltstring);
  lonLabel->setText (lnstring);
}



//!  Resize signal from the 2D trackMap map class.

void
pfmEdit3D::slotTrackResize (QResizeEvent *e __attribute__ ((unused)))
{
}



/*!  Prior to drawing coastline signal from the 2D trackMap map class.  This is where we do our heavy lifting.  We do both the track map
     contours as well as the 3D contours (on the top surface of the scale) here if requested.  */

void 
pfmEdit3D::slotTrackPreRedraw (NVMAP_DEF l_mapdef __attribute__ ((unused)))
{
  void scribe (nvMap *map, OPTIONS *options, MISC *misc, float *ar, int32_t ncc, int32_t nrr, double xorig, double yorig);
  void scribe3D (nvMapGL *map, OPTIONS *options, MISC *misc, float *ar, int32_t ncc, int32_t nrr, double xorig, double yorig);


  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  uint8_t small_enough = NVTrue;
  uint8_t big_enough = NVTrue;


  int32_t grid_height = NINT ((misc.displayed_area.max_y - misc.displayed_area.min_y) / misc.y_grid_size);
  int32_t grid_width = NINT ((misc.displayed_area.max_x - misc.displayed_area.min_x) / misc.x_grid_size);


  //  Compute the adjusted min y and min x for the grid indexing.  This makes the grid an 
  //  exact number of grid spaces wide and high.

  double adj_rymin = misc.displayed_area.max_y - ((float) grid_height * misc.y_grid_size);
  double adj_rxmin = misc.displayed_area.max_x - ((float) grid_width * misc.x_grid_size);


  //  If the area to be binned is too big or small, flag it.

  if (grid_width * grid_height > 1000000) small_enough = NVFalse;
  if (grid_width * grid_height <= 0) big_enough = NVFalse;


  float *grid = NULL;
  uint16_t *count = NULL;
  int32_t length;
  float numshades = (float) (NUMSHADES - 1);

  if (options.display_contours && small_enough && big_enough)
    {
      grid = (float *) malloc (sizeof (float) * grid_height * grid_width);

      if (grid == NULL)
        {
          fprintf (stderr, "%s %s %s %d - grid - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
          exit (-1);
        }


      count = (uint16_t *) malloc (sizeof (uint16_t) * grid_height * grid_width);

      if (count == NULL)
        {
          fprintf (stderr, "%s %s %s %d - count - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
          exit (-1);
        }

      for (int32_t i = 0 ; i < grid_height * grid_width ; i++)
        {
          count[i] = 0;
          grid[i] = CHRTRNULL;
        }
    }


  for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
    {
      int32_t xyz_x, xyz_y, xyz_z;
      double dummy;
      if (!(misc.data[i].val & (PFM_DELETED | PFM_INVAL)))
        {
          //  Check for single line display.

          if (!misc.num_lines || check_line (&misc, misc.data[i].line))
            {
              //  Check against the viewing area.

              int32_t trans = 0;
              if ((trans = check_bounds (&options, &misc, misc.data[i].x, misc.data[i].y, misc.data[i].z, misc.data[i].val, NVFalse, misc.data[i].pfm,
                                         NVTrue, NVFalse)) < 2)
                {
                  int32_t c_index = NINT (numshades - ((-misc.data[i].z) - misc.color_min_z) / misc.color_range_z * numshades);


                  //  Check for out of range data.

                  if (c_index < 0) c_index = 0;
                  if (c_index > NUMSHADES - 1) c_index = NUMSHADES - 1;


                  QColor tmp = misc.track_color_array[c_index];


                  //  Don't do this part if the track map isn't being displayed.

                  if (options.track_tab)
                    {
                      trackMap->map_to_screen (1, &misc.data[i].x, &misc.data[i].y, &dummy, &xyz_x, &xyz_y, &xyz_z);
                      trackMap->fillRectangle (xyz_x, xyz_y, 2, 2, tmp, NVFalse);
                    }


                  if (big_enough && small_enough && options.display_contours)
                    {
                      //  Load the grid (figure out which bin the point is in).

                      c_index = NINT ((int32_t) ((misc.data[i].y - adj_rymin) / misc.y_grid_size) * grid_width + (int32_t) ((misc.data[i].x - adj_rxmin) /
                                                                                                                            misc.x_grid_size));


                      //  Make sure there is no memory overrun.

                      if (c_index < grid_width * grid_height)
                        {
                          if (!count[c_index]) grid[c_index] = 0.0;
                          grid[c_index] = (grid[c_index] * (float) count[c_index] + (misc.data[i].z * options.z_factor + options.z_offset)) /
                            ((float) count[c_index] + 1.0);
                          count[c_index]++;
                        }
                    }
                }
            }
        }
    }


  //  Plot contours on 2D tracker if requested.

  if (big_enough && small_enough && options.display_contours)
    {
      float *spline_array_x_in = (float *) malloc (sizeof (float) * grid_width);
      float *spline_array_y_in = (float *) malloc (sizeof (float) * grid_width);
      float *spline_array_x_out = (float *) malloc (sizeof (float) * grid_width);
      float *spline_array_y_out = (float *) malloc (sizeof (float) * grid_width);
      if (spline_array_x_in == NULL || spline_array_y_in == NULL ||
          spline_array_x_out == NULL || spline_array_y_out == NULL)
        {
          fprintf (stderr, "%s %s %s %d - spline - %s\n", misc.progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
          exit (-1);
        }


      //  Fill the spline arrays

      int32_t k;
      for (int32_t i = 0 ; i < grid_height ; i++)
        {
          k = 0;
          for (int32_t j = 0 ; j < grid_width ; j++)
            {
              if (count[i * grid_width + j])
                {
                  //  Make sure there is no memory overrun.

                  if (k < grid_width)
                    {
                      spline_array_x_in[k] = (float) j;
                      spline_array_y_in[k] = grid[i * grid_width + j];
                      k++;
                    }
                }
            }


          //  Do not interpolate lines with fewer than 3 points so the slope doesn't go wacko.
                        
          if (k > 2)
            {
              interpolate (1.0, k, 0.0, grid_width - 1, &length, spline_array_x_in, spline_array_y_in, spline_array_x_out, spline_array_y_out);


              //  Set all points that do not have a 'real' neighbor within 3 bins to be CHRTRNULL.  
              //  This way they won't be contoured.

              uint8_t good_flag;
              for (int32_t j = 0 ; j < length ; j++)
                {
                  good_flag = NVFalse;
                  for (int32_t m = 0 ; m < k ; m++)
                    {
                      if (ABS (j - spline_array_x_in[m]) < 3)
                        {
                          good_flag = NVTrue;
                          break;
                        }
                    }

                  if (good_flag)
                    {
                      grid[i * grid_width + j] = spline_array_y_out[j];
                    }
                  else
                    {
                      grid[i * grid_width + j] = CHRTRNULL;
                    }
                }
            }
        }


      //  Contour it - only do the track map if it's being displayed.  If it's not being displayed, we also need to restore the
      //  override cursor in that case since slotTrackPostRedraw won't be called.

      scribe3D (map, &options, &misc, grid, grid_width, grid_height, adj_rxmin, adj_rymin);
      if (options.track_tab)
        {
          scribe (trackMap, &options, &misc, grid, grid_width, grid_height, adj_rxmin, adj_rymin);
          trackMap->flush ();
        }
      else
        {
          qApp->restoreOverrideCursor ();
        }


      //  Free all of the memory.

      if (grid != NULL) free (grid);
      if (count != NULL) free (count);
      if (spline_array_x_in != NULL) free (spline_array_x_in);
      if (spline_array_y_in != NULL) free (spline_array_y_in);
      if (spline_array_x_out != NULL) free (spline_array_x_out);
      if (spline_array_y_out != NULL) free (spline_array_y_out);
    }
}



//!  Post grid drawing signal from 2D trackMap map class.  Mostly just cleanup work.

void 
pfmEdit3D::slotTrackPostRedraw (NVMAP_DEF mapdef __attribute__ ((unused)))
{
  qApp->restoreOverrideCursor ();
}



//!  Mouse move signal from the Histogram map class.

void
pfmEdit3D::slotHistogramMouseMove (QMouseEvent *e __attribute__ ((unused)), double x, double y __attribute__ ((unused)))
{
  //  If it's still drawing don't do anything

  if (misc.busy) return;


  int32_t ndx = options.color_index - PRE_ATTR;


  //  Set the value name.

  QString str;
  switch (options.color_index)
    {
    case 0:
      str = tr ("Z value");
      break;

    case 1:
      str = tr ("Line");
      break;

    case 2:
      str = tr ("Horizontal uncertainty");
      break;

    case 3:
      str = tr ("Vertical uncertainty");
      break;

    default:
      str = QString (misc.abe_share->open_args[0].head.ndx_attr_name[misc.attrStatNum[ndx]]);
    }


  int32_t bar = -1;
  for (int32_t i = 0 ; i < NUM_HIST_BARS ; i++)
    {
      if (x >= hist_bar_mbr[i].min_x && x <= hist_bar_mbr[i].max_x)
        {
          bar = i;
          break;
        }
    }


  if (bar >= 0)
    {
      if (ndx == misc.time_attr)
        {
          QString startTime, endTime;

          int32_t year, jday, hour, minute;
          float second;

          time_t tv_sec = NINT (hist_bar_mbr[bar].min_x * 60.0);

          cvtime (tv_sec, 0, &year, &jday, &hour, &minute, &second);
          startTime = QString ("%1-%2 %3:%4").arg (year + 1900).arg (jday, 3, 10, QChar('0')).arg (hour, 2, 10, QChar('0')).arg (minute, 2, 10, QChar('0'));

          tv_sec = NINT (hist_bar_mbr[bar].max_x * 60.0);

          cvtime (tv_sec, 0, &year, &jday, &hour, &minute, &second);
          endTime = QString ("%1-%2 %3:%4").arg (year + 1900).arg (jday, 3, 10, QChar('0')).arg (hour, 2, 10, QChar('0')).arg (minute, 2, 10, QChar('0'));

          histogram->setToolTip (str + QString (": %1 to %2, Count = %3").arg (startTime).arg (endTime).arg (hist_bar_count[bar]));
        }
      else
        {
          if (options.color_index)
            {
              //  Check for integer attributes (not uncertainties though).

              if (options.color_index > 3 && misc.abe_share->open_args[0].head.ndx_attr_scale[misc.attrStatNum[ndx]] == 1.0)
                {
                  //  No range...

                  if (NINT (hist_bar_mbr[bar].min_x) == NINT (hist_bar_mbr[bar].max_x))
                    {
                      histogram->setToolTip (str + QString (": %1, Count = %3").arg (NINT (hist_bar_mbr[bar].min_x)).arg (hist_bar_count[bar]));
                    }
                  else
                    {
                      histogram->setToolTip (str + QString (": %1 to %2, Count = %3").arg (NINT (hist_bar_mbr[bar].min_x)).arg
                                             (NINT (hist_bar_mbr[bar].max_x)).arg (hist_bar_count[bar]));
                    }
                }
              else
                {
                  histogram->setToolTip (str + QString (": %1 to %2, Count = %3").arg (hist_bar_mbr[bar].min_x).arg
                                         (hist_bar_mbr[bar].max_x).arg (hist_bar_count[bar]));
                }
            }
          else
            {
              histogram->setToolTip (str + QString (": %1 to %2, Count = %3").arg (options.z_orientation * -hist_bar_mbr[bar].max_x).arg
                                     (options.z_orientation * -hist_bar_mbr[bar].min_x).arg (hist_bar_count[bar]));
            }
        }
    }
  else
    {
      histogram->setToolTip ("");
    }
}



//!  Resize signal from the Histogram map class.

void
pfmEdit3D::slotHistogramResize (QResizeEvent *e __attribute__ ((unused)))
{
}



//!  Pre-coastline (not applicable in this case) signal from the Histogram map class.  This is where we do our heavy lifting.

void 
pfmEdit3D::slotHistogramPreRedraw (NVMAP_DEF l_mapdef __attribute__ ((unused)))
{
  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  double dnum_hist_bars = ((double) NUM_HIST_BARS);


  //  Note that we're using attr_min and attr_max even for depth.  This is because redrawMap adds 10% to the Z min and max to allow
  //  room in the 3D space.  In compute_bounds, if we're coloring by depth we save the actual Z min and max in the attr_min and attr_max
  //  variables since they aren't used for anything else in that case.

  int32_t attr_ndx = misc.attrStatNum[options.color_index - PRE_ATTR];
  double start = misc.attr_min;
  double end = misc.attr_max;
  double range = misc.attr_max - misc.attr_min;


  //  Check for no range or color by line.

  if (fabs (range) < 0.0000001 || range < 0.0 || options.color_index == 1) return;


  for (int32_t i = 0 ; i < NUM_HIST_BARS ; i++) hist_bar_count[i] = 0;


  for (int32_t i = 0 ; i < misc.abe_share->point_cloud_count ; i++)
    {
      if (!(misc.data[i].val & (PFM_DELETED | PFM_REFERENCE)))
        {
          //  Check for single line display.

          if (!misc.num_lines || check_line (&misc, misc.data[i].line))
            {
              //  Check against the viewing area.

              int32_t trans = 0;
              if ((trans = check_bounds (&options, &misc, misc.data[i].x, misc.data[i].y, misc.data[i].z, misc.data[i].val, NVFalse,
                                         misc.data[i].pfm, NVTrue, NVFalse)) < 2)
                {
                  //  If we're displaying invalid data and we have the invalid_edit_color flag set, we don't want to use the invalid points
                  //  in the computations.

                  if (!options.invalid_edit_color || !(misc.data[i].val & PFM_INVAL))
                    {
                      int32_t bar_ndx = -1;

                      switch (options.color_index)
                        {
                        case 0:
                          bar_ndx = NINT ((-(misc.data[i].z) - start) / range * dnum_hist_bars);
                          break;

                        case 1:
                          bar_ndx = NINT ((misc.data[i].line - start) / range * dnum_hist_bars);
                          break;

                        case 2:
                          bar_ndx = NINT ((misc.data[i].herr - start) / range * dnum_hist_bars);
                          break;

                        case 3:
                          bar_ndx = NINT ((misc.data[i].verr - start) / range * dnum_hist_bars);
                          break;

                        default:
                          bar_ndx = NINT ((misc.data[i].attr[attr_ndx] - start) / range * dnum_hist_bars);
                          break;
                        }


                      //  For the case where the value is exactly the same as the max value we want to put the count in the last bin.

                      if (bar_ndx == NUM_HIST_BARS) bar_ndx--;

                      hist_bar_count[bar_ndx]++;
                    }
                }
            }
        }
    }

  NV_F64_XYMBR bounds;

  bounds.max_y = -999999999.0;

  for (int32_t i = 0 ; i < NUM_HIST_BARS ; i++) bounds.max_y = qMax ((double) hist_bar_count[i], bounds.max_y);

  bounds.min_x = start;
  bounds.max_x = end;
  bounds.min_y = 0.0;

  double range_x = bounds.max_x - bounds.min_x;
  double range_y = bounds.max_y - bounds.min_y;
  double orig_min_x = bounds.min_x;


  double interval = range_x / dnum_hist_bars;


  //  Subtract 2% from min X and Y.

  bounds.min_x = bounds.min_x - (range_x * 0.02);
  bounds.min_y = bounds.min_y - (range_y * 0.02);


  //  Add 2% to max X and Y.

  bounds.max_x = bounds.max_x + (range_x * 0.02);
  bounds.max_y = bounds.max_y + (range_y * 0.02);


  //  Reset the range for computing bar location.

  range_x = bounds.max_x - bounds.min_x;


  histogram->resetBounds (bounds);


  //  Set the bar colors for the current range.

  double numshades = ((double) (NUMSHADES - 1));


  double min_z = misc.attr_min;
  double max_z = misc.attr_max;

  double z_range = max_z - min_z;


  //  Make sure we don't divide by 0.0

  if (fabs (z_range) < 0.0000001) z_range = 1.0;

  double inc = z_range / (dnum_hist_bars - 1.0);


  for (int32_t i = 0 ; i < NUM_HIST_BARS ; i++)
    {
      //  Compute and save the bar bounds.

      double x[2], y[2];
      x[0] = hist_bar_mbr[i].min_x = orig_min_x + (double) i * interval;
      x[1] = hist_bar_mbr[i].max_x = x[0] + interval;
      y[0] = hist_bar_mbr[i].min_y = 0.0;
      y[1] = hist_bar_mbr[i].max_y = (double) hist_bar_count[i];


      double value = min_z + (double) i * inc;

      int32_t c_index = 0;


      //  Color by Z

      if (!options.color_index)
        {
          c_index = NINT (numshades - ((value) - misc.color_min_z) / misc.color_range_z * numshades);
        }


      //  Color by line number, uncertainty, or attribute

      else
        {
          c_index = NINT (numshades - (value - misc.attr_color_min) / misc.attr_color_range * numshades);
        }


      //  Check for out of range values (this is the easy way to work the locked color scale).

      if (c_index < 0) c_index = 0;
      if (c_index > NUMSHADES - 1) c_index = NUMSHADES - 1;


      QColor clr;
      clr.setRed (misc.color_array[c_index][0]);
      clr.setGreen (misc.color_array[c_index][1]);
      clr.setBlue (misc.color_array[c_index][2]);
      clr.setAlpha (misc.color_array[c_index][3]);


      histogram->drawRectangle (x[0], y[0], x[1], y[1], Qt::gray, 2, Qt::SolidLine, NVFalse);
      histogram->fillRectangle (x[0], y[0], x[1], y[1], clr, NVTrue);
    }

  histogram->flush ();
}



//!  Post grid drawing signal from Histogram map class.  Mostly just cleanup work.

void 
pfmEdit3D::slotHistogramPostRedraw (NVMAP_DEF mapdef __attribute__ ((unused)))
{
  qApp->restoreOverrideCursor ();
}



//!  Clean exit routine

void 
pfmEdit3D::clean_exit (int32_t ret)
{
  //  Let go of the shared memory.

  misc.dataShare->unlock ();
  misc.dataShare->detach ();
  misc.abeShare->detach ();

  exit (ret);
}



void
pfmEdit3D::about ()
{
  QMessageBox::about (this, misc.program_version, pfmEdit3DAboutText);
}



void
pfmEdit3D::slotAcknowledgments ()
{
  QMessageBox::about (this, misc.program_version, acknowledgmentsText);
}



void
pfmEdit3D::aboutQt ()
{
  QMessageBox::aboutQt (this, misc.program_version);
}



void
pfmEdit3D::setStatusAttributes ()
{
  //  Set the attributes for the current "top" PFM.

  for (int32_t i = 0 ; i < NUM_ATTR ; i++)
    {
      misc.attrBoxFlag[i] = 0;

      if (i < misc.abe_share->open_args[0].head.num_ndx_attr)
        {
          QString tmp;


          //  Special attribute settings for POSIX time.

          if (QString (misc.abe_share->open_args[0].head.ndx_attr_name[i]).contains ("Time (POSIX minutes)"))
            {
              tmp = QString (tr ("Date/Time (minutes)"));

              attrBoxName[i]->setText (tmp);
              attrBoxValue[i]->setToolTip (tr ("This is Date/Time (minutes) at the cursor"));

              misc.time_attr = i;
            }
          else
            {
              tmp = QString (pfm_standard_attr_name (1, 0, misc.abe_share->open_args[0].head.ndx_attr_name[i]));

              attrBoxName[i]->setText (tmp);
              attrBoxValue[i]->setToolTip (tr ("This is the value of %1 at the cursor").arg (tmp));
            }

          attrBoxName[i]->setToolTip (tmp +
                                      QString (tr (" - Click to place this attribute in the first attribute slot on the Status tab.")));


          if (i == misc.czmil_urban_attr)
            {
              attrBoxName[i]->setWhatsThis (CZMILUrbanNoiseFlagsText);
            }
          else
            {
              attrBoxName[i]->setWhatsThis (tmp +
                                            QString (tr ("<br><br>Click to place this attribute in the first attribute slot on the "
                                                         "<b>Status</b> tab.  Cyan fade indicates the attribute is already on the "
                                                         "<b>Status</b> tab.  Red fade indicates that it is on the <b>Status</b> tab "
                                                         "and that the surface is colored by this attribute.")));
            }

          attrBoxName[i]->show ();
          attrBoxValue[i]->show ();
        }
      else
        {
          attrBoxName[i]->setText ("");

          attrBoxValue[i]->setText ("");

          attrBoxName[i]->hide ();
          attrBoxValue[i]->hide ();
        }
    }


  //  Figure out which attributes that were in the Status tab attributes we still have.

  int32_t atcnt = 0;

  for (int32_t i = 0 ; i < POST_ATTR ; i++)
    {
      if (options.attrStatName[i] != "")
        {
          misc.attrStatNum[i] = -1;

          for (int32_t j = 0 ; j < misc.abe_share->open_args[0].head.num_ndx_attr ; j++)
            {
              QString tmp = QString (pfm_standard_attr_name (1, 0, misc.abe_share->open_args[0].head.ndx_attr_name[j]));

              if (options.attrStatName[i] == tmp)
                {
                  misc.attrBoxFlag[j] = 1;
                  misc.attrStatNum[atcnt] = j;

                  if (j == misc.time_attr)
                    {
                      attrName[atcnt + PRE_ATTR]->setText (tr ("Date/Time (minutes)"));
                    }
                  else
                    {
                      attrName[atcnt + PRE_ATTR]->setText (tmp);
                    }
                  atcnt++;
                  break;
                }
            }


          //  If we didn't get a match, clear the string.

          if (misc.attrStatNum[i] < 0) options.attrStatName[i] = "";
        }
    }
}
