
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
#include "hpFilterThread.hpp"
#include "avtFilterThread.hpp"


typedef struct
{
  uint32_t              rec;
  uint32_t              val;
} REC_VAL;


typedef struct
{
  std::vector<REC_VAL>  rv;
  uint32_t              num_soundings;
  float                 standard_dev;
  float                 avg_filtered_depth;
  uint8_t               data;
} BIN_DEPTH;


static BIN_DEPTH        **bin_depth;
NV_F64_XYMBR            bounds;
static std::vector<FEATURE_RECORD> feature;
static int32_t          feature_count = 0, rows = 0, cols = 0;
static double           x_bin_size, y_bin_size;
static float            min_z, max_z;
static FILTER_POS_DATA *pos;
static FILTER_BIN_DATA **bin_data;


#define SQR(x) ((x)*(x))


/***************************************************************************/
/*!

  - Module Name:        compute_bin_values

  - Programmer(s):      Jan C. Depner

  - Date Written:       December 2000

  - Purpose:            Recomputes the bin record values from the depth
                        records.  Only computes the average filtered value,
                        the standard deviation, and the number of valid
                        depths.

  - Arguments:
                        - bin_depth       =   bin_depth record

  - Return Value:
                        - Number of valid points

****************************************************************************/

void compute_bin_values (BIN_DEPTH *bin_depth, MISC *misc)
{
  double sum_filtered = 0.0, sum2_filtered = 0.0;
  int32_t filtered_count = 0;


  for (uint32_t i = 0 ; i < bin_depth->num_soundings ; i++)
    {
      //  DO NOT use records marked as deleted.

      if (!(bin_depth->rv[i].val & PFM_DELETED))
        {
          if (!(bin_depth->rv[i].val & PFM_INVAL))
            {
              sum_filtered += misc->data[bin_depth->rv[i].rec].z;
              sum2_filtered += SQR (misc->data[bin_depth->rv[i].rec].z);

              filtered_count++;
            }
        }
    }


  if (!filtered_count)
    {
      bin_depth->data = NVFalse;
    }
  else
    {
      bin_depth->avg_filtered_depth = sum_filtered / (double) filtered_count; 
      if (filtered_count > 1)
        {
          bin_depth->standard_dev = sqrt ((sum2_filtered - ((double) filtered_count * (pow ((double) bin_depth->avg_filtered_depth, 2.0)))) / 
                                          ((double) filtered_count - 1.0));
        }
      else
        {
          bin_depth->standard_dev = 0.0;
        }
      bin_depth->data = NVTrue;
    }

  return;
}



/********************************************************************/
/*!
 
 - Function Name :  InitializeAreaFilter
 
 - Description : Perform any initialization specific to the area based
                 filter here.
 
*********************************************************************/

uint8_t InitializeAreaFilter (int32_t width, int32_t height, MISC *misc)
{
  //  Allocate the memory for all of the bins.

  bin_depth = (BIN_DEPTH **) calloc (height, sizeof (BIN_DEPTH *));

  if (bin_depth == NULL)
    {
      QMessageBox::critical (0, pfmEdit3D::tr ("pfmEdit3D filter"), pfmEdit3D::tr ("Unable to allocate memory for filtering!"));
      return (NVFalse);
    }

  for (int32_t i = 0 ; i < height ; i++)
    {
      bin_depth[i] = (BIN_DEPTH *) calloc (width, sizeof (BIN_DEPTH));

      if (bin_depth[i] == NULL)
        {
          free (bin_depth);
          QMessageBox::critical (0, pfmEdit3D::tr ("pfmEdit3D filter"), pfmEdit3D::tr ("Unable to allocate memory for filtering!"));
          return (NVFalse);
        }
    }


  //  Populate all of the bins with depth data.

  for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
    {
      int32_t row = NINT ((misc->data[i].y - bounds.min_y) / y_bin_size);
      int32_t col = NINT ((misc->data[i].x - bounds.min_x) / x_bin_size);


      //  This should never happen.

      if (row < 0 || col < 0)
        {
          for (int32_t j = 0 ; j < height ; j++)
            {
              if (bin_depth[j] != NULL) 
                {
                  for (int32_t k = 0 ; k < width ; k++)
                    {
                      bin_depth[j][k].rv.clear ();
                    }
                  free (bin_depth[j]);
                }
            }
          return (NVFalse);
        }

      int32_t recnum = bin_depth[row][col].num_soundings;

      try
        {
          bin_depth[row][col].rv.resize (recnum + 1);
        }
      catch (std::bad_alloc&)
        {
          fprintf (stderr, "%s %s %s %d - bin_depth[row][col].rv - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
          exit (-1);
        }


      bin_depth[row][col].rv[recnum].val = misc->data[i].val;
      bin_depth[row][col].rv[recnum].rec = i;

      bin_depth[row][col].num_soundings++;
    }


  //  Now compute the average and standard deviation for the bin.

  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          compute_bin_values (&bin_depth[i][j], misc);
        }
    }

  return (NVTrue);
}



