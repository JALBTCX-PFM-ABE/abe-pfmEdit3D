
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




/*!<  So, what, exactly, is czmilUrbanVicinityThread doing?  Well, when pfmLoader builds a PFM from
      CZMIL data and the "CZMIL Urban noise flags" attribute is loaded, it sets a flag for any return
      that is in a shot that has all of its channels processed as topo (CZMIL_URBAN_TOPO).  It also
      sets a flag for any return, on a topo only shot, that has only one or two valid returns
      (CZMIL_URBAN_HARD_HIT).  The other thing it does is to set yet another flag for any return on a
      topo processed shot where the majority (4 or more of 7 channels) of the channels have 3 or
      fewer valid returns (CZMIL_URBAN_MAJORITY).  What that does is to define urban features.  In
      order to expand the area of urban features, czmilUrbanVicinityThread searches within a user
      specified distance (horizontal and vertical) of returns marked as CZMIL_URBAN_HARD_HIT for
      returns that are marked as CZMIL_URBAN_MAJORITY.  When it finds these returns it marks them as
      CZMIL_URBAN_VICINITY.  This is a precursor to czmilUrbanFilterThread where the actual filtering
      takes place.  */


#include "czmilUrbanVicinityThread.hpp"

czmilUrbanVicinityThread::czmilUrbanVicinityThread (QObject *parent)
  : QThread(parent)
{
}



czmilUrbanVicinityThread::~czmilUrbanVicinityThread ()
{
}



void czmilUrbanVicinityThread::vicinity (OPTIONS *op, MISC *mi, FILTER_BIN_DATA **bd, FILTER_POS_DATA *pd, int32_t rws, int32_t cls, int32_t w,
                                         int32_t h, int32_t rw, int32_t cl)
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

  if (!isRunning ()) start ();
}



void czmilUrbanVicinityThread::run ()
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


  mutex.unlock ();


  //  Compute the number of bin rows and columns in this block.

  int32_t bin_width = cols / width;
  int32_t bin_height = rows / height;


  //  Compute the start and end bin rows and columns for this thread.

  int32_t start_row = qMax (bin_height * row, 0);
  int32_t start_col = qMax (bin_width * col, 0);
  int32_t end_row = qMin (bin_height * (row + 1), rows - 1);
  int32_t end_col = qMin (bin_width * (col + 1), cols - 1);


  float hp_half_height = options->czmil_urban_height[0] / 2.0;


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
                  uint8_t quota = NVFalse;

                  int32_t ndx = bin_data[i][j].data[k];
                  int32_t urban = NINT (misc->data[ndx].attr[misc->czmil_urban_attr]);


                  //  Only use data flagged as CZMIL_URBAN_HARD_HIT that isn't already flagged as CZMIL_URBAN_MAJORITY
                  //  and/or CZMIL_URBAN_VICINITY.

                  if ((urban & CZMIL_URBAN_HARD_HIT) && !(urban & (CZMIL_URBAN_MAJORITY | CZMIL_URBAN_VICINITY)))
                    {
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


                                  //  Don't check against itself.

                                  if (ndx != indx)
                                    {
                                      //  Simple check for exceeding distance in X or Y direction (prior to a radius check).

                                      double diff_x = fabs (pos[ndx].x - pos[indx].x);
                                      double diff_y = fabs (pos[ndx].y - pos[indx].y);


                                      //  The radius plus both horizontal uncertainties.

                                      double dist = options->czmil_urban_radius[0];// + misc->data[ndx].herr + misc->data[indx].herr;


                                      //  Simple check for either X or Y exceeding our distance (so we don't do too many SQRTs)

                                      if (diff_x <= dist && diff_y <= dist)
                                        {
                                          double pnt_dist = sqrt (diff_x * diff_x + diff_y * diff_y);


                                          //  Check the distance.

                                          if (pnt_dist <= dist)
                                            {
                                              //  Check the Z difference.

                                              if (fabs (misc->data[ndx].z - misc->data[indx].z) < hp_half_height)
                                                {
                                                  //  All we need to look for is a single point in the hockey puck that is flagged as 
                                                  //  CZMIL_URBAN_MAJORITY.  When we find one of those we can mark this point to be
                                                  //  flagged as CZMIL_URBAN_VICINITY so that it can be used for filtering later.

                                                  if (NINT (misc->data[indx].attr[misc->czmil_urban_attr]) & CZMIL_URBAN_MAJORITY)
                                                    {
                                                      misc->data[ndx].attr[misc->czmil_urban_attr] =
                                                        (float) (NINT (misc->data[ndx].attr[misc->czmil_urban_attr]) | CZMIL_URBAN_VICINITY);
                                                      quota = NVTrue;
                                                      break;
                                                    }
                                                }
                                            }
                                        }
                                    }


                                  //  Already met our quota so stop the X bin block loop.

                                  if (quota) break;
                                }


                              //  Already met our quota so stop the Y bin block loop.

                              if (quota) break;
                            }
                        }
                    }
                }
            }
        }
    }
}
