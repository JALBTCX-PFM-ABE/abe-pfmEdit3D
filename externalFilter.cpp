
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



#include "externalFilter.hpp"


//!  This is used to run an external filter program.  At present we only use it for hofWaveFilter and cpfDeepFilter.

externalFilter::externalFilter (QWidget *parent, nvMapGL *ma, OPTIONS *op, MISC *mi, int32_t prog, uint8_t *failed, int32_t *x_bounds,
                                int32_t *y_bounds, int32_t num_vertices):
  QProcess (parent)
{
  int32_t buildCommand (QString progString, QString actionString, MISC *misc, int32_t nearest_point, QString *cmd, QStringList *args, int32_t kill_switch);

  map = ma;
  options = op;
  misc = mi;
  pa = parent;
  *failed = NVFalse;


  QString progString = options->prog[prog];

  QString actionString = options->action[prog];
  buildCommand (progString, actionString, misc, -999, &cmd, &args, -1);


  misc->statusProgLabel->setText (tr ("Running ") + options->name[prog]);
  misc->statusProgLabel->setVisible (true);


  //  If we are runnng cpfDeepFilter we may need some extra arguments, we have a different status bar range than hofWaveFilter, and we have to set the 
  //  external flags for cpfDeepFilter to know which points to work on.


  //  This won't actually happen (since cpfDeepFilter isn't used anymore) but it has been left in for reference...

  if (prog == CPF_DEEP_FILTER)
    {
      uint8_t check_invalid = NVTrue, init2D = NVTrue;

      if (!options->czmil_cpf_filter_inval)
        {
          check_invalid = NVFalse;

          args += "-v";
          QString num;
          num.setNum (options->czmil_cpf_filter_amp_min);
          args += num;
        }


      double *mx = NULL, *my = NULL;


      //  We have to convert to double so that the "inside" function will work.

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

      int32_t px = -1, py = -1;

      for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
        {
          //  Clear the exflag

          misc->data[i].exflag = NVFalse;


          //  Only CZMIL data.

          if (misc->data[i].type == PFM_CZMIL_DATA)
            {
              //  Only work on the deep channel data.

              if (misc->data[i].sub / 100 == CZMIL_DEEP_CHANNEL)
                {
                  //  Check for single line display.

                  if (!misc->num_lines || check_line (misc, misc->data[i].line))
                    {
                      //  Check against the displayed minimum bounding rectangle.  This is all that is needed for 
                      //  a rectangle and it cuts down on the computations for a polygon.   Also, DO NOT allow changes to null value status.

                      if (!check_bounds (options, misc, misc->data[i].x, misc->data[i].y, misc->data[i].z, misc->data[i].val, misc->data[i].mask,
                                         misc->data[i].pfm, check_invalid, misc->slice) && misc->data[i].z < misc->null_val[misc->data[i].pfm])
                        {
                          //  We have two possible filters here.  First, we may want to invalidate deep channel returns.  Second, we may want to 
                          //  revalidate last deep channel returns.  Due to this we turned of validity checking in the above check_bounds call
                          //  and will do it here.

                          uint8_t check_it = NVFalse;
                          if (options->czmil_cpf_filter_inval)
                            {
                              //  No point in trying to invalidate already invalid data.

                              if (!(misc->data[i].val & PFM_INVAL)) check_it = NVTrue;
                            }
                          else
                            {
                              //  We only want to validate filter invalid data.

                              if (misc->data[i].val & PFM_FILTER_INVAL) check_it = NVTrue;
                            }


                          if (check_it)
                            {
                              //  Convert the X, Y, and Z value to a projected pixel position

                              map->get2DCoords (misc->data[i].x, misc->data[i].y, -misc->data[i].z, &px, &py, &init2D);


                              //  Always check against the polygon's minimum bounding rectangle before checking against the polygon.
                              //  Hopefully, that will make the whole thing a bit faster (that's what everybody says on the interwebs anyway ;-)

                              if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
                                {
                                  //  If it's inside the MBR we have to check to make sure it's inside the polygon as well.

                                  if (inside_polygon2 (mx, my, num_vertices, (double) px, (double) py)) misc->data[i].exflag = NVTrue;
                                }
                            }
                        }
                    }
                }
            }
        }

      misc->statusProg->setRange (0, 100);
    }
  else
    {
      misc->statusProg->setRange (0, 0);
    }

  misc->statusProg->setTextVisible (true);
  qApp->processEvents();


  connect (this, SIGNAL (finished (int, QProcess::ExitStatus)), this, SLOT (slotDone (int, QProcess::ExitStatus)));
  connect (this, SIGNAL (readyReadStandardError ()), this, SLOT (slotReadyReadStandardError ()));
  connect (this, SIGNAL (readyReadStandardOutput ()), this, SLOT (slotReadyReadStandardOutput ()));
  connect (this, SIGNAL (error (QProcess::ProcessError)), this, SLOT (slotError (QProcess::ProcessError)));


  misc->dataShare->unlock ();


  this->start (cmd, args);
}



