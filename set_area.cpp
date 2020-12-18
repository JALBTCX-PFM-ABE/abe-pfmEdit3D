
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

  - Module Name:        set_area

  - Programmer(s):      Jan C. Depner

  - Date Written:       November 1995

  - Purpose:            Invalidates, restores, sets, unsets reference data
                        in a rectangle or polygon.

  - Arguments:
                        - map             =   nvmap object to draw on
                        - options         =   OPTIONS structure
                        - misc            =   MISC structure
                        - data            =   point data used to define bounds
                        - x_bounds        =   array of x vertices
                        - y_bounds        =   array of y vertices
                        - z_bounds        =   array of z vertices (east & south views)
                        - num_vertices    =   number of vertices
                        - rect_flag       =   1 for rectangle, 0 for polygon

  - Return Value:
                        - void

****************************************************************************/

void set_area (nvMapGL *map, OPTIONS *options, MISC *misc, int32_t *x_bounds, int32_t *y_bounds, int32_t num_vertices, int32_t rect_flag)
{
  double *mx = NULL, *my = NULL;
  uint8_t undo_on = NVFalse;
  uint8_t init2D = NVTrue;


  //  These are the "flipped" values for when the user is displaying elevation instead of depth.

  float min_z = options->filter_min_z;
  float max_z = options->filter_max_z;
  if (options->z_orientation < 0.0)
    {
      min_z = -options->filter_max_z;
      max_z = -options->filter_min_z;
    }


  //  If it's a polygon we have to convert to double so that the "inside" function will work.

  if (!rect_flag)
    {
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


  //  CLEAR_POLYGON is a special case because we have to search the highlight/filter kill list instead of all of the points.

  if (options->function == CLEAR_POLYGON)
    {
      //  Check for highlight or filter kill list

      if (misc->filter_kill_count)
        {
          for (int32_t i = 0 ; i < misc->filter_kill_count ; i++)
            {
              int32_t j = misc->filter_kill_list[i];


              //  Convert the X, Y, and Z value to a projected pixel position

              map->get2DCoords (misc->data[j].x, misc->data[j].y, -misc->data[j].z, &px, &py, &init2D);


              //  Always check against the polygon's minimum bounding rectangle before checking against the polygon.
              //  Hopefully, that will make the whole thing a bit faster (that's what everybody says on the interwebs anyway ;-)

              if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
                {
                  //  Mark the points that fall inside the polygon.

                  if (inside_polygon2 (mx, my, num_vertices, (double) px, (double) py)) misc->filter_kill_list[i] = -1;
                }
            }

          free (mx);
          free (my);


          //  Now reorder and pack the filter_kill_list array.

          int32_t new_count = 0;
          for (int32_t i = 0 ; i < misc->filter_kill_count ; i++)
            {
              if (misc->filter_kill_list[i] >= 0)
                {
                  misc->filter_kill_list[new_count] = misc->filter_kill_list[i];
                  new_count++;
                }
            }

          misc->filter_kill_count = new_count;

          try
            {
              misc->filter_kill_list.resize (misc->filter_kill_count);
            }
          catch (std::bad_alloc&)
            {
              fprintf (stderr, "%s %s %s %d - filter_kill_list - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          return;
        }
      else
        {
          for (int32_t i = 0 ; i < misc->highlight_count ; i++)
            {
              int32_t j = misc->highlight[i];


              //  Convert the X, Y, and Z value to a projected pixel position

              map->get2DCoords (misc->data[j].x, misc->data[j].y, -misc->data[j].z, &px, &py, &init2D);


              //  Always check against the polygon's minimum bounding rectangle before checking against the polygon.
              //  Hopefully, that will make the whole thing a bit faster (that's what everybody says on the interwebs anyway ;-)

              if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
                {
                  //  Mark the points that fall inside the polygon.

                  if (inside_polygon2 (mx, my, num_vertices, (double) px, (double) py)) misc->highlight[i] = -1;
                }
            }

          free (mx);
          free (my);


          //  Now reorder and pack the highlight array.

          int32_t new_count = 0;
          for (int32_t i = 0 ; i < misc->highlight_count ; i++)
            {
              if (misc->highlight[i] >= 0)
                {
                  misc->highlight[new_count] = misc->highlight[i];
                  new_count++;
                }
            }

          misc->highlight_count = new_count;

          try
            {
              misc->highlight.resize (misc->highlight_count);
            }
          catch (std::bad_alloc&)
            {
              fprintf (stderr, "%s %s %s %d - highlight - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
              exit (-1);
            }

          return;
        }
    }


  //  Whether we're making an undo block.

  uint8_t check_invalid = NVTrue;
  switch (options->function)
    {
    case RESTORE_RECTANGLE:
    case RESTORE_POLYGON:
      undo_on = NVTrue;
      check_invalid = NVFalse;
      break;

    case DELETE_RECTANGLE:
    case UNSET_REFERENCE:
    case SET_REFERENCE:
    case DELETE_POLYGON:
      undo_on = NVTrue;
      break;

    default:
      undo_on = NVFalse;
    }


  //  Check for masking because we have to do the check_bounds differently.

  if (options->function == MASK_INSIDE_RECTANGLE || options->function == MASK_OUTSIDE_RECTANGLE ||
      options->function == MASK_INSIDE_POLYGON || options->function == MASK_OUTSIDE_POLYGON)
    {
      for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
        {
          //  Check for single line display.

          if (!misc->num_lines || check_line (misc, misc->data[i].line))
            {
              //  Check against the displayed minimum bounding rectangle.  This is all that is needed for 
              //  a rectangle and it cuts down on the computations for a polygon.   Also, DO NOT 
              //  allow changes to null value status.  Since we're masking here we don't check for masking in
              //  check bounds because even if MASK_TYPE or MASK_FLAG is set we still want to set MASK_SHAPE.
              //  The masks aren't mutually exclusive.

              if (!check_bounds (options, misc, misc->data[i].x, misc->data[i].y, misc->data[i].z, misc->data[i].val, NVFalse,
                                 misc->data[i].pfm, check_invalid, misc->slice) && misc->data[i].z < misc->null_val[misc->data[i].pfm])
                {
                  //  Convert the X, Y, and Z value to a projected pixel position

                  map->get2DCoords (misc->data[i].x, misc->data[i].y, -misc->data[i].z, &px, &py, &init2D);


                  //  Rectangle.

                  if (rect_flag)
                    {
                      if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
                        {
                          if (options->function == MASK_INSIDE_RECTANGLE) misc->data[i].mask |= MASK_SHAPE;
                        }
                      else
                        {
                          if (options->function == MASK_OUTSIDE_RECTANGLE) misc->data[i].mask |= MASK_SHAPE;
                        }
                    }

                  //  Polygon.

                  else
                    {
                      if (options->function == MASK_OUTSIDE_POLYGON)
                        {
                          //  Always check against the polygon's minimum bounding rectangle before checking against the polygon.
                          //  Hopefully, that will make the whole thing a bit faster (that's what everybody says on the interwebs anyway ;-)

                          if (px < min_x && px > max_x && py < min_y && py > max_y)
                            {
                              //  If it's outside the MBR it's automatically outside the polygon so we set the flag.

                              misc->data[i].mask |= MASK_SHAPE;
                            }
                          else
                            {
                              //  If it's inside the MBR we have to check to make sure it's outside the polygon.

                              if (!inside_polygon2 (mx, my, num_vertices, (double) px, (double) py))
                                {
                                  misc->data[i].mask |= MASK_SHAPE;
                                }
                            }
                        }
                      else
                        {
                          //  Always check against the polygon's minimum bounding rectangle before checking against the polygon.
                          //  Hopefully, that will make the whole thing a bit faster (that's what everybody says on the interwebs anyway ;-)

                          if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
                            {
                              //  If it's inside the MBR we have to check to make sure it's inside the polygon as well.

                              if (inside_polygon2 (mx, my, num_vertices, (double) px, (double) py))
                                {
                                  if (options->function == MASK_INSIDE_POLYGON) misc->data[i].mask |= MASK_SHAPE;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  else
    {
      for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
        {
          //  Check for single line display.

          if (!misc->num_lines || check_line (misc, misc->data[i].line))
            {
              //  Check against the displayed minimum bounding rectangle.  This is all that is needed for 
              //  a rectangle and it cuts down on the computations for a polygon.   Also, DO NOT 
              //  allow changes to null value status.

              if (!check_bounds (options, misc, misc->data[i].x, misc->data[i].y, misc->data[i].z, misc->data[i].val, misc->data[i].mask,
                                 misc->data[i].pfm, check_invalid, misc->slice) && misc->data[i].z < misc->null_val[misc->data[i].pfm])
                {
                  //  Convert the X, Y, and Z value to a projected pixel position

                  map->get2DCoords (misc->data[i].x, misc->data[i].y, -misc->data[i].z, &px, &py, &init2D);


                  //  Rectangle.

                  if (rect_flag)
                    {
                      if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
                        {
                          switch (options->function)
                            {
                            case RESTORE_RECTANGLE:

                              //  Use display invalid button states to determine what to restore.  If neither button is set, restore everything.

                              if ((!options->display_man_invalid && !options->display_flt_invalid) ||
                                  (options->display_man_invalid && (misc->data[i].val & PFM_MANUALLY_INVAL)) ||
                                  (options->display_flt_invalid && (misc->data[i].val & PFM_FILTER_INVAL)))
                                {
                                  store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);
                                  misc->data[i].val &= ~PFM_MANUALLY_INVAL;
                                  misc->data[i].val &= ~PFM_FILTER_INVAL;
                                }
                              break;

                            case DELETE_RECTANGLE:
                              store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);
                              misc->data[i].val |= PFM_MANUALLY_INVAL;


                              //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
                              //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
                              //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
                              //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
                              //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

                              misc->data[i].val &= ~PFM_SUSPECT;
                              break;

                            case UNSET_REFERENCE:
                              store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);
                              misc->data[i].val &= ~PFM_REFERENCE;
                              break;

                            case SET_REFERENCE:
                              store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);
                              misc->data[i].val |= PFM_REFERENCE;
                              break;

                            case RECT_FILTER_MASK:
                              misc->data[i].fmask = NVTrue;
                              break;
                            }
                        }
                    }

                  //  Polygon.

                  else
                    {
                      //  Always check against the polygon's minimum bounding rectangle before checking against the polygon.
                      //  Hopefully, that will make the whole thing a bit faster (that's what everybody says on the interwebs anyway ;-)

                      if (px >= min_x && px <= max_x && py >= min_y && py <= max_y)
                        {
                          //  If it's inside the MBR we have to check to make sure it's inside the polygon as well.

                          if (inside_polygon2 (mx, my, num_vertices, (double) px, (double) py))
                            {
                              switch (options->function)
                                {
                                case RESTORE_POLYGON:

                                  //  Use display invalid button states to determine what to restore.  If neither button is set, restore everything.

                                  if ((!options->display_man_invalid && !options->display_flt_invalid) ||
                                      (options->display_man_invalid && (misc->data[i].val & PFM_MANUALLY_INVAL)) ||
                                      (options->display_flt_invalid && (misc->data[i].val & PFM_FILTER_INVAL)))
                                    {
                                      store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);
                                      misc->data[i].val &= ~PFM_MANUALLY_INVAL;
                                      misc->data[i].val &= ~PFM_FILTER_INVAL;
                                    }
                                  break;

                                case DELETE_POLYGON:
                                  store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);
                                  misc->data[i].val |= PFM_MANUALLY_INVAL;


                                  //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
                                  //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
                                  //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
                                  //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
                                  //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

                                  misc->data[i].val &= ~PFM_SUSPECT;
                                  break;

                                case UNSET_REFERENCE:
                                  store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);
                                  misc->data[i].val &= ~PFM_REFERENCE;
                                  break;

                                case SET_REFERENCE:
                                  store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);
                                  misc->data[i].val |= PFM_REFERENCE;
                                  break;

                                case POLY_FILTER_MASK:
                                  misc->data[i].fmask = NVTrue;
                                  break;

                                case HIGHLIGHT_POLYGON:
                                  try
                                    {
                                      misc->highlight.resize (misc->highlight_count + 1);
                                    }
                                  catch (std::bad_alloc&)
                                    {
                                      fprintf (stderr, "%s %s %s %d - highlight - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
                                      exit (-1);
                                    }

                                  misc->highlight[misc->highlight_count] = i;
                                  misc->highlight_count++;
                                  break;

                                case CHANGE_LIDAR_CLASS:

                                  uint8_t change_it = NVTrue;


                                  //  If we're not allowing "Any" class to be changed we need to check the existing class against the allowed
                                  //  from_lidar_class[] fields.

                                  if (!options->from_lidar_class[0])
                                    {
                                      change_it = NVFalse;


                                      //  Note that we don't need to check against [0].

                                      for (int32_t cls = 1 ; cls < LIDAR_CLASSES ; cls++)
                                        {
                                          if (options->from_lidar_class[cls])
                                            {
                                              if (misc->data[i].attr[misc->lidar_class_attr] == misc->lidar_class_num[cls])
                                                {
                                                  change_it = NVTrue;
                                                  break;
                                                }
                                            }
                                        }
                                    }


                                  if (change_it)
                                    {
                                      //  Last check.  If we are range limiting the Z data we don't want to actually mark a point outside
                                      //  our Z range.

                                      if (!options->filter_apply_z_range || (misc->data[i].z >= min_z && misc->data[i].z <= max_z))
                                        {
                                          //  Due to the way the LiDAR classification change filter works we're just going to put the points into
                                          //  the filter kill list.

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
                                  break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

  if (!rect_flag)
    {
      free (mx);
      free (my);
    }


  //  Close the undo block

  if (undo_on) end_undo_block (misc);
}
