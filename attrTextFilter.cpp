
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


//!  Filter based on the attribute that we are coloring by (for example, horizontal uncertainty).

void attrTextFilter (OPTIONS *options, MISC *misc, float *amin, float *amax, int32_t count)
{
  int32_t ndx = options->color_index - PRE_ATTR;


  //  These are the "flipped" values for when the user is displaying elevation instead of depth.

  float min_z = options->filter_min_z;
  float max_z = options->filter_max_z;
  if (options->z_orientation < 0.0)
    {
      min_z = -options->filter_max_z;
      max_z = -options->filter_min_z;
    }


  misc->statusProgLabel->setText (pfmEdit3D::tr ("Running attribute filter"));
  misc->statusProgLabel->setVisible (true);
  qApp->processEvents();


  for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
    {
      if (!(misc->data[i].val & (PFM_DELETED | PFM_REFERENCE | PFM_INVAL)))
        {
          //  Check for single line display.

          if (!misc->num_lines || check_line (misc, misc->data[i].line))
            {
              //  Check the normal reasons for excluding a point.

              if (!check_bounds (options, misc, misc->data[i].x, misc->data[i].y, misc->data[i].z, NVFalse, misc->data[i].mask, misc->data[i].pfm,
                                 NVFalse, misc->slice))
                {

                  //  Finally, check the filter mask.

                  if (!misc->data[i].fmask)
                    {
                      uint8_t filter_it = NVFalse;


                      for (int32_t j = 0 ; j < count ; j++)
                        {
                          //  Note that we don't filter on depth/elevation.

                          switch (options->color_index)
                            {
                            case 1:
                              if (misc->data[i].line <= amax[j] && misc->data[i].line >= amin[j]) filter_it = NVTrue;
                              break;

                            case 2:
                              if (misc->data[i].herr <= amax[j] && misc->data[i].herr >= amin[j]) filter_it = NVTrue;
                              break;

                            case 3:
                              if (misc->data[i].verr <= amax[j] && misc->data[i].verr >= amin[j]) filter_it = NVTrue;
                              break;

                            default:
                              if (misc->data[i].attr[misc->attrStatNum[ndx]] <= amax[j] &&
                                  misc->data[i].attr[misc->attrStatNum[ndx]] >= amin[j]) filter_it = NVTrue;
                              break;
                            }
                        }


                      if (filter_it)
                        {
                          //  Last check.  If we are range limiting the Z data we don't want to actually invalidate a point outside
                          //  our Z range.

                          if (!options->filter_apply_z_range || (misc->data[i].z >= min_z && misc->data[i].z <= max_z))
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

                              misc->filter_kill_list[misc->filter_kill_count] = i;
                              misc->filter_kill_count++;
                            }
                        }
		    }
                }
            }
        }
    }


  misc->statusProgLabel->setVisible (true);
  misc->statusProg->setTextVisible (true);
  qApp->processEvents();
}
