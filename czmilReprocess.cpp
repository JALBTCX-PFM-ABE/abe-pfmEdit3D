
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



#include "czmilReprocess.hpp"
#include "czmil.h"


typedef struct
{
  int32_t      file;
  int32_t      rec;
  uint8_t      chan;
} FILE_STRUCT;


typedef struct
{
  int32_t      file;
  int32_t      rec;
  uint8_t      reprocess_chan[9];
} MERGED_STRUCT;


//!  This is a two key sort for qsort.

static int32_t compare_file_and_record_numbers (const void *a, const void *b)
{
  FILE_STRUCT *sa = (FILE_STRUCT *)(a);
  FILE_STRUCT *sb = (FILE_STRUCT *)(b);


  if (sa->file < sb->file) return (-1);
  if (sa->file > sb->file) return (+1);


  //  File is equal so go to record

  if (sa->rec < sb->rec) return (-1);
  if (sa->rec > sb->rec) return (+1);


  //  All of the keys are equal

  return (0);
}



/***************************************************************************/
/*!

   - Module :        czmil_reprocess

   - Programmer :    Jan C. Depner (PFM Software)

   - Date :          06/07/13

   - Purpose :       Start the external process to reprocess CZMIL waveform
                     data for the selected points.

   - Caveats:        Even though this does almost exactly what hotkeyPolygon
                     does, the key word is "almost".  We have a lot or other
                     operations that need to be performed both internally and
                     externally since in this case we are actually modifying
                     data (as in X, Y, and Z) in both the original input files
                     and the PFM.  This is not something we would normally do
                     since changing X and/or Y may change the location of the
                     point in the bins.  Here's a quick rundown on what we
                     have to do:

                     - In this program:
                       - If there is undo data, confirm with the user that
                         we're unloading all edits
                       - Clear all undo data
                       - Clear all highlighted data
                       - Unload changes (put_buffer)
                       - Filter selected shots based on options.czmil_reprocess_flags
                         and reprocesing mode (land/water/shallow water)
                       - Sort shots by file and record number

                     - In the external program:
                       - Read the CPF data for the selected shot/channel
                       - Find all points from this shot/channel in the PFM
                         and set to PFM_DELETED
                       - Reprocess the waveforms
                       - Write the CPF data back to the file
                       - Append the new records to the PFM (using attributes
                         and flags from the .ipf file)
                       - Recompute the bin for the new records (set to
                         unchecked/unverified!)

                     - In this program:
                       - Reload the points from the PFM for the area being
                         edited (get_buffer)
                       - Redisplay the point cloud in the current view

****************************************************************************/

