
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



#include "hotkeyPolygon.hpp"


//!  This is the sort function for qsort.

static int32_t compare_line_numbers (const void *a, const void *b)
{
    SHARED_FILE_STRUCT *sa = (SHARED_FILE_STRUCT *)(a);
    SHARED_FILE_STRUCT *sb = (SHARED_FILE_STRUCT *)(b);

    return (sa->line < sb->line ? 0 : 1);
}



/*!
  Runs a process on the data that is inside the hotkey polygon defined by the user.  The process may be internal or
  an external ancillary program.
*/

hotkeyPolygon::hotkeyPolygon (QWidget *parent, nvMapGL *ma, OPTIONS *op, MISC *mi, int32_t prog, uint8_t *failed):
  QProcess (parent)
{
  int32_t buildCommand (QString progString, QString actionString, MISC *misc, int32_t nearest_point, QString *cmd, QStringList *args, int32_t kill_switch);

  map = ma;
  options = op;
  misc = mi;
  pa = parent;
  *failed = NVFalse;


  //  We have to convert to double so that the "inside" function will work.

  double *mx = (double *) malloc (misc->hotkey_poly_count * sizeof (double));

  if (mx == NULL)
    {
      fprintf (stderr, "%s %s %s %d - mx - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  double *my = (double *) malloc (misc->hotkey_poly_count * sizeof (double));

  if (my == NULL)
    {
      fprintf (stderr, "%s %s %s %d - my - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  for (int32_t i = 0 ; i < misc->hotkey_poly_count ; i++)
    {
      mx[i] = (double) misc->hotkey_poly_x[i];
      my[i] = (double) misc->hotkey_poly_y[i];
    }

  int32_t count = misc->hotkey_poly_count;


  //  Get rid of the hotkey polygon.

  misc->hotkey_poly_count = 0;
  free (misc->hotkey_poly_x);
  free (misc->hotkey_poly_y);
  misc->hotkey_poly_x = NULL;
  misc->hotkey_poly_y = NULL;



  int32_t px = -1, py = -1, inc;
  char tmp[512];
  int16_t type;
  uint8_t init2D = NVTrue;
  static std::vector<SHARED_FILE_STRUCT> shared_data;
  double min_x = 999.0, min_y = 999.0, max_x = -999.0, max_y = -999.0;


  //  Special case for invalidating features.

  if (prog == INVALIDATE_FEATURES)
    {
      misc->statusProgLabel->setText (tr ("Invalidating features..."));
      misc->statusProgLabel->setVisible (true);
      misc->statusProg->setRange (0, misc->bfd_header.number_of_records);
      misc->statusProg->setTextVisible (true);
      qApp->processEvents();


      for (uint32_t i = 0 ; i < misc->bfd_header.number_of_records ; i++)
        {
          misc->statusProg->setValue (i);
          qApp->processEvents ();

          if (misc->feature[i].confidence_level)
            {
              if (!check_bounds (options, misc, misc->feature[i].longitude, misc->feature[i].latitude, misc->feature[i].depth, PFM_USER,
                                 NVFalse, 0, NVFalse, misc->slice))
                {
                  //  Convert the X, Y, and Z value to a projected pixel position

                  map->get2DCoords (misc->feature[i].longitude, misc->feature[i].latitude, -misc->feature[i].depth, &px, &py, &init2D);


                  //  Now check the point against the polygon.

                  if (inside_polygon2 (mx, my, count, (double) px, (double) py))
                    {
                      BFDATA_RECORD bfd_record;
                      if (binaryFeatureData_read_record (misc->bfd_handle, i, &bfd_record) < 0)
                        {
                          QString msg = QString (binaryFeatureData_strerror ());
                          QMessageBox::warning (parent, tr ("Invalidate Feature"), tr ("Unable to read feature record\nReason: ") + msg);
                          break;
                        }


                      //  Zero out the confidence value

                      bfd_record.confidence_level = misc->feature[i].confidence_level = 0;


                      if (binaryFeatureData_write_record (misc->bfd_handle, i, &bfd_record, NULL, NULL) < 0)
                        {
                          QString msg = QString (binaryFeatureData_strerror ());
                          QMessageBox::warning (parent, tr ("Invalidate Feature"), tr ("Unable to update feature record\nReason: ") + msg);
                          break;
                        }

                      misc->feature_mod = NVTrue;
                    }
                }
            }
        }

      if (misc->bfd_open) binaryFeatureData_close_file (misc->bfd_handle);
      misc->bfd_open = NVFalse;

      if ((misc->bfd_handle = binaryFeatureData_open_file (misc->abe_share->open_args[0].target_path, &misc->bfd_header, BFDATA_UPDATE)) < 0)
        {
          QString msg = QString (binaryFeatureData_strerror ());
          QMessageBox::warning (parent, "pfmEdit3D", tr ("Unable to open feature file\nReason: %1").arg (msg));
          return;
        }

      if (binaryFeatureData_read_all_short_features (misc->bfd_handle, &misc->feature) < 0)
        {
          QString msg = QString (binaryFeatureData_strerror ());
          QMessageBox::warning (parent, "pfmEdit3D", tr ("Unable to read feature records\nReason: %1").arg (msg));
          binaryFeatureData_close_file (misc->bfd_handle);
          return;
        }
      else
        {
          misc->bfd_open = NVTrue;
        }


      //  This forces the main program to redraw.

      *failed = NVTrue;
    }
  else
    {
      misc->statusProgLabel->setText (tr ("Reading..."));
      misc->statusProgLabel->setVisible (true);
      misc->statusProg->setRange (0, misc->abe_share->point_cloud_count);
      misc->statusProg->setTextVisible (true);
      qApp->processEvents();


      //  Don't let the progress bar eat up all the time.

      inc = misc->abe_share->point_cloud_count / 5;
      if (!inc) inc = 1;


      shared_count = 0;

      for (int32_t j = 0 ; j < misc->abe_share->point_cloud_count ;  j++)
        {
          if (!(j % inc))
            {
              misc->statusProg->setValue (j);
              qApp->processEvents ();
            }


          //  If we are displaying and editing only a single line, only get those points that are in that 
          //  line.

          if (!misc->num_lines || check_line (misc, misc->data[j].line))
            {
              //  Check against the displayed minimum bounding rectangle.  This is all that is needed for 
              //  a rectangle and it cuts down on the computations for a polygon.

              if (!check_bounds (options, misc, j, NVTrue, misc->slice))
                {

                  //  Convert the X, Y, and Z value to a projected pixel position

                  map->get2DCoords (misc->data[j].x, misc->data[j].y, -misc->data[j].z, &px, &py, &init2D);


                  //  Now check the point against the polygon.

                  if (inside_polygon2 (mx, my, count, (double) px, (double) py))
                    {
                      //  We need to try to build a geographic MBR of the points that fall inside the polygon
                      //  in case we want to run an external program that needs one.  Right now (11/14/08) that
                      //  would only be tofWaterkill.  tofWaterKill is no more but we'll leave this here just in case (04/14/11).

                      min_x = qMin (min_x, misc->data[j].x);
                      min_y = qMin (min_y, misc->data[j].y);
                      max_x = qMax (max_x, misc->data[j].x);
                      max_y = qMax (max_y, misc->data[j].y);


                      //  Check the data type.

                      if (!misc->data[j].type || options->data_type[prog][misc->data[j].type])
                        {
                          try
                            {
                              shared_data.resize (shared_count + 1);
                            }
                          catch (std::bad_alloc&)
                            {
                              fprintf (stderr, "%s %s %s %d - shared_data - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
                              exit (-1);
                            }

                          shared_data[shared_count].point = j;
                          shared_data[shared_count].x = misc->data[j].x;
                          shared_data[shared_count].y = misc->data[j].y;
                          shared_data[shared_count].z = misc->data[j].z;
                          shared_data[shared_count].herr = misc->data[j].herr;
                          shared_data[shared_count].verr = misc->data[j].verr;
                          shared_data[shared_count].val = misc->data[j].val;
                          shared_data[shared_count].pfm = misc->data[j].pfm;
                          shared_data[shared_count].file = misc->data[j].file;
                          shared_data[shared_count].line = misc->data[j].line;
                          shared_data[shared_count].rec = misc->data[j].rec;
                          shared_data[shared_count].sub = misc->data[j].sub;
                          shared_data[shared_count].exflag = misc->data[j].exflag;

                          shared_count++;
                        }
                    }
                }
            }
        }


      misc->statusProg->reset ();
      misc->statusProgLabel->setVisible (false);
      misc->statusProg->setTextVisible (false);
      qApp->processEvents();


      if (shared_count)
        {
          //  Save the geographic minimum bounding rectangle for use in external ancillary programs.

          misc->abe_share->polygon_count = 4;
          misc->abe_share->polygon_x[0] = min_x;
          misc->abe_share->polygon_y[0] = min_y;
          misc->abe_share->polygon_x[1] = min_x;
          misc->abe_share->polygon_y[1] = max_y;
          misc->abe_share->polygon_x[2] = max_x;
          misc->abe_share->polygon_y[2] = max_y;
          misc->abe_share->polygon_x[3] = max_x;
          misc->abe_share->polygon_y[3] = min_y;


          qsort (shared_data.data (), shared_count, sizeof (SHARED_FILE_STRUCT), compare_line_numbers);

          FILE *fp;
          misc->shared_file = QDir::tempPath () + SEPARATOR + QString ("PFM_EDIT_3D_SHARED_FILE_%1.tmp").arg (getpid ());

          if ((fp = fopen (misc->shared_file.toLatin1 (), "w")) != NULL)
            {
              int32_t prev_f = -1;


              //  Don't let the progress bar eat up all the time.

              inc = shared_count / 5;
              if (!inc) inc = 1;


              misc->statusProgLabel->setText (tr ("Writing..."));
              misc->statusProgLabel->setVisible (true);
              misc->statusProg->setRange (0, shared_count);
              misc->statusProg->setTextVisible (true);
              qApp->processEvents();

              for (int32_t i = 0 ; i < shared_count ; i++)
                {
                  if (!(i % inc))
                    {
                      misc->statusProg->setValue (i);
                      qApp->processEvents ();
                    }


                  if (!i || prev_f != shared_data[i].file)
                    {
                      if (i) fprintf (fp, "EOD\n");

                      read_list_file (misc->pfm_handle[shared_data[i].pfm], shared_data[i].file, tmp, &type);

                      fprintf (fp, "%s\n", tmp);

                      prev_f = shared_data[i].file;
                    }

                  fprintf (fp, "%d %d %0.9f %0.9f %0.3f %0.3f %0.3f %d %d %d %d %d %d %d\n", shared_data[i].point, i, shared_data[i].x, 
                           shared_data[i].y, shared_data[i].z, shared_data[i].herr, shared_data[i].verr, shared_data[i].val, shared_data[i].pfm,
                           shared_data[i].file, shared_data[i].line, shared_data[i].rec, shared_data[i].sub, shared_data[i].exflag);
                }

              fprintf (fp, "EOF\n");
              fclose (fp);


              misc->statusProg->reset ();
              misc->statusProgLabel->setVisible (false);
              misc->statusProg->setTextVisible (false);
              qApp->processEvents();


              misc->statusProgLabel->setText (tr ("Running ") + options->name[prog]);
              misc->statusProgLabel->setVisible (true);
              misc->statusProg->setRange (0, 0);
              misc->statusProg->setTextVisible (true);
              qApp->processEvents();


              //  We need to strip out [INPUT_FILE] and [RECORD] and replace them with the shared file name and 
              //  --shared_file option.

              QString progString = options->prog[prog];

              progString.remove (QString ("[INPUT_FILE]"));
              progString.remove (QString ("[RECORD]"));


              //  Append the shared file stuff.

              progString.append (" --shared_file ");
              progString.append (misc->shared_file);


              QString actionString = options->action[prog];
              buildCommand (progString, actionString, misc, -999, &cmd, &args, -1);


              connect (this, SIGNAL (finished (int, QProcess::ExitStatus)), this, SLOT (slotDone (int, QProcess::ExitStatus)));
              connect (this, SIGNAL (readyReadStandardError ()), this, SLOT (slotReadyReadStandardError ()));
              connect (this, SIGNAL (readyReadStandardOutput ()), this, SLOT (slotReadyReadStandardOutput ()));
              connect (this, SIGNAL (error (QProcess::ProcessError)), this, SLOT (slotError (QProcess::ProcessError)));

              this->start (cmd, args);
            }
          else
            {
              QString msg = tr ("Unable to open shared data file \n%1\nReason: %2").arg (QFileInfo (misc->shared_file).absolutePath ()).arg (strerror (errno));
              QMessageBox::warning (parent, tr ("Ancillary Program"), msg);

              *failed = NVTrue;
            }


          shared_data.clear ();
        }
      else
        {
          QString msg = tr ("No points found that match ancillary program data type for:\n") + options->name[prog];
          QMessageBox::warning (parent, tr ("pfmEdit3D Ancillary Program"), msg);

          *failed = NVTrue;
        }
    }


  free (mx);
  free (my);
}



hotkeyPolygon::~hotkeyPolygon ()
{
}



void 
hotkeyPolygon::slotError (QProcess::ProcessError error)
{
  QString commandLine = cmd;
  for (int32_t i = 0 ; i < args.size () ; i++) commandLine += (" " + args.at (i));

  switch (error)
    {
    case QProcess::FailedToStart:
      QMessageBox::critical (pa, tr ("pfmEdit3D polygon program"), tr ("Unable to start the polygon program!") + 
                             "\n" + commandLine);
      break;

    case QProcess::Crashed:
      QMessageBox::critical (pa, tr ("pfmEdit3D polygon program"), tr ("The polygon program crashed!") + 
                             "\n" + commandLine);
      break;

    case QProcess::Timedout:
      QMessageBox::critical (pa, tr ("pfmEdit3D polygon program"), tr ("The polygon program timed out!") + 
                             "\n" + commandLine);
      break;

    case QProcess::WriteError:
      QMessageBox::critical (pa, tr ("pfmEdit3D polygon program"), 
                             tr ("There was a write error to the polygon program!") + "\n" + commandLine);
      break;

    case QProcess::ReadError:
      QMessageBox::critical (pa, tr ("pfmEdit3D polygon program"),
                             tr ("There was a read error from the polygon program!") + "\n" + commandLine);
      break;

    case QProcess::UnknownError:
      QMessageBox::critical (pa, tr ("pfmEdit3D polygon program"), 
                             tr ("The polygon program died with an unknown error!") + "\n" + commandLine);
      break;
    }
}



void 
hotkeyPolygon::slotDone (int exitCode __attribute__ ((unused)), QProcess::ExitStatus exitStatus __attribute__ ((unused)))
{
  FILE *fp;
  int16_t tmpi16;


  misc->statusProgLabel->setText (tr ("Retrieving..."));
  misc->statusProgLabel->setVisible (true);
  misc->statusProg->setRange (0, shared_count);
  misc->statusProg->setTextVisible (true);
  qApp->processEvents();


  if ((fp = fopen (misc->shared_file.toLatin1 (), "r")) != NULL)
    {
      char string[512];
      SHARED_FILE_STRUCT ret;
      int32_t count, inc;
      uint32_t modified;


      //  Don't let the progress dialog eat up all the time.

      inc = shared_count / 5;
      if (!inc) inc = 1;


      switch (misc->abe_share->modcode)
        {
        default:
        case NO_ACTION_REQUIRED:
          misc->abe_share->modcode = 0;
          break;


          //  From tofWaterKill.

        case PFMEDIT_FORCE_SHARE:
          while (fgets (string, sizeof (string), fp) != NULL)
            {
              //  EOF is the end of data sentinel

              if (strstr (string, "EOF"))
                {
                  end_undo_block (misc);
                  break;
                }


              //  If we forced the read there will be a NEW COUNT record at the beginning of the file.

              if (strstr (string, "NEW COUNT"))
                {
                  sscanf (string, "NEW COUNT = %d", &shared_count);
                  misc->statusProg->setRange (0, shared_count);
                  qApp->processEvents ();
                  inc = shared_count / 5;
                  if (!inc) inc = 1;
                }
              else
                {
                  sscanf (string, "%d %d %lf %lf %f %f %f %d %hd %hd %d %d %d %hd %d\n", &ret.point, &count, &ret.x, &ret.y, &ret.z,
                          &ret.herr, &ret.verr, &ret.val, &ret.pfm, &ret.file, &ret.line, &ret.rec, &ret.sub, &tmpi16, &modified);
                  ret.exflag = (uint8_t) tmpi16;


                  for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
                    {
                      //  If we are displaying and editing only a single line, only get those points that are in that 
                      //  line.

                      if (!misc->num_lines || check_line (misc, misc->data[i].line))
                        {
                          if (!check_bounds (options, misc, i, NVTrue, misc->slice))
                            {
                              if (modified && misc->data[i].pfm == ret.pfm && misc->data[i].file == ret.file && misc->data[i].rec == ret.rec &&
                                  misc->data[i].sub == ret.sub)
                                {
                                  //  Save the undo information.

                                  store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);

                                  misc->data[i].val = ret.val;
                                  break;
                                }
                            }
                        }
                    }
                }

              if (!(count % inc))
                {
                  misc->statusProg->setValue (count);
                  qApp->processEvents ();
                }
            }
          break;


          //  From hofReturnKill

        case PFM_CHARTS_HOF_DATA:
          while (fgets (string, sizeof (string), fp) != NULL)
            {
              //  EOF is the end of data sentinel

              if (strstr (string, "EOF"))
                {
                  end_undo_block (misc);
                  break;
                }


              sscanf (string, "%d %d %lf %lf %f %f %f %d %hd %hd %d %d %d %hd %d\n", &ret.point, &count, &ret.x, &ret.y, &ret.z,
                      &ret.herr, &ret.verr, &ret.val, &ret.pfm, &ret.file, &ret.line, &ret.rec, &ret.sub, &tmpi16, &modified);
              ret.exflag = (uint8_t) tmpi16;

              if (modified)
                {
                  //  Save the undo information.

                  store_undo (misc, options->undo_levels, misc->data[ret.point].val, ret.point, NULL);


                  misc->data[ret.point].val = ret.val;
                }


              if (!(count % inc))
                {
                  misc->statusProg->setValue (count);
                  qApp->processEvents ();
                }
            }
          break;
        }

      fclose (fp);
    }


  remove (misc->shared_file.toLatin1 ());


  //  Force a redraw of the waveform monitor(s) just in case.

  misc->abe_share->modcode = WAVEMONITOR_FORCE_REDRAW;


  emit hotkeyPolygonDone ();
}



void 
hotkeyPolygon::slotReadyReadStandardError ()
{
  //  Passing the process back to the parent so that the messages can be displayed in the message box.

  emit readStandardErrorSignal (this);
}



//  This shouldn't be called.

void 
hotkeyPolygon::slotReadyReadStandardOutput ()
{
  QByteArray response = this->readAllStandardOutput ();
  char *res = response.data ();

  printf ("%s\n", res);
}
