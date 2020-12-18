
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



#include "pfmEdit3DDef.hpp"

/*!
  This function compares the entered point (current point) to the stack of geographically nearest
  points and then puts it into the proper place in the stack if it is nearer to the cursor than
  one, some, or all of the points already in the stack of nearest points.
*/

uint8_t compare_to_stack (int32_t current_point, double dist, MISC *misc, double z)
{
  int32_t hit_previous = 0;
  for (int32_t j = MAX_STACK_POINTS - 1 ; j >= 0 ; j--)
    {
      if (dist < misc->nearest_stack.dist[j])
        {
          if (!j)
            {
              for (int32_t k = MAX_STACK_POINTS - 1 ; k > 0 ; k--)
                {
                  misc->nearest_stack.dist[k] = misc->nearest_stack.dist[k - 1];
                  misc->nearest_stack.point[k] = misc->nearest_stack.point[k - 1];
                }

              misc->nearest_stack.point[0] = current_point;
              misc->nearest_stack.dist[0] = dist;


	      //  Return NVTrue since this is the minimum distance so far.

              return (NVTrue);
            }

          hit_previous = j;
        }


      //  If the distances are exactly the same we want to check for the closest Z value.  This can be a problem with 
      //  CZMIL NULL values which are channels with no returns.  In those cases we use the shot reference lat/lon
      //  to load the point and that is the same as the first valid return in the shot.

      else if (dist == misc->nearest_stack.dist[j])
        {
          if (!j)
            {
              //  Check the Z difference.

              if (fabs (misc->data[misc->nearest_stack.point[0]].z - z) < fabs (misc->data[current_point].z - z))
                {
                  for (int32_t k = MAX_STACK_POINTS - 1 ; k > 0 ; k--)
                    {
                      misc->nearest_stack.dist[k] = misc->nearest_stack.dist[k - 1];
                      misc->nearest_stack.point[k] = misc->nearest_stack.point[k - 1];
                    }

                  misc->nearest_stack.point[0] = current_point;
                  misc->nearest_stack.dist[0] = dist;


                  //  Return NVTrue since this is the minimum distance so far.

                  return (NVTrue);
                }
            }

          hit_previous = j;
        }
      else
        {
          if (hit_previous)
            {
              for (int32_t k = MAX_STACK_POINTS - 1 ; k > hit_previous ; k--)
                {
                  misc->nearest_stack.dist[k] = misc->nearest_stack.dist[k - 1];
                  misc->nearest_stack.point[k] = misc->nearest_stack.point[k - 1];
                }

              misc->nearest_stack.point[hit_previous] = current_point;
              misc->nearest_stack.dist[hit_previous] = dist;

              break;
            }
        }
    }


  //  If we got here we didn't replace the minimum distance point.

  return (NVFalse);
}