/********************************************************************/
/*!

 - Function Name : AreaFilter
 
 - Description :   Buffers three rows of PFM data and filters it.
 
 - Inputs :
                   - pfm          = PFM layer number
                   - coord        = current cell coordinate that we are processing.
                   - bin_diagonal = diagonal distance between bins in meters
 
 - Method :        If the slope is low (< 1 degree) we'll use an average of the cell standard
                   deviations to beat the depths against.  Otherwise, we'll compute the
                   standard deviation from the cell averages.  Since we're using the average
                   of the computed standard deviations of all of the nine cells or the 
                   standard deviations of the averages of all nine cells we multiply the
                   resulting standard deviation (?) by two to get a reasonable result,
                   otherwise the standard deviation surface is too smooth and we end up
                   cutting out too much good data.  I must admit I arrived at these numbers
                   by playing with the filter using em3000 shallow water data and em121a deep
                   water data but they appear to work properly.  This way three sigma seems
                   to cut out what you would expect three sigma to cut out.  If you leave it
                   as is it cuts out about 30%.  This is called empirically determining a
                   value (From Nero's famous statement "I'm the emperor and I can do what I
                   damn well please, now hand me my fiddle.").   JCD

********************************************************************/

uint8_t AreaFilter (OPTIONS *options, MISC *misc, double bin_diagonal, int32_t row, int32_t col, double *mx, double *my, int32_t poly_count)
{
  //  If the center cell has no valid data, return.

  if (!bin_depth[row][col].data) return (NVFalse);


  double avgsum = 0.0;
  double stdsum = 0.0;
  double sum2 = 0.0;
  int32_t sumcount = 0;


  //  Get the information from the 8 cells surrounding this cell (and save the center cell data).

  for (int32_t i = row - 1 ; i <= row + 1 ; i++)
    {
      for (int32_t j = col - 1 ; j <= col + 1 ; j++)
        {
          if (bin_depth[i][j].data)
            {
              avgsum += bin_depth[i][j].avg_filtered_depth;
              stdsum += bin_depth[i][j].standard_dev;
              sum2 += SQR (bin_depth[i][j].avg_filtered_depth);

              sumcount++;
            }
        }
    }


  //  Make sure that at least 5 of the surrounding cells have valid data.  This saves us from deleting a lot of edge points
  //  without any real evidence that they were bad.

  if (sumcount < 5) return (NVFalse);


  //  Compute the eight slopes from the center cell to find out if it's flat enough to use the average of the
  //  standard deviations or if we need to use the standard deviation of the averages.

  uint8_t flat = NVTrue;
  double dx, slope;

  for (int32_t i = row - 1, k = 0 ; i <= row + 1 ; i++, k++)
    {
      for (int32_t j = col - 1 ; j <= col ; j++)
        {
          if (i != row || j != col)
            {
              if (bin_depth[i][j].data)
                {
                  dx = bin_diagonal;

                  slope = (fabs (bin_depth[row][col].avg_filtered_depth - bin_depth[i][j].avg_filtered_depth)) / dx;

                  if (slope > 1.0)
                    {
                      flat = NVFalse;
                      break;
                    }
                }
            }
        }
    }


  int32_t count = 0;

  /*
      If the slope is low (< 1 degree) we'll use an average of the cell standard deviations to beat the 
      depths against.  Otherwise, we'll compute the standard deviation from the cell averages.  Since 
      we're using the average of the computed standard deviations of all of the nine cells or the 
      standard deviations of the averages of all nine cells we multiply the resulting standard
      deviation (?) by two to get a reasonable result, otherwise the standard deviation surface is too
      smooth and we end up cutting out too much good data.  I must admit I arrived at these numbers by
      playing with the filter using em3000 shallow water data and em121a deep water data but they appear
      to work properly.  This way three sigma seems to cut out what you would expect three sigma to cut
      out.  If you leave it as is it cuts out about 30%.  This is called empirically determining a value
      (From Nero's famous statement "I'm the emperor and I can do what I damn well please, now hand me
      my fiddle.").   JCD
  */

  double avg = avgsum / (double) sumcount;
  double std;
  if (flat)
    {
      std = (stdsum / (double) sumcount) * 2.0;
    }
  else
    {
      std = (sqrt ((sum2 - ((double) sumcount * SQR (avg))) / ((double) sumcount - 1.0))) * 2.0;
    }

  double BinSigmaFilter = options->area_filter_std * std;


  for (uint32_t i = 0 ; i < bin_depth[row][col].num_soundings ; i++)
    {
      //  Only check those that haven't been checked before.

      if (!(bin_depth[row][col].rv[i].val & (PFM_INVAL | PFM_DELETED)))
        {
          double depth = misc->data[bin_depth[row][col].rv[i].rec].z;


          //  Check in both directions first

          if (fabs (depth - avg) >= BinSigmaFilter)
            {
              //  Check the deep filter only flag and, if set, check in the deep direction only

              if (!misc->data[bin_depth[row][col].rv[i].rec].fmask && (!options->deep_filter_only || (depth - avg) >= BinSigmaFilter))
                {
                  bin_depth[row][col].rv[i].val |= PFM_MANUALLY_INVAL;


                  //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
                  //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
                  //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
                  //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
                  //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

                  bin_depth[row][col].rv[i].val &= ~PFM_SUSPECT;


                  //  Check for single line display.

                  if (!misc->num_lines || check_line (misc, misc->data[i].line))
                    {
                      //  Check for all of the limits on this point.

                      if (!check_bounds (options, misc, misc->data[bin_depth[row][col].rv[i].rec].x, misc->data[bin_depth[row][col].rv[i].rec].y, 
                                         misc->data[bin_depth[row][col].rv[i].rec].z, NVFalse, misc->data[bin_depth[row][col].rv[i].rec].mask,
                                         misc->data[bin_depth[row][col].rv[i].rec].pfm, NVFalse, misc->slice))
                        {
                          //  Only update the record if the point is within the polygon

                          if (inside_polygon2 (mx, my, poly_count, misc->data[bin_depth[row][col].rv[i].rec].x, misc->data[bin_depth[row][col].rv[i].rec].y))
                            {
                              //  Check the point against the features (if any).  Only update the point if it is not within 
                              //  options->feature_radius of a feature and is not within any feature polygon whose feature is
                              //  in the filter area.  Also check against any temporary filter masked areas.

                              uint8_t filter_it = NVTrue;
                              for (int32_t j = 0 ; j < feature_count ; j++)
                                {
                                  double dist;

                                  geo_distance (feature[j].lat, feature[j].lon, misc->data[bin_depth[row][col].rv[i].rec].y, 
                                                misc->data[bin_depth[row][col].rv[i].rec].x, &dist);

                                  if (dist < options->feature_radius)
                                    {
                                      filter_it = NVFalse;
                                      break;
                                    }

                                  if (feature[j].poly_count && inside_polygon2 (feature[j].poly_x, feature[j].poly_y, feature[j].poly_count,
                                                                                misc->data[bin_depth[row][col].rv[i].rec].x,
                                                                                misc->data[bin_depth[row][col].rv[i].rec].y))
                                    {
                                      filter_it = NVFalse;
                                      break;
                                    }
                                }


                              if (filter_it)
                                {
                                  //  Last check.  If we are range limiting the Z data we don't want to actually invalidate a point outside
                                  //  our Z range.

                                  if (!options->filter_apply_z_range || (depth >= min_z && depth <= max_z))
                                    {
                                      try
                                        {
                                          misc->filter_kill_list.resize (misc->filter_kill_count + 1);
                                        }
                                      catch (std::bad_alloc&)
                                        {
                                          fprintf (stderr, "%s %s %s %d - filter_kill_list - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__,
                                                   strerror (errno));
                                          exit (-1);
                                        }

                                      misc->filter_kill_list[misc->filter_kill_count] = bin_depth[row][col].rv[i].rec;
                                      misc->filter_kill_count++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

      count++;
    }


  //  Recompute the bin record based on the modified contents of the depth array.

  compute_bin_values (&bin_depth[row][col], misc);


  return (NVTrue);
}



//!  This kicks off the pseudo-statistical filtering of a polygonal area.

uint8_t filterPolyArea (OPTIONS *options, MISC *misc, double *mx, double *my, int32_t count)
{
  //  Check to see if we're using OTF bins in pfmView.  If we are we want to use that bin size for filtering, not the average PFM bin size.

  if (misc->abe_share->otf_width)
    {
      x_bin_size = misc->abe_share->otf_x_bin_size;
      y_bin_size = misc->abe_share->otf_y_bin_size;
    }
  else
    {
      x_bin_size = misc->avg_x_bin_size_degrees;
      y_bin_size = misc->avg_y_bin_size_degrees;
    }


  //  Compute the bin diagonal distance for the filter.

  double bin_diagonal;
  double az;
  if (misc->abe_share->open_args[0].head.mbr.min_y <= 0.0)
    {
      invgp (NV_A0, NV_B0, misc->abe_share->open_args[0].head.mbr.max_y, misc->abe_share->open_args[0].head.mbr.min_x, 
             misc->abe_share->open_args[0].head.mbr.max_y - (y_bin_size), 
             misc->abe_share->open_args[0].head.mbr.min_x + (x_bin_size), &bin_diagonal, &az);
    }
  else
    {
      invgp (NV_A0, NV_B0, misc->abe_share->open_args[0].head.mbr.min_y, misc->abe_share->open_args[0].head.mbr.min_x, 
             misc->abe_share->open_args[0].head.mbr.min_y + (y_bin_size), 
             misc->abe_share->open_args[0].head.mbr.min_x + (x_bin_size), &bin_diagonal, &az);
    }


  //  Figure out the width, start row, start column, end row, and end column using the bin sizes from the level 0 PFM.

  bounds.min_x = 999999.0;
  bounds.min_y = 999999.0;
  bounds.max_x = -999999.0;
  bounds.max_y = -999999.0;
  for (int32_t i = 0 ; i < count ; i++)
    {
      bounds.min_x = qMin (bounds.min_x, mx[i]);
      bounds.min_y = qMin (bounds.min_y, my[i]);
      bounds.max_x = qMax (bounds.max_x, mx[i]);
      bounds.max_y = qMax (bounds.max_y, my[i]);
    }


  //  Add one grid cell distance around the area just to make sure we get everything

  bounds.min_y -= y_bin_size;
  bounds.max_y += y_bin_size;
  bounds.min_x -= x_bin_size;
  bounds.max_x += x_bin_size;


  int32_t width = (NINT ((bounds.max_x - bounds.min_x) / x_bin_size)) + 1;
  int32_t height = (NINT ((bounds.max_y - bounds.min_y) / y_bin_size)) + 1;


  if (!InitializeAreaFilter (width, height, misc))
    {
      if (feature_count)
        {
          feature.clear ();
          feature_count = 0;
        }
      return (NVFalse);
    }
      

  misc->statusProgLabel->setText (pfmEdit3D::tr ("Filtering..."));
  misc->statusProgLabel->setVisible (true);
  misc->statusProg->setRange (0, height);
  misc->statusProg->setTextVisible (true);
  qApp->processEvents();


  //  Note that we're not filtering the edge cells since we won't have surrounding data.

  for (int32_t i = 1 ; i < height - 1 ; i++)
    {
      misc->statusProg->setValue (i);


      for (int32_t j = 1 ; j < width - 1 ; j++)
        {
          AreaFilter (options, misc, bin_diagonal, i, j, mx, my, count);
        }


      //  Check the event queue to see if the user wants to interrupt the filter.  Only 
      //  check every 10th row so it doesn't slow things down too much.  */

      if (!(i % 10))
        {
          if (qApp->hasPendingEvents ())
            {
              qApp->processEvents();
              if (misc->drawing_canceled)
                {
                  misc->filter_kill_count = 0;
                  misc->statusProg->reset ();
                  misc->statusProg->setRange (0, 100);
                  misc->statusProg->setValue (0);
                  misc->statusProgLabel->setVisible (false);
                  misc->statusProg->setTextVisible (false);
                  misc->drawing_canceled = NVFalse;
                  qApp->processEvents();
                  return (NVFalse);
                }
            }
        }
    }

  misc->statusProg->reset ();
  misc->statusProg->setRange (0, 100);
  misc->statusProg->setValue (0);
  misc->statusProgLabel->setVisible (false);
  misc->statusProg->setTextVisible (false);

  qApp->processEvents();


  //  Free the memory used by the filter.

  for (int32_t i = 0 ; i < height ; i++)
    {
      if (bin_depth[i] != NULL) 
        {
          for (int32_t j = 0 ; j < width ; j++)
            {
              bin_depth[i][j].rv.clear ();
            }
          free (bin_depth[i]);
        }
    }


  //  Free the feature memory (if any)

  if (feature_count)
    {
      feature.clear ();
      feature_count = 0;
    }


  return (NVTrue);
}



uint8_t startHpFilterThread (OPTIONS *options, MISC *misc)
{
  //  Here we're going to make sure that the box is large enough in bins (rows/columns) to thread it.  There's no point in trying to
  //  break 4 bins up into two sections.  Our cutoff is 8 bins or 18 bins (for 4 or 16 threads respectively).

  int32_t width = 0, height = 0;

  if (rows < 8)
    {
      height = 1;
    }
  else if (rows < 18)
    {
      height = 2;
    }

  if (cols < 8)
    {
      width = 1;
    }
  else if (cols < 18)
    {
      width = 2;
    }


  //  Check to see if we just want one pass.  I added this to get rid of the standalone single pass function.

  if (options->num_hp_threads == 1) width = height = 1;


  //  If we haven't limited our width or height compute the number of blocks in width and height.  Since num_hp_threads will either be
  //  4 or 16 this will be either 2 or 4 respectively.

  int32_t num_half_blocks = NINT (sqrt ((double) options->num_hp_threads));

  if (!width) width = num_half_blocks;
  if (!height) height = num_half_blocks;


  int32_t adjusted_num_threads = width * height;


  hpFilterThread *hp_filter_thread = new hpFilterThread [adjusted_num_threads];


  std::vector<int32_t> *kill_list = (std::vector<int32_t> *) calloc (adjusted_num_threads, sizeof (std::vector<int32_t>));

  if (kill_list == NULL)
    {
      fprintf (stderr, "%s %s %s %d - kill_list - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  int32_t *kill_count = (int32_t *) calloc (adjusted_num_threads, sizeof (int32_t));

  if (kill_count == NULL)
    {
      fprintf (stderr, "%s %s %s %d - kill_count - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }


  //  We're starting all adjusted_num_threads filter passes concurrently.
 
  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          hp_filter_thread[ndx].filter (options, misc, bin_data, pos, rows, cols, width, height, i, j, &kill_list[ndx], &kill_count[ndx],
                                        &feature, feature_count);
        }
    }


  //  Wait until all of the threads are finished.  Since the Hockey Puck filter is an iterative process we can't really give a status so
  //  there's no need to play around with signals from the threadds.

  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          hp_filter_thread[ndx].wait ();
        }
    }


  delete[] hp_filter_thread;


  //  Copy the individual thread kill lists into the filter_kill_list.

  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          for (int32_t k = 0 ; k < kill_count[ndx] ; k++)
            {
              try
                {
                  misc->filter_kill_list.resize (misc->filter_kill_count + 1);
                }
              catch (std::bad_alloc&)
                {
                  fprintf (stderr, "%s %s %s %d - filter_kill_list - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__,
                           strerror (errno));
                  exit (-1);
                }

              misc->filter_kill_list[misc->filter_kill_count] = kill_list[ndx][k];
              misc->filter_kill_count++;
            }

          kill_list[ndx].clear ();
        }
    }


  //  Free all of the memory we allocated.

  free (kill_list);
  free (kill_count);

  for (int32_t i = 0 ; i < rows ; i++)
    {
      for (int32_t j = 0 ; j < cols ; j++)
        {
          bin_data[i][j].data.clear ();
        }
      free (bin_data[i]);
    }
  free (bin_data);

  free (pos);

  return (NVTrue);
}



uint8_t startAvtFilterThread (OPTIONS *options, MISC *misc)
{
  //  Here we're going to make sure that the box is large enough in bins (rows/columns) to thread it.  There's no point in trying to
  //  break 4 bins up into two sections.  Our cutoff is 8 bins or 18 bins (for 4 or 16 threads respectively).

  int32_t width = 0, height = 0;

  if (rows < 8)
    {
      height = 1;
    }
  else if (rows < 18)
    {
      height = 2;
    }

  if (cols < 8)
    {
      width = 1;
    }
  else if (cols < 18)
    {
      width = 2;
    }


  //  Check to see if we just want one pass.  I added this to get rid of the standalone single pass function.

  if (options->num_hp_threads == 1) width = height = 1;


  //  If we haven't limited our width or height compute the number of blocks in width and height.  Since num_hp_threads will either be
  //  4 or 16 this will be either 2 or 4 respectively.

  int32_t num_half_blocks = NINT (sqrt ((double) options->num_hp_threads));

  if (!width) width = num_half_blocks;
  if (!height) height = num_half_blocks;


  int32_t adjusted_num_threads = width * height;


  avtFilterThread *avt_filter_thread = new avtFilterThread [adjusted_num_threads];


  std::vector<int32_t> *kill_list = (std::vector<int32_t> *) calloc (adjusted_num_threads, sizeof (std::vector<int32_t>));

  if (kill_list == NULL)
    {
      fprintf (stderr, "%s %s %s %d - kill_list - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  int32_t *kill_count = (int32_t *) calloc (adjusted_num_threads, sizeof (int32_t));

  if (kill_count == NULL)
    {
      fprintf (stderr, "%s %s %s %d - kill_count - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }


  //  We're starting all adjusted_num_threads filter passes concurrently.
 
  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          avt_filter_thread[ndx].filter (options, misc, bin_data, pos, rows, cols, width, height, i, j, &kill_list[ndx], &kill_count[ndx],
                                         &feature, feature_count);
        }
    }


  //  Wait until all of the threads are finished.  Since the Hockey Puck filter is an iterative process we can't really give a status so
  //  there's no need to play araound with signals from the threadds.

  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          avt_filter_thread[ndx].wait ();
        }
    }


  delete[] avt_filter_thread;


  //  Copy the individual thread kill lists into the filter_kill_list.

  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          for (int32_t k = 0 ; k < kill_count[ndx] ; k++)
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

              misc->filter_kill_list[misc->filter_kill_count] = kill_list[ndx][k];
              misc->filter_kill_count++;
            }

          kill_list[ndx].clear ();
        }
    }


  //  Free all of the memory we allocated.

  free (kill_list);
  free (kill_count);

  for (int32_t i = 0 ; i < rows ; i++)
    {
      for (int32_t j = 0 ; j < cols ; j++)
        {
          bin_data[i][j].data.clear ();
        }
      free (bin_data[i]);
    }
  free (bin_data);

  free (pos);

  return (NVTrue);
}



uint8_t Filter (OPTIONS *options, MISC *misc, double *mx, double *my, int32_t count)
{
  uint8_t ret = NVFalse;


  //  Get the relevant feature information.

  for (int32_t pfm = misc->abe_share->pfm_count - 1 ; pfm >= 0 ; pfm--)
    {
      if (strcmp (misc->abe_share->open_args[pfm].target_path, "NONE"))
        {
          //  Get the feature information for those features inside the area.

          if (options->feature_radius > 0.0)
            {
              if (misc->bfd_open)
                {
                  feature_count = 0;

                  for (uint32_t i = 0 ; i < misc->bfd_header.number_of_records ; i++)
                    {
                      if (inside_polygon2 (mx, my, count, misc->feature[i].longitude, misc->feature[i].latitude))
                        {
                          try
                            {
                              feature.resize (feature_count + 1);
                            }
                          catch (std::bad_alloc&)
                            {
                              fprintf (stderr, "%s %s %s %d - feature - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
                              exit (-1);
                            }

                          feature[feature_count].lat = misc->feature[i].latitude;
                          feature[feature_count].lon = misc->feature[i].longitude;
                          feature[feature_count].poly_count = misc->feature[i].poly_count;

                          if (misc->feature[i].poly_count && misc->feature[i].poly_type)
                            {
                              BFDATA_POLYGON bfd_polygon;

                              binaryFeatureData_read_polygon (misc->bfd_handle, i, &bfd_polygon);

                              for (uint32_t j = 0 ; j < misc->feature[i].poly_count ; j++)
                                {
                                  feature[feature_count].poly_y[j] = bfd_polygon.latitude[j];
                                  feature[feature_count].poly_x[j] = bfd_polygon.longitude[j];
                                }
                            }

                          feature_count++;
                        }
                    }
                }
            }
        }
    }


  //  These are the "flipped" values for when the user is displaying elevation instead of depth.

  min_z = options->filter_min_z;
  max_z = options->filter_max_z;
  if (options->z_orientation < 0.0)
    {
      min_z = -options->filter_max_z;
      max_z = -options->filter_min_z;
    }


  //  If we're running anything other than the pseudo-statstical filter we're going to need the position (in meters) of all the 
  //  points plus a bunch of pseudo-bins based on the relevant radius (hp or stat).

  if (options->filter_type != AREA_FILTER)
    {
      float min_z_buffer = min_z - (1.5 * options->hp_filter_height);
      float max_z_buffer = max_z + (1.5 * options->hp_filter_height);

      pos = (FILTER_POS_DATA *) calloc (misc->abe_share->point_cloud_count, sizeof (FILTER_POS_DATA));

      if (pos == NULL)
        {
          fprintf (stderr, "%s %s %s %d - pos - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
          exit (-1);
        }


      //  Populate the position array with the position of the point in meters from the southwest corner of the area.  This allows us to
      //  directly compute distance between points using just the sqrt function.  Since we will be comparing distances between every point
      //  in a 9 bin area against every other point in a 9 bin area, and doing that for every bin in the entire area, this will save loads
      //  of time.

      uint8_t fast = NVTrue;
      if (misc->abe_share->edit_area.min_y > 64.0 || misc->abe_share->edit_area.max_y < -64.0) fast = NVFalse;


      for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
        {
          //  Fast approximation for data inside 64N/64S.  This is usually good to within a few millimeters to a few centimeters 
          //  depending on the bin size.

          if (fast)
            {
              pos[i].x = ((misc->data[i].x - misc->abe_share->edit_area.min_x) / misc->abe_share->open_args[0].head.x_bin_size_degrees) *
                misc->abe_share->open_args[0].head.bin_size_xy;
              pos[i].y = ((misc->data[i].y - misc->abe_share->edit_area.min_y) / misc->abe_share->open_args[0].head.y_bin_size_degrees) *
                misc->abe_share->open_args[0].head.bin_size_xy;
            }
          else
            {
              double x, y, az;
              invgp (NV_A0, NV_B0, misc->abe_share->edit_area.min_y, misc->abe_share->edit_area.min_x, misc->abe_share->edit_area.min_y,
                     misc->data[i].x, &x, &az);
              pos[i].x = (float) x;
              invgp (NV_A0, NV_B0, misc->abe_share->edit_area.min_y, misc->abe_share->edit_area.min_x, misc->data[i].y,
                     misc->abe_share->edit_area.min_x, &y, &az);
              pos[i].y = (float) y;
            }

          pos[i].count = 0;
          pos[i].killed = NVFalse;
        }


      //  We need to build an array of bins (twice the size of the search radius) so that we can efficiently perform the dreaded
      //  Hockey Puck of Confidence (TM) proximity valid point search.

      double search_bin_size_meters;
      if (options->filter_type == STAT_FILTER)
        {
          search_bin_size_meters = options->stat_filter_radius * 2.0;
        }
      else
        {
          search_bin_size_meters = options->hp_filter_radius * 2.0;
        }
      double width_meters, height_meters, az;


      invgp (NV_A0, NV_B0, misc->abe_share->edit_area.min_y, misc->abe_share->edit_area.min_x, misc->abe_share->edit_area.max_y,
             misc->abe_share->edit_area.min_x, &height_meters, &az);
      invgp (NV_A0, NV_B0, misc->abe_share->edit_area.min_y, misc->abe_share->edit_area.min_x, misc->abe_share->edit_area.min_y,
             misc->abe_share->edit_area.max_x, &width_meters, &az);


      //  Plus 2 to get all the way to the right and top of the area.

      rows = (int32_t) (height_meters / search_bin_size_meters) + 2;
      cols = (int32_t) (width_meters / search_bin_size_meters) + 2;

      bin_data = (FILTER_BIN_DATA **) calloc (rows, sizeof (FILTER_BIN_DATA *));
      if (bin_data == NULL)
        {
          fprintf (stderr, "%s %s %s %d - bin_data - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
          exit (-1);
        }

      for (int32_t i = 0 ; i < rows ; i++)
        {
          bin_data[i] = (FILTER_BIN_DATA *) calloc (cols, sizeof (FILTER_BIN_DATA));
          if (bin_data[i] == NULL)
            {
              fprintf (stderr, "%s %s %s %d - bin_data[i] - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }
        }


      //  Now let's load the record pointers into the bin array.

      for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
        {
          //  If we're limiting the Z range, we can ignore points that are more than 1.5 times options->hp_filter_height outside the
          //  Z range.  We'll use points inside 1.5 times options->hp_filter_height outside of the range to validate the points
          //  inside the range.

          if (options->filter_type == STAT_FILTER || !options->filter_apply_z_range ||
              (misc->data[i].z <= max_z_buffer && misc->data[i].z >= min_z_buffer))
            {
              int32_t row = (int32_t) (pos[i].y / search_bin_size_meters);
              int32_t col = (int32_t) (pos[i].x / search_bin_size_meters);

              try
                {
                  bin_data[row][col].data.resize (bin_data[row][col].count + 1);
                }
              catch (std::bad_alloc&)
                {
                  fprintf (stderr, "%s %s %s %d - bin_data[row][col].data - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__,
                           strerror (errno));
                  exit (-1);
                }

              bin_data[row][col].data[bin_data[row][col].count] = i;
              bin_data[row][col].count++;
            }
        }
    }


  switch (options->filter_type)
    {
    case AREA_FILTER:
      misc->statusProgLabel->setText (pfmEdit3D::tr ("Running area-based filter"));
      misc->statusProgLabel->setVisible (true);
      qApp->processEvents ();

      ret = filterPolyArea (options, misc, mx, my, count);
      break;

    case STAT_FILTER:
      misc->statusProgLabel->setText (pfmEdit3D::tr ("Running statistical filter"));
      misc->statusProgLabel->setVisible (true);
      qApp->processEvents ();

      ret = startAvtFilterThread (options, misc);
      break;

    case HP_FILTER:
      misc->statusProgLabel->setText (pfmEdit3D::tr ("Running Hockey Puck filter"));
      misc->statusProgLabel->setVisible (true);
      qApp->processEvents ();

      ret = startHpFilterThread (options, misc);
      break;
    }


  misc->statusProgLabel->setVisible (true);
  misc->statusProg->setTextVisible (true);
  qApp->processEvents();


  return (ret);
}