externalFilter::~externalFilter ()
{
}



void 
externalFilter::slotError (QProcess::ProcessError error)
{
  QString commandLine = cmd;
  for (int32_t i = 0 ; i < args.size () ; i++) commandLine += (" " + args.at (i));

  switch (error)
    {
    case QProcess::FailedToStart:
      QMessageBox::critical (pa, tr ("pfmEdit3D external filter program"), tr ("Unable to start the external filter program!") + "\n" + commandLine);
      break;

    case QProcess::Crashed:
      QMessageBox::critical (pa, tr ("pfmEdit3D external filter program"), tr ("The external filter program crashed!") + "\n" + commandLine);
      break;

    case QProcess::Timedout:
      QMessageBox::critical (pa, tr ("pfmEdit3D external filter program"), tr ("The external filter program timed out!") + "\n" + commandLine);
      break;

    case QProcess::WriteError:
      QMessageBox::critical (pa, tr ("pfmEdit3D external filter program"), tr ("There was a write error to the external filter program!") + "\n" +
                             commandLine);
      break;

    case QProcess::ReadError:
      QMessageBox::critical (pa, tr ("pfmEdit3D external filter program"), tr ("There was a read error from the external filter program!") + "\n" +
                             commandLine);
      break;

    case QProcess::UnknownError:
      QMessageBox::critical (pa, tr ("pfmEdit3D external filter program"), tr ("The external filter program died with an unknown error!") + "\n" +
                             commandLine);
      break;
    }


  misc->statusProgLabel->setVisible (true);
  misc->statusProg->reset ();
  misc->statusProg->setTextVisible (true);
  qApp->processEvents();


  //  Clear the exflags just in case this was cpfDeepFilter

  for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++) misc->data[i].exflag = NVFalse;


  misc->dataShare->lock ();
  emit externalFilterDone ();
}



