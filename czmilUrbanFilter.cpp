
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
#include "czmilUrbanVicinityThread.hpp"
#include "czmilUrbanFilterThread.hpp"

uint8_t czmilUrbanFilter (OPTIONS *options, MISC *misc)
{
  int32_t          rows = 0, cols = 0;
  FILTER_POS_DATA  *pos;
  FILTER_BIN_DATA  **bin_data;


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
      int32_t urban = NINT (misc->data[i].attr[misc->czmil_urban_attr]);


      pos[i].data = NVFalse;


      //  We're only going to load valid, topo data since invalid data and/or bathy data are not used.
      //  Yes, I know, we're wasting memory ... but we're saving time!

      if (!(misc->data[i].val & (PFM_INVAL | PFM_DELETED | PFM_REFERENCE)) && (urban & CZMIL_URBAN_TOPO))
        {
          //  Don't use data flagged as CZMIL_URBAN_SOFT_HIT.

          if (!(urban & (CZMIL_URBAN_SOFT_HIT)))
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

              pos[i].data = NVTrue;
              pos[i].count = 0;
              pos[i].killed = NVFalse;
            }
        }
    }



  //****************************************** Vicinity search *********************************************


  //  We need to build an array of bins (twice the size of the search radius) so that we can efficiently search for vicinity points.

  double search_bin_size_meters = options->czmil_urban_radius[0] * 2.0;

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
      if (pos[i].data)
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


  //  I'm using this title since the vicinity search is usually done in a couple of seconds.

  misc->statusProgLabel->setText (pfmEdit3D::tr ("Running CZMIL urban noise filter"));
  misc->statusProgLabel->setVisible (true);
  qApp->processEvents ();


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


  //  First we have to run the czmilUrbanVicinityThreads to expand the urban feature areas.

  czmilUrbanVicinityThread *czmil_urban_vicinity_thread = new czmilUrbanVicinityThread [adjusted_num_threads];


  //  We're starting all adjusted_num_threads vicinity passes concurrently.
 
  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          czmil_urban_vicinity_thread[ndx].vicinity (options, misc, bin_data, pos, rows, cols, width, height, i, j);
        }
    }


  //  Wait until all of the threads are finished.

  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          czmil_urban_vicinity_thread[ndx].wait ();
        }
    }


  delete[] czmil_urban_vicinity_thread;


  //  Free all of the memory we allocated.

  for (int32_t i = 0 ; i < rows ; i++)
    {
      for (int32_t j = 0 ; j < cols ; j++)
        {
          bin_data[i][j].data.clear ();
        }
      free (bin_data[i]);
    }
  free (bin_data);



  //*************************************** Urban noise filter *********************************************


  //  We need to build an array of bins (twice the size of the search radius) so that we can efficiently search for filter points.

  search_bin_size_meters = options->czmil_urban_radius[1] * 2.0;


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
      if (pos[i].data)
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


  //  Here we're going to make sure that the box is large enough in bins (rows/columns) to thread it.  There's no point in trying to
  //  break 4 bins up into two sections.  Our cutoff is 8 bins or 18 bins (for 4 or 16 threads respectively).

  width = height = 0;

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

  num_half_blocks = NINT (sqrt ((double) options->num_hp_threads));

  if (!width) width = num_half_blocks;
  if (!height) height = num_half_blocks;


  adjusted_num_threads = width * height;


  czmilUrbanFilterThread *czmil_urban_filter_thread = new czmilUrbanFilterThread [adjusted_num_threads];


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


  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          czmil_urban_filter_thread[ndx].filter (options, misc, bin_data, pos, rows, cols, width, height, i, j, &kill_list[ndx],
                                                 &kill_count[ndx], ndx);
        }
    }


  //  Wait until all of the threads are finished.

  for (int32_t i = 0 ; i < height ; i++)
    {
      for (int32_t j = 0 ; j < width ; j++)
        {
          int32_t ndx = i * width + j;

          czmil_urban_filter_thread[ndx].wait ();
        }
    }


  delete[] czmil_urban_filter_thread;


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
