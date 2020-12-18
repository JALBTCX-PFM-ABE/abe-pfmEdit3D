
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

  - Module Name:        put_buffer

  - Programmer(s):      Jan C. Depner

  - Date Written:       May 1999

  - Purpose:            Write the modified points from the point cloud
                        back to the file and recompute the bin values.

  - Inputs:

  - Return Value:
                        - 0 if nothing has been modified
                        - 1 if something has been modified

****************************************************************************/

int32_t put_buffer (MISC *misc)
{
  NV_I32_COORD2   coord;
  int32_t         mod_flag;
  DEPTH_RECORD    depth;
  BIN_RECORD      bin;
  uint8_t         *bin_map;

  void            czmil_write_reprocess (MISC *misc);


  mod_flag = 0;


  //  If this was started in read only mode, don't save anything!

  if (!misc->abe_share->read_only)
    {
      misc->statusProg->setRange (0, misc->abe_share->point_cloud_count);
      misc->statusProgLabel->setText (pfmEdit3D::tr ("Saving edits"));
      misc->statusProgLabel->setVisible (true);
      misc->statusProg->setTextVisible (true);
      qApp->processEvents();


      for (int32_t pfm = 0 ; pfm < misc->abe_share->pfm_count ; pfm++)
        {
          QString progText = pfmEdit3D::tr ("Saving edits for %1").arg (QFileInfo (QString (misc->abe_share->open_args[pfm].list_path)).fileName ().remove (".pfm"));

          misc->statusProgLabel->setText (progText);
          misc->statusProg->reset ();


          //  Compute the width and the height.

          int32_t width = (misc->ur[pfm].x - misc->ll[pfm].x) + 1;
          int32_t height = (misc->ur[pfm].y - misc->ll[pfm].y) + 1;


          if (width > 0  && height > 0)
            {
              //  Allocate and clear the bin map.

              bin_map = (uint8_t *) malloc (width * height);
              memset (bin_map, 0, width * height);


              //  This seems a bit silly but it takes a long time to spin through the data
              //  with a QProgressDialog running so we're only updating it at 10% intervals.

              int32_t inc = misc->abe_share->point_cloud_count / 10;
              if (!inc) inc = 1;


              for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
                {
                  if (!(i % inc))
                    {
                      misc->statusProg->setValue (i);
                      qApp->processEvents();
                    }


                  //  Only deal with points that are in the current PFM (i.e. misc->abe_share->open_args[pfm])

                  if (misc->data[i].pfm == pfm)
                    {
                      //  If the validity has changed since this point was read in or the LiDAR classification has changed, we need to save it.
                      //  We need to check both because the validity may not actually change when we change the LiDAR classification.  Here's
                      //  the scenario where it wouldn't change - we edit some data and change the LiDAR classification for a specific point,
                      //  this sets the PFM_MODIFIED bit and the PFM_SELECTED_SOUNDING bit in the validity as well as the lidar_class_change
                      //  flag.  When we exit from the editor the validity doesn't match the original validity so it is saved back to the PFM.
                      //  We then edit a new area that overlaps the previous area.  We change the classification of the same point to some
                      //  other classification value.  This sets the PFM_MODIFIED bit and the PFM_SELECTED_SOUNDING bit in the validity as well
                      //  as the lidar_class_change flag.  When we exit from the editor the validity DOES match the original validity since it
                      //  had both the PFM_MODIFIED bit and the PFM_SELECTED_SOUNDING bit set when it was read in.  So, in order to catch the
                      //  change to classification we have to check the lidar_clas_change flag as well as the validity.  Since the validity
                      //  change will usually happen anyway, we check it first so that the second part of the if statement isn't usually
                      //  evaluated because it is an or (||) clause.

                      if (misc->data[i].oval != misc->data[i].val || misc->data[i].lidar_class_change)
                        {
                          //  Set the coordinate x and y value (trying to compute it from position is not accurate).

                          depth.coord.x = misc->data[i].xcoord;
                          depth.coord.y = misc->data[i].ycoord;


                          //  Get the address, file number, record number, and subrecord number to uniquely identify the sounding.

                          depth.address.block = misc->data[i].addr;
                          depth.address.record = misc->data[i].pos;
                          depth.file_number = misc->data[i].file;
                          depth.ping_number = misc->data[i].rec;
                          depth.beam_number = misc->data[i].sub;


                          //  Set the validity.  Either update_depth_record_index or change_depth_record_index will automatically set the 
                          //  PFM_MODIFIED bit.

                          depth.validity = misc->data[i].val;


                          //  Check to see if we changed the LiDAR classification attribute.  If so, we need to update both the validity and the 
                          //  LiDAR classification attribute.

                          if (misc->data[i].lidar_class_change)
                            {
                              //  We have to put all of the things that change_depth_record_index will modify into the DEPTH_RECORD prior to calling 
                              //  it.  This includes the depth, horizontal error, vertical error, validity, and the attributes.

                              depth.xyz.z = misc->data[i].z;
                              depth.horizontal_error = misc->data[i].herr;
                              depth.vertical_error = misc->data[i].verr;


                              for (int32_t m = 0 ; m < misc->max_attr ; m++)
                                {
                                  if (misc->abe_share->open_args[pfm].head.num_ndx_attr)
                                    {

#pragma message ("WARNING: CZMIL channel number attribute will have 1 subtracted from it in put_buffer.  See put_buffer.cpp for complete explanation")

                                      /***************************************** IMPORTANT NOTE ******************************************** \

                                         Note that we're subtracting 1 from the CZMIL channel attribute because we added 1 to the CZMIL
                                         channel number in put_buffer.cpp.  To read the explanation for this insanity, please see the
                                         get_buffer.cpp source code.

                                      \***************************************** IMPORTANT NOTE ********************************************/

                                          if (misc->czmil_chan_attr == m)
                                            {
                                              depth.attr[m] = misc->data[i].attr[m] - 1.0;
                                            }
                                          else
                                            {
                                              depth.attr[m] = misc->data[i].attr[m];
                                            }
                                        }
                                    }


                              //  Update the Z, horizontal error, vertical error, validity, and the attributes.

                              change_depth_record_index (misc->pfm_handle[pfm], &depth);
                            }
                          else
                            {
                              //  Update just the validity.

                              update_depth_record_index (misc->pfm_handle[pfm], &depth);
                            }


                          //  Set the bin map modified flag.

                          bin_map[(depth.coord.y - misc->ll[pfm].y) * width + (depth.coord.x - misc->ll[pfm].x)] = 1;

                          mod_flag = 1;
                        }
                    }
                }


              //  Recompute the bin values for those bins that were modified.

              for (coord.y = misc->ll[pfm].y ; coord.y <= misc->ur[pfm].y ; coord.y++)
                {
                  for (coord.x = misc->ll[pfm].x ; coord.x <= misc->ur[pfm].x ; coord.x++)
                    {
                      if (bin_map[(coord.y - misc->ll[pfm].y) * width + (coord.x - misc->ll[pfm].x)])
                        {
                          bin.validity |= PFM_DATA;
                          recompute_bin_values_index (misc->pfm_handle[pfm], coord, &bin, PFM_DATA);
                        }
                    }
                }

              free (bin_map);
            }
        }


      misc->statusProg->reset ();
      misc->statusProgLabel->setVisible (false);
      misc->statusProg->setTextVisible (false);
      qApp->processEvents ();
    }


  return (mod_flag);
}