void 
externalFilter::slotDone (int exitCode __attribute__ ((unused)), QProcess::ExitStatus exitStatus __attribute__ ((unused)))
{
  misc->dataShare->lock ();
  misc->filter_undo.count = 0;


  switch (misc->abe_share->modcode)
    {
    default:
    case NO_ACTION_REQUIRED:
      misc->abe_share->modcode = 0;
      break;


      //  From hofWaveFilter

    case PFM_CHARTS_HOF_DATA:

      for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
        {
          //  Check the exflag but don't mess with things that are masked.

          if (misc->data[i].exflag && !misc->data[i].mask)
            {
              try
                {
                  misc->filter_kill_list.resize (misc->filter_kill_count + 1);
                }
              catch (std::bad_alloc&)
                {
                  fprintf (stderr, "%s %s %s %d - filter_kill_list - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
                  exit (-1);
                }

              misc->filter_kill_list[misc->filter_kill_count] = i;
              misc->filter_kill_count++;
            }


          misc->data[i].exflag = NVFalse;
        }
      misc->abe_share->modcode = 0;
      break;


      //  From cpfDeepFilter

    case PFM_CZMIL_DATA:

      //  OK.  We have two possibilities here.  First, and easiest, is that we are invalidating deep returns.  In that case we just add them
      //  to the kill list.  Easy Peasy.  Second, and harder, we are validating deep returns.  In that case we need to go ahead and validate
      //  them and then put them in the kill list so they will show up in the display (because they were probably invalid prior to running
      //  the filter.  Now, after we show the user the points to be invalidated or validated he will be able to either accept or discard the
      //  points.  If he is invalidating and he accepts, then we just do the normal "invalidate the kill list points" thing.  If he is
      //  invalidating and he discards then we just clear the kill list.  If he is validating and he accepts, then we just have to clear the
      //  kill list.  If he is validating and he rejects we can just "undo" the operation and clear the kill list.

      for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
        {
          //  Check the exflag but don't mess with things that are masked.

          if (misc->data[i].exflag && !misc->data[i].mask)
            {
              try
                {
                  misc->filter_kill_list.resize (misc->filter_kill_count + 1);
                }
              catch (std::bad_alloc&)
                {
                  fprintf (stderr, "%s %s %s %d - filter_kill_list - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
                  exit (-1);
                }

              misc->filter_kill_list[misc->filter_kill_count] = i;


              //  If validating, save the undo information and clear the invalid flags (including PFM_FILTER_INVAL)

              if (!options->czmil_cpf_filter_inval)
                {
                  //  We want to save the temporary undo information as it is prior to validating the points.

                  try
                    {
                      misc->filter_undo.val.resize (misc->filter_undo.count + 1);
                    }
                  catch (std::bad_alloc&)
                    {
                      QMessageBox::critical (0, tr ("pfmEdit3D filter undo"),
                                             tr ("Unable to allocate filter UNDO validity memory!  Reason : %1").arg (strerror (errno)));
                      exit (-1);
                    }

                  try
                    {
                      misc->filter_undo.num.resize (misc->filter_undo.count + 1);
                    }
                  catch (std::bad_alloc&)
                    {
                      QMessageBox::critical (0, tr ("pfmEdit3D filter undo"),
                                             tr ("Unable to allocate filter UNDO num memory!  Reason : %1").arg (strerror (errno)));
                      exit (-1);
                    }

                  misc->filter_undo.val[misc->filter_undo.count] = misc->data[i].val;
                  misc->filter_undo.num[misc->filter_undo.count] = i;
                  misc->filter_undo.count++;

                  misc->data[i].val &= ~PFM_INVAL;
                }

              misc->filter_kill_count++;
            }


          misc->data[i].exflag = NVFalse;
        }


      //  If we're validating, end the undo block.

      if (!options->czmil_cpf_filter_inval) end_undo_block (misc);


      misc->abe_share->modcode = 0;
      break;
    }


  //  Force a redraw of the waveMonitor just in case.

  misc->abe_share->modcode = WAVEMONITOR_FORCE_REDRAW;


  misc->statusProgLabel->setVisible (true);
  misc->statusProg->setRange (0, 100);
  misc->statusProg->reset ();
  misc->statusProg->setTextVisible (true);
  qApp->processEvents();


  emit externalFilterDone ();
}



void 
externalFilter::slotReadyReadStandardError ()
{
  //  Passing the process back to the parent so that the messages can be displayed in the message box.

  emit readStandardErrorSignal (this);
}



//  This is only used for cpfDeepFilter at the moment.

void 
externalFilter::slotReadyReadStandardOutput ()
{
  /*
      This function should receive lines from the cpfDeepFilter program that are in one of these forms:

      XXX% processed\n          -  Indicates the progress of filtering
      Complete\n                -  Indicates the filtering process is done

      These can be parsed to update the progress bar.

  */

  static QString resp_string = "";


  QByteArray response = this->readAllStandardOutput ();


  //  Parse the return response for line feeds

  for (int32_t i = 0 ; i < response.length () ; i++)
    {
      if (response.at (i) == '\n')
        {
          if (!resp_string.contains ("Complete"))
            {
              QString percent = resp_string.section ('%', 0, 0).section ('%', 0, 0);

              misc->statusProg->setValue (percent.toInt ());
              qApp->processEvents ();
            }
          else
            {
              misc->statusProg->reset ();
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
