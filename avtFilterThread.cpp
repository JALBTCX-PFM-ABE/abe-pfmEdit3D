
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



#include "avtFilterThread.hpp"

avtFilterThread::avtFilterThread (QObject *parent)
  : QThread(parent)
{
}



avtFilterThread::~avtFilterThread ()
{
}



void avtFilterThread::filter (OPTIONS *op, MISC *mi, FILTER_BIN_DATA **bd, FILTER_POS_DATA *pd, int32_t rws, int32_t cls, int32_t w, int32_t h, int32_t rw,
                              int32_t cl, std::vector<int32_t> *kl, int32_t *kc, std::vector<FEATURE_RECORD> *fr, int32_t fc)
{
  QMutexLocker locker (&mutex);

  l_options = op;
  l_misc = mi;
  l_bin_data = bd;
  l_pos = pd;
  l_rows = rws;
  l_cols = cls;
  l_width = w;
  l_height = h;
  l_row = rw;
  l_col = cl;
  l_kill_list = kl;
  l_kill_count = kc;
  l_feature = fr;
  l_feature_count = fc;

  if (!isRunning ()) start ();
}



void avtFilterThread::run ()
{
  mutex.lock ();

  OPTIONS *options = l_options;
  MISC *misc = l_misc;
  FILTER_BIN_DATA **bin_data = l_bin_data;
  FILTER_POS_DATA *pos = l_pos;
  int32_t rows = l_rows;
  int32_t cols = l_cols;
  int32_t width = l_width;
  int32_t height = l_height;
  int32_t row = l_row;
  int32_t col = l_col;
  std::vector<int32_t> *kill_list = l_kill_list;
  int32_t *kill_count = l_kill_count;
  std::vector<FEATURE_RECORD> *feature = l_feature;
  int32_t feature_count = l_feature_count;


  mutex.unlock ();


  //  Compute the number of bin rows and columns in this block.

  int32_t bin_width = cols / width;
  int32_t bin_height = rows / height;


  //  Compute the start and end bin rows and columns for this thread.

  int32_t start_row = qMax (bin_height * row, 0);
  int32_t start_col = qMax (bin_width * col, 0);
  int32_t end_row = qMin (bin_height * (row + 1), rows - 1);
  int32_t end_col = qMin (bin_width * (col + 1), cols - 1);


  //  These are the "flipped" values for when the user is displaying elevation instead of depth.

  float min_z = options->filter_min_z;
  float max_z = options->filter_max_z;
  if (options->z_orientation < 0.0)
    {
      min_z = -options->filter_max_z;
      max_z = -options->filter_min_z;
    }


  //  Determine which points need to be deleted.  This uses a standard AVT algorithm except with a variable number of standard
  //  deviations.  We only want to search in one bin around the current bin.  This means we'll search 9 total bins and that should
  //  give us enough nearby data for any point in the center bin.  We have to run through this process iteratively since a point may
  //  be deemed good based on nearby points that might later be deemed invalid.  To do this we run the loop until we
  //  don't kill any more points or we reach the required number of passes.

  int32_t new_killed = 0, pass = 0;
  do
    {
      new_killed = 0;

      for (int32_t i = start_row ; i < end_row ; i++)
        {
          //  Compute the start and end Y bins for the 9 bin block (min = 0, max = rows - 1).  Note that we're checking against rows - 1 and not
          //  end_row - 1.  This allows the filter to work on the last row in this block while still checking against the first row in the next
          //  block.  By the same token, using 0 - 1 instead of start_row - 1 allows filtering of the first row while checking against the last
          //  row in the previous block.

          int32_t start_y = qMax (0, i - 1);
          int32_t end_y = qMin (rows - 1, i + 1);

          for (int32_t j = start_col ; j < end_col ; j++)
            {
              //  No point in checking if we have no data points in the bin.

              if (bin_data[i][j].count)
                {
                  //  Compute the start and end X bins for the 9 bin block (min = 0, max = cols - 1).  Note that we're checking against cols - 1
                  //  and not end_col - 1.  This allows the filter to work on the last column in this block while still checking against the first
                  //  column in the next block.  By the same token, using 0 - 1 instead of start_col - 1 allows filtering of the first column
                  //  while checking against the last column in the previous block.

                  int32_t start_x = qMax (0, j - 1);
                  int32_t end_x = qMin (cols - 1, j + 1);


                  //  Loop through the current bin checking against all points in any of the 9 bins.

                  for (int32_t k = 0 ; k < bin_data[i][j].count ; k++)
                    {
                      int32_t ndx = bin_data[i][j].data[k];

                      pos[ndx].count = 0;


                      //  If the point is already invalid we certainly don't need to check it.

                      if (!(misc->data[ndx].val & (PFM_INVAL | PFM_DELETED | PFM_REFERENCE)) && !pos[ndx].killed)
                        {
                          double sum = 0.0, sum2 = 0.0;
                          int32_t sum_count = 0;


                          //  Y bin block loop.

                          for (int32_t m = start_y ; m <= end_y ; m++)
                            {
                              //  X bin block loop.

                              for (int32_t n = start_x ; n <= end_x ; n++)
                                {
                                  //  Loop though all points in the bin.

                                  for (int32_t p = 0 ; p < bin_data[m][n].count ; p++)
                                    {
                                      int32_t indx = bin_data[m][n].data[p];


                                      //  Don't check against invalid data.

                                      if (!(misc->data[indx].val & (PFM_INVAL | PFM_DELETED | PFM_REFERENCE)) && !pos[indx].killed)
                                        {
                                          //  Simple check for exceeding distance in X or Y direction (prior to a radius check).

                                          double diff_x = fabs (pos[ndx].x - pos[indx].x);
                                          double diff_y = fabs (pos[ndx].y - pos[indx].y);


                                          //  The radius plus both horizontal uncertainties.

                                          double dist = options->stat_filter_radius;// + misc->data[ndx].herr + misc->data[indx].herr;


                                          //  Simple check for either X or Y exceeding our distance (so we don't do too many SQRTs)

                                          if (diff_x <= dist && diff_y <= dist)
                                            {
                                              double pnt_dist = sqrt (diff_x * diff_x + diff_y * diff_y);


                                              //  Check the distance.

                                              if (pnt_dist <= dist)
                                                {
                                                  //  Sum the data and squares.

                                                  sum += misc->data[indx].z;
                                                  sum2 += (misc->data[indx].z * misc->data[indx].z);
                                                  sum_count++;
                                                }
                                            }
                                        }
                                    }
                                }
                            }



                          //  Compute the standard deviation.

                          if (sum_count >= options->stat_filter_count)
                            {
                              double std = 0.0;

                              double avg = sum / (double) sum_count;

                              double variance = ((sum2 - ((double) sum_count * (pow (avg, 2.0)))) / ((double) sum_count - 1.0));

                              if (variance >= 0.0) std = sqrt (variance);


                              //  Check the point against the computed average using the computed standard deviation.

                              if (fabs (misc->data[ndx].z - avg) > options->stat_filter_std * std)
                                {
                                  //  Check for single line display.

                                  if (!misc->num_lines || check_line (misc, misc->data[ndx].line))
                                    {
                                      //  Don't remove hidden data.

                                      if (!check_bounds (options, misc, misc->data[ndx].x, misc->data[ndx].y, misc->data[ndx].z, misc->data[ndx].val,
                                                         misc->data[ndx].mask, 0, NVTrue, misc->slice))
                                        {
                                          //  Last check.  If we are range limiting the Z data we don't want to actually invalidate a point outside
                                          //  our Z range.  We still want to mark the temporary invalid flag though so that we won't use the point
                                          //  to validate points inside our Z range.

                                          if (!options->filter_apply_z_range || (misc->data[ndx].z >= min_z && misc->data[ndx].z <= max_z))
                                            {
                                              //  Check the point against the features (if any).  Only update the point if it is not within 
                                              //  options->feature_radius of a feature and is not within any feature polygon whose feature is
                                              //  in the filter area.  Also check against any temporary filter masked areas.

                                              uint8_t filter_it = NVTrue;
                                              for (int32_t j = 0 ; j < feature_count ; j++)
                                                {
                                                  double dist;

                                                  geo_distance (feature->at (j).lat, feature->at(j).lon, misc->data[ndx].y, misc->data[ndx].x, &dist);

                                                  if (dist < options->feature_radius)
                                                    {
                                                      filter_it = NVFalse;
                                                      break;
                                                    }

                                                  if (feature->at (j).poly_count && inside_polygon2 (feature->at (j).poly_x, feature->at (j).poly_y,
                                                                                                     feature->at (j).poly_count, misc->data[ndx].x,
                                                                                                     misc->data[ndx].y))
                                                    {
                                                      filter_it = NVFalse;
                                                      break;
                                                    }
                                                }


                                              if (filter_it)
                                                {
                                                  try
                                                    {
                                                      kill_list->resize (*kill_count + 1);
                                                    }
                                                  catch (std::bad_alloc&)
                                                    {
                                                      fprintf (stderr, "%s %s %s %d - kill_list - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__,
                                                               strerror (errno));
                                                      exit (-1);
                                                    }

                                                  kill_list->at(*kill_count) = ndx;
                                                  (*kill_count)++;


                                                  //  Add one to the while loop's kill count so we'll know when to stop.

                                                  new_killed++;
                                                }
                                            }
                                        }


                                      //  Even though the point may be hidden we still want to set the temporarily invalid flag so we won't use it to
                                      //  validate points that aren't hidden.

                                      pos[ndx].killed = NVTrue;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


      pass++;
      if (options->stat_filter_passes && pass >= options->stat_filter_passes) break;


    } while (new_killed);
}
