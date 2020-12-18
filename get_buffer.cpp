
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


/***************************************************************************/
/*!

  - Module Name:        get_buffer

  - Programmer(s):      Jan C. Depner

  - Date Written:       May 1999

  - Purpose:            Read the point cloud data for the area defined in
                        shared memory (passed from pfmView or pfmEditShell).
                        Populate the shared memory point cloud buffer with
                        the data.  We also get unique data types and set some
                        flags to indicate if particular data types are present.

  - Return Value:       void

****************************************************************************/

void get_buffer (MISC *misc, OPTIONS *options, uint8_t  reload_flag)
{
  NV_I32_COORD2   coord;
  int32_t         recnum;
  NV_F64_COORD2   xy;
  uint8_t         found;
  DEPTH_RECORD    *depth;


  //  Check for the max number of index attributes for any of the PFM files.  Compute average bin size
  //  for all of the PFM files.

  misc->max_attr = 0;
  misc->x_grid_size = 0.0;
  misc->y_grid_size = 0.0;


  for (int32_t pfm = 0 ; pfm < misc->abe_share->pfm_count ; pfm++)
    {
      misc->max_attr = qMax (misc->abe_share->open_args[pfm].head.num_ndx_attr, misc->max_attr);

      misc->x_grid_size += misc->abe_share->open_args[pfm].head.x_bin_size_degrees;
      misc->y_grid_size += misc->abe_share->open_args[pfm].head.y_bin_size_degrees;
    }


  misc->x_grid_size /= (double) misc->abe_share->pfm_count;
  misc->y_grid_size /= (double) misc->abe_share->pfm_count;


  //  Clear some important variables

  memset (misc->line_number, 0, sizeof (misc->line_number));
  misc->line_count = 0;
  misc->abe_share->point_cloud_count = 0;

  misc->min_z = 99999999.0;
  misc->max_z = -99999999.0;
  int16_t *data_type_lut = NULL;


  //  Build a lookup table for the file types and get the unique data types.

  data_type_lut = (int16_t *) calloc (MAX_PFM_FILES * PFM_MAX_FILES, sizeof (int16_t));
  if (data_type_lut == NULL)
    {
      fprintf (stderr, "%s %s %s %d - data_type_lut - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  misc->unique_count = 0;

  int32_t point_count = 0;

  for (int32_t pfm = misc->abe_share->pfm_count - 1 ; pfm >= 0 ; pfm--)
    {
      //  File types.

      int32_t last = get_next_list_file_number (misc->pfm_handle[pfm]);


      for (int32_t list = 0 ; list < last ; list++)
        {
          char tmp[512];

          read_list_file (misc->pfm_handle[pfm], list, tmp, &data_type_lut[pfm * MAX_PFM_FILES + list]);


          //  Unique data types.

          uint8_t hit = NVFalse;
          for (int32_t i = 0 ; i < misc->unique_count ; i++)
            {
              if (misc->unique_type[i] == data_type_lut[pfm * MAX_PFM_FILES + list])
                {
                  hit = NVTrue;
                  break;
                }
            }

          if (!hit)
            {
              misc->unique_type[misc->unique_count] = data_type_lut[pfm * MAX_PFM_FILES + list];
              misc->unique_count++;
            }
        }


      //  Check for LIDAR and/or GSF data so we can determine what to do with LIDAR and/or GSF tool bar buttons.

      for (int32_t i = 0 ; i < misc->unique_count ; i++)
        {
          if (misc->unique_type[i] == PFM_GSF_DATA) misc->gsf_present = NVTrue;

          if (misc->unique_type[i] == PFM_CHARTS_HOF_DATA) misc->hof_present = NVTrue;

          if (misc->unique_type[i] == PFM_CHARTS_HOF_DATA || misc->unique_type[i] == PFM_SHOALS_1K_DATA ||
              misc->unique_type[i] == PFM_CZMIL_DATA) misc->hydro_lidar_present = NVTrue;

          if (misc->unique_type[i] == PFM_CHARTS_HOF_DATA || misc->unique_type[i] == PFM_SHOALS_1K_DATA ||
              misc->unique_type[i] == PFM_SHOALS_TOF_DATA || misc->unique_type[i] == PFM_CZMIL_DATA) misc->lidar_present = NVTrue;

          if (misc->unique_type[i] == PFM_CZMIL_DATA) misc->czmil_present = NVTrue;
          if (misc->unique_type[i] == PFM_LAS_DATA) misc->las_present = NVTrue;
        }


      //  Read the buffer.

      xy.y = misc->abe_share->edit_area.min_y;
      xy.x = misc->abe_share->edit_area.min_x;

      compute_index_ptr (xy, &misc->ll[pfm], &misc->abe_share->open_args[pfm].head);

      if (misc->ll[pfm].x < 0) misc->ll[pfm].x = 0;
      if (misc->ll[pfm].y < 0) misc->ll[pfm].y = 0;


      xy.y = misc->abe_share->edit_area.max_y;
      xy.x = misc->abe_share->edit_area.max_x;
        
      compute_index_ptr (xy, &misc->ur[pfm], &misc->abe_share->open_args[pfm].head);

      if (misc->ur[pfm].x > misc->abe_share->open_args[pfm].head.bin_width - 1)
        misc->ur[pfm].x = misc->abe_share->open_args[pfm].head.bin_width - 1;
      if (misc->ur[pfm].y > misc->abe_share->open_args[pfm].head.bin_height - 1)
        misc->ur[pfm].y = misc->abe_share->open_args[pfm].head.bin_height - 1;


      misc->null_val[pfm] = misc->abe_share->open_args[pfm].head.null_depth;


      //  Determine the floating point format for the attributes.

      for (int32_t i = 0 ; i < misc->max_attr ; i++)
        {
          int32_t decimals = (int32_t) (log10 (misc->abe_share->open_args[pfm].head.ndx_attr_scale[i]) + 0.05);
          sprintf (misc->attr_format[pfm][i], "%%.%df", decimals);
        }


      //  In order to determine the data size up front (so we can put it in shared memory for other processes to access) we
      //  have to pre-scan the bin records to get the number of points in each.  This shouldn't take too long though and it
      //  saves us a ton of memory usage in some ancillary programs.  Note that the point count computed here will NOT be the
      //  same as the actual point count.  This is because PFM_DELETED points won't get loaded, and/or PFM_INVAL points won't
      //  get loaded if the no_invalid option is set, and/or PFM_REFERENCE points won't get loaded if the no_reference option
      //  is set, even though these are counted in the num_soundings part of the bin record.
      //  In the case of no_invalid and no_reference we're probably wasting a bunch of memory space but the editor will run a
      //  hell of a lot faster.

      int32_t row_width = (misc->ur[pfm].x - misc->ll[pfm].x) + 1;

      if (row_width > 0)
        {
          BIN_RECORD *row = (BIN_RECORD *) calloc (row_width, sizeof (BIN_RECORD));
          if (row == NULL)
            {
              fprintf (stderr, "%s %s %s %d - row - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }


          for (int32_t i = misc->ll[pfm].y ; i <= misc->ur[pfm].y ; i++)
            {
              read_bin_row (misc->pfm_handle[pfm], row_width, i, misc->ll[pfm].x, row);

              double slat = misc->abe_share->open_args[pfm].head.mbr.min_y + (double) i *
                misc->abe_share->open_args[pfm].head.y_bin_size_degrees;
              double nlat = slat + misc->abe_share->open_args[pfm].head.y_bin_size_degrees;

              for (int32_t j = 0 ; j < row_width ; j++)
                {
                  //  For polygons we can eliminate bins that are completely outside of the polygon.  For rectangles we're just going
                  //  to live with the extra slop around the edges.

                  if (misc->abe_share->polygon_count)
                    {
                      double wlon = misc->abe_share->open_args[pfm].head.mbr.min_x + (double) (j + misc->ll[pfm].x) *
                        misc->abe_share->open_args[pfm].head.x_bin_size_degrees;
                      double elon = wlon + misc->abe_share->open_args[pfm].head.x_bin_size_degrees;

                      if (inside_polygon2 (misc->abe_share->polygon_x, misc->abe_share->polygon_y, misc->abe_share->polygon_count, wlon, slat)
                          ||
                          inside_polygon2 (misc->abe_share->polygon_x, misc->abe_share->polygon_y, misc->abe_share->polygon_count, wlon, nlat)
                          ||
                          inside_polygon2 (misc->abe_share->polygon_x, misc->abe_share->polygon_y, misc->abe_share->polygon_count, elon, nlat)
                          ||
                          inside_polygon2 (misc->abe_share->polygon_x, misc->abe_share->polygon_y, misc->abe_share->polygon_count, elon, slat))
                        {
                          point_count += row[j].num_soundings;
                        }
                    }
                  else
                    {
                      point_count += row[j].num_soundings;
                    }
                }
            }

          free (row);
        }
    }


  //  Check to see if the point cloud memory already exists (due to a previous editor crashed).  If so, attach it, delete it, and then
  //  create it again.  The key is the parent process ID (the one used for the main ABE shared memory area) plus "_abe_pfmEdit".  

  QString key;
  key.sprintf ("%d_abe_pfmEdit", misc->abe_share->ppid);

  misc->dataShare = new QSharedMemory (key);

  if (!misc->dataShare->create (sizeof (POINT_CLOUD) * point_count, QSharedMemory::ReadWrite))
    {
      misc->dataShare->attach (QSharedMemory::ReadWrite);
      misc->dataShare->detach ();

      if (!misc->dataShare->create (sizeof (POINT_CLOUD) * point_count, QSharedMemory::ReadWrite))
        {
          QString msg = pfmEdit3D::tr ("%1 %2 %3 %4 - Unable to allocate %5 points (%6 bytes) of shared data memory.  Reason:%7").arg
            (misc->progname).arg (__FILE__).arg (__FUNCTION__).arg (__LINE__).arg (point_count).arg
            (sizeof (POINT_CLOUD) * point_count).arg (misc->dataShare->errorString ());

          QMessageBox::critical (0, pfmEdit3D::tr ("pfmEdit3D get_buffer"), msg);

          qDebug () << msg;
          exit (-1);
        }
    }

  misc->data = (POINT_CLOUD *) misc->dataShare->data ();


  //  Go ahead and lock it.  We'll unlock it if we need to share with an external program.

  misc->dataShare->lock ();


  //  Now we have to load the points that are inside our area of interest.

  for (int32_t pfm = misc->abe_share->pfm_count - 1 ; pfm >= 0 ; pfm--)
    {
      for (int32_t i = misc->ll[pfm].y ; i <= misc->ur[pfm].y ; i++)
        {
          coord.y = i;
          for (int32_t j = misc->ll[pfm].x ; j <= misc->ur[pfm].x ; j++)
            {
              coord.x = j;

              if (!read_depth_array_index (misc->pfm_handle[pfm], coord, &depth, &recnum))
                {
                  for (int32_t k = 0 ; k < recnum ; k++)
                    {
                      //  DO NOT use data marked as PFM_DELETED.

                      if (!(depth[k].validity & PFM_DELETED))
                        {
                          //  Don't load invalid data if requested (-n option).

                          if (!misc->no_invalid || (!(depth[k].validity & PFM_INVAL)))
                            {
                              //  Don't load reference data if requested (-r option).

                              if (!misc->no_reference || (!(depth[k].validity & PFM_REFERENCE)))
                                {
                                  //  If the polygon count is zero we are loading a rectangle so we want to check against that.
                                  //  Otherwise it's a polygon so we need to check against that.

                                  uint8_t in = NVFalse;
                                  if (misc->abe_share->polygon_count)
                                    {
                                      if (inside_polygon2 (misc->abe_share->polygon_x, misc->abe_share->polygon_y,
                                                           misc->abe_share->polygon_count, depth[k].xyz.x, depth[k].xyz.y)) in = NVTrue;
                                    }
                                  else
                                    {
                                      if (depth[k].xyz.x >= misc->abe_share->edit_area.min_x &&
                                          depth[k].xyz.x <= misc->abe_share->edit_area.max_x &&
                                          depth[k].xyz.y >= misc->abe_share->edit_area.min_y &&
                                          depth[k].xyz.y <= misc->abe_share->edit_area.max_y) in = NVTrue;
                                    }


                                  if (in)
                                    {
                                      for (int32_t m = 0 ; m < misc->max_attr ; m++)
                                        {
                                          misc->data[misc->abe_share->point_cloud_count].attr[m] = 0.0;
                                          if (misc->abe_share->open_args[pfm].head.num_ndx_attr)
                                            {

#pragma message ("WARNING: CZMIL channel number attribute will have 1 added to it in get_buffer.  See get_buffer.cpp for complete explanation")

                                              /************************************* IMPORTANT NOTE **************************************** \

                                                Note that we're adding 1 to the CZMIL channel attribute so that references to the channel
                                                number in pfmEdit3D will match what Optech calls them.  The text strings returned from the
                                                CZMIL API to describe channels 0 through 8 (from czmil_get_channel_string) refer to them as
                                                channels 1 through 9.  Internally they are referenced as channels 0 through 8.  Since
                                                pfmEdit3D never saves the channel numbers back to the PFM or, indirectly, back to the input
                                                CPF file this is usually a non-issue (except when the user runs lidarMonitor and notices
                                                that channel 1 in pfmEdit3D is listed as channel 0 in lidarMonitor).  The one situation
                                                that can be a problem is when pfmEdit3D works with external programs.  The major one being
                                                czmilReprocess.  In those cases it is important to always use the sub-record number to
                                                derive the actual, internal channel number.  The sub-record number for CZMIL data is made
                                                up of the channel number times 100 plus the return number so, to retrieve the actual,
                                                internal channel number just do an integer divide of the sub-record by 100.

                                              \************************************* IMPORTANT NOTE ****************************************/

                                              if (misc->czmil_chan_attr == m)
                                                {
                                                  misc->data[misc->abe_share->point_cloud_count].attr[m] = depth[k].attr[m] + 1.0;
                                                }
                                              else
                                                {
                                                  misc->data[misc->abe_share->point_cloud_count].attr[m] = depth[k].attr[m];
                                                }
                                            }
                                        }

                                      misc->data[misc->abe_share->point_cloud_count].x = depth[k].xyz.x;
                                      misc->data[misc->abe_share->point_cloud_count].y = depth[k].xyz.y;
                                      misc->data[misc->abe_share->point_cloud_count].z = depth[k].xyz.z;


                                      //  Compute min and max Z values for displayed data (valid or not).

                                      misc->min_z = qMin (misc->data[misc->abe_share->point_cloud_count].z, misc->min_z);
                                      misc->max_z = qMax (misc->data[misc->abe_share->point_cloud_count].z, misc->max_z);


                                      misc->data[misc->abe_share->point_cloud_count].herr = depth[k].horizontal_error;
                                      misc->data[misc->abe_share->point_cloud_count].verr = depth[k].vertical_error;
                                      misc->data[misc->abe_share->point_cloud_count].val = depth[k].validity;
                                      misc->data[misc->abe_share->point_cloud_count].pfm = pfm;
                                      misc->data[misc->abe_share->point_cloud_count].file = depth[k].file_number;
                                      misc->data[misc->abe_share->point_cloud_count].type = data_type_lut[pfm * MAX_PFM_FILES +
                                                                                                          depth[k].file_number];


                                      //  We need the line numbers to be unique because we compare line numbers
                                      //  to line numbers in various places in the code.  They won't be unique if we're 
                                      //  displaying multiple PFM files so we're cheating and adding the PFM number
                                      //  times SHARED_LINE_MULT to make them unique and still be able to determine which
                                      //  PFM they are associated with.  Clever, no ;-)

                                      misc->data[misc->abe_share->point_cloud_count].line = pfm * SHARED_LINE_MULT + depth[k].line_number;


                                      misc->data[misc->abe_share->point_cloud_count].rec = depth[k].ping_number;
                                      misc->data[misc->abe_share->point_cloud_count].sub = depth[k].beam_number;
                                      misc->data[misc->abe_share->point_cloud_count].addr = depth[k].address.block;
                                      misc->data[misc->abe_share->point_cloud_count].pos = depth[k].address.record;
                                      misc->data[misc->abe_share->point_cloud_count].xcoord = coord.x;
                                      misc->data[misc->abe_share->point_cloud_count].ycoord = coord.y;
                                      misc->data[misc->abe_share->point_cloud_count].exflag = NVFalse;
                                      misc->data[misc->abe_share->point_cloud_count].mask = MASK_NONE;
                                      misc->data[misc->abe_share->point_cloud_count].fmask = NVFalse;


                                      //  An explanation of the following two fields.  The "oval" field is a copy of the original validity
                                      //  word in the input depth record.  We could save some memory space by just using a flag to signify
                                      //  change but that would mean that we'd have to manually set the flag if something was changed.  This
                                      //  could be done but it would be problematic for external filters.  As it is, all we have to do is
                                      //  check the point cloud validity word against the original validity word.  If it has changed we need
                                      //  to save the validity.  Originally, ABE was designed to be a pass/fail data editor in that the only
                                      //  thing that changed was the validity word.  We've now added the ability to change the CZMIL and/or
                                      //  LAS classification value.  Since this is only done sparingly it was simpler to just use a one-byte
                                      //  flag instead of trying to save the original classification value.  The advantage to using the flag
                                      //  is that we could in future use just specific bits of the flag to represent changes to the LiDAR
                                      //  classification as well as other possible values.

                                      misc->data[misc->abe_share->point_cloud_count].oval = depth[k].validity;
                                      misc->data[misc->abe_share->point_cloud_count].lidar_class_change = NVFalse;


                                      //  Check for "reference data".

                                      if (depth[k].validity & PFM_REFERENCE) misc->reference_flag = NVTrue;


                                      //  Define which lines are loaded in the edit window.

                                      found = NVFalse;
                                      for (int32_t m = 0 ; m < misc->line_count ; m++)
                                        {
                                          if (misc->data[misc->abe_share->point_cloud_count].line == misc->line_number[m])
                                            {
                                              //  We're saving the index of the line to be used in unique line coloring.

                                              misc->data[misc->abe_share->point_cloud_count].line_index = misc->line_index[m];


                                              found = NVTrue;
                                              break;
                                            }
                                        }

                                      if (!found)
                                        {
                                          misc->line_index[misc->line_count] = misc->line_count;


                                          //  We're saving the index of the line to be used in unique line coloring.

                                          misc->data[misc->abe_share->point_cloud_count].line_index = misc->line_count;


                                          misc->line_number[misc->line_count] = misc->data[misc->abe_share->point_cloud_count].line;

                                          misc->line_type[misc->line_count] = misc->data[misc->abe_share->point_cloud_count].type;

                                          misc->line_count++;
                                        }

                                      misc->abe_share->point_cloud_count++;
                                    }
                                }
                            }
                        }
                    }
                  free (depth);
                }
            }
        }
    }


  //  Free the data type lookup table.

  free (data_type_lut);


  //  Since the lines are loaded into the misc->line_number array based on when they were read in we're
  //  going to try to separate the line colors somewhat by moving the lines around in the line_number array.
  //  Hopefully this will give us better color separation in color by line mode.

  int32_t *ln, *lt;

  ln = (int32_t *) calloc (misc->line_count, sizeof (int32_t));
  if (ln == NULL)
    {
      QMessageBox::critical (0, pfmEdit3D::tr ("pfmEdit3D get_buffer"), pfmEdit3D::tr ("Unable to allocate line number array"));
      exit (-1);
    }

  lt = (int32_t *) calloc (misc->line_count, sizeof (int32_t));
  if (lt == NULL)
    {
      QMessageBox::critical (0, pfmEdit3D::tr ("pfmEdit3D get_buffer"), pfmEdit3D::tr ("Unable to allocate line type array"));
      exit (-1);
    }

  for (int32_t i = 0, j = 0, k = misc->line_count - 1 ; i < misc->line_count ; i++)
    {
      if (i % 2)
        {
          ln[k] = misc->line_number[i];
          lt[k] = misc->line_type[i];
          k--;
        }
      else
        {
          ln[j] = misc->line_number[i];
          lt[j] = misc->line_type[i];
          j++;
        }
    }

  for (int32_t i = 0 ; i < misc->line_count ; i++)
    {
      misc->line_number[i] = ln[i];
      misc->line_type[i] = lt[i];
    }

  free (ln);
  free (lt);


  //  If we're doing a reload (for now only used by czmilReprocess) we don't want to reset the view.

  if (!reload_flag)
    {
      //  Define the bounds + 2%

      double width_2 = (misc->abe_share->edit_area.max_x - misc->abe_share->edit_area.min_x) * 0.02;
      double height_2 = (misc->abe_share->edit_area.max_y - misc->abe_share->edit_area.min_y) * 0.02;
      misc->abe_share->edit_area.min_y -= height_2;
      misc->abe_share->edit_area.max_y += height_2;
      misc->abe_share->edit_area.min_x -= width_2;
      misc->abe_share->edit_area.max_x += width_2;


      misc->displayed_area.min_x = misc->abe_share->edit_area.min_x;
      misc->displayed_area.max_x = misc->abe_share->edit_area.max_x;
      misc->displayed_area.min_y = misc->abe_share->edit_area.min_y;
      misc->displayed_area.max_y = misc->abe_share->edit_area.max_y;
    }


  //  Check for exceeding the sparse point limit

  if (misc->abe_share->point_cloud_count > options->sparse_limit) misc->need_sparse = NVTrue;


  //  Try to open the feature file and read the features into memory.

  if (strcmp (misc->abe_share->open_args[0].target_path, "NONE"))
    {
      if (misc->bfd_open) binaryFeatureData_close_file (misc->bfd_handle);
      misc->bfd_open = NVFalse;

      if ((misc->bfd_handle = binaryFeatureData_open_file (misc->abe_share->open_args[0].target_path, &misc->bfd_header, BFDATA_UPDATE)) >= 0)
        {
          if (binaryFeatureData_read_all_short_features (misc->bfd_handle, &misc->feature) < 0)
            {
              QMessageBox::warning (0, "pfmEdit3D", pfmEdit3D::tr ("Unable to read feature records\nReason: %1").arg
                                    (binaryFeatureData_strerror ()));

              binaryFeatureData_close_file (misc->bfd_handle);
            }
          else
            {
              misc->bfd_open = NVTrue;
            }
        }
    }
}
