
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

  - Module Name:        keep_area

  - Programmer(s):      Rebecca Martinolich

  - Date Written:       October 1998

  - Purpose:            Invalidates data outside a polygon.

  - Arguments:
                        - x_bounds        =   array of x vertices
                        - y_bounds        =   array of y vertices
                        - z_bounds        =   array of z vertices (east &
                                              south views)
                        - num_vertices    =   number of vertices

  - Return Value:
                        - void

****************************************************************************/

void keep_area (nvMapGL *map, OPTIONS *options, MISC *misc, int32_t *x_bounds, int32_t *y_bounds, int32_t num_vertices)
{

  //  We have to convert to double so that the "inside" function will work.

  double *mx = (double *) malloc (num_vertices * sizeof (double));

  if (mx == NULL)
    {
      fprintf (stderr, "%s %s %s %d - mx - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  double *my = (double *) malloc (num_vertices * sizeof (double));

  if (my == NULL)
    {
      fprintf (stderr, "%s %s %s %d - mx - %s\n", misc->progname, __FILE__, __FUNCTION__, __LINE__, strerror (errno));
      exit (-1);
    }

  for (int32_t i = 0 ; i < num_vertices ; i++)
    {
      mx[i] = (double) x_bounds[i];
      my[i] = (double) y_bounds[i];
    }


  int32_t px = -1, py = -1;
  uint8_t init2D = NVTrue;

  for (int32_t i = 0 ; i < misc->abe_share->point_cloud_count ; i++)
    {
      //  Check for single line display.

      if (!misc->num_lines || check_line (misc, misc->data[i].line))
        {
          //  Check against the rectangular or polygonal area that is displayed.

          if (!check_bounds (options, misc, i, NVFalse, misc->slice))
            {
              //  Convert the X, Y, and Z value to a projected pixel position

              map->get2DCoords (misc->data[i].x, misc->data[i].y, -misc->data[i].z, &px, &py, &init2D);


              //  Now check the point against the polygon.

              if (!inside_polygon2 (mx, my, num_vertices, (double) px, (double) py))
                {
                  //  Save the undo information.

                  store_undo (misc, options->undo_levels, misc->data[i].val, i, NULL);

                  misc->data[i].val |= PFM_MANUALLY_INVAL;


                  //  If the point has been set to PFM_SUSPECT, we want to clear that flag since the user has made the decision
                  //  to delete the point.  If the user does an undo, this will be restored.  If, on the other hand, the user
                  //  manually validates the deleted point it won't be restored.  In that case the user made the decision that
                  //  the point was good.  In that case (or if it left invalid) it should no longer be marked as suspect.  This
                  //  has the effect of clearing the PFM_SUSPECT flag in the bin record.

                  misc->data[i].val &= ~PFM_SUSPECT;
                }
            }
        }
    }

  free (mx);
  free (my);


  //  Close the undo block.

  end_undo_block (misc);
}