czmilReprocess::czmilReprocess (QWidget *parent, nvMapGL *ma, OPTIONS *op, MISC *mi, int32_t *x_bounds, int32_t *y_bounds,
                                int32_t num_vertices, int32_t type, uint8_t *failed):QProcess (parent)
{
  int32_t put_buffer (MISC *misc);

  uint8_t optech_class = CZMIL_OPTECH_CLASS_UNDEFINED;
  uint8_t rect_flag = NVFalse;


  //  Check for a rectangle.

  if (!num_vertices)
    {
      rect_flag = NVTrue;
      num_vertices = 4;
    }


  map = ma;
  options = op;
  misc = mi;
  pa = parent;
  *failed = NVFalse;
  prog_state = -1;

  switch (type)
    {
    case CZMIL_REPROCESS_LAND:
      optech_class = CZMIL_OPTECH_CLASS_LAND;
      break;

    case CZMIL_REPROCESS_WATER:
      optech_class = CZMIL_OPTECH_CLASS_WATER;
      break;

    case CZMIL_REPROCESS_SHALLOW_WATER:
      optech_class = CZMIL_OPTECH_CLASS_SHALLOW_WATER;
      break;
    }


  //  If we don't have an undo count then we haven't done any editing.  If we have an undo count, we have to allow the user to make the
  //  decision on whether to save the changes or get out.

  if (misc->undo_count)
    {
      QString rep_str = tr ("To reprocess CZMIL data you must save all changes made during this session.\nDo you wish to continue?");
      int32_t ret = QMessageBox::information (map, "pfmEdit3D", rep_str, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);

      if (ret == QMessageBox::No) return;


      //  Clear the undo data.

      for (int32_t i = 0 ; i < misc->undo_count ; i++)
        {
          if (misc->undo[i].count)
            {
              misc->undo[i].val.clear ();
              misc->undo[i].num.clear ();
            }
        }
      misc->undo_count = 0;


      //  Clear up any highlight memory we had.

      if (misc->highlight_count)
        {
          misc->highlight.clear ();
          misc->highlight_count = 0;
        }


      //  Save the changes.

      put_buffer (misc);
    }


  double *mx = NULL, *my = NULL;


  //  Convert to double so that the "inside" function will work.

  mx = (double *) malloc (num_vertices * sizeof (double));

  if (mx == NULL)
    {
      fprintf (stderr, "%s %s %s %d - mx - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  my = (double *) malloc (num_vertices * sizeof (double));

  if (my == NULL)
    {
      fprintf (stderr, "%s %s %s %d - my - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  for (int32_t i = 0 ; i < num_vertices ; i++)
    {
      mx[i] = (double) x_bounds[i];
      my[i] = (double) y_bounds[i];
    }


  //  Get the minimum bounding rectangle (X and Y in pixels).

  int32_t min_y = 9999999;
  int32_t max_y = -1;
  int32_t min_x = 9999999;
  int32_t max_x = -1;
  for (int32_t j = 0 ; j < num_vertices ; j++)
    {
      min_y = qMin (min_y, y_bounds[j]);
      max_y = qMax (max_y, y_bounds[j]);
      min_x = qMin (min_x, x_bounds[j]);
      max_x = qMax (max_x, x_bounds[j]);
    }


  int32_t px = -1, py = -1, file_count = 0, merged_count = 0;
  uint8_t init2D = NVTrue;
  std::vector<FILE_STRUCT> file_data;
  std::vector<MERGED_STRUCT> merged_data;


  //  Scan for eligible points inside the polygon.

  for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
    {
      //  Check for single line display.

      if (!misc->num_lines || check_line (misc, misc->data[i].line))
        {
          //  Check against the displayed minimum bounding rectangle.  This is all that is needed for 
          //  a rectangle and it cuts down on the computations for a polygon.  This also checks validity 
          //  against the display options (like Display Invalid).

          if (!check_bounds (options, misc, i, NVTrue, misc->slice))
            {
              //  Make sure the point is a CZMIL record and that is is valid

              if (misc->data[i].type == PFM_CZMIL_DATA)
                {
                  //  Check against options.czmil_reprocess_flags.

                  if (((options->czmil_reprocess_flags[0]) && misc->data[i].attr[misc->czmil_proc_attr] < CZMIL_OPTECH_CLASS_HYBRID) ||
                      ((options->czmil_reprocess_flags[1]) && misc->data[i].attr[misc->czmil_proc_attr] > CZMIL_OPTECH_CLASS_HYBRID))
                    {

                      //  We don't want to reprocess land as land, water as water, or shallow water as shallow water.

                      if ((type == CZMIL_REPROCESS_LAND && misc->data[i].attr[misc->czmil_proc_attr] > CZMIL_OPTECH_CLASS_HYBRID) ||
                          (type == CZMIL_REPROCESS_WATER && misc->data[i].attr[misc->czmil_proc_attr] < CZMIL_OPTECH_CLASS_HYBRID) ||
                          (type == CZMIL_REPROCESS_SHALLOW_WATER && misc->data[i].attr[misc->czmil_proc_attr] != CZMIL_OPTECH_CLASS_SHALLOW_WATER))
                        {
                          //  Convert the X, Y, and Z value to a projected pixel position

                          map->get2DCoords (misc->data[i].x, misc->data[i].y, -misc->data[i].z, &px, &py, &init2D);


                          //  Check against the polygon (or the rectangle if the rectangle flag is set).

                          if ((rect_flag && (misc->data[i].y >= min_y && misc->data[i].y <= max_y && misc->data[i].x >= min_x && misc->data[i].x <= max_x)) ||
                              inside_polygon2 (mx, my, num_vertices, (double) px, (double) py))
                            {
                              try
                                {
                                  file_data.resize (file_count + 1);
                                }
                              catch (std::bad_alloc&)
                                {
                                  fprintf (stderr, "%s %s %s %d - file_data - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
                                  exit (-1);
                                }

                              file_data[file_count].file = misc->data[i].file;
                              file_data[file_count].rec = misc->data[i].rec;
                              file_data[file_count].chan = misc->data[i].sub / 100;

                              file_count++;
                            }
                        }
                    }
                }
            }
        }
    }


  //  Free the temporary MBR data.

  free (mx);
  free (my);


  if (file_count)
    {
      char tmp[512];
      int16_t dtype;


      //  Now sort the data by file and record.

      qsort (file_data.data (), file_count, sizeof (FILE_STRUCT), compare_file_and_record_numbers);


      //  Remove duplicates and set the channel reprocess_flags.

      int32_t prev_rec = -1, prev_file = -1;
      for (int32_t i = 0 ; i < file_count ; i++)
        {
          if (prev_rec != file_data[i].rec || prev_file != file_data[i].file)
            {
              try
                {
                  merged_data.resize (merged_count + 1);
                }
              catch (std::bad_alloc&)
                {
                  fprintf (stderr, "%s %s %s %d - merged_data - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
                  exit (-1);
                }

              merged_data[merged_count].file = file_data[i].file;
              merged_data[merged_count].rec = file_data[i].rec;


              //  The first time for a specific file/record combination we need to clear the channel array.

              for (int32_t j = 0 ; j < 9 ; j++) merged_data[merged_count].reprocess_chan[j] = CZMIL_OPTECH_CLASS_UNDEFINED;

              merged_count++;
            }


          //  Now we set the channel reprocessing mode for this channel.  This effectively combines all of the channel reprocessing 
          //  modes for every file/record combination.

          merged_data[merged_count - 1].reprocess_chan[file_data[i].chan] = optech_class;

          prev_file = file_data[i].file;
          prev_rec = file_data[i].rec;
        }


      //  Save the last channel flag

      if (prev_rec != file_data[file_count -1].rec || prev_file != file_data[file_count - 1].file)
        {
          try
            {
              merged_data.resize (merged_count + 1);
            }
          catch (std::bad_alloc&)
            {
              fprintf (stderr, "%s %s %s %d - merged_data - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          merged_data[merged_count].file = file_data[file_count - 1].file;
          merged_data[merged_count].rec = file_data[file_count - 1].rec;
          merged_data[merged_count].reprocess_chan[file_count - 1] = optech_class;

          merged_count++;
        }


      file_data.clear ();


      FILE *fp;
      misc->shared_file = QDir::tempPath () + SEPARATOR + QString ("PFM_EDIT_3D_SHARED_FILE_%1.tmp").arg (getpid ());

      if ((fp = fopen (misc->shared_file.toLatin1 (), "w")) != NULL)
        {
          //  The first record includes the type of data to be reprocessed and a list of probability of detection values per channel.

          fprintf (fp, "%d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f\n", options->czmil_reprocess_flags[0], options->czmil_reprocess_flags[1],
                   options->czmil_probability[0], options->czmil_probability[1], options->czmil_probability[2], options->czmil_probability[3],
                   options->czmil_probability[4], options->czmil_probability[5], options->czmil_probability[6], options->czmil_probability[7], 
                   options->czmil_probability[8]);


          int32_t prev_f = -1;


          //  Don't let the progress bar eat up all the time.

          inc = file_count / 5;
          if (!inc) inc = 1;


          misc->statusProgLabel->setText (tr ("Writing..."));
          misc->statusProgLabel->setVisible (true);
          misc->statusProg->setRange (0, merged_count);
          misc->statusProg->setTextVisible (true);
          qApp->processEvents ();

          for (int32_t i = 0 ; i < merged_count ; i++)
            {
              if (!(i % inc))
                {
                  misc->statusProg->setValue (i);
                  qApp->processEvents ();
                }


              if (!i || prev_f != merged_data[i].file)
                {
                  if (i) fprintf (fp, "EOD\n");

                  read_list_file (misc->pfm_handle[0], merged_data[i].file, tmp, &dtype);

                  fprintf (fp, "%s\n", tmp);


                  //  We need to make sure the czmil_lidar_params.dat file is available for every CPF file.  If it's not there then we can't reprocess anything.

                  QString param_file;

                  param_file = QString (pfm_dirname (tmp)) + SEPARATOR + "czmil_lidar_params.dat";

                  if (!QFileInfo (param_file).exists ())
                    {
                      merged_data.clear ();
                      fclose (fp);
                      QFile (misc->shared_file).remove ();

                      QMessageBox::critical (pa, tr ("pfmEdit3D czmilReprocess"),
                                             tr ("The parameter file %1 is missing!\nReprocessing will not be done.").arg (param_file));

                      qApp->restoreOverrideCursor ();
                      misc->statusProg->reset ();
                      misc->statusProg->setRange (0, 100);
                      misc->statusProgLabel->setVisible (false);
                      misc->statusProg->setTextVisible (false);
                      qApp->processEvents ();

                      *failed = NVTrue;

                      return;
                    }
                }


              //  Write the record.

              fprintf (fp, "%d %d %d %d %d %d %d %d %d %d %d\n", merged_data[i].file, merged_data[i].rec, merged_data[i].reprocess_chan[0],
                       merged_data[i].reprocess_chan[1], merged_data[i].reprocess_chan[2], merged_data[i].reprocess_chan[3],
                       merged_data[i].reprocess_chan[4], merged_data[i].reprocess_chan[5], merged_data[i].reprocess_chan[6],
                       merged_data[i].reprocess_chan[7], merged_data[i].reprocess_chan[8]);

              prev_f = merged_data[i].file;
            }

          fprintf (fp, "EOF\n");
          fclose (fp);


          merged_data.clear ();


          misc->statusProg->reset ();
          misc->statusProgLabel->setVisible (false);
          misc->statusProg->setTextVisible (false);
          qApp->processEvents ();


          misc->statusProgLabel->setText (tr ("Running ") + cmd);
          misc->statusProgLabel->setVisible (true);
          misc->statusProg->setRange (0, 0);
          misc->statusProg->setTextVisible (true);
          qApp->processEvents ();


          cmd = "czmilReprocess";


          //  Set up the arguments.

          args.clear ();

          switch (type)
            {
            case CZMIL_REPROCESS_LAND:
              args += QString ("--mode=%1").arg (CZMIL_OPTECH_CLASS_LAND);
              break;

            case CZMIL_REPROCESS_WATER:
              args += QString ("--mode=%1").arg (CZMIL_OPTECH_CLASS_WATER);
              break;

            case CZMIL_REPROCESS_SHALLOW_WATER:
              args += QString ("--mode=%1").arg (CZMIL_OPTECH_CLASS_SHALLOW_WATER);
              break;
            }


          //  Add the shared memory key.

          args += QString ("--shared_memory_key=%1").arg (misc->abe_share->ppid);


          //  Add the shared file name.

          args += QString ("--shared_file=%1").arg (misc->shared_file);


          QString sortLabel = tr ("Sorting %1 CZMIL returns...").arg (file_count);
          misc->statusProgLabel->setText (sortLabel);
          misc->statusProgLabel->setVisible (true);
          misc->statusProg->setRange (0, 0);
          misc->statusProg->setTextVisible (true);
          qApp->processEvents ();


          connect (this, SIGNAL (finished (int, QProcess::ExitStatus)), this, SLOT (slotDone (int, QProcess::ExitStatus)));
          connect (this, SIGNAL (readyReadStandardError ()), this, SLOT (slotReadyReadStandardError ()));
          connect (this, SIGNAL (readyReadStandardOutput ()), this, SLOT (slotReadyReadStandardOutput ()));
          connect (this, SIGNAL (error (QProcess::ProcessError)), this, SLOT (slotError (QProcess::ProcessError)));

          this->start (cmd, args);
        }
      else
        {
          QString msg = tr ("Unable to open shared data file \n%1\nReason: %2").arg (QFileInfo (misc->shared_file).absolutePath ()).arg (strerror (errno));
          QMessageBox::warning (parent, "czmilReprocess", msg);

          misc->highlight.clear ();

          *failed = NVTrue;
        }
    }
  else
    {
      QMessageBox::warning (parent, tr ("pfmEdit3D CzmilReprocess"), tr ("No points found that match allowed czmilReprocess data types\n"));

      *failed = NVTrue;
    }
}



czmilReprocess::~czmilReprocess ()
{
}



void 
czmilReprocess::slotError (QProcess::ProcessError error)
{
  qApp->restoreOverrideCursor ();

  misc->statusProg->reset ();
  misc->statusProg->setRange (0, 100);
  misc->statusProgLabel->setVisible (false);
  misc->statusProg->setTextVisible (false);
  qApp->processEvents ();


  QString commandLine = cmd;
  for (int32_t i = 0 ; i < args.size () ; i++) commandLine += (" " + args.at (i));

  switch (error)
    {
    case QProcess::FailedToStart:
      QMessageBox::critical (pa, tr ("pfmEdit3D czmilReprocess"), tr ("Unable to start the czmilReprocess program!") + 
                             "\n" + commandLine);
      break;

    case QProcess::Crashed:
      QMessageBox::critical (pa, tr ("pfmEdit3D czmilReprocess"), tr ("The czmilReprocess program crashed!") + 
                             "\n" + commandLine);
      break;

    case QProcess::Timedout:
      QMessageBox::critical (pa, tr ("pfmEdit3D czmilReprocess"), tr ("The czmilReprocess program timed out!") + 
                             "\n" + commandLine);
      break;

    case QProcess::WriteError:
      QMessageBox::critical (pa, tr ("pfmEdit3D czmilReprocess"), 
                             tr ("There was a write error to the czmilReprocess program!") + "\n" + commandLine);
      break;

    case QProcess::ReadError:
      QMessageBox::critical (pa, tr ("pfmEdit3D czmilReprocess"),
                             tr ("There was a read error from the czmilReprocess program!") + "\n" + commandLine);
      break;

    case QProcess::UnknownError:
      QMessageBox::critical (pa, tr ("pfmEdit3D czmilReprocess"), 
                             tr ("The czmilReprocess program died with an unknown error!") + "\n" + commandLine);
      break;
    }

  QFile (misc->shared_file).remove ();
}



void 
czmilReprocess::slotDone (int exitCode __attribute__ ((unused)), QProcess::ExitStatus exitStatus __attribute__ ((unused)))
{
  //  Since we're now trapping the error messages and displaying them we don't really need to test this.

  switch (misc->abe_share->modcode)
    {
    case CZMILREPROCESS_ERROR:
    case CZMILREPROCESS_FATAL_ERROR:

      //  CZMIL data was reprocessed and not all points could be handled.

      misc->abe_share->modcode = 0;

      break;
    }


  QFile (misc->shared_file).remove ();


  //  DANGER WILL ROBINSON!  Do not move the following line.  It must be the last thing done here because it
  //  is tested for in the slotTrackCursor function of pfmEdit3D.  This allows us to wait if the error messages
  //  dialog is being displayed.  This takes the place of the normal signal/slot method.

  misc->czmilReprocessFlag = NVTrue;
}



void 
czmilReprocess::slotReadyReadStandardOutput ()
{
  /*
      This function should receive lines from the czmilReprocess program that are in one of these forms:

        Reprocessing CZMIL waveforms - XXX% processed\n  -  Indicates the progress of deleting points and reprocessing waveforms
        Reprocessing CZMIL waveforms - Complete\n        -  Indicates deleting points and reprocessing is done
        Reloading CZMIL points - XXX% processed\n        -  Indicates the progress of the reload process
        Reloading CZMIL points - Complete\n              -  Indicates the reload process is done
        Recomputing PFM bins - XXX% processed\n          -  Indicates the progress of the recompute process
        Recomputing PFM bins - Complete\n                -  Indicates the recompute process is done

      These can be parsed to update the progress bar.

  */

  static QString resp_string = "";


  QByteArray response = this->readAllStandardOutput ();


  //  Parse the return response for line feeds

  for (int32_t i = 0 ; i < response.length () ; i++)
    {
      if (response.at (i) == '\n')
        {
          if (!resp_string.startsWith ("Setting MAXIMUM Cache Size"))
            {
              QString processLabel = resp_string.section ('-', 0, 0).simplified () + "...";
              misc->statusProgLabel->setText (processLabel);

              if (prog_state < 0)
                {
                  misc->statusProg->setRange (0, 100);
                  misc->statusProg->setTextVisible (true);
                  qApp->processEvents ();

                  prog_state = 0;
                }

              if (!resp_string.contains ("Complete"))
                {
                  QString percent = resp_string.section ('-', 1, 1).section ('%', 0, 0);

                  misc->statusProg->setValue (percent.toInt ());
                  qApp->processEvents ();
                }
            }

          resp_string = "";
        }
      else
        {
          resp_string += response.at (i);
        }
    }
}



void 
czmilReprocess::slotReadyReadStandardError ()
{
  emit readStandardErrorSignal (this);
}
